#include <chrono>
#include <stdexcept>
#include <string>
#include <list>
#include <vector>
#include <deque>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <cassert>

#include "fastallocator.h"

//template<typename T, typename Alloc = std::allocator<T>>
//using List = std::list<T, Alloc>;

//template<typename T>
//using FastAllocator = std::allocator<T>;

template <typename Alloc = std::allocator<int>>
void BasicListTest() {
    List<int, Alloc> lst;

    assert(lst.size() == 0);

    lst.push_back(3);
    lst.push_back(4);
    lst.push_front(2);
    lst.push_back(5);
    lst.push_front(1);

    std::reverse(lst.begin(), lst.end());
    // now lst is 5 4 3 2 1
    
    assert(lst.size() == 5);

    std::string s;
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "54321");
    //std::cerr << "Tests log: check 1.1 ok, list contains 5 4 3 2 1" << std::endl;

    auto cit = lst.cbegin();
    std::advance(cit, 3);
    
    lst.insert(cit, 6);
    lst.insert(cit, 7);

    std::advance(cit, -3);
    lst.insert(cit, 8);
    lst.insert(cit, 9);
    // now lst is 5 4 8 9 3 6 7 2 1
    
    assert(lst.size() == 9);

    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "548936721");
    //std::cerr << "Tests log: check 1.2 ok, list contains 5 4 8 9 3 6 7 2 1" << std::endl;

    lst.erase(lst.cbegin());
    lst.erase(cit);

    lst.pop_front();
    lst.pop_back();

    const auto copy = lst;
    assert(lst.size() == 5);
    assert(copy.size() == 5);
    // now both lists are 8 9 6 7 2

    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "89672");
    //std::cerr << "Tests log: check 1.3 ok, list contains 8 9 6 7 2" << std::endl;
    
    auto rit = lst.rbegin();
    ++rit;
    lst.erase(rit.base());
    assert(lst.size() == 4);

    rit = lst.rbegin();
    *rit = 3;

    // now lst: 8 9 6 3, copy: 8 9 6 7 2
    s.clear();
    for (int x: lst) {
        s += std::to_string(x);
    }
    assert(s == "8963");
    
    assert(copy.size() == 5);

    s.clear();
    for (int x: copy) {
        s += std::to_string(x);
    }
    assert(s == "89672");

    //std::cerr << "Tests log: check 1.4 ok, list contains 8 9 6 3, another list is still 8 9 6 7 2" << std::endl;

    typename List<int, Alloc>::const_reverse_iterator crit = rit;
    crit = copy.rbegin();
    assert(*crit == 2);

    cit = crit.base();
    std::advance(cit, -2);
    assert(*cit == 7);

}

struct VerySpecialType {
    int x = 0;
    explicit VerySpecialType(int x): x(x) {}
};

struct NotDefaultConstructible {
    NotDefaultConstructible() = delete;
    NotDefaultConstructible(VerySpecialType x): x(x) {}
    VerySpecialType x;
};

