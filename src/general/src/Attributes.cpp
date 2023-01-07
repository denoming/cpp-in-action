#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <thread>

class Data {
public:
    explicit Data(std::string name)
        : _name{std::move(name)}
    {
    }

    [[nodiscard]] std::thread // Using 'nodiscard' attribute, which warn if you miss return value
    startThreadWithCopyOfThis()
    {
        using namespace std::literals;
        return std::thread([*this]() {
            std::this_thread::sleep_for(3s);
            std::cout << "Name: " << _name << std::endl;
        });
    }

    [[deprecated]] std::thread // Using 'deprecated' attribute (warn if you are trying to use it)
    startThreadWithCopyOfThis([[maybe_unused]] int timeout)
    {
        return std::thread{};
    }

private:
    std::string _name;
};

TEST(Attributes, NoDiscard)
{
    std::thread t;
    {
        Data d{"Denys"};
        t = d.startThreadWithCopyOfThis();
    }
    if (t.joinable()) {
        t.join();
    }
}
