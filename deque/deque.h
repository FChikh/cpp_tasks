#include <iostream>
#include <vector>
#include <type_traits>

template<typename T>
class Deque {
public:
    template<bool IsConst>
    class common_iterator {
    public:
        T *ptr;
        T **node;
        T *first;
        T *last;

        common_iterator() = default;

        template<bool isConst_other>
        common_iterator(const common_iterator<isConst_other> &x) : node(x.node) {
            ptr = x.ptr;

            first = x.first;
            last = x.last;
        }

        std::conditional_t<IsConst, const T &, T &> operator*() {
            return *ptr;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return ptr;
        }

        void set_node(std::conditional_t<IsConst, const T **, T **> new_node) {
            node = new_node;
            first = *new_node;
            last = first + bucket_sz;
        }

        common_iterator &operator++() {
            ++ptr;
            if (ptr == last) {
                node = node + 1;
                first = *node;
                last = first + bucket_sz;
                ptr = first;
            }
            return *this;
        }

        common_iterator operator++(int) {
            common_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        common_iterator &operator--() {
            if (ptr == first) {
                node = node - 1;
                first = *node;
                last = first + bucket_sz;
                ptr = last;
            }
            --ptr;
            return *this;
        }

        common_iterator operator--(int) {
            common_iterator tmp = *this;
            --*this;
            return tmp;
        }


        common_iterator &operator+=(int64_t n) {
            int64_t offset = n + (ptr - first);
            if (offset >= 0 && offset < int64_t(bucket_sz)) {
                ptr += n;
            } else {
                int64_t node_offset;
                if (offset > 0) {
                    node_offset = offset / int64_t(bucket_sz);
                } else {
                    node_offset = -((-offset - 1) / int64_t(bucket_sz) + 1);
                }
                node = node + node_offset;
                first = *node;
                last = first + bucket_sz;
                ptr = first + (offset - node_offset * bucket_sz);
            }

            return *this;
        }

        common_iterator operator+(int64_t n) const {
            common_iterator tmp = *this;
            return tmp += n;
        }

        common_iterator &operator-=(int64_t n) {
            return *this += -n;
        }

        common_iterator operator-(int64_t n) const {
            common_iterator tmp = *this;
            return tmp -= n;
        }

        friend int64_t operator-(common_iterator x, common_iterator y) {
            if (x == y) {
                return 0;
            } else if (x < y) {
                return -((y.node - x.node - 1) * bucket_sz + (x.last - x.ptr) + (y.ptr - y.first));
            } else {
                return (x.node - y.node - 1) * bucket_sz + (x.ptr - x.first) + (y.last - y.ptr);
            }
        }

        const T &operator[](int64_t n) const {
            return *(*this + n);
        }

        T &operator[](int64_t n) {
            return *(*this + n);
        }

        template<bool IsConst_x>
        bool operator==(common_iterator<IsConst_x> x) const {
            return (ptr == x.ptr);
        }

        template<bool IsConst_x>
        bool operator!=(common_iterator<IsConst_x> x) const {
            return !(*this == x);
        }

        template<bool IsConst_x>
        bool operator<(common_iterator<IsConst_x> x) const {
            if (node == x.node) {
                return ptr < x.ptr;
            } else {
                return node < x.node;
            }
        }

        template<bool IsConst_x>
        bool operator>(common_iterator<IsConst_x> x) const {
            return x < *this;
        }

        template<bool IsConst_x>
        bool operator<=(common_iterator<IsConst_x> x) const {
            return (*this == x) || (*this < x);
        }

        template<bool IsConst_x>
        bool operator>=(common_iterator<IsConst_x> x) const {
            return (*this == x) || (*this > x);
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Deque(const size_t n = 0) {
        size_t num_nodes = n / bucket_sz + 1;
        nodes_size = num_nodes * 2 + 2;

        arr = reinterpret_cast<T **>(new int8_t *[nodes_size * sizeof(T *)]);
        try {
            for (T **ptr = arr; ptr < arr + nodes_size; ++ptr) {
                *ptr = reinterpret_cast<T *>(new int8_t[bucket_sz * sizeof(T)]);
            }
        } catch (...) {
            for (T **ptr = arr; ptr < arr + nodes_size; ++ptr) {
                delete[] reinterpret_cast<int8_t *>(*ptr);
            }
            delete[] reinterpret_cast<int8_t *>(arr);
            throw;
        }
        start.set_node(arr);
        start.ptr = start.first;

        finish.set_node(arr + nodes_size - 1);
        finish.ptr = finish.last;

        T **tmp_start = arr + (nodes_size - num_nodes) / 2;
        T **tmp_finish = tmp_start + num_nodes - 1;

        begin_.set_node(tmp_start);
        begin_.ptr = begin_.first;

        end_.set_node(tmp_finish);
        end_.ptr = end_.first + n % bucket_sz;

        sz = n;

    }

    Deque(const size_t n, const T value) : Deque(n) {
        try {
            for (T **ptr = begin_.node; ptr < end_.node; ++ptr) {
                std::uninitialized_fill_n(*ptr, bucket_sz, value);
            }
            std::uninitialized_fill_n(end_.first, end_.ptr - end_.first, value);
        } catch (...) {
            for (T **ptr = arr; ptr < arr + nodes_size; ++ptr) {
                delete[] reinterpret_cast<int8_t *>(*ptr);
            }
            delete[] reinterpret_cast<int8_t *>(arr);
            throw;
        }


    }


    Deque(const Deque<T> &d) : Deque() {
        while (sz != 0) {
            pop_back();
        }
        for (const_iterator it = d.begin(); it < d.end(); ++it) {
            push_back(*it);
        }

    }

    Deque<T> operator=(Deque<T> d) {
        while (sz != 0) {
            pop_back();
        }
        for (iterator it = d.begin(); it < d.end(); ++it) {
            push_back(*it);
        }
        return *this;
    }

    ~Deque() {
        while (sz != 0) {
            pop_back();
        }
        for (T **ptr = arr; ptr < arr + nodes_size; ++ptr) {
            delete[] reinterpret_cast<int8_t *>(*ptr);
        }
        delete[] reinterpret_cast<int8_t *>(arr);
    }


    void increase_capacity() {
        size_t num_nodes = sz / bucket_sz + 1;
        size_t new_nodes_size = num_nodes * 2 + 2;

        T **new_arr = reinterpret_cast<T **>(new int8_t *[nodes_size * sizeof(T *)]);

        try {
            for (T **ptr = new_arr; ptr < new_arr + new_nodes_size; ++ptr) {
                *ptr = reinterpret_cast<T *>(new int8_t[bucket_sz * sizeof(T)]);
            }
        } catch (...) {
            for (T **ptr = new_arr; ptr < new_arr + new_nodes_size; ++ptr) {
                delete[] reinterpret_cast<int8_t *>(*ptr);
            }
            delete[] reinterpret_cast<int8_t *>(arr);
            throw;
        }

        start.set_node(new_arr);
        start.ptr = start.first;

        finish.set_node(new_arr + new_nodes_size - 1);
        finish.ptr = finish.last;

        T **tmp_start = new_arr + (new_nodes_size - num_nodes) / 2;
        T **tmp_finish = tmp_start + num_nodes - 1;
        if (begin_.node != nullptr) {
            try {
                T **tmp = tmp_start;
                for (T **ptr = begin_.node; ptr <= end_.node; ++ptr) {
                    *tmp = *ptr;
                    ++tmp;
                }
            } catch (...) {
                for (T **ptr = new_arr; ptr < new_arr + new_nodes_size; ++ptr) {
                    delete[] reinterpret_cast<int8_t *>(*ptr);
                }
                delete[] reinterpret_cast<int8_t *>(arr);
                delete tmp_start;
                delete tmp_finish;
                throw;
            }
        }

        delete[] reinterpret_cast<int8_t *>(arr);

        size_t start_el = begin_.ptr - begin_.first;
        size_t finish_el = end_.ptr - end_.first;
        begin_.set_node(tmp_start);
        begin_.ptr = begin_.first + start_el;

        end_.set_node(tmp_finish);
        end_.ptr = end_.first + finish_el;


        arr = new_arr;
        nodes_size = new_nodes_size;

    }

    size_t size() const {
        return sz;
    }

    T &operator[](size_t i) {
        return begin_[i];
    }

    const T &operator[](size_t i) const {
        return begin_[i];
    }

    T &at(size_t i) {
        if (i >= sz || i < 0) {
            throw std::out_of_range("index error");
        }
        return begin_[i];
    }

    const T &at(size_t i) const {
        if (i >= sz || i < 0) {
            throw std::out_of_range("index error");
        }
        return begin_[i];
    }

    void push_back(const T value) {
        if (end_.ptr + 1 == finish.last) {
            increase_capacity();
        }
        new(end_.ptr) T(value);
        ++sz;
        ++end_;
    }

    void pop_back() {
        --end_;
        (end_.ptr)->~T();
        --sz;

    }

    void push_front(const T value) {
        if (begin_.ptr == start.first) {
            increase_capacity();
        }
        --begin_;
        new(begin_.ptr) T(value);
        ++sz;

    }

    void pop_front() {
        (begin_.ptr)->~T();
        --sz;
        ++begin_;
    }

    void insert(iterator in, const T value) {
        if (end_.ptr + 1 == finish.last) {
            increase_capacity();
        }
        ++end_;
        ++sz;
        for (iterator tmp = end_ - 1; tmp > in; --tmp) {
            *tmp = *(tmp - 1);
        }
        *in = value;
    }

    void erase(iterator out) {
        (out.ptr)->~T();
        for (iterator tmp = out; tmp < end_ - 1; ++tmp) {
            *tmp = *(tmp + 1);
        }
        --end_;
        --sz;
    }

    iterator begin() {
        return begin_;
    }

    iterator end() {
        return end_;
    }

    const_iterator begin() const {
        const_iterator tmp_begin(begin_);
        return tmp_begin;
    }

    const_iterator end() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    const_iterator cbegin() const {
        const_iterator tmp_begin(begin_);
        return tmp_begin;
    }

    const_iterator cend() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    reverse_iterator rbegin() {
        return begin_;
    }

    reverse_iterator rend() {
        return end_;
    }

    const_reverse_iterator rbegin() const {
        const_reverse_iterator tmp_begin(begin_);
        return tmp_begin;
    }

    const_reverse_iterator rend() const {
        const_reverse_iterator tmp_end(end_);
        return tmp_end;
    }

    const_reverse_iterator crbegin() const {
        const_reverse_iterator tmp_begin(begin_);
        return tmp_begin;
    }

    const_reverse_iterator crend() const {
        const_reverse_iterator tmp_end(end_);
        return tmp_end;
    }

private:
    T **arr;
    size_t sz;
    size_t nodes_size;

    static const size_t bucket_sz = 10000;
    iterator start, finish;
    iterator begin_, end_;
};

