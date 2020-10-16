#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <thread>

class Data {
public:
    Data(const std::string& name)
        : _name{name}
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

    [[deprecated]] std::thread // Using 'deprecated' attribute (warn if you trying to use it)
    startThreadWithCopyOfThis([[maybe_unused]] int timeout)
    {
        return std::thread{};
    }

private:
    std::string _name;
};

TEST(LambdaExtensionTest, CopyOfThis)
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
