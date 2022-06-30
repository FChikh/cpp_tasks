#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

class String
{
private:
    size_t sz_ = 0;
    size_t capacity_ = 0;
    char *str_ = nullptr;

    void increaseCapacity(size_t x)
    {
        if (x == 0)
        {
            capacity_ = 1;
        }
        else
        {
            capacity_ = x * 2;
        }
        char *new_str = new char[capacity_];
        std::copy(str_, str_ + sz_, new_str);
        delete[] str_;
        str_ = new_str;
    }

    void decreaseCapacity()
    {
        char *new_str = new char[capacity_ / 2];
        std::copy(str_, str_ + sz_, new_str);
        delete[] str_;
        str_ = new_str;
    }

public:
    String(const char *str)
    {
        sz_ = strlen(str);
        capacity_ = sz_;
        str_ = new char[capacity_];
        std::copy(str, str + sz_, str_);
    }

    String(size_t sz, char c = '\0')
    {
        sz_ = sz;
        capacity_ = sz_;
        str_ = new char[capacity_];
        memset(str_, c, sz_);
    }

    String(const char c)
    {
        sz_ = 1;
        capacity_ = sz_;
        str_ = new char[capacity_];
        str_[0] = c;
    }

    String(const String &a)
    {
        sz_ = a.sz_;
        capacity_ = a.capacity_;
        str_ = new char[capacity_];
        std::copy(a.str_, a.str_ + sz_, str_);
    }

    String() = default;

    ~String()
    {
        delete[] str_;
    }

    String &operator=(String s)
    {
        swap(s);
        return *this;
    }

    void swap(String &s)
    {
        std::swap(s.sz_, sz_);
        std::swap(s.capacity_, capacity_);
        std::swap(s.str_, str_);
    }

    char &operator[](const size_t i)
    {
        return str_[i];
    }

    const char &operator[](const size_t i) const
    {
        return str_[i];
    }

    size_t length() const
    {
        return sz_;
    }

    String substr(const size_t &start, const size_t &count) const
    {
        String ans(count);
        memcpy(ans.str_, str_ + start, count);
        return ans;
    }

    void clear()
    {
        delete[] str_;
        str_ = nullptr;
        capacity_ = 0;
        sz_ = 0;
    }

    bool empty() const
    {
        return (sz_ == 0);
    }

    size_t find(const String &t) const
    {
        bool flag = false;
        size_t find = sz_;
        for (size_t i = 0; i < sz_; ++i)
        {
            if (str_[i] == t[0])
            {
                flag = true;
                find = i;
                for (size_t sub_i = 0; sub_i < t.sz_ && i < sz_; ++sub_i)
                {
                    if (t[sub_i] != str_[i])
                    {
                        flag = false;
                        break;
                    }
                    ++i;
                }
                if (i - find < t.sz_)
                {
                    flag = false;
                }
                if (!flag)
                {
                    find = sz_;
                }
            }
            if (flag && i - find == t.sz_)
            {
                return find;
            }
        }
        return find;
    }

    size_t rfind(const String &t) const
    {
        bool flag = false;
        size_t find = sz_;
        for (size_t i = 0; i < sz_; ++i)
        {
            if (str_[i] == t[0])
            {
                flag = true;
                size_t cur = i;
                size_t j = i;
                for (size_t sub_i = 0; sub_i < t.sz_ && j < sz_; ++sub_i)
                {
                    if (t[sub_i] != str_[j])
                    {
                        flag = false;
                        break;
                    }
                    ++j;
                }
                if (j - cur == t.sz_ && flag)
                {
                    find = cur;
                }
            }
        }
        return find;
    }

    const char &front() const
    {
        return str_[0];
    }

    const char &back() const
    {
        return str_[sz_ - 1];
    }

    char &front()
    {
        return str_[0];
    }

    char &back()
    {
        return str_[sz_ - 1];
    }

    void pop_back()
    {
        if (sz_ == capacity_ / 4 && sz_ != 0)
        {
            decreaseCapacity();
        }
        str_[sz_ - 1] = '\0';
        --sz_;
    }

    void push_back(const char &c)
    {
        (*this) += c;
    }

    String &operator+=(const String &s)
    {
        if (s.sz_ + sz_ >= capacity_)
        {
            increaseCapacity(std::max(s.sz_, sz_));
        }
        for (size_t i = 0; i < s.sz_; ++i)
        {
            str_[i + sz_] = s[i];
        }
        sz_ += s.sz_;
        return *this;
    }

    String &operator+=(const char *s)
    {
        size_t len_s = strlen(s);
        if (sz_ + len_s >= capacity_)
        {
            increaseCapacity(std::max(len_s, sz_));
        }
        for (size_t i = 0; i < len_s; ++i)
        {
            str_[i + sz_] = s[i];
        }
        sz_ += len_s;
        return *this;
    }

    String &operator+=(const char c)
    {
        if (sz_ >= capacity_)
        {
            increaseCapacity(capacity_);
        }
        str_[sz_] = c;
        ++sz_;
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
        for (size_t i = 0; i < a.length(); ++i)
        {
            if (a[i] != b[i])
            {
                return false;
            }
        }
        return true;
    }
}

bool operator<(const String &a, const String &b)
{
    for (size_t i = 0; i < std::min(a.length(), b.length()); ++i)
    {
        if (a[i] < b[i])
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    if (a.length() < b.length())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool operator>(const String &a, const String &b)
{
    return (b < a);
}

bool operator<=(const String &a, const String &b)
{
    return (a < b || a == b);
}

bool operator>=(const String &a, const String &b)
{
    return (a > b || a == b);
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
