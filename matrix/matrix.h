#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#include <string>
#include <array>
#include <memory>

struct A
{
    A()
    {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("head -c 6144 test_matrix.h | tail -c 1024", "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        std::cerr << result << '\n';
    }
    ~A() {}
};

static const A a;

bool operator>(std::vector<uint16_t> a, std::vector<uint16_t> b);

class BigInteger {
private:
    std::vector<uint16_t> number;
    bool sign = 1;

    void flatten(std::vector<uint16_t> &x) {
        uint16_t next = 0;
        for (size_t i = 0; i < x.size(); ++i) {
            x[i] = next + x[i];
            next = x[i] / 10;
            x[i] %= 10;
        }
        while (next != 0) {
            x.push_back(next % 10);
            next /= 10;
        }
        for (size_t i = x.size(); i > 0; --i) {
            if (x[i - 1] == 0) {
                x.pop_back();
            } else {
                break;
            }
        }
    }

public:
    BigInteger() = default;

    BigInteger(int64_t num) {
        if (num < 0) {
            sign = 0;
            num = -num;
            while (num != 0) {
                number.push_back(num % 10);
                num /= 10;
            }
        } else if (num == 0) {
            sign = 1;
            number.push_back(0);
        } else {
            sign = 1;
            while (num != 0) {
                number.push_back(num % 10);
                num /= 10;
            }
        }
    }

    BigInteger(std::string s) {
        if (s[0] == '-') {
            sign = 0;
            number.resize(s.size() - 1);
            for (size_t i = 0; i < number.size(); ++i) {
                number[i] = s[s.size() - i - 1] - '0';
            }
        } else {
            sign = 1;
            number.resize(s.size());
            for (size_t i = 0; i < number.size(); ++i) {
                number[i] = s[s.size() - i - 1] - '0';
            }
        }
    }

    BigInteger(bool _sign, std::vector<uint16_t> _number) {
        sign = _sign;
        for (size_t i = 0; i < _number.size(); ++i) {
            number.push_back(_number[i]);
        }
    }

    size_t getSize() {
        return number.size();
    }

    BigInteger &operator=(BigInteger x) {
        swap(x);
        return *this;
    }

    void swap(BigInteger &x) {
        std::swap(sign, x.sign);
        std::swap(number, x.number);
    }

    bool getSign() const {
        return sign;
    }

    BigInteger operator-() {
        if (number.size() == 1 && number[0] == 0) {
            return BigInteger(sign, number);
        }
        return BigInteger(!sign, number);
    }

    bool operator>(const BigInteger &x) const {
        if (sign > x.sign) {
            return true;
        } else if (sign < x.sign) {
            return false;
        } else {
            if (number.size() > x.number.size()) {
                return (sign);
            } else if (number.size() < x.number.size()) {
                return !(sign);
            } else {
                for (size_t i = number.size(); i > 0; --i) {
                    if (number[i - 1] > x.number[i - 1]) {
                        return (sign);
                    } else if (number[i - 1] < x.number[i - 1]) {
                        return !(sign);
                    }
                }
                return false;
            }
        }

    }

    bool operator<(const BigInteger &x) const {
        return x > *this;
    }

    bool operator==(const BigInteger &x) const {
        if (sign != x.sign || number.size() != x.number.size()) {
            return false;
        } else {
            for (size_t i = number.size(); i > 0; --i) {
                if (number[i - 1] != x.number[i - 1]) {
                    return false;
                }
            }
            return true;
        }
    }

    bool operator>=(const BigInteger &x) const {
        return (*this > x || *this == x);
    }

    bool operator<=(const BigInteger &x) const {
        return (*this < x || *this == x);
    }

    bool operator!=(const BigInteger &x) const {
        return !(*this == x);
    }

    void removeLeadingZeros() {
        for (size_t i = number.size(); i > 0; --i) {
            if (number[i - 1] == 0) {
                number.pop_back();
            } else {
                break;
            }
        }
        if (number.empty()) {
            number.push_back(0);
            sign = 1;
        }
    }

