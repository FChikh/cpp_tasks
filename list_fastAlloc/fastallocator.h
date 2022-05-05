#include <vector>
#include <ctime>
#include <type_traits>
#include <list>

const int MAX_ALLOC_SIZE = 50;

template<size_t chunkSize>
class FixedAllocator {
private:
    std::vector<void*> pool_alloc_;
public:
    FixedAllocator() = default;

    static FixedAllocator<chunkSize>& get_alloc() {
        static FixedAllocator<chunkSize> alloc;
        return alloc;
    }

    void* allocate() {
        if (pool_alloc_.size() == 0) {
            try {
                int8_t* ptr = reinterpret_cast<int8_t*>(::operator new(MAX_ALLOC_SIZE * chunkSize));
                for (int i = 0; i < MAX_ALLOC_SIZE; ++i)
                    pool_alloc_.push_back(reinterpret_cast<void*>(ptr + i * chunkSize));
            } catch (...) {
                throw;
            }
        }

        void* ptr = pool_alloc_.back();
        pool_alloc_.pop_back();
        return ptr;
    }

    void deallocate(void* element, size_t) { 
        pool_alloc_.push_back(element); 
    }

    ~FixedAllocator() = default;
};


template<typename T>
class FastAllocator {
public:
    FastAllocator() = default;

    template <typename U>
    FastAllocator(const FastAllocator<U>&) {}

    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    T* allocate(size_t cnt) {
        switch(cnt * sizeof(T)) {
            case 4:
                return static_cast<T*>(FixedAllocator<4>::get_alloc().allocate());
            case 8:
                return static_cast<T*>(FixedAllocator<8>::get_alloc().allocate());
            case 16:
                return static_cast<T*>(FixedAllocator<16>::get_alloc().allocate());
            case 24:
                return static_cast<T*>(FixedAllocator<24>::get_alloc().allocate());
            case 32:
                return static_cast<T*>(FixedAllocator<32>::get_alloc().allocate());
            default:
                return static_cast<T*>(::operator new(cnt * sizeof(T)));
        }
    }
    void deallocate(T* ptr, size_t cnt) {
        switch(cnt * sizeof(T)) {
            case 4:
                FixedAllocator<4>::get_alloc().deallocate(ptr, 4);
                break;
            case 8:
                FixedAllocator<8>::get_alloc().deallocate(ptr, 8);
                break;
            case 16:
                FixedAllocator<16>::get_alloc().deallocate(ptr, 16);
                break;
            case 24:
                FixedAllocator<24>::get_alloc().deallocate(ptr, 24);
                break;
            case 32:
                FixedAllocator<24>::get_alloc().deallocate(ptr, 32);
                break;
            default:
                ::operator delete(ptr);
        }
    }

    template <class Type> struct rebind { 
        FastAllocator<Type> other; 
    };

    ~FastAllocator() = default; 
};




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
        Node(BaseNode *prev, BaseNode *next, const T &value) : BaseNode(prev, next) {
            value_ = value;
        }
    
    };

    size_t size_ = 0;
    Node *begin_;
    Node *end_;
    typename std::allocator_traits<Allocator>::template rebind_alloc<Node> alloc_;
    using AllocTraits = std::allocator_traits<typename std::allocator_traits<Allocator>::template rebind_alloc<Node>>;
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
            return *(ptr_->value_);
        }

        common_iterator &operator++() {
            ptr_ = reinterpret_cast<Node*>(ptr_->next_);
            return *this;
        }

        common_iterator &operator--() {
            ptr_ = reinterpret_cast<Node*>(ptr_->prev_);
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
			return std::conditional_t<IsConst, common_iterator<true>, common_iterator<false>>(reinterpret_cast<Node*>(ptr_->next_));
		}

        std::conditional_t<IsConst, const T &, T &> operator*() {
            return ptr_->value_;
        }

        std::conditional_t<IsConst, const T *, T *> operator->() {
            return *(ptr_->value_);
        }

        common_reverse_iterator &operator++() {
            ptr_ = reinterpret_cast<Node*>(ptr_->prev_);
            return *this;
        }

        common_reverse_iterator &operator--() {
            ptr_ = reinterpret_cast<Node*>(ptr_->next_);
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
        begin_ = reinterpret_cast<Node*>(beg);
        end_ = reinterpret_cast<Node*>(end);
        begin_->next_ = end_;
   }

    explicit List(const Allocator&) : List() {}

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
            previous = reinterpret_cast<Node*>(current);
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

    List(const List& other) : List() {
		alloc_ = AllocTraits::select_on_container_copy_construction(other.alloc_);
		for (auto &it: other) {
			push_back(it);
		}
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

    void insert(const_iterator in, const T &value) {
        ++size_;
        BaseNode *tmp = alloc_.allocate(1);
        AllocTraits::construct(alloc_, tmp, in.ptr_->prev_, reinterpret_cast<BaseNode*>(in.ptr_));
        Node *current = reinterpret_cast<Node*>(tmp);
        new (&(current->value_)) T(value);
        current->next_->prev_ = current;
        current->prev_->next_ = current;
    }

    void erase(const_iterator out) {
        --size_;
        Node *tmp = reinterpret_cast<Node*>(out.ptr_->prev_);
        out.ptr_ = reinterpret_cast<Node*>(out.ptr_->next_);
        AllocTraits::destroy(alloc_, reinterpret_cast<Node*>(out.ptr_->prev_));
        alloc_.deallocate(reinterpret_cast<Node*>(out.ptr_->prev_), 1);
        out.ptr_->prev_ = tmp;
        out.ptr_->prev_->next_ = out.ptr_;    
    }

    iterator begin() {
        return iterator(reinterpret_cast<Node*>(begin_->next_));
    }

    iterator end() {
        return iterator(end_);
    }

    const_iterator begin() const {
        const_iterator tmp_begin(reinterpret_cast<Node*>(begin_->next_));
        return tmp_begin;
    }

    const_iterator end() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    const_iterator cbegin() const {
        const_iterator tmp_begin(reinterpret_cast<Node*>(begin_->next_));
        return tmp_begin;
    }

    const_iterator cend() const {
        const_iterator tmp_end(end_);
        return tmp_end;
    }

    reverse_iterator rbegin() {
        return reverse_iterator(reinterpret_cast<Node*>(end_->prev_));
    }

    reverse_iterator rend() {
        return reverse_iterator(end_);
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(reinterpret_cast<Node*>(end_->prev_));
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin_);
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(reinterpret_cast<Node*>(end_->prev_));
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(begin_);
    }

    ~List() {
        while (size_ != 0) {
            pop_back();
        }
        AllocTraits::destroy(alloc_, reinterpret_cast<BaseNode*>(begin_));
        alloc_.deallocate(begin_, 1);
        AllocTraits::destroy(alloc_, reinterpret_cast<BaseNode*>(end_));
        alloc_.deallocate(end_, 1);
    }

};

