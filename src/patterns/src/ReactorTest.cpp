#include <gtest/gtest.h>

#include "Reactor.hpp"

#include <iostream>
#include <chrono>

using namespace std::literals;

TEST(ReactorTest, Reactor)
{
    class MyEventHandler final : public EventHandler {
    public:
        void
        process(Handle handle, short what) final
        {
            std::cout << "Handle Me!\n";
        }
    };

    Reactor reactor;
    reactor.run();
    EXPECT_TRUE(reactor.active());

    auto event = reactor.registerHandler(std::make_shared<MyEventHandler>());
    std::this_thread::sleep_for(100ms);
    reactor.signal(event);
    std::this_thread::sleep_for(100ms);
    reactor.removeHandler(event);
}
