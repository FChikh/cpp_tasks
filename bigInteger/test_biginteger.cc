#include "gtest/gtest.h"
#include "biginteger.h"
#include <cassert>
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>

TEST(AssignementStreamTest, IntAndStream)
{

    BigInteger b, k;
    b = 0, k = 1234567;
    std::ostringstream oss;
    oss << b << ' ' << k;
    // if (oss.str() != "0 1234567") {
    //     throw std::runtime_error("Assignment from int, or stream output failed.");
    // }
    ASSERT_EQ(oss.str(), "0 1234567");
}

TEST(ToString, Positive)
{
    BigInteger a = 9876543;
    std::string testString = a.toString();
    ASSERT_EQ(testString, "9876543");
}

TEST(UnaryMinus, Positive)
{
    BigInteger b = 0, k = 1234567;
    BigInteger a = b;
    a = -a;
    std::string testString = a.toString() + " " + (-k).toString();
    // if (testString != "0 -1234567") {
    //     throw std::runtime_error("Unary minus or toString method failed.");
    // }
    ASSERT_EQ(testString, "0 -1234567");
}

TEST(AssignementTest, Addition)
{
    BigInteger a, b;
    a = 999, b = 1000;
    a = a += a;
    std::string testString = a.toString();
    ASSERT_EQ(testString, "1998");
    // if (testString != "1998")
    // {
    //     throw std::runtime_error("Operator = or += failed.");
    // }
}

TEST(Arithmetic, WithAssignments)
{
    BigInteger a = 1998;
    BigInteger b = 1000;
    ++a %= a /= a *= a -= b++;
    std::ostringstream oss2;
    oss2 << 5 + a << ' ' << 1 - b; // 5 -1000
    ASSERT_EQ(oss2.str(), "5 -1000");
    // if (oss2.str() != "5 -1000")
    // {
    //     throw std::runtime_error("Arithmetic operations failed.");
    // }
}

TEST(ToBool, Positive)
{
    BigInteger a = 0;
    std::ostringstream oss3;
    oss3 << (a = (bool(a) ? -1 : -2));
    ASSERT_EQ(oss3.str(), "-2");
    // if (oss3.str() != "-2")
    // {
    //     throw std::runtime_error("Tests failed.");
    // }
}

TEST(IOStreams, Positive)
{
    BigInteger a = 0, b = 1001;
    std::istringstream iss("26 5");
    iss >> a >> b;
    std::ostringstream oss4;
    oss4 << b << ' ' << a << ' ';
    ASSERT_EQ(oss4.str(), "5 26 ");
    // if (oss4.str() != "5 26 ")
    // {
    //     throw std::runtime_error("Stream input or output failed.");
    // }
}

TEST(Arithmetic, Simple)
{
    std::ostringstream oss4;
    BigInteger a = 26, b = 5;
    oss4 << a + b << ' ' << a - b << ' ' << a * b << ' ' << a / b << ' ' << a % b << '\n';
    oss4 << b + a << ' ' << b - a << ' ' << b * a << ' ' << b / a << ' ' << b % a;
    ASSERT_EQ(oss4.str(), "31 21 130 5 1\n31 -21 130 0 5");
    // if (oss4.str() != "31 21 130 5 1\n31 -21 130 0 5")
    // {
    //     throw std::runtime_error("Arithmetic operations failed.");
    // }
}

TEST(Relations, Positive)
{
    std::vector<BigInteger> v;
    for (size_t i = 0; i < 1000; ++i)
    {
        v.push_back(1000 - i);
    }
    std::sort(v.begin(), v.end());
    std::ostringstream oss5;
    oss5 << v[0] << ' ' << v[500] << ' ' << v[999] << ' ';
    BigInteger a = 26, b = 5;
    oss5 << (a != b) << ' ' << (a < b) << ' ' << (a > b) << ' ';
    oss5 << (a <= b) << ' ' << (a >= b);
    ASSERT_EQ(oss5.str(), "1 501 1000 1 0 1 0 1");
    // if (oss5.str() != "1 501 1000 1 0 1 0 1")
    // {
    //     throw std::runtime_error("Relation operations failed.");
    // }
}

TEST(BigNumbers, IOStreams)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::ostringstream oss6;
    oss6 << b << a;
    ASSERT_EQ(oss6.str(), "-10000001000000000000000000000000000000000");
    // if (oss6.str() != "-10000001000000000000000000000000000000000")
    // {
    //     throw std::runtime_error("Stream input or output failed.");
    // }
}

