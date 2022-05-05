#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <string>

template<typename T, typename Allocator = std::allocator<T>>
class List {
private:
    class BaseNode {
    public:
        BaseNode *prev_;
        BaseNode *next_;

        BaseNode() = default;

        BaseNode(BaseNode *prev, BaseNode *next) : prev_(prev), next_(next) {}

        BaseNode(const BaseNode &other) : prev_(other.prev_), next_(other.next_) {}
    };

    class Node : public BaseNode {
    public:
        using BaseNode::prev_, BaseNode::next_;
        T value_;
        size_t hash_;

        Node() = default;

        Node &operator=(Node other) {
            swap(other);
            return *this;
        }

        void swap(Node &other) {
            prev_ = other.prev_;
            next_ = other.prev_;
            value_ = other.value_;
        }

        Node(BaseNode *prev, BaseNode *next) : BaseNode(prev, next) {}

        Node(BaseNode *prev, BaseNode *next, const T &value, const size_t &hash) : BaseNode(prev, next), value_(value) {
            hash_ = hash;
        }

        Node(BaseNode *prev, BaseNode *next, T &&value, const size_t &hash) : BaseNode(prev, next),
                                                                              value_(std::forward<T>(value)) {
            hash_ = hash;
        }

        template<typename Pair>
        Node(BaseNode *prev, BaseNode *next, Pair &&value, const size_t &hash) : BaseNode(prev, next),
                                                                                 value_(std::forward<Pair>(value)) {
            hash_ = hash;
        }

        template<typename...Args>
        Node(BaseNode *prev, BaseNode *next, const size_t &hash, Args &&... args) : BaseNode(prev, next),
                                                                                    value_(std::forward<Args>(
                                                                                            args)...) {
            hash_ = hash;
        }

    };

    size_t size_ = 0;
    Node *begin_;
    Node *end_;
    typename std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc_;
    Allocator base_alloc_;
    using AllocTraits = std::allocator_traits<typename std::allocator_traits<Allocator>::template rebind_alloc<Node>>;
    using Traits = std::allocator_traits<Allocator>;
public:
    template<bool IsConst>
    class common_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = std::conditional_t<IsConst, const T &, T &>;
        Node *ptr_;

        common_iterator() = default;

        common_iterator(Node *new_ptr) : ptr_(new_ptr) {}

        common_iterator(const common_iterator<false> &x) : ptr_(x.ptr_) {}

        common_iterator &operator=(common_iterator x) {
            swap(x);
            return *this;
        }

        void swap(common_iterator &x) {
            std::swap(ptr_, x.ptr_);
        }

        std::conditional_t<IsConst, const T &, T &> operator*() {
            return ptr_->value_;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return &(ptr_->value_);
        }

        size_t get_hash() const {
            return ptr_->hash_;
        }

        void set_hash(size_t new_hash) {
            ptr_->hash_ = new_hash;
        }

        common_iterator &operator++() {
            ptr_ = reinterpret_cast<Node *>(ptr_->next_);
            return *this;
        }

        common_iterator &operator--() {
            ptr_ = reinterpret_cast<Node *>(ptr_->prev_);
            return *this;
        }

        common_iterator operator++(int) {
            common_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        common_iterator operator--(int) {
            common_iterator tmp = *this;
            --*this;
            return tmp;
        }

        template<bool IsConst_x>
        bool operator==(common_iterator<IsConst_x> x) const {
            return (ptr_ == x.ptr_);
        }

        template<bool IsConst_x>
        bool operator!=(common_iterator<IsConst_x> x) const {
            return !(*this == x);
        }

    };

    template<bool IsConst>
    class common_reverse_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = std::conditional_t<IsConst, const T &, T &>;
        Node *ptr_;

        common_reverse_iterator() = default;

        common_reverse_iterator(Node *new_ptr) : ptr_(new_ptr) {}

        common_reverse_iterator &operator=(common_reverse_iterator x) {
            swap(x);
            return *this;
        }

        common_reverse_iterator(const common_reverse_iterator<false> &x) : ptr_(x.ptr_) {}

        void swap(common_reverse_iterator &x) {
            std::swap(ptr_, x.ptr_);
        }

        std::conditional_t<IsConst, common_iterator<true>, common_iterator<false>> base() {
            return std::conditional_t<IsConst, common_iterator<true>, common_iterator<false>>(
                    reinterpret_cast<Node *>(ptr_->next_));
        }

        std::conditional_t<IsConst, const T &, T &> operator*() {
            return ptr_->value_;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return *(ptr_->value_);
        }

        common_reverse_iterator &operator++() {
            ptr_ = reinterpret_cast<Node *>(ptr_->prev_);
            return *this;
        }

        common_reverse_iterator &operator--() {
            ptr_ = reinterpret_cast<Node *>(ptr_->next_);
            return *this;
        }

        common_reverse_iterator operator++(int) {
            common_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        common_reverse_iterator operator--(int) {
            common_iterator tmp = *this;
            --*this;
            return tmp;
        }

        template<bool IsConst_x>
        bool operator==(common_reverse_iterator<IsConst_x> x) const {
            return (ptr_ == x.ptr_);
        }

        template<bool IsConst_x>
        bool operator!=(common_reverse_iterator<IsConst_x> x) const {
            return !(*this == x);
        }

    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;

    using reverse_iterator = common_reverse_iterator<false>;
    using const_reverse_iterator = common_reverse_iterator<true>;

    List() {
        BaseNode *beg = alloc_.allocate(1);
        AllocTraits::construct(alloc_, beg, nullptr, nullptr);
        BaseNode *end = alloc_.allocate(1);
        AllocTraits::construct(alloc_, end, beg, nullptr);
        begin_ = reinterpret_cast<Node *>(beg);
        end_ = reinterpret_cast<Node *>(end);
        begin_->next_ = end_;
    }

    explicit List(const Allocator &) : List() {}

    List(size_t count) : List() {
        size_ = count;
        Node *previous = begin_;
        for (size_t i = 0; i < count; ++i) {
            Node *current = alloc_.allocate(1);
            AllocTraits::construct(alloc_, current, previous, previous);
            if (i == 0) {
                begin_->next_ = current;
            } else {
                previous->next_ = current;
            }
            if (i == count - 1) {
                current->next_ = end_;
                end_->prev_ = current;
            }
            previous = reinterpret_cast<Node *>(current);
        }
    }

    List(size_t count, const T &value) : List() {
        size_ = count;
        Node *previous = begin_;
        for (size_t i = 0; i < count; ++i) {
            Node *current = alloc_.allocate(1);
            AllocTraits::construct(alloc_, current, previous, previous, value);
            if (i == 0) {
                begin_->next_ = current;
            } else previous->next_ = current;
            if (i == count - 1) {
                current->next_ = end_;
                end_->prev_ = current;
            }
            previous = current;
        }
    }

    List(const List &other) : List() {
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
        for (auto &it: other) {
            push_back(it);
        }
    }

    List(List &&other) : List() {
        alloc_ = AllocTraits::select_on_container_move_construction(other.alloc_);
        begin_->next_ = other.begin_->next_;
        begin_->next_->prev_ = begin_;
        end_->prev_ = other.end_->prev_;
        end_->prev_->next_ = end_;

        size_ = other.size_;

        other.begin_->next_ = other.end_;
        other.end_->prev_ = other.begin_;
        other.size_ = 0;
    }

    List &operator=(const List &other) {
        if (&other == this) {
            return *this;
        }
        if (AllocTraits::propagate_on_container_copy_assignment::value) {
            alloc_ = other.alloc_;
        }
        while (size_ != 0) {
            pop_back();
        }
        for (auto &it: other) {
            push_back(it);
        }
        size_ = other.size_;
        return *this;
    }

    List &operator=(List &&other) noexcept {
        if (&other == this) {
            return *this;
        }
        if (AllocTraits::propagate_on_container_move_assignment::value) {
            alloc_ = other.alloc_;
        }
        while (size_ != 0) {
            pop_back();
        }
        begin_->next_ = other.begin_->next_;
        begin_->next_->prev_ = begin_;
        end_->prev_ = other.end_->prev_;
        end_->prev_->next_ = end_;
        size_ = other.size_;

        other.begin_->next_ = other.end_;
        other.end_->prev_ = other.begin_;
        other.size_ = 0;
        return *this;
    }

    const typename std::allocator_traits<Allocator>::template rebind_alloc<Node> &get_allocator() {
        return alloc_;
    }

    size_t size() const {
        return size_;
    }

    void push_back(const T &value) {
        insert(end(), value);
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }

    void pop_back() {
        erase(--end());
    }

    void pop_front() {
        erase(begin());
    }

    void insert(const_iterator in, const T &value, const size_t &hash = 0) {
        ++size_;
        Node *tmp = alloc_.allocate(1);
        AllocTraits::construct(alloc_, tmp, in.ptr_->prev_, reinterpret_cast<BaseNode *>(in.ptr_), value, hash);
        tmp->next_->prev_ = tmp;
        tmp->prev_->next_ = tmp;
    }

    void insert(const_iterator in, T &&value, const size_t &hash = 0) {
        ++size_;
        Node *tmp = alloc_.allocate(1);
        AllocTraits::construct(alloc_, tmp, in.ptr_->prev_, reinterpret_cast<BaseNode *>(in.ptr_),
                               std::forward<T>(value), hash);
        tmp->next_->prev_ = tmp;
        tmp->prev_->next_ = tmp;
    }

    template<typename Pair>
    void insert(const_iterator in, Pair &&value, const size_t &hash = 0) {
        ++size_;
        Node *tmp = alloc_.allocate(1);
        AllocTraits::construct(alloc_, tmp, in.ptr_->prev_, reinterpret_cast<BaseNode *>(in.ptr_),
                               std::forward<Pair>(value), hash);
        tmp->next_->prev_ = tmp;
        tmp->prev_->next_ = tmp;
    }

    template<typename...Args>
    T &emplace(const_iterator in, const size_t, Args &&... args) {
        ++size_;
        BaseNode *tmp = alloc_.allocate(1);
        AllocTraits::construct(alloc_, tmp, in.ptr_->prev_, reinterpret_cast<BaseNode *>(in.ptr_));
        Node *current = reinterpret_cast<Node *>(tmp);
        base_alloc_.allocate(1);
        Traits::construct(base_alloc_, &(current->value_), std::forward<Args>(args)...);
        current->next_->prev_ = current;
        current->prev_->next_ = current;
        return current->value_;
    }

    iterator place_properly(const_iterator in, const_iterator out) {
        in.ptr_->next_->prev_ = in.ptr_->prev_;
        in.ptr_->prev_->next_ = in.ptr_->next_;
        in.ptr_->next_ = out.ptr_;
        in.ptr_->prev_ = out.ptr_->prev_;
        out.ptr_->prev_->next_ = in.ptr_;
        out.ptr_->prev_ = in.ptr_;
        return iterator(in.ptr_);
    }


    void erase(const_iterator out) {
        --size_;
        Node *tmp = reinterpret_cast<Node *>(out.ptr_->prev_);
        out.ptr_ = reinterpret_cast<Node *>(out.ptr_->next_);
        AllocTraits::destroy(alloc_, reinterpret_cast<Node *>(out.ptr_->prev_));
        alloc_.deallocate(reinterpret_cast<Node *>(out.ptr_->prev_), 1);
        out.ptr_->prev_ = tmp;
        out.ptr_->prev_->next_ = out.ptr_;
    }

    void decreaseSize(const size_t &val) {
        size_ -= val;
    }

    iterator begin() {
        return iterator(reinterpret_cast<Node *>(begin_->next_));
    }

    iterator end() {
        return iterator(end_);
    }

    const_iterator begin() const {
        const_iterator tmp_begin(reinterpret_cast<Node *>(begin_->next_));
        return tmp_begin;
    }

    const_iterator end() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    const_iterator cbegin() const {
        const_iterator tmp_begin(reinterpret_cast<Node *>(begin_->next_));
        return tmp_begin;
    }

    const_iterator cend() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(reinterpret_cast<Node *>(end_->prev_));
    }

