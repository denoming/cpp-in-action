// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <thread>
#include <mutex>
#include <map>
#include <memory>

namespace net = boost::asio;
namespace sys = boost::system;

class TcpAsyncClient : public boost::noncopyable {
public:
    using RequestId = std::uint64_t;
    using RequestCallback
        = std::function<void(const RequestId, std::string response, const sys::error_code)>;

    explicit TcpAsyncClient(std::size_t numberOfThread = std::thread::hardware_concurrency());

    [[maybe_unused]] RequestId
    communicate(std::string message,
                std::string_view address,
                net::ip::port_type port,
                RequestCallback callback);

    void
    cancel(RequestId requestId);

    void
    close();

private:
    struct Session {
        using Ptr = std::shared_ptr<Session>;

        explicit Session(RequestId id,
                         std::string request,
                         std::string_view address,
                         net::ip::port_type port,
                         RequestCallback callback,
                         net::io_context& context)
            : id{id}
            , request{std::move(request)}
            , callback{std::move(callback)}
            , endpoint{net::ip::make_address(address), port}
            , socket{context, endpoint.protocol()}
        {
        }

        const RequestId id;
        const std::string request;
        RequestCallback callback;
        bool cancel{false};
        net::streambuf responseBuffer;
        std::string response;
        net::ip::tcp::endpoint endpoint;
        net::ip::tcp::socket socket;
        std::mutex guard;
    };

    static RequestId
    getRequestId();

private:
    void
    onConnectDone(const Session::Ptr& session, sys::error_code ec);

    void
    onWriteDone(const Session::Ptr& session, std::size_t bytesWritten, sys::error_code ec);

    void
    onReadDone(const Session::Ptr& session, std::size_t bytesRead, sys::error_code ec);

    void
    onComplete(const Session::Ptr& session, sys::error_code ec);

private:
    net::io_context _context;
    std::vector<std::thread> _threads;
    std::mutex _sessionsGuard;
    std::map<RequestId, Session::Ptr> _sessions;
};