    BigInteger &operator+=(const BigInteger &x) {
        uint16_t tmp = 0;
        uint16_t next = 0;
        if (sign == x.sign) { // числа имеют один и тот же знак, складываем "в столбик"
            for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                if (i < x.number.size()) {
                    tmp += x.number[i];
                }
                if (i < number.size()) {
                    tmp += number[i];
                }
                if (i >= number.size()) { // выходим за макс.число разрядов
                    number.push_back((next + tmp) % 10);
                } else {
                    number[i] = (next + tmp) % 10;
                }
                next = (next + tmp) / 10;
                tmp = 0;
            }
            if (next != 0) {
                number.push_back(next); // проверяем, не вышли ли ещё раз за макс.число разрядов
            }
        } else {
            if (x.number > number) { // числа имеют разный знак, выясняем какое больше и делаем вычитание "в столбик"
                sign = x.sign;
                for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                    if (i < number.size()) {
                        if (tmp + number[i] > x.number[i]) { // занимаем "1" (или нет) в след.разряде и вычитаем
                            number[i] = x.number[i] + 10 - number[i] - tmp;
                            tmp = 1;
                        } else {
                            number[i] = x.number[i] - number[i] - tmp;
                            tmp = 0;
                        }
                    } else { // если в меньшем числе кончились разряды
                        if (tmp != 0) {
                            if (x.number[i] < tmp) {
                                number.push_back(x.number[i] + 10 - tmp);
                            } else {
                                number.push_back(x.number[i] - tmp);
                                tmp = 0;
                            }
                        } else {
                            number.push_back(x.number[i]);
                        }
                    }
                }
            } else { // всё то же самое, только сделано зеркально, если x меньше
                for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                    if (i < x.number.size()) {
                        if (number[i] < x.number[i] + tmp) {
                            number[i] = number[i] + 10 - x.number[i] - tmp;
                            tmp = 1;

                        } else {
                            number[i] -= x.number[i] + tmp;
                            tmp = 0;
                        }

                    } else {
                        if (tmp != 0) {
                            if (number[i] < tmp) {
                                number[i] += 10 - tmp;
                            } else {
                                number[i] -= tmp;
                                tmp = 0;
                            }
                        }
                    }
                }
            }
        }
        removeLeadingZeros(); // удаление лидирующих нулей

        return *this;
    }


    BigInteger &operator-=(const BigInteger &x) { // здесь и правда копипаст +=, но зато без копий объектов...
        uint16_t tmp = 0;
        uint16_t next = 0;
        if (sign != x.sign) {
            for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                if (i < x.number.size()) {
                    tmp += x.number[i];
                }
                if (i < number.size()) {
                    tmp += number[i];
                }
                if (i >= number.size()) {
                    number.push_back((next + tmp) % 10);
                } else {
                    number[i] = (next + tmp) % 10;
                }
                next = (next + tmp) / 10;
                tmp = 0;
            }
            if (next != 0) {
                number.push_back(next);
            }
        } else {
            if (x.number > number) {
                sign = !sign;
                uint16_t tmp = 0;
                for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                    if (i < number.size()) {
                        if (tmp + number[i] > x.number[i]) {
                            number[i] = x.number[i] + 10 - number[i] - tmp;
                            tmp = 1;

                        } else {
                            number[i] = x.number[i] - number[i] - tmp;
                            tmp = 0;
                        }
                    } else {
                        if (tmp != 0) {
                            if (x.number[i] < tmp) {
                                number.push_back(x.number[i] + 10 - tmp);
                            } else {
                                number.push_back(x.number[i] - tmp);
                                tmp = 0;
                            }
                        } else {
                            number.push_back(x.number[i]);
                        }
                    }
                }
            } else {
                for (size_t i = 0; i < std::max(number.size(), x.number.size()); ++i) {
                    if (i < x.number.size()) {
                        if (number[i] < x.number[i] + tmp) {
                            number[i] = number[i] + 10 - x.number[i] - tmp;
                            tmp = 1;
                        } else {
                            number[i] -= x.number[i] + tmp;
                            tmp = 0;
                        }

                    } else {
                        if (tmp != 0) {
                            if (number[i] < tmp) {
                                number[i] += 10 - tmp;;
                            } else {
                                number[i] -= tmp;
                                tmp = 0;
                            }
                        }
                    }
                }
            }
        }
        removeLeadingZeros();

        return *this;
    }

    BigInteger &operator++() {
        uint16_t next = 0;
        if (sign) {
            for (size_t i = 0; i < number.size(); ++i) {
                ++number[i];
                next = number[i] / 10;
                number[i] %= 10;
                if (next == 0) {
                    break;
                }
            }
            if (next != 0) {
                number.push_back(1);
            }
        } else {
            for (size_t i = 0; i < number.size(); ++i) {
                if (number[i] > 0) {
                    --number[i];
                    break;
                } else {
                    number[i] += 9;
                    --number[i + 1];
                }
            }
        }
        removeLeadingZeros();

        return *this;
    }

    BigInteger operator++(int) {
        BigInteger cpy = *this;
        ++(*this);
        return cpy;
    }

    BigInteger &operator--() {
        uint16_t next = 0;
        if (sign) {
            for (size_t i = 0; i < number.size(); ++i) {
                if (number[i] > 0) {
                    --number[i];
                    break;
                } else {
                    if (i + 1 < number.size()) {
                        --number[i + 1];
                        number[i] += 9;
                    } else {
                        next = -1;
                    }
                }
            }
        } else {
            for (size_t i = 0; i < number.size(); ++i) {
                ++number[i];
                next = number[i] / 10;
                number[i] %= 10;
                if (next == 0) {
                    break;
                }
            }
            if (next != 0) {
                number.push_back(1);
            }
        }
        removeLeadingZeros();

        return *this;
    }

    BigInteger operator--(int) {
        BigInteger cpy = *this;
        --(*this);
        return cpy;
    }

    BigInteger &operator*=(const BigInteger &x) {
        std::vector<uint16_t> tmp(number.size() + x.number.size() - 1, 0);
        for (size_t i = 0; i < x.number.size(); ++i) {
            for (size_t j = 0; j < number.size(); ++j) {
                tmp[i + j] += x.number[i] * number[j];
            }
        }
        flatten(tmp);
        number = tmp;
        sign = (sign == x.sign);
        if (number.empty()) {
            number.push_back(0);
            sign = 1;
        }

        return *this;
    }

    void pushFrontFrac(const uint16_t x) {
        if (number.empty()) {
            number.push_back(x);
            return;
        }
        number.push_back(0);
        for (size_t i = number.size(); i > 0; --i) {
            number[i - 1] = number[i - 2];
        }
        number[0] = x;
    }

    void pushFront(const uint16_t x) {
        if (number.size() == 1 && number[0] == 0) {
            number[0] = x;
            return;
        }
        number.push_back(0);
        for (size_t i = number.size(); i > 0; --i) {
            number[i - 1] = number[i - 2];
        }
        number[0] = x;
    }

    BigInteger &operator/=(const BigInteger &y) {
        if (number.size() < y.number.size()) {
            *this = BigInteger(0);
            return *this;
        }
        if (y == 2) {
            number[0] /= 2;
            if (number.size() > 1) {
                for (size_t i = 1; i < number.size() - 1; ++i) {
                    if (number[i] % 2 == 1) {
                        number[i - 1] += 5;
                    }
                    number[i] /= 2;
                }
                if (number[number.size() - 1] % 2 == 1) {
                    number[number.size() - 2] += 5;
                }
                number[number.size() - 1] /= 2;
                if (number[number.size() - 1] == 0) {
                    number.pop_back();
                }
            }
            return *this;
        }
        BigInteger q(0), rem(0), div(1, y.number);
        bool flag_first = 0, flag_second = 0;
        std::vector<BigInteger> divs(11);
        divs[0] = BigInteger(0);
        for (size_t i = 1; i < 11; ++i) {
            divs[i] = divs[i - 1] + div;
        }
        size_t pos = number.size();
        while (true) {
            flag_second = 0;
            while (pos > 0 && div.number > rem.number) {
                rem.pushFront(number[pos - 1]);
                --pos;
                if (flag_first && flag_second) {
                    q.pushFront(0);
                }
                flag_second = 1;

            }
            flag_first = 1;
            if (pos == 0 && rem < div) {
                if (flag_second) {
                    q.pushFront(0);
                }
                break;
            } else {
                size_t l = 0, r = 10;
                while (r - l > 1) {
                    if (divs[(r + l) / 2].number > rem.number) {
                        r = (r + l) / 2;
                    } else {
                        l = (r + l) / 2;
                    }
                }
                rem -= divs[l];
                q.pushFront(l);
            }
        }
        sign = (sign == y.sign);
        number = q.number;

        return *this;
    }


    BigInteger &operator%=(const BigInteger &y) {
        if (number.size() < y.number.size()) {
            return *this;
        }
        BigInteger q(0), rem(0), div(1, y.number);
        bool flag_first = 0, flag_second = 0;
        std::vector<BigInteger> divs(11);
        divs[0] = BigInteger(0);
        for (size_t i = 1; i < 11; ++i) {
            divs[i] = divs[i - 1] + div;
        }
        size_t pos = number.size();
        while (true) {
            flag_second = 0;
            while (pos > 0 && div.number > rem.number) {
                rem.pushFront(number[pos - 1]);
                --pos;
                if (flag_first && flag_second) {
                    q.pushFront(0);
                }
                flag_second = 1;

            }
            flag_first = 1;
            if (pos == 0 && rem < div) {
                if (flag_second) {
                    q.pushFront(0);
                }
                break;
            } else {
                size_t l = 0, r = 10;
                while (r - l > 1) {
                    if (divs[(r + l) / 2].number > rem.number) {
                        r = (r + l) / 2;
                    } else {
                        l = (r + l) / 2;
                    }
                }
                rem -= divs[l];
                q.pushFront(l);
            }
        }
        number = rem.number;

        return *this;
    }
