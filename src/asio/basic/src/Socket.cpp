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

#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include <thread>
#include <chrono>

using namespace testing;
using namespace boost;

using tcp = asio::ip::tcp;
using SocketPtr = std::shared_ptr<tcp::socket>;

class Socket : public Test {
public:
    static constexpr asio::ip::port_type Port{3333};
    static constexpr std::string_view Localhost{"127.0.0.1"};

public:
    asio::io_context ioc;
};

class Session {
public:
    using Ptr = std::shared_ptr<Session>;

    Session(SocketPtr socket, std::size_t bufferSize)
        : _socket{std::move(socket)}
    {
        assert(bufferSize > 0);
        _buffer.reset(new char[_bufferSize = bufferSize]);
    }

    Session(SocketPtr socket, const std::string& text)
        : Session(std::move(socket), text.size())
    {
        std::memcpy(_buffer.get(), text.data(), _bufferSize);
    }

    [[nodiscard]] std::size_t
    remainingBytes() const
    {
        return (_bufferSize > _treatBytesCount) ? (_bufferSize - _treatBytesCount) : 0;
    }

    [[nodiscard]] auto
    remainingBuffer() const
    {
        assert(_treatBytesCount < _bufferSize);
        return asio::buffer(_buffer.get() + _treatBytesCount, remainingBytes());
    }

    [[nodiscard]] bool
    complete() const
    {
        return (remainingBytes() == 0);
    }

    [[nodiscard]] bool
    bump(std::size_t bytesWritten)
    {
        _treatBytesCount += bytesWritten;
        return complete();
    }

    tcp::socket&
    socket()
    {
        assert(_socket);
        return *_socket;
    }

    [[nodiscard]] const tcp::socket&
    socket() const
    {
        assert(_socket);
        return *_socket;
    }

    [[nodiscard]] std::string
    str() const
    {
        assert(_bufferSize > 0);
        return std::string{_buffer.get(), _buffer.get() + _bufferSize};
    }

private:
    SocketPtr _socket;
    std::unique_ptr<char[]> _buffer;
    std::size_t _bufferSize{0};
    std::size_t _treatBytesCount{0};
};

void
asyncWriteHandler(system::error_code ec, std::size_t bytesWritten, const Session::Ptr& session)
{
    if (ec) {
        std::cerr << "Error occurred while writing bytes" << std::endl;
        return;
    }

    if (session->bump(bytesWritten)) {
        std::cout << "All bytes have been written" << std::endl;
        return;
    }

    const auto callback = [session](auto a1, auto a2) { asyncWriteHandler(a1, a2, session); };
    session->socket().async_write_some(session->remainingBuffer(), callback);
}

void
asyncReadHandler(system::error_code ec, std::size_t bytesRead, const Session::Ptr& session)
{
    if (ec) {
        std::cerr << "Error occurred while reading bytes" << std::endl;
        return;
    }

    if (session->bump(bytesRead)) {
        std::cout << "All bytes have been read" << std::endl;
        return;
    }

    const auto callback = [session](auto a1, auto a2) { asyncReadHandler(a1, a2, session); };
    session->socket().async_read_some(session->remainingBuffer(), callback);
}

TEST_F(Socket, CreateActiveSocket)
{
    // Active socket is used to
    //  - send/receive data to and from a remote application
    //  - initiate a connection establishment process with it
    asio::ip::tcp::socket socket{ioc};
    system::error_code ec;
    socket.open(asio::ip::tcp::v4(), ec);
    ASSERT_FALSE(ec);
}

TEST_F(Socket, CreatePassiveSocket)
{
    // Passive socket (acceptor) is used to wait for connection establishment requests only
    asio::ip::tcp::acceptor acceptor{ioc};
    system::error_code ec;
    acceptor.open(asio::ip::tcp::v4(), ec);
    ASSERT_FALSE(ec);
}

TEST_F(Socket, BindTcp)
{
    asio::ip::tcp::endpoint endpoint{asio::ip::address_v4::any(), Port};
    asio::ip::tcp::acceptor acceptor{ioc};

    system::error_code ec;
    acceptor.open(endpoint.protocol(), ec);
    ASSERT_FALSE(ec);
    acceptor.bind(endpoint, ec);
    ASSERT_FALSE(ec);
}

