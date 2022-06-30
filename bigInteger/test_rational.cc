#include "gtest/gtest.h"
#include "biginteger.h"
#include <cassert>
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>

TEST(Arithmetic, AssignmentSmall)
{
    Rational r;
    r = 5;
    r += 3;
    r *= 7; // r==56
    BigInteger b = 15;
    (r /= 8) -= b; // r = 7-15=-8
    ASSERT_EQ(-r, 8);
    // if (-r != 8)
    //     throw std::runtime_error("Test 1 failed.");
}

TEST(Arithmetic, LargeDivisors)
{
    Rational s, t;
    s = Rational(85) / 37, t = Rational(29) / BigInteger(-163);
    s += t;
    t = 1;
    for (int i = 0; i < 15; ++i)
        t *= s;
    ASSERT_EQ((1 / t).toString(), "507972178875842800075597772950831264898404875587494819951"
                                  "/39717526884730183825748150063721595142668632339785349901549568");
    // if ((1/t).toString() != "507972178875842800075597772950831264898404875587494819951"
    //         "/39717526884730183825748150063721595142668632339785349901549568")
    //     throw std::runtime_error("Test 2 failed.");
}

TEST(Arithmetic, NearZero) {
    Rational s, t;
    s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11; // 2^2×3×7×13×19×41×43×11
    t = -17 * 13 * 23 * 79;
    s *= s * s, t *= t * t;
    Rational q = s / t;
    ASSERT_EQ(q.toString(), "-29650611427828166204352/29472131485369");
    ASSERT_LT(q / 1000000000, 1);
    ASSERT_EQ(0 / q, 0);
    // if (q.toString() != "-29650611427828166204352/29472131485369")
    //     throw std::runtime_error("Test 3 failed.");
    // if (q / 1000000000 >= 1)
    //     throw std::runtime_error("Test 4 failed.");
    // if (0 / q != 0)
    //     throw std::runtime_error("Test 5 failed.");
}

TEST(Arithmetic, InverseNumber) {
    Rational s, t;
    s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11; // 2^2×3×7×13×19×41×43×11
    t = -17 * 13 * 23 * 79;
    s *= s * s, t *= t * t;
    Rational q = s / t;
    q *= t / s;
    ASSERT_EQ(q, 1);
    ASSERT_EQ(q.toString(), "1");
    // q *= t / s;
    // if (q != 1 || q.toString() != "1")
    //     throw std::runtime_error("Test 6 failed.");
}

TEST(AsDecimal, Simple) {
    Rational s, t;
    s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11;
    t = s - 25; // t=402365939
    ((s = 1000000007) *= 1000000009) *= 2147483647;
    ASSERT_EQ((s / t).asDecimal(10), "5337140829307966068.3989202202");
    // if ((s / t).asDecimal(10) != "5337140829307966068.3989202202")
    //     throw std::runtime_error("Test 7 failed.");
}

TEST(AsDecimal, NearZero) {
    Rational s, t;
    s = 4 * 3 * 7 * 13 * 19 * 41 * 43 * 11;
    t = s - 25; // t=402365939
    ((s = 1000000007) *= 1000000009) *= 2147483647;
    t = -t;
    ASSERT_EQ((t / s).asDecimal(25), "-0.0000000000000000001873662");
    // if ((t / s).asDecimal(25) != "-0.0000000000000000001873662")
    //     throw std::runtime_error("Test 8 failed.");
}
