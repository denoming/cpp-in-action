#include <gtest/gtest.h>

#include <bitset>

TEST(Bits, Manipulation)
{
    std::uint8_t number = 0b00110010;
    std::cout << std::boolalpha;
    std::cout << "std::has_single_bit(0b00110010): " << std::has_single_bit(number) << '\n';
    std::cout << "std::bit_ceil(0b00110010): " << std::bitset<8>(std::bit_ceil(number)) << '\n';
    std::cout << "std::bit_floor(0b00110010): " << std::bitset<8>(std::bit_floor(number)) << '\n';
    std::cout << "std::bit_width(5u): " << std::bit_width(5u) << '\n';
    std::cout << "std::rotl(0b00110010, 2): " << std::bitset<8>(std::rotl(number, 2)) << '\n';
    std::cout << "std::rotr(0b00110010, 2): " << std::bitset<8>(std::rotr(number, 2)) << '\n';
    std::cout << "std::countl_zero(0b00110010): " << std::countl_zero(number) << '\n';
    std::cout << "std::countl_one(0b00110010): " << std::countl_one(number) << '\n';
    std::cout << "std::countr_zero(0b00110010): " << std::countr_zero(number) << '\n';
    std::cout << "std::countr_one(0b00110010): " << std::countr_one(number) << '\n';
    std::cout << "std::popcount(0b00110010): " << std::popcount(number) << '\n';
}