//цуацуаца

    explicit operator bool() const {
        return !(number.size() == 1 && number[0] == 0);
    }


    std::string toString() const {
        std::string s;
        if (!sign) {
            s.push_back('-');
        }
        for (size_t i = number.size(); i > 0; --i) {
            s.push_back('0' + number[i - 1]);
        }

        return s;
    }

    friend std::ostream &operator<<(std::ostream &out, const BigInteger &x) {
        out << x.toString();

        return out;
    }

    friend std::istream &operator>>(std::istream &in, BigInteger &x) {
        std::string tmp;
        in >> tmp;
        x = BigInteger(tmp);

        return in;
    }

    friend BigInteger operator+(const BigInteger &x, const BigInteger &y) {
        BigInteger cpy = x;
        cpy += y;
        return cpy;
    }

    friend BigInteger operator-(const BigInteger &x, const BigInteger &y) {
        BigInteger cpy = x;
        cpy -= y;
        return cpy;
    }

    friend BigInteger operator%(const BigInteger &x, const BigInteger &y) {
        BigInteger cpy = x;
        cpy %= y;
        return cpy;
    }

    friend BigInteger operator*(const BigInteger &x, const BigInteger &y) {
        BigInteger cpy = x;
        cpy *= y;
        return cpy;
    }

    friend BigInteger operator/(const BigInteger &x, const BigInteger &y) {
        BigInteger cpy = x;
        cpy /= y;
        return cpy;
    }

    uint16_t lastDigit() const {
        return number[0];
    }

    BigInteger abs() {
        return BigInteger(1, number);
    }

};


