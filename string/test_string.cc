#include "gtest/gtest.h"
#include "string.h"
#include <cassert>
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>


TEST(BasicReading, Positive)
{
    String s("abcdef");
    ASSERT_EQ(s.length(), 6);
    ASSERT_EQ(s[0], 'a');
    ASSERT_EQ(s[3], 'd');
    ASSERT_EQ(s[5], 'f');
    ASSERT_EQ(s.back(), 'f');
    ASSERT_EQ(s.front(), 'a');
}

TEST(BasicModificationOperation, Positive)
{
    String s("abcdef");
    s[1] = 'x';
    s.back() = 'y';
    s.front() = 'y';
    ASSERT_EQ(s[0], 'y');
    ASSERT_EQ(s[5], 'y');
    ASSERT_EQ(s[1], 'x');
}

TEST(Constness, Positive)
{
    const String s("abcdef");
    ASSERT_TRUE((!std::is_assignable_v<decltype(static_cast<const String &>(String())[0]), char>));
    ASSERT_TRUE((!std::is_assignable_v<decltype(static_cast<const String &>(String()).front()), char>));
    ASSERT_TRUE((!std::is_assignable_v<decltype(static_cast<const String &>(String()).back()), char>));
}

TEST(CopyConstructor, Positive)
{
    String s("aaa");
    for (int i = 0; i < 500'000; ++i)
    {
        s.push_back('a');
    }
    {
        String ss = s;
        ASSERT_EQ(ss.length(), 500'003);
        ASSERT_EQ(s[500'000], 'a');
        ss[500'000] = 'b';
    }
    ASSERT_EQ(s.length(), 500'003);
    ASSERT_EQ(s[500'000], 'a');

    String ss = s;
    for (int i = 0; i < 500'000; ++i)
    {
        s.pop_back();
    }

    ASSERT_EQ(s.length(), 3);
    ASSERT_EQ(ss.length(), 500'003);
    ss = s;
    s = String("abc");
    ASSERT_EQ(ss.length(), 3);
    ASSERT_EQ(ss[2], 'a');

    for (int i = 0; i < 500'000; ++i)
    {
        const String sss(1000, 'a');
        String sz = sss;
        s += sz.back();
    }
    s = s;
    ASSERT_EQ(s.length(), 500'003);
    ASSERT_EQ(s[500'002], 'a');
    ASSERT_EQ(s[266'532], 'a');

    (s += String(100, 'b')) += String(100, 'c');
    ASSERT_EQ(s.length(), 500'203);
    ASSERT_EQ(s.back(), 'c');

    for (int i = 0; i < 150; ++i)
    {
        s.pop_back();
    }
    ASSERT_EQ(s.back(), 'b');

    s = ss = s;
    (ss = s) = ss;
}

TEST(InputOutputStreams, Positive)
{
    std::istringstream iss("abcdefg\nABCDE");

    String s;
    ASSERT_TRUE(s.empty());

    iss >> s;
    ASSERT_EQ(s.length(), 7);

    String ss;
    iss >> ss;
    ASSERT_FALSE(ss.empty());
    ASSERT_EQ(ss.length(), 5);
    ASSERT_EQ((s + ss).length(), 12);

    auto sum = ss + s;
    sum.pop_back();
    ASSERT_EQ(sum.length(), 11);

    sum[0] = 'x';
    ASSERT_EQ(ss.length(), 5);
    ASSERT_EQ(s.length(), 7);

    std::ostringstream oss;
    oss << ('!' + sum + '?');

    ASSERT_EQ(oss.str(), "!xBCDEabcdef?");
}

TEST(Reference, Positive)
{
    String s = "abcde";

    auto &ss = s += 'f';

    ss += 'g';

    std::ostringstream oss;
    oss << ss;

    ASSERT_EQ(oss.str(), "abcdefg");
    ASSERT_EQ(s.length(), 7);

    {
        auto &ss = (s = s);
        ASSERT_EQ(ss[5], 'f');
        auto sss = ss;
        ss.clear();
        ASSERT_EQ(sss.length(), 7);
    }
    ASSERT_TRUE(ss.empty());
}

TEST(FindAndSubstr, Positive)
{
    const String s = "122333444455555";
    //  0123456789ABCDE

    ASSERT_EQ(s.find("44"), 6);
    ASSERT_EQ(s.rfind("44"), 8);

    ASSERT_EQ(s.find("12345"), (std::is_same_v<String, std::string> ? std::string::npos : s.length()));

    ASSERT_EQ(s.substr(5, 4), "3444");

    decltype(s.substr(1, 1)) ss = s;
    ss[0] = '0';
    ASSERT_EQ(s[0], '1');

    decltype(ss = ss) sss = ss;
    sss[0] = '2';
    ASSERT_EQ(ss[0], '2');

    decltype(ss += 'a') ssss = ss;
    ssss.front() = '3';
    ASSERT_EQ(ss.front(), '3');
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}