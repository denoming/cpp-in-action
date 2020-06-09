#include <gtest/gtest.h>

#include <complex>

TEST(OptionalTest, ChangingValue)
{
    using DoubleComplex = std::complex<double>;
    using DoubleComplexOrNull = std::optional<DoubleComplex>;

    DoubleComplexOrNull op{std::in_place, 3.0, 4.0};
    EXPECT_DOUBLE_EQ(op.value().real(), 3.0);
    EXPECT_DOUBLE_EQ(op.value().imag(), 4.0);
}