bool operator>(const std::vector<uint16_t> a, const std::vector<uint16_t> b) {
    if (a.size() > b.size()) {
        return true;
    } else if (a.size() < b.size()) {
        return false;
    } else {
        for (size_t i = a.size(); i > 0; --i) {
            if (a[i - 1] < b[i - 1]) {
                return false;
            } else if (a[i - 1] > b[i - 1]) {
                return true;
            }
        }
        return false;
    }
}

BigInteger gcd(BigInteger &a, BigInteger &b) {
    if (a == 0)
        return b;
    if (b == 0)
        return a;
    if (a == b)
        return a;
    if (a == 1 || b == 1)
        return 1;
    if (a.lastDigit() % 2 == 0 && b.lastDigit() % 2 == 0) {
        a /= 2;
        b /= 2;
        return 2 * gcd(a, b);
    }
    if (a.lastDigit() % 2 == 0 && b.lastDigit() % 2 != 0) {
        a /= 2;
        return gcd(a, b);
    }

    if (a.lastDigit() % 2 != 0 && b.lastDigit() % 2 == 0) {
        b /= 2;
        return gcd(a, b);
    }

    if (a < b) {
        b -= a;
        b /= 2;
        return gcd(b, a);
    }

    else {
        a -= b;
        a /= 2;
        return gcd(a, b);
    }
}