TEST_F(Socket, BindUdp)
{
    asio::ip::udp::endpoint endpoint{asio::ip::address_v4::any(), Port};
    asio::ip::udp::socket socket{ioc};

    system::error_code ec;
    socket.open(endpoint.protocol(), ec);
    ASSERT_FALSE(ec);
    socket.bind(endpoint, ec);
    ASSERT_FALSE(ec);
}

TEST_F(Socket, Connect)
{
    static std::string_view StrHost{"www.google.com"};
    static std::string_view StrPort{"80"};

    try {
        asio::ip::tcp::resolver resolver{ioc};
        const auto results = resolver.resolve(StrHost, StrPort);
        if (results.empty()) {
            FAIL() << "IP address is missing";
        }
        asio::ip::tcp::socket socket{ioc};
        asio::connect(socket, results);
        EXPECT_TRUE(socket.is_open());
    } catch (const system::system_error& e) {
        FAIL() << e.what() << std::endl;
    }
}

TEST_F(Socket, WriteSome)
{
    /**
     * Before running test setup TCP server in command line:
     *  $ nc -l 3333
     */
    try {
        asio::ip::tcp::endpoint endpoint{asio::ip::make_address_v4(Localhost), Port};

        asio::ip::tcp::socket socket{ioc, endpoint.protocol()};
        socket.connect(endpoint);
        ASSERT_TRUE(socket.is_open());

        const std::string_view data{"Hello Boost.Asio, I'm here\n"};
        std::size_t totalSent{0};
        while (totalSent != data.size()) {
            const auto dataBuffer{asio::buffer(data.data() + totalSent, data.size() - totalSent)};
            totalSent += socket.write_some(dataBuffer);
        }
    } catch (system::system_error& e) {
        FAIL() << e.what() << std::endl;
    }
}

TEST_F(Socket, Write)
{
    /**
     * Before running test setup TCP server in command line:
     *  $ nc -l 3333
     */
    try {
        asio::ip::tcp::endpoint endpoint{asio::ip::make_address_v4(Localhost), Port};
        asio::ip::tcp::socket socket{ioc, endpoint.protocol()};
        socket.connect(endpoint);
        ASSERT_TRUE(socket.is_open());
        const std::string_view data{"Hello Boost.Asio, I'm here\n"};
        asio::write(socket, asio::buffer(data));
    } catch (system::system_error& e) {
        FAIL() << e.what() << std::endl;
    }
}

TEST_F(Socket, ReadSome)
{
    static constexpr std::size_t BufferSize{6};

    /**
     * Before running test setup TCP server in command line and type the word "Hello":
     *  $ nc -l 3333
     *  Hello
     */
    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        asio::ip::tcp::socket socket(ioc, endpoint.protocol());
        socket.connect(endpoint);
        EXPECT_TRUE(socket.is_open());

        std::size_t total{0};
        std::array<char, BufferSize> buffer{0};
        while (total != (BufferSize - 1)) {
            const auto asioBuffer = asio::buffer(buffer.data() + total, BufferSize - total - 1);
            total += socket.read_some(asioBuffer);
        }
        std::cout << "Output: " << buffer.data() << std::endl;
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, Read)
{
    static constexpr int BufferSize{6};

    /**
     * Before running test setup TCP server in command line and type the word "Hello":
     *  $ nc -l 3333
     *  Hello
     */
    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        asio::ip::tcp::socket socket(ioc, endpoint.protocol());
        socket.connect(endpoint);
        EXPECT_TRUE(socket.is_open());

        std::array<char, BufferSize> buffer{0};
        asio::read(socket, asio::buffer(buffer, BufferSize - 1));
        std::cout << "Output: " << buffer.data() << std::endl;
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, ReadUntil)
{
    static constexpr std::string_view Delim{"EOF"};

    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        asio::ip::tcp::socket socket(ioc, endpoint.protocol());
        socket.connect(endpoint);
        EXPECT_TRUE(socket.is_open());

        asio::streambuf buffer;
        asio::read_until(socket, buffer, Delim);

        std::istream is{&buffer};
        std::string message;
        while (is) {
            std::getline(is, message);
            if (message != Delim) {
                std::cout << message << std::endl;
            }
        }
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, AsyncWriteSome)
{
    /**
     * Before running test setup TCP server in command line:
     *  $ nc -l 3333
     */
    try {
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        auto socket = std::make_shared<asio::ip::tcp::socket>(ioc, endpoint.protocol());
        socket->connect(endpoint);
        EXPECT_TRUE(socket->is_open());

        auto session = std::make_shared<Session>(socket, "Hello Boost.Asio, I'm here\n");
        const auto callback = [session](boost::system::error_code ec, std::size_t bytesWritten) {
            asyncWriteHandler(ec, bytesWritten, session);
        };
        socket->async_write_some(session->remainingBuffer(), callback);

        // Blocks until all async operations are done
        ioc.run();
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, AsyncWrite)
{
    /**
     * Before running test setup TCP server in command line:
     *  $ nc -l 3333
     */
    try {
        tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        tcp::socket socket(ioc, endpoint.protocol());
        socket.connect(endpoint);
        EXPECT_TRUE(socket.is_open());

        std::string data{"Hello Boost.Asio, I'm here\n"};
        const auto callback = [](system::error_code ec, std::size_t bytesWritten) {
            if (ec) {
                std::cerr << "Error occurred while writing" << std::endl;
                return;
            }
            std::cout << "All bytes have been written" << std::endl;
        };
        asio::async_write(socket, asio::buffer(data), callback);

        // Blocks until all async operations are done
        ioc.run();
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, AsyncReadSome)
{
    /**
     * Before running test setup TCP server in command line and type some text:
     *  $ nc -l 3333
     *  ...
     */
    try {
        tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        auto socket = std::make_shared<tcp::socket>(ioc, endpoint.protocol());
        socket->connect(endpoint);
        EXPECT_TRUE(socket->is_open());

        static constexpr std::size_t ReadBufferSize{20};
        auto session = std::make_shared<Session>(socket, ReadBufferSize);
        const auto callback = [session](boost::system::error_code ec, std::size_t bytesRead) {
            asyncReadHandler(ec, bytesRead, session);
        };
        socket->async_read_some(session->remainingBuffer(), callback);

        // Blocks until all async operations are done
        ioc.run();

        std::cout << "Read bytes: " << session->str() << std::endl;
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, AsyncRead)
{
    /**
     * Before running test setup TCP server in command line and type some text:
     *  $ nc -l 3333
     *  ...
     */
    try {
        tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        tcp::socket socket(ioc, endpoint.protocol());
        socket.connect(endpoint);
        EXPECT_TRUE(socket.is_open());

        static constexpr std::size_t ReadBufferSize{20};
        static std::array<char, ReadBufferSize> buffer;
        const auto callback = [](system::error_code ec, std::size_t bytesRead) {
            if (ec) {
                std::cerr << "Error occurred while reading" << std::endl;
                return;
            }
            std::cout << "All bytes have been read" << std::endl;
        };
        asio::async_read(socket, asio::buffer(buffer), callback);

        // Blocks until all async operations are done
        ioc.run();

        std::string str{buffer.data(), buffer.data() + ReadBufferSize};
        std::cout << "Read bytes: " << str << std::endl;
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}

TEST_F(Socket, Cancel)
{
    using namespace std::chrono_literals;

    try {
        tcp::endpoint endpoint(asio::ip::make_address_v4(Localhost), Port);
        tcp::socket socket(ioc, endpoint.protocol());

        socket.async_connect(endpoint, [](const system::error_code ec) {
            if (ec) {
                if (ec == asio::error::operation_aborted) {
                    std::cout << "Operation was aborted" << std::endl;
                } else {
                    FAIL() << ec.message();
                }
                return;
            }

            std::cout << "Connection was established" << std::endl;
        });

        auto worker = std::jthread(
            [](asio::io_context& context) {
                try {
                    const auto guard = asio::make_work_guard(context);
                    context.run();
                } catch (const system::system_error& e) {
                    FAIL() << e.what();
                }
            },
            std::ref(ioc));

        std::cout << "Attempt to cancel operation" << std::endl;
        socket.cancel();
        std::cout << "Operation was cancelled" << std::endl;

        ioc.stop();
    } catch (const system::system_error& e) {
        FAIL() << e.what();
    }
}