template <class List>
int test_list(List&& l) {
    using namespace std::chrono;
    
    std::ostringstream oss;

    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < 1'000'000; ++i) {
        l.push_back(i);
    }
    auto it = l.begin();
    for (int i = 0; i < 1'000'000; ++i) {
        l.push_front(i);
    }
    oss << *it;

    auto it2 = std::prev(it);
    for (int i = 0; i < 2'000'000; ++i) {
        l.insert(it, i);
        if (i % 534'555 == 0) {
            oss << *it;
        }
    }
    oss << *it;

    for (int i = 0; i < 1'500'000; ++i) {
        l.pop_back();
        if (i % 342'985 == 0) oss << *l.rbegin();
    }
    oss << *l.rbegin();
    
    for (int i = 0; i < 1'000'000; ++i) {
        l.erase(it2++);
        if (i % 432'098 == 0) oss << *it2;
    }
    oss << *it2;

    for (int i = 0; i < 1'000'000; ++i) {
        l.pop_front();
    }
    oss << *l.begin();

    for (int i = 0; i < 1'000'000; ++i) {
        l.push_back(i);
    }
    oss << *l.rbegin();

    assert(oss.str() == "00000099999865701331402819710431628058149999904320988641969999991000000999999");

    auto finish = high_resolution_clock::now();
    return duration_cast<milliseconds>(finish - start).count();
}

void test_std_deque(std::deque<char, FastAllocator<char>>&& d) {
    d.push_back(1);
    assert(d.back() == 1);

    d.resize(2'500'000, 5);
    assert(d[1'000'000] == 5);

    d.pop_back();
    for (int i = 0; i < 2'000'000; ++i) {
        d.push_back(i % 100);
    }
    
    assert(d.size() == 4'499'999);
    //std::cerr << (int)d[4'000'000] << std::endl;
    assert(d[4'000'000] == 1);

    for (int i = 0; i < 4'000'000; ++i) {
        d.pop_front();
    }
    
    assert(d[400'000] == 1);
}

struct Accountant {
    // Some field of strange size
    char arr[40];

    static size_t ctor_calls;
    static size_t dtor_calls;

    Accountant() {
        ++ctor_calls;
    }
    Accountant(const Accountant&) {
        ++ctor_calls;
    }

    Accountant& operator=(const Accountant&) {
        ++ctor_calls;
        ++dtor_calls;
        return *this;
    }

    Accountant(Accountant&&) = delete;
    Accountant& operator=(Accountant&&) = delete;

    ~Accountant() {
        ++dtor_calls;
    }
};

size_t Accountant::ctor_calls = 0;
size_t Accountant::dtor_calls = 0;

template <typename Alloc = std::allocator<NotDefaultConstructible>>
void TestNotDefaultConstructible() {
    List<NotDefaultConstructible, Alloc> lst;
    assert(lst.size() == 0);
    lst.push_back(VerySpecialType(0));
    assert(lst.size() == 1);
    lst.pop_front();
    assert(lst.size() == 0);
}

template<typename Alloc = std::allocator<Accountant>>
void TestAccountant() {
    Accountant::ctor_calls = 0;
    Accountant::dtor_calls = 0;
    
    {
        List<Accountant, Alloc> lst(5);
        assert(lst.size() == 5);
        assert(Accountant::ctor_calls == 5);

        List<Accountant, Alloc> another = lst;
        assert(another.size() == 5);
        assert(Accountant::ctor_calls == 10);
        assert(Accountant::dtor_calls == 0);

        another.pop_back();
        another.pop_front();
        assert(Accountant::dtor_calls == 2);

        lst = another; // dtor_calls += 5, ctor_calls += 3
        assert(another.size() == 3);
        assert(lst.size() == 3);

        assert(Accountant::ctor_calls == 13);
        assert(Accountant::dtor_calls == 7);
    
    } // dtor_calls += 6
    
    assert(Accountant::ctor_calls == 13);
    assert(Accountant::dtor_calls == 13);
}

template <typename T, bool PropagateOnConstruct, bool PropagateOnAssign>
struct WhimsicalAllocator : public std::allocator<T> {
    std::shared_ptr<int> number;

    auto select_on_container_copy_construction() const {
        return PropagateOnConstruct 
            ? WhimsicalAllocator<T, PropagateOnConstruct, PropagateOnAssign>() 
            : *this;
    }

    struct propagate_on_container_copy_assignment
        : std::conditional_t<PropagateOnAssign, std::true_type, std::false_type> 
    {};

    template <typename U>
    struct rebind {
        using other = WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>;
    };

    WhimsicalAllocator(): number(std::make_shared<int>(counter)) {
        ++counter;
    }

    template <typename U>
    WhimsicalAllocator(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) : number(another.number)
    {}

    template <typename U>
    auto& operator=(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) {
        number = another.number;
        return *this;
    }
    
    template <typename U>
    bool operator==(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) const {
        return *number == *another.number;
    }

    template <typename U>
    bool operator!=(const WhimsicalAllocator<U, PropagateOnConstruct, PropagateOnAssign>& another) const {
        return *number != *another.number;
    }

    static size_t counter;
};

template <typename T, bool PropagateOnConstruct, bool PropagateOnAssign>
size_t WhimsicalAllocator<T, PropagateOnConstruct, PropagateOnAssign>::counter = 0;

void TestWhimsicalAllocator() {
    {
        List<int, WhimsicalAllocator<int, true, true>> lst;

        lst.push_back(1);
        lst.push_back(2);

        auto copy = lst;
        assert(copy.get_allocator() != lst.get_allocator());

        lst = copy;
        assert(copy.get_allocator() == lst.get_allocator());
    }
    {
        List<int, WhimsicalAllocator<int, false, false>> lst;

        lst.push_back(1);
        lst.push_back(2);

        auto copy = lst;
        assert(copy.get_allocator() == lst.get_allocator());

        lst = copy;
        assert(copy.get_allocator() == lst.get_allocator());
    }
    {
        List<int, WhimsicalAllocator<int, true, false>> lst;

        lst.push_back(1);
        lst.push_back(2);

        auto copy = lst;
        assert(copy.get_allocator() != lst.get_allocator());

        lst = copy;
        assert(copy.get_allocator() != lst.get_allocator());
    }
}


template <template<typename, typename> class Container>
void TestPerformance() {
    std::ostringstream oss_first;
    std::ostringstream oss_second;
    int first = test_list(Container<int, std::allocator<int>>());
    int second = test_list(Container<int, FastAllocator<int>>());
    std::ignore = first;
    std::ignore = second;
    first = 0, second = 0;
    double mean_first = 0.0;
    double mean_second = 0.0;

    for (int i = 0; i < 5; ++i) {
        first = test_list(Container<int, std::allocator<int>>());
        mean_first += first;
        oss_first << first << " ";

        second = test_list(Container<int, FastAllocator<int>>());
        mean_second += second;
        oss_second << second << " ";
    }

    mean_first /= 5;
    mean_second /= 5;

    std::cerr << "Tests log: Results with std::allocator: " << oss_first.str() 
            << " ms, results with FastAllocator: " << oss_second.str() << " ms " << std::endl;
    
    if (mean_first * 0.9 < mean_second) {
        throw std::runtime_error("FastAllocator expected to be at least 10\% faster than std::allocator, but mean time were "
                + std::to_string(mean_second) + " ms comparing with " + std::to_string(mean_first) + " :((( ...\n");
    }
}



int main() {

    static_assert(!std::is_assignable_v<decltype(*List<int>().cbegin()), int>);
    static_assert(!std::is_assignable_v<List<int>::iterator, List<int>::const_iterator>);
    //static_assert(!std::is_assignable_v<List<int>::reverse_iterator, List<int>::const_reverse_iterator>);

    static_assert(std::is_same_v<std::iterator_traits<List<int>::iterator>::iterator_category,
            std::bidirectional_iterator_tag>);

    //std::ofstream std::cerr("logs.txt");
    
    //std::cerr << "Tests log: compile-time checks passed." << std::endl;

    //std::cerr << "Tests log: starting test 1 (basic list operations)." << std::endl;

    BasicListTest<>();

    std::cerr << "Tests log: test 1 with std::allocator passed. Now will repeat with FastAllocator" << std::endl;

    BasicListTest<FastAllocator<int>>();

    std::cerr << "Tests log: test 1 with FastAllocator passed." << std::endl;
    //std::cerr << "Tests log: starting test 2 (counting ctor and dtor calls)." << std::endl;

    TestAccountant<>();
    
    std::cerr << "Tests log: test 2 with std::allocator passed. Now will repeat with FastAllocator" << std::endl;
    
    TestAccountant<FastAllocator<Accountant>>();

    std::cerr << "Tests log: test 2 with FastAllocator passed." << std::endl;
    //std::cerr << "Tests log: starting test 3 (not default constructible type)." << std::endl;

    TestNotDefaultConstructible<>();
    TestNotDefaultConstructible<FastAllocator<NotDefaultConstructible>>();
    
    std::cerr << "Tests log: test 3 passed. Now will try (test 4) to use FastAllocator with std::deque (just to check STL compatibility)" << std::endl;

    test_std_deque(std::deque<char, FastAllocator<char>>());

    std::cerr << "Tests log: test 4 passed. Now will check List's allocator awareness (propagating etc.)" << std::endl;
    
    TestWhimsicalAllocator();
    
    std::cerr << "Tests log: test 5 passed. Now starting the final one: performance test." << std::endl; 

    std::cerr << "Tests log: First, let's test performance of different allocators with std::list." << std::endl;

    TestPerformance<std::list>();

    std::cerr << "Tests log: Well, still looks good! Finally let's test with your List!" << std::endl;

    TestPerformance<List>();

    std::cerr << "Tests log: Tests passed, my sweetheart!" << std::endl;

    if (std::is_assignable_v<List<int>, std::list<int>> || std::is_assignable_v<std::list<int>, List<int>>) {
        std::cerr << "....but you must use your own List, not std::list!" << std::endl;
        throw std::runtime_error("Bad guy!");
    }

    std::cout << 0;
}