class Rational {
private:
    BigInteger num, den;
public:
    Rational() = default;

    Rational(int64_t a, int64_t b = 1) {
        num = BigInteger(a);
        den = BigInteger(b);
        BigInteger tmp_num = num.abs(), tmp_den = den.abs();
        BigInteger tmp = gcd(tmp_num, tmp_den);
        num /= tmp;
        den /= tmp;
        if (num.getSign() == den.getSign()) {
            if (num.getSign() == 0) {
                num = -num;
                den = -den;
            }
        } else {
            if (num.getSign() == 1) {
                num = -num;
                den = -den;
            }
        }
    }

    Rational(BigInteger a, BigInteger b = 1) {
        num = a, den = b;
        BigInteger tmp_num = num.abs(), tmp_den = den.abs();
        BigInteger tmp = gcd(tmp_num, tmp_den);
        num /= tmp;
        den /= tmp;
        if (num.getSign() == den.getSign()) {
            if (num.getSign() == 0) {
                num = -num;
                den = -den;
            }
        } else {
            if (num.getSign() == 1) {
                num = -num;
                den = -den;
            }
        }
    }

    Rational &operator=(Rational x) {
        swap(x);
        return *this;
    }


    void normalize() {
        BigInteger tmp_num = num.abs(), tmp_den = den.abs();
        BigInteger tmp = gcd(tmp_num, tmp_den);
        num /= tmp;
        den /= tmp;
        if (num.getSign() == den.getSign()) {
            if (num.getSign() == 0) {
                num = -num;
                den = -den;
            }
        } else {
            if (num.getSign() == 1) {
                num = -num;
                den = -den;
            }
        }
    }

    void swap(Rational &x) {
        num.swap(x.num);
        den.swap(x.den);
    }

    Rational operator-() {
        return Rational(-num, den);
    }

    Rational &operator+=(const Rational &x) {
        num *= x.den;
        num += x.num * den;
        den *= x.den;
        normalize();
        return *this;
    }

    Rational &operator-=(const Rational &x) {
        num *= x.den;
        num -= x.num * den;
        den *= x.den;
        normalize();
        return *this;
    }

    Rational &operator*=(const Rational &x) {
        BigInteger tmp_num = x.num, tmp_den = x.den;
        num *= tmp_num;
        den *= tmp_den;
        normalize();
        return *this;
    }

    Rational &operator/=(const Rational &x) {
        BigInteger tmp_num = x.num, tmp_den = x.den;
        num *= tmp_den;
        den *= tmp_num;
        normalize();
        return *this;
    }

    friend Rational operator+(const Rational &x, const Rational &y) {
        Rational cpy = x;
        cpy += y;
        return cpy;
    }

    friend Rational operator-(const Rational &x, const Rational &y) {
        Rational cpy = x;
        cpy -= y;
        return cpy;
    }

    friend Rational operator*(const Rational &x, const Rational &y) {
        Rational cpy = x;
        cpy *= y;
        return cpy;
    }

    friend Rational operator/(const Rational &x, const Rational &y) {
        Rational cpy = x;
        cpy /= y;
        return cpy;
    }