TEST(BigNumbers, Addition)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::istringstream iss3("453234523460009834520987234598234502345987029345436345634563 "
                            "234523452034623049872345234520983475325345234232578");
    BigInteger c, d;
    iss3 >> c >> d;

    std::istringstream iss4("-23534576554950000000000000009999990000999900000 "
                            "8888888888884444444444433333333333332222222222222111112222777777777");
    BigInteger e, f;
    iss4 >> e >> f;

    std::ostringstream oss7;
    oss7 << a + b << ' ' << c + d << ' ' << e + f;
    ASSERT_EQ(oss7.str(), "999999999999999999999999999000000 "
                          "453234523694533286555610284470579736866970504670781579867141 "
                          "8888888888884444444420898756778383332222222222212111122221777877777");
    // if (oss7.str() != "999999999999999999999999999000000 "
    //                   "453234523694533286555610284470579736866970504670781579867141 "
    //                   "8888888888884444444420898756778383332222222222212111122221777877777")
    // {
    //     throw std::runtime_error("Operator + failed.");
    // }
}

TEST(BigNumbers, Substraction)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::istringstream iss3("453234523460009834520987234598234502345987029345436345634563 "
                            "234523452034623049872345234520983475325345234232578");
    BigInteger c, d;
    iss3 >> c >> d;

    std::istringstream iss4("-23534576554950000000000000009999990000999900000 "
                            "8888888888884444444444433333333333332222222222222111112222777777777");
    BigInteger e, f;
    iss4 >> e >> f;

    std::ostringstream oss8;
    oss8 << a - b << ' ' << c - d << ' ' << e - f;
    ASSERT_EQ(oss8.str(), "1000000000000000000000000001000000 "
                          "453234523225486382486364184725889267825003554020091111401985 "
                          "-8888888888884444444467967909888283332222222222232111102223777677777");
    // if (oss8.str() != "1000000000000000000000000001000000 "
    //                   "453234523225486382486364184725889267825003554020091111401985 "
    //                   "-8888888888884444444467967909888283332222222222232111102223777677777")
    // {
    //     throw std::runtime_error("Operator - failed.");
    // }
}

TEST(BigNumbers, Multiplication)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::istringstream iss3("453234523460009834520987234598234502345987029345436345634563 "
                            "234523452034623049872345234520983475325345234232578");
    BigInteger c, d;
    iss3 >> c >> d;

    std::istringstream iss4("-23534576554950000000000000009999990000999900000 "
                            "8888888888884444444444433333333333332222222222222111112222777777777");
    BigInteger e, f;
    iss4 >> e >> f;

    std::ostringstream oss9;
    oss9 << a * b << ' ' << c * d << ' ' << e * f;
    ASSERT_EQ(oss9.str(), "-1000000000000000000000000000000000000000 "
                          "106294125023108851855435239407742287054686671354449187194200406809777590661604024572718537860109672117737393414 "
                          "-209196236043895401881977738593593744982694026094492829962212043149123345328234038901116544451103777729999222300000");
    // if (oss9.str() != "-1000000000000000000000000000000000000000 "
    //                   "106294125023108851855435239407742287054686671354449187194200406809777590661604024572718537860109672117737393414 "
    //                   "-209196236043895401881977738593593744982694026094492829962212043149123345328234038901116544451103777729999222300000")
    // {
    //     throw std::runtime_error("Operator * failed.");
    // }
}

TEST(BigNumbers, Division)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::istringstream iss3("453234523460009834520987234598234502345987029345436345634563 "
                            "234523452034623049872345234520983475325345234232578");
    BigInteger c, d;
    iss3 >> c >> d;

    std::istringstream iss4("-23534576554950000000000000009999990000999900000 "
                            "8888888888884444444444433333333333332222222222222111112222777777777");
    BigInteger e, f;
    iss4 >> e >> f;
    std::ostringstream oss10;
    oss10 << a / b << ' ' << c / d << ' ' << e / f;
    ASSERT_EQ(oss10.str(), "-1000000000000000000000000000 1932576548 0");
    // if (oss10.str() != "-1000000000000000000000000000 1932576548 0")
    // {
    //     throw std::runtime_error("Operator / failed.");
    // }
}

TEST(BigNumbers, Modulo)
{
    BigInteger a, b;
    std::istringstream iss2("1000000000000000000000000000000000 -1000000");
    iss2 >> a >> b;
    std::istringstream iss3("453234523460009834520987234598234502345987029345436345634563 "
                            "234523452034623049872345234520983475325345234232578");
    BigInteger c, d;
    iss3 >> c >> d;

    std::istringstream iss4("-23534576554950000000000000009999990000999900000 "
                            "8888888888884444444444433333333333332222222222222111112222777777777");
    BigInteger e, f;
    iss4 >> e >> f;

    std::ostringstream oss11;
    oss11 << a % b << ' ' << c % d << ' ' << e % f;
    ASSERT_EQ(oss11.str(), "0 101894444317458440603421824036688159663989325253819 "
                           "-23534576554950000000000000009999990000999900000");
    // if (oss11.str() != "0 101894444317458440603421824036688159663989325253819 "
    //                    "-23534576554950000000000000009999990000999900000")
    // {
    //     throw std::runtime_error("Operator % failed.");
    // }
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}