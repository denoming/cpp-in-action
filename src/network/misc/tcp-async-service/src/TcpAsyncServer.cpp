#include "TcpAsyncServer.h"

namespace {

std::thread
spawnContextThread(net::io_context& context)
{
    return std::thread{[&]() {
        const auto guard = net::make_work_guard(context);
        context.run();
    }};
}

} // namespace

TcpAsyncServer::TcpAsyncServer(net::io_context& context)
    : _context{context}
{
}

TcpAsyncServer::~TcpAsyncServer()
{
    stop();
}

void
TcpAsyncServer::start(net::ip::port_type port, std::size_t threadsNum)
{
    _acceptor = std::make_unique<TcpAsyncAcceptor>(_context, port);

    assert(threadsNum > 0);
    while (threadsNum--) {
        _threads.push_back(spawnContextThread(_context));
    }

    _acceptor->start();
}

void
TcpAsyncServer::stop()
{
    if (_acceptor) {
        _acceptor->stop();
        _acceptor.reset();
    }

    if (!_context.stopped()) {
        _context.stop();
    }

    for (auto& thread : _threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}