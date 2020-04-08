#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include <map>
#include <filesystem>

TEST(EmbeddedInitTest, InsertWithCheck)
{
    std::map<std::string, unsigned> coll;
    coll.insert({"New", 42});
    if (auto [p, ok] = coll.insert({"New", 38}); !ok) {
        const auto& [key, val] = *p;
        EXPECT_EQ(val, 42);
        return;
    }
    FAIL() << "Not expected";
}

TEST(EmbeddedInitTest, Switch)
{
    namespace fs = std::filesystem;
    switch (fs::path p{"/tmp"}; status(p).type()) {
    case fs::file_type::not_found:
        SUCCEED() << "Expected => Directory not found";
        break;
    case fs::file_type::directory:
        SUCCEED() << "Expected => Directory found";
        break;
    default:
        FAIL() << "Not expected";
    }
}
