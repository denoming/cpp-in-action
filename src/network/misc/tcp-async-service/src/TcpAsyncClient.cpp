#include "TcpAsyncClient.h"

namespace {

std::thread
spawnThread(net::io_context& context)
{
    return std::thread{[&]() {
        const auto guard = net::make_work_guard(context);
        context.run();
    }};
}

} // namespace

TcpAsyncClient::TcpAsyncClient(std::size_t numberOfThread)
{
    assert(numberOfThread > 0);
    while (numberOfThread--) {
        _threads.push_back(spawnThread(_context));
    }
}

TcpAsyncClient::RequestId
TcpAsyncClient::communicate(std::string_view address,
                            net::ip::port_type port,
                            RequestCallback callback)
{
    std::string request{"Ping\n"};
    auto session = std::make_shared<Session>(
        0, std::move(request), address, port, std::move(callback), _context);

    const auto id{getRequestId()};
    std::unique_lock lock{_sessionsGuard};
    _sessions[id] = session;
    lock.unlock();

    session->socket.async_connect(
        session->endpoint, [this, session](sys::error_code ec) { onConnectDone(session, ec); });

    return id;
}

void
TcpAsyncClient::cancel(RequestId requestId)
{
    std::lock_guard lock{_sessionsGuard};
    if (auto it = _sessions.find(requestId); it != _sessions.end()) {
        const auto [_, session] = *it;
        std::lock_guard sessionLock{session->guard};
        session->cancel = true;
        session->socket.cancel();
    }
}

void
TcpAsyncClient::close()
{
    _context.stop();

    for (auto& thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void
TcpAsyncClient::onConnectDone(const Session::Ptr& session, sys::error_code ec)
{
    if (ec) {
        onComplete(session, ec);
        return;
    }

    assert(session);
    if (session->cancel) {
        onComplete(session, net::error::operation_aborted);
        return;
    }

    net::async_write(session->socket,
                     net::buffer(session->request),
                     [this, session](sys::error_code ec, std::size_t bytesWritten) {
                         onWriteDone(session, bytesWritten, ec);
                     });
}

void
TcpAsyncClient::onWriteDone(const Session::Ptr& session,
                            std::size_t /*byteWritten*/,
                            sys::error_code ec)
{
    if (ec) {
        onComplete(session, ec);
        return;
    }

    if (session->cancel) {
        onComplete(session, net::error::operation_aborted);
        return;
    }

    assert(session);
    net::async_read_until(session->socket,
                          session->responseBuffer,
                          '\n',
                          [this, session](sys::error_code ec, std::size_t bytesRead) {
                              onReadDone(session, bytesRead, ec);
                          });
}

void
TcpAsyncClient::onReadDone(const Session::Ptr& session,
                           std::size_t /*bytesRead*/,
                           sys::error_code ec)
{
    if (!ec) {
        assert(session);
        std::istream is{&session->responseBuffer};
        std::getline(is, session->response);
    }
    onComplete(session, ec);
}

void
TcpAsyncClient::onComplete(const Session::Ptr& session, sys::error_code ec)
{
    assert(session);
    session->socket.shutdown(net::socket_base::shutdown_both);
    session->socket.close();

    std::unique_lock lock{_sessionsGuard};
    _sessions.erase(session->id);
    lock.unlock();

    if (session->callback) {
        session->callback(session->id, std::move(session->response), ec);
    }
}

TcpAsyncClient::RequestId
TcpAsyncClient::getRequestId()
{
    static RequestId id{0};
    return ++id;
}