    reverse_iterator rend() {
        return reverse_iterator(end_);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(reinterpret_cast<Node *>(end_->prev_));
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin_);
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(reinterpret_cast<Node *>(end_->prev_));
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin_);
    }

    ~List() {
        while (size_ != 0) {
            pop_back();
        }
        AllocTraits::destroy(alloc_, reinterpret_cast<BaseNode *>(begin_));
        alloc_.deallocate(begin_, 1);
        AllocTraits::destroy(alloc_, reinterpret_cast<BaseNode *>(end_));
        alloc_.deallocate(end_, 1);
    }

};


template<typename Key, typename Value, typename Hash = std::hash<Key>, typename Equal = std::equal_to<Key>, typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
public:
    using NodeType = std::pair<const Key, Value>;
    using Iterator = typename List<NodeType, Alloc>::iterator;
    using ConstIterator = typename List<NodeType, Alloc>::const_iterator;
    typename std::allocator_traits<Alloc> alloc_;
    using AllocTraits = std::allocator_traits<typename std::allocator_traits<Alloc>>;

    UnorderedMap() {
        capacity_ = 100;
        max_load_factor_ = 1.0;
        pointers_ = std::vector<Iterator>(capacity_);
        hash_function_ = Hash();
        equal_function_ = Equal();
        alloc_ = std::allocator_traits<Alloc>();
        for (Iterator &item: pointers_) {
            item = data_.end();
        }
    }

    UnorderedMap(const UnorderedMap &other) :
            capacity_(other.capacity_),
            max_load_factor_(other.max_load_factor_),
            hash_function_(other.hash_function_),
            equal_function_(other.equal_function_) {
        pointers_ = std::vector<Iterator>(capacity_);
        for (Iterator &item: pointers_) {
            item = data_.end();
        }
        for (auto &item: other) {
            insert(item);
        }
        alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
    }

    UnorderedMap(UnorderedMap &&other) {
        data_ = std::move(other.data_);
        pointers_ = std::move(other.pointers_);
        capacity_ = std::move(other.capacity_);
        max_load_factor_ = std::move(other.max_load_factor_);
        hash_function_ = std::move(other.hash_function_);
        equal_function_ = std::move(other.equal_function_);
        if (AllocTraits::propagate_on_container_move_assignment::value) {
            alloc_ = std::move(other.alloc_);
        }
    }

    UnorderedMap &operator=(const UnorderedMap &other) {
        data_ = other.data_;
        pointers_ = other.pointers_;
        capacity_ = other.capacity_;
        max_load_factor_ = other.max_load_factor_;
        hash_function_ = other.hash_function_;
        equal_function_ = other.equal_function_;
        if (AllocTraits::propagate_on_container_copy_assignment::value) {
            alloc_ = other.alloc_;
        }
        return *this;
    }

    UnorderedMap &operator=(UnorderedMap &&other) noexcept {
        data_ = std::move(other.data_);
        pointers_ = std::move(other.pointers_);
        for (auto &item: pointers_) {
            if (item == other.end()) {
                item = end();
            }
        }
        capacity_ = std::move(other.capacity_);
        max_load_factor_ = std::move(other.max_load_factor_);
        hash_function_ = std::move(other.hash_function_);
        equal_function_ = std::move(other.equal_function_);
        if (AllocTraits::propagate_on_container_move_assignment::value) {
            alloc_ = std::move(other.alloc_);
        }
        return *this;
    }

    ~UnorderedMap() {}

    template<typename...Args>
    std::pair<Iterator, bool> emplace(Args &&... args) {
        if (load_factor() > max_load_factor()) {
            rehash(capacity_ * 2);
        }
        size_t hash = 0;
        data_.emplace(data_.begin(), hash, std::forward<Args>(args)...);
        Iterator it = data_.begin();
        const Key &tmp = (*it).first;
        hash = hash_function_(tmp);
        it.set_hash(hash);
        hash %= capacity_;
        if (pointers_[hash] == end()) {
            pointers_[hash] = data_.begin();
            it = pointers_[hash];
        } else {
            it = data_.place_properly(data_.begin(), pointers_[hash]);
        }
        return {it, true};
    }


    std::pair<Iterator, bool> insert(const NodeType &new_el) {
        if (load_factor() > max_load_factor()) {
            rehash(capacity_ * 2);
        }
        Iterator find_result = find(std::forward<const Key>(new_el.first));
        if (find_result != end()) {
            return {find_result, false};
        }
        size_t hash = hash_function_(new_el.first);
        if (pointers_[hash % capacity_] != data_.end()) {
            data_.insert(pointers_[hash % capacity_], new_el, hash);
        } else {
            data_.insert(data_.begin(), new_el, hash);
            pointers_[hash % capacity_] = data_.begin();
        }
        return {pointers_[hash % capacity_], true};
    }

    template<typename Pair>
    std::pair<Iterator, bool> insert(Pair &&value) {
        if (load_factor() > max_load_factor()) {
            rehash(capacity_ * 2);
        }
        Iterator find_result = find(std::forward<const Key>(value.first));
        if (find_result != end()) {
            return {find_result, false};
        }
        size_t hash = hash_function_(value.first);
        if (pointers_[hash % capacity_] != data_.end()) {
            data_.insert(pointers_[hash % capacity_], std::forward<Pair>(value), hash);
        } else {
            data_.insert(data_.begin(), std::forward<Pair>(value), hash);
            pointers_[hash % capacity_] = data_.begin();
        }
        return {pointers_[hash % capacity_], true};
    }

    std::pair<Iterator, bool> insert(NodeType &&pair) {
        return insert<>(pair);
    }


    template<class InputIt>
    void insert(const InputIt &start, const InputIt &finish) {
        InputIt it = start;
        while (it != finish) {
            (*this).insert(*it);
            ++it;
        }
    }

    void erase(const Iterator &pos) {
        if (pointers_[pos.get_hash() % capacity_] == pos) {
            Iterator next = pos;
            ++next;
            if (next.get_hash() % capacity_ == pos.get_hash() % capacity_) {
                pointers_[pos.get_hash() % capacity_] = next;
            } else {
                pointers_[pos.get_hash() % capacity_] = data_.end();
            }
        }
        data_.erase(pos);
    }

    void erase(const Iterator &start, const Iterator &finish) {
        Iterator it = start;
        while (it != finish) {
            (*this).erase(it);
            ++it;
        }
    }

    Iterator find(const Key &k) {
        size_t hash_modulo = hash_function_(k) % capacity_;
        if (pointers_[hash_modulo] != data_.end()) {
            for (Iterator it = pointers_[hash_modulo]; it.get_hash() % capacity_ == hash_modulo; ++it) {
                if (equal_function_(k, (*it).first)) {
                    return it;
                }
            }
            return end();
        } else {
            return end();
        }
    }

    Value &operator[](const Key &k) {
        if (find(k) == end()) {
            auto insert_result = insert({k, Value()}).first;
            return (*insert_result).second;
        } else {
            return (*find(k)).second;
        }
    }

    Value &at(const Key &k) {
        if (find(k) == end()) {
            throw std::out_of_range("index error");
        } else {
            return (*find(k)).second;
        }
    }

    size_t size() const {
        return data_.size();
    }

    void reserve(size_t count) {
        if (static_cast<double>(count) / static_cast<double>(capacity_) < max_load_factor_) {
            return;
        }
        rehash(count / max_load_factor_);
    }

    double load_factor() const {
        return static_cast<double>(data_.size()) / static_cast<double>(capacity_);
    }

    double max_load_factor() const {
        return max_load_factor_;
    }

    void max_load_factor(double ml) {
        max_load_factor_ = ml;
    }

    size_t max_size() const {
        return std::numeric_limits<std::size_t>::max() / sizeof(NodeType);
    }

    void rehash(size_t cnt) {
        List<NodeType, Alloc> new_data;
        std::vector<Iterator> new_pointers(cnt);
        for (auto &item: new_pointers) {
            item = data_.end();
        }
        for (Iterator it = data_.begin(); it != data_.end(); it = data_.begin()) {
            size_t hash = it.get_hash() % cnt;
            if (new_pointers[hash] == data_.end()) {
                new_data.place_properly(it, new_data.begin());
                new_pointers[hash] = new_data.begin();
            } else {
                new_data.place_properly(it, new_pointers[hash]);
            }
            data_.decreaseSize(1);
            new_data.decreaseSize(-1);
        }
        capacity_ = cnt;
        pointers_ = new_pointers;
        data_ = std::move(new_data);
    }

    Iterator begin() {
        return data_.begin();
    }

    Iterator end() {
        return data_.end();
    }

    ConstIterator begin() const {
        return data_.begin();
    }

    ConstIterator end() const {
        return data_.end();
    }

    ConstIterator cbegin() const {
        return data_.cbegin();
    }

    ConstIterator cend() const {
        return data_.cbegin();
    }

private:
    size_t capacity_;
    double max_load_factor_;

    std::vector<Iterator> pointers_;
    List<NodeType, Alloc> data_;
    Hash hash_function_;
    Equal equal_function_;
};