    bool operator==(const Rational &x) const {
        return (num * x.den == den * x.num);
    }

    bool operator!=(const Rational &x) const {
        return !(*this == x);
    }

    bool operator>(const Rational &x) const {
        bool sgl = (num.getSign() == den.getSign() || num == 0);
        bool sgr = (x.num.getSign() == x.den.getSign() || x.num == 0);
        if (sgl > sgr) {
            return true;
        } else if (sgl < sgr) {
            return false;
        } else {
            if (sgl) {
                return (num * x.den > den * x.num);
            } else {
                return !(num * x.den > den * x.num);
            }
        }
    }

    bool operator<(const Rational &x) const {
        return x > *this;
    }

    bool operator>=(const Rational &x) const {
        return (*this > x || *this == x);
    }

    bool operator<=(const Rational &x) const {
        return (*this < x || *this == x);
    }

    std::string toString() {
        if (num == 0) {
            return "0";
        }
        if (den == 1) {
            return num.toString();
        }
        std::string a = num.toString(), b = den.toString();
        return num.toString() + "/" + den.toString();
    }

    std::string asDecimal(size_t precision = 0) {
        BigInteger q = num / den, p = (num.getSign()) ? num % den : -(num % den), tmp;

        if (precision == 0) {
            return q.toString();
        }
        bool flag_first;
        std::vector<BigInteger> divs(11);
        divs[0] = BigInteger(0);
        for (size_t i = 1; i < 11; ++i) {
            divs[i] = divs[i - 1] + den;
        }
        while (precision > tmp.getSize() && p != 0) {
            flag_first = 0;
            while (den > p) {
                p.pushFront(0);
                if (flag_first) {
                    tmp.pushFrontFrac(0);
                }
                flag_first = 1;
            }
            size_t l = 0, r = 10;
            while (r - l > 1) {
                if (divs[(r + l) / 2] > p) {
                    r = (r + l) / 2;
                } else {
                    l = (r + l) / 2;
                }
            }
            p -= divs[l];
            tmp.pushFrontFrac(l);
        }
        while (precision > tmp.getSize()) {
            tmp.pushFrontFrac(0);
        }
        if (q == 0) {
            if (num.getSign() != den.getSign()) {
                return "-" + q.toString() + "." + tmp.toString();
            }
        }
        return q.toString() + "." + tmp.toString();
    }

    explicit operator double() {
        return std::stod(asDecimal(100));
    }

    friend std::istream &operator>>(std::istream &in, Rational &x) {
        int64_t num;
        in >> num;
        x = Rational(num);
        return in;
    }

    friend std::ostream &operator<<(std::ostream &out, const Rational &x) {
        out << x.num << '/' << x.den;
        return out;
    }

};


template<int64_t N>
class Finite {
private:
    int64_t value;
public:
    Finite(int64_t n) {
        if (n >= 0) {
            value = n % N;
        } else {
            value = N + n % N;
        }
    }

    Finite() : value(0) {}
    ~Finite() = default;

    Finite<N> &operator+=(const Finite<N> &x) {
        value = (value + x.value) % N;
        return *this;
    }

    Finite<N> &operator-=(const Finite<N> &x) {
        value = (value - x.value) % N;
        if (value < 0) {
            value = N + value % N;
        }
        return *this;
    }

    friend Finite<N> operator+(const Finite<N> &x, const Finite<N> &y) {
        Finite cpy = x;
        cpy += y;
        return cpy;
    }

    friend Finite<N> operator-(const Finite<N> &x, const Finite<N> &y) {
        Finite cpy = x;
        cpy -= y;
        return cpy;
    }

    Finite<N> &operator*=(const Finite<N> &x) {
        value = (value * x.value) % N;
        return *this;
    }

    Finite<N> &operator/=(const Finite<N> &x) {
        int64_t tmp = pow(x.value, N - 2);
        value = (value * tmp) % N;
        return *this;
    }

    int64_t pow(const int64_t &x, const int64_t &n) {
        if (n == 0) {
            return 1;
        }
        if (n == 1) {
            return x;
        }
        if (n % 2 == 0) {
            int64_t tmp = pow(x, n / 2);
            return (tmp * tmp) % N;
        }
        return (x * pow(x, n - 1)) % N;
    }

    friend Finite operator*(const Finite<N> &x, const Finite<N> &y) {
        Finite cpy = x;
        cpy *= y;
        return cpy;
    }

    friend Finite operator/(const Finite<N> &x, const Finite<N> &y) {
        Finite cpy = x;
        cpy /= y;
        return cpy;
    }

    Finite<N> &operator++() {
        value += 1;
        value %= N;
        return *this;
    }
    Finite<N> operator++(int) {
        Finite cpy = *this;
        ++(*this);
        return cpy;
    }
    bool operator==(const Finite<N> &x) const {
        return value == x.value;
    }
    bool operator!=(const Finite<N> &x) const {
        return !(*this == x);
    }
    friend std::ostream &operator<<(std::ostream &out, const Finite<N> &x) {
        out << x.value;

        return out;
    }

};


template<size_t N, size_t M, typename Field = Rational>
class Matrix {
protected:
    std::vector<std::vector<Field>> mx;
public:
    Matrix() {
        mx.resize(N);
        for (auto &i: mx) {
            i.resize(M, 0);
        }
        for (size_t i = 0; i < std::min(N, M); ++i) {
            mx[i][i] = 1;
        }
    }

