#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

class String
{
private:
    size_t sz = 0;
    size_t capacity = 0;
    char *str = nullptr;

    void increaseCapacity(size_t x)
    {
        if (x == 0)
        {
            capacity = 1;
        }
        else
        {
            capacity = x * 2;
        }
        char *new_str = new char[capacity];
        std::copy(str, str + sz, new_str);
        delete[] str;
        str = new_str;
    }

    void decreaseCapacity()
    {
        char *new_str = new char[capacity / 2];
        std::copy(str, str + sz, new_str);
        delete[] str;
        str = new_str;
    }

public:
    String(const char *_str)
    {
        sz = strlen(_str);
        capacity = sz;
        str = new char[capacity];
        std::copy(_str, _str + sz, str);
    }

    String(size_t _sz, char c = '\0')
    {
        sz = _sz;
        capacity = sz;
        str = new char[capacity];
        memset(str, c, sz);
    }

    String(const char c)
    {
        sz = 1;
        capacity = sz;
        str = new char[sz];
        str[0] = c;
    }

    String(const String &a)
    {
        sz = a.sz;
        capacity = a.capacity;
        str = new char[capacity];
        std::copy(a.str, a.str + sz, str);
    }

    ~String()
    {
        delete[] str;
    }

    String() = default;

    String &operator=(String s)
    {
        swap(s);
        return *this;
    }

    void swap(String &s)
    {
        std::swap(s.sz, sz);
        std::swap(s.capacity, capacity);
        std::swap(s.str, str);
    }

    char &operator[](const size_t i)
    {
        return str[i];
    }

    const char &operator[](const size_t i) const
    {
        return str[i];
    }

    size_t length() const
    {
        return sz;
    }

    String substr(const size_t &start, const size_t &count) const
    {
        String ans(count);
        memcpy(ans.str, str + start, count);
        return ans;
    }

    void clear()
    {
        delete[] str;
        str = nullptr;
        capacity = 0;
        sz = 0;
    }

    bool empty() const
    {
        return (sz == 0);
    }

    size_t find(const String &t) const
    {
        bool flag = false;
        size_t find = sz;
        for (size_t i = 0; i < sz; ++i)
        {
            if (str[i] == t[0])
            {
                flag = true;
                find = i;
                for (size_t sub_i = 0; sub_i < t.sz && i < sz; ++sub_i)
                {
                    if (t[sub_i] != str[i])
                    {
                        flag = false;
                        break;
                    }
                    ++i;
                }
                if (i - find < t.sz)
                {
                    flag = false;
                }
                if (!flag)
                {
                    find = sz;
                }
            }
            if (flag && i - find == t.sz)
            {
                return find;
            }
        }
        return find;
    }

    size_t rfind(const String &t) const
    {
        bool flag = false;
        size_t find = sz;
        for (size_t i = 0; i < sz; ++i)
        {
            if (str[i] == t[0])
            {
                flag = true;
                size_t cur = i;
                size_t j = i;
                for (size_t sub_i = 0; sub_i < t.sz && j < sz; ++sub_i)
                {
                    if (t[sub_i] != str[j])
                    {
                        flag = false;
                        break;
                    }
                    ++j;
                }
                if (j - cur == t.sz && flag)
                {
                    find = cur;
                }
            }
        }
        return find;
    }

    const char &front() const
    {
        return str[0];
    }

    const char &back() const
    {
        return str[sz - 1];
    }

    char &front()
    {
        return str[0];
    }

    char &back()
    {
        return str[sz - 1];
    }

    void pop_back()
    {
        if (sz == capacity / 4 && sz != 0)
        {
            decreaseCapacity();
        }
        str[sz - 1] = '\0';
        --sz;
    }

    void push_back(const char &c)
    {
        (*this) += c;
    }

    String &operator+=(const String &s)
    {
        if (s.sz + sz >= capacity)
        {
            increaseCapacity(std::max(s.sz, sz));
        }
        for (size_t i = 0; i < s.sz; ++i)
        {
            str[i + sz] = s[i];
        }
        sz += s.sz;
        return *this;
    }

    String &operator+=(const char *s)
    {
        size_t len_s = strlen(s);
        if (sz + len_s >= capacity)
        {
            increaseCapacity(std::max(len_s, sz));
        }
        for (size_t i = 0; i < len_s; ++i)
        {
            str[i + sz] = s[i];
        }
        sz += len_s;
        return *this;
    }

    String &operator+=(const char c)
    {
        if (sz >= capacity)
        {
            increaseCapacity(capacity);
        }
        str[sz] = c;
        ++sz;
        return *this;
    }
};

String operator+(const String &a, const String &b)
{
    String res = a;
    res += b;
    return res;
}

bool operator==(const String &a, const String &b)
{
    if (a.length() != b.length())
    {
        return false;
    }
    else
    {
        size_t i = 0;
        for (; i < a.length(); ++i)
        {
            if (a[i] != b[i])
            {
                return false;
            }
        }
        return true;
    }
}

std::ostream &operator<<(std::ostream &out, const String &s)
{
    for (size_t i = 0; i < s.length(); ++i)
    {
        out << s[i];
    }
    return out;
}

std::istream &operator>>(std::istream &in, String &s)
{
    s.clear();
    char c;
    while (in.get(c) && isspace(c) == false && c != '\0')
    {
        s.push_back(c);
    }
    return in;
}
