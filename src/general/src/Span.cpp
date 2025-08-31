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

#include <span>

void
printMe(std::span<int> seq)
{
    std::cout << "Size = " << seq.size() << " (" << seq.size_bytes() << " bytes): ";
    for (auto e : seq) {
        std::cout << e << ' ';
    }
    std::cout << std::endl;
}

TEST(Span, Create)
{
    std::vector<int> numbers{23, 42, 56};

    std::span<int> dynamicSpan{numbers};
    printMe(dynamicSpan);
    std::span<int, 2> staticSpan(numbers);
    printMe(staticSpan); // Implicitly converted into a dynamic span

    // staticSpan = dynamicSpan; ERROR
    dynamicSpan = staticSpan;
    printMe(dynamicSpan);
}

TEST(Span, SizeDeducing)
{
    int a1[] = {1, 2, 3, 4};
    printMe(a1);

    std::vector<int> v{23, 42, 56};
    printMe(v);

    std::array<int, 4> a2 = {4, 3, 2, 1};
    printMe(a2);
}

TEST(Span, Subspan)
{
    std::vector<int> numbers{1, 2, 3, 4, 5, 6};
    printMe(numbers);

    std::span s1{numbers};

    std::span s2{s1.first(3)};
    printMe(s2);
    std::span s3{s1.last(3)};
    printMe(s3);

    std::span s4{s1.subspan(1, s1.size() - 1)};
    std::transform(s4.begin(), s4.end(), s4.begin(), [](int i) { return i * i; });
    printMe(s4);
}