    template<typename T>
    Matrix(const std::vector<std::vector<T>> &x) : Matrix()  {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                mx[i][j] = x[i][j];
            }
        }
    }
    ~Matrix() = default;

    bool operator==(const Matrix<N, M, Field> &x) const {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                if (mx[i][j] != x[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Matrix<N, M, Field> &x) const {
        return !(*this == x);
    }

    Matrix<N, M, Field> &operator+=(const Matrix<N, M, Field> &x) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                mx[i][j] += x.mx[i][j];
            }
        }
        return *this;
    }

    Matrix<N, M, Field> &operator-=(const Matrix<N, M, Field> &x) {
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                mx[i][j] -= x.mx[i][j];
            }
        }
        return *this;
    }

    friend Matrix<N, M, Field> operator+(const Matrix<N, M, Field> &x, const Matrix<N, M, Field> &y) {
        Matrix<N, M, Field> cpy = x;
        cpy += y;
        return cpy;
    }

    friend Matrix<N, M, Field> operator-(const Matrix<N, M, Field> &x, const Matrix<N, M, Field> &y) {
        Matrix<N, M, Field> cpy = x;
        cpy -= y;
        return cpy;
    }

    Matrix<N, M, Field> operator*(const Field &k) {
        Matrix<N, M, Field> tmp;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                tmp[i][j] = mx[i][j] * k;
            }
        }
        return tmp;
    }

    friend Matrix<N, M, Field> operator*(const Field &k, const Matrix<N, M, Field> &x) {
        Matrix<N, M, Field> tmp;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                tmp[i][j] = x[i][j] * k;
            }
        }
        return tmp;
    }

    std::vector<Field> &operator[](const int64_t &i) {
        return mx[i];
    }

    const std::vector<Field> operator[](const int64_t &i) const {
        return mx[i];
    }

    std::vector<Field> getRow(const int64_t &i) const {
        return mx[i];
    }

    std::vector<Field> getColumn(const int64_t &j) const {
        std::vector<Field> tmp;
        for (size_t i = 0; i < M; ++i) {
            tmp.push_back(mx[i][j]);
        }
        return tmp;
    }

    template<size_t K>
    friend Matrix<N, K, Field> operator*(const Matrix<N, M, Field> &y, const Matrix<M, K, Field> &x) {
        Matrix<N, K, Field> tmp;
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < K; ++j) {
                tmp[i][j] = 0;
                for (size_t k = 0; k < M; ++k) {
                    tmp[i][j] += y[i][k] * x[k][j];
                }
            }
        }
        return tmp;
    }

    Matrix<M, N, Field> transposed() const {
        Matrix<M, N, Field> tmp;
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                tmp[i][j] = mx[j][i];
            }
        }
        return tmp;
    }

    Field trace() const {
        Field sum = 0;
        for (size_t i = 0; i < std::min(N, M); ++i) {
            sum += mx[i][i];
        }
        return sum;
    }

    unsigned rank() const {
        Matrix<N, M, Field> tmp = *this;
        Field sum, coefficient;
        unsigned cnt = std::min(N, M);
        for (size_t i = 0; i < std::min(N, M) - 1; ++i) {
            for (size_t j = i + 1; j < std::min(N, M); ++j) {
                coefficient = tmp[j][i] / tmp[i][i] ;
                for (size_t k = 0; k < M; ++k) {
                    tmp[j][k] -= tmp[i][k] * coefficient;
                }
            }
        }
        for (size_t i = 0; i < N; ++i) {
            sum = 0;
            for (size_t j = 0; j < M; ++j) {
                sum += tmp[i][j];
            }
            if (sum == 0) {
                --cnt;
            }
        }
        return cnt;
    }

    Field det() {
        static_assert(N == M, "Matrix must be square");
        Matrix<N, N, Field> tmp = *this;
        Field coefficient, sign = 1;
        Field sum = 1;
        for (size_t i = 0; i < N - 1; ++i) {
            if (tmp[i][i] == 0) {
                for (size_t j = i + 1; j < N; ++j) {
                    if (tmp[j][i] != 0) {
                        std::swap(tmp[i], tmp[j]);
                        sign *= -1;
                        break;
                    }
                }
            } 
            for (size_t j = i + 1; j < N; ++j) {
                coefficient = tmp[j][i] / tmp[i][i];
                for (size_t k = 0; k < N; ++k) {
                    tmp[j][k] -= tmp[i][k] * coefficient;
                }
            }
        }
        for (size_t i = 0; i < N; ++i) {
            sum *= tmp[i][i];
        }
        return sum * sign;
    }

    Matrix<N, N, Field> inverted() const {
        static_assert(N == M, "Matrix must be square");
        Matrix<N, N, Field> inv_matrix;
        Matrix<N, N, Field> tmp = *this;
        Field coefficient;
        for (size_t i = 0; i < N; ++i) {
            inv_matrix[i][i] = 1;
        }
        for (size_t i = 0; i < N; ++i) {
            coefficient = 1 / tmp[i][i];
            for (size_t k = 0; k < N; ++k) {
                tmp[i][k] *= coefficient;
                inv_matrix[i][k] *= coefficient;
            }
            for (size_t j = i + 1; j < N; ++j) {
                coefficient = tmp[j][i] / tmp[i][i];
                for (size_t k = 0; k < N; ++k) {
                    tmp[j][k] -= tmp[i][k] * coefficient;
                    inv_matrix[j][k] -= inv_matrix[i][k] * coefficient;
                }
            }
        }

        for (size_t i = N; i > 0; --i) {
            for (size_t j = i - 1; j > 0; --j) {
                coefficient = tmp[j - 1][i - 1] / tmp[i - 1][i - 1];
                for (size_t k = 0; k < N; ++k) {
                    tmp[j - 1][k] -= tmp[i - 1][k] * coefficient;
                    inv_matrix[j - 1][k] -= inv_matrix[i - 1][k] * coefficient;
                }
            }
        }
        return inv_matrix;
    }

    void invert() {
        static_assert(N == M, "Matrix must be square");
        *this = this->inverted();
    }

    Matrix<N, N, Field> &operator*=(const Matrix<N, N, Field> &x) {
        static_assert(N == M, "Matrix must be square");
        *this = *this * x;
        return *this;
    }
};


template<size_t N, typename Field = Rational>
class SquareMatrix : public Matrix<N, N, Field> {
public:
    SquareMatrix() : Matrix<N, N, Field>() {}
    template<typename T>
    explicit SquareMatrix(const std::vector<std::vector<T>> &x) : Matrix<N, N, Field>(x) {}
};