#include <gmock/gmock.h>

#include <iostream>
#include <functional>
#include <type_traits>

template<typename C, typename... Args>
decltype(auto) call(C c, Args&&... args)
{
    if constexpr (std::is_void_v<std::invoke_result<C, Args...>>) {
        c(std::forward<Args>(args)...);
    }
    else {
        return c(std::forward<Args>(args)...);
    }
}

TEST(CompileTimeIfTest, Test1)
{
    auto vCall = [] (std::string message) -> void {
        std::cout << "Message: " << message << std::endl;
    };

    auto nCall = [] (std::string message) -> int {
        std::cout << "Message: " << message << std::endl;
        return 0;
    };

    call(vCall, "Hello from void");
    EXPECT_EQ(call(nCall, "Hello from int"), 0);
}
