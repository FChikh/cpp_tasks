// Before refactoring

#include "string.h"

#include <cassert>
#include <string>
#include <iostream>
#include <type_traits>
#include <sstream>

//using String = std::string;

void test1() {
    String s("abcdef");
    assert(s.length() == 6);
    assert(s[0] == 'a');
    assert(s[3] == 'd');
    assert(s[5] == 'f');
    assert(s.back() == 'f');
    assert(s.front() == 'a');
    
    s[1] = 'x';
    s.back() = 'y';
    s.front() = 'y';
    assert(s[0] == 'y');
    assert(s[5] == 'y');
    assert(s[1] == 'x');
}

void test2() {    
    const String s("abcdef");
    assert(s.length() == 6);
    assert(s[0] == 'a');
    assert(s[3] == 'd');
    assert(s[5] == 'f');
    assert(s.back() == 'f');
    assert(s.front() == 'a');
    assert((!std::is_assignable_v<decltype(static_cast<const String&>(String())[0]), char>));
    assert((!std::is_assignable_v<decltype(static_cast<const String&>(String()).front()), char>));
    assert((!std::is_assignable_v<decltype(static_cast<const String&>(String()).back()), char>));
}

void test3() {
    String s("aaa");
    for (int i = 0; i < 500'000; ++i) {
        s.push_back('a');
    }
//std::cerr << 1 << std::endl;
    {
        String ss = s;
        assert(ss.length() == 500'003);
        assert(s[500'000] == 'a');
        ss[500'000] = 'b';
    }
    assert(s.length() == 500'003);
    assert(s[500'000] == 'a');
//std::cerr << 2 << std::endl;

    String ss = s;
    for (int i = 0; i < 500'000; ++i) {
        s.pop_back();
    }
//std::cerr << 3 << std::endl;

    assert(s.length() == 3);
    assert(ss.length() == 500'003);
    ss = s;
    s = String("abc");
    assert(ss.length() == 3);
    assert(ss[2] == 'a');
//std::cerr << 4 << std::endl;

    for (int i = 0; i < 500'000; ++i) {
        const String sss(1000, 'a');
        String sz = sss;
        s += sz.back();
    }
//std::cerr << 5 << std::endl;

    s = s;
    assert(s.length() == 500'003);
    assert(s[500'002] == 'a');
    assert(s[266'532] == 'a');
//std::cerr << 6 << std::endl;

    (s += String(100, 'b')) += String(100, 'c');
    assert(s.length() == 500'203);
    assert(s.back() == 'c');
//std::cerr << 7 << std::endl;

    for (int i = 0; i < 150; ++i) {
        s.pop_back();
    }
    assert(s.back() == 'b');

    s = ss = s;
    (ss = s) = ss;
}

void test4() {
    std::istringstream iss("abcdefg\nABCDE");

    String s;
    assert(s.empty());

    iss >> s;
    assert(s.length() == 7);
    
    String ss;
    iss >> ss;
    assert(!ss.empty());
    assert(ss.length() == 5);
    assert((s + ss).length() == 12);

    auto sum = ss + s;
    sum.pop_back();
    assert(sum.length() == 11);

    sum[0] = 'x';
    assert(ss.length() == 5);
    assert(s.length() == 7);

    std::ostringstream oss;
    oss << ('!' + sum + '?');

    assert(oss.str() == "!xBCDEabcdef?");
}

void test5() {
    String s = "abcde";

    auto& ss = s += 'f';
    
    ss += 'g';

    std::ostringstream oss;
    oss << ss;

    assert(oss.str() == "abcdefg");
    assert(s.length() == 7);

    {
        auto& ss = (s = s);
        assert(ss[5] == 'f');
        auto sss = ss;
        ss.clear();
        assert(sss.length() == 7);
    }
    assert(ss.empty());
}

void test6() {
    const String s = "122333444455555";
                  //  0123456789ABCDE
    
    assert(s.find("44") == 6);
    assert(s.rfind("44") == 8);

    assert(s.find("12345") == (std::is_same_v<String, std::string> ? std::string::npos : s.length()));

    assert(s.substr(5, 4) == "3444");    
    
    decltype(s.substr(1,1)) ss = s;
    ss[0] = '0';
    assert(s[0] == '1');

    decltype(ss = ss) sss = ss;
    sss[0] = '2';
    assert(ss[0] == '2');

    decltype(ss += 'a') ssss = ss;
    ssss.front() = '3';
    assert(ss.front() == '3');
}


int main() {
    test1();
    std::cerr << "Test 1 (basic) passed." << std::endl;

    test2();
    std::cerr << "Test 2 (constness) passed." << std::endl;

    test3();
    std::cerr << "Test 3 passed." << std::endl;

    test4();
    std::cerr << "Test 4 passed." << std::endl;

    test5();
    std::cerr << "Test 5 passed." << std::endl;

    test6();
    std::cerr << "Test 6 passed." << std::endl;

    std::cout << 0;
}