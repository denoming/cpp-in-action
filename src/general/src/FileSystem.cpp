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

#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <fstream>

namespace fs = std::filesystem;

TEST(FileSystemTest, ReadAccess)
{
    using namespace std::string_view_literals;

    switch (fs::path p{"/etc/os-release"sv}; status(p).type()) {
    case fs::file_type::not_found:
        std::clog << "Path '" << p.string() << "' doesn't exists" << std::endl;
        break;
    case fs::file_type::regular:
        std::clog << "File '" << p.string() << "' exists with " << fs::file_size(p) << " bytes"
                  << std::endl;
        break;
    case fs::file_type::directory:
        std::clog << "'" << p.string() << "' is a directory containing: " << std::endl;
        for (auto& e : fs::directory_iterator(p)) {
            std::clog << "\t'" << e.path().string() << std::endl;
        }
        break;
    default:
        std::clog << "'" << p.string() << "' is special file" << std::endl;
    }
}

TEST(FileSystemTest, WriteAccess)
{
    using namespace std::string_view_literals;

    // Remove test folder
    fs::path testFolder{"tmp"sv};
    fs::remove_all(testFolder);

    // Create test folder
    auto testSubFolder = testFolder / "test";
    fs::create_directories(testSubFolder);

    // Create test file
    fs::path testFile = testSubFolder / "file.txt"sv;
    std::ofstream s{testFile};
    ASSERT_TRUE(s);
    s << "The asnwer is 42" << std::endl;

    // Create symlink
    fs::create_directory_symlink("test"sv, testSubFolder.parent_path() / "slink");

    // List all content
    std::clog << "Current path is '" << fs::current_path().string() << "'" << std::endl;
    auto options{fs::directory_options::follow_directory_symlink};
    for (const auto& e : fs::recursive_directory_iterator("."sv, options)) {
        std::clog << "\t" << e.path().lexically_normal().string() << std::endl;
    }
}
