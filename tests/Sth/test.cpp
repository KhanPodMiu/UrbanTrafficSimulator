#include <gtest/gtest.h>

TEST(MathTest, Addition)
{
    EXPECT_EQ(2 + 3, 5);
}

TEST(MathTest, Subtraction)
{
    EXPECT_EQ(10 - 4, 6);
}

TEST(MathTest, Multiplication)
{
    EXPECT_EQ(7 * 8, 56);
}

TEST(MathTest, Division)
{
    EXPECT_EQ(20 / 5, 4);
}

TEST(MathTest, Boolean)
{
    EXPECT_TRUE(10 > 5);
}

TEST(MathTest, StringCompare)
{
    std::string a = "hello";
    std::string b = "hello";

    EXPECT_EQ(a, b);
}

TEST(MathTest, DoubleCompare)
{
    EXPECT_DOUBLE_EQ(
        0.1 + 0.2,
        0.3
    );
}
