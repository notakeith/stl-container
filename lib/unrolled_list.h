#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>

template <typename T, size_t NodeMaxSize = 10, typename Allocator = std::allocator<T>>
class unrolled_list {
   public:
    using value_type = T;
    using allocator_type = Allocator;
    using alloc_traits = std::allocator_traits<Allocator>;
    using reference = value_type&;
    using pointer = value_type*;
    using const_reference = const value_type&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    struct Node;

    using NodeAllocator = alloc_traits::template rebind_alloc<Node>;
    using NodeAllocatorTraits = std::allocator_traits<NodeAllocator>;

    struct Node {
        size_type size_;
        pointer array_;
        Node* next_;
        Node* prev_;
        Allocator array_alloc_;

        Node(const Allocator& alloc = Allocator())
            : size_(0), next_(nullptr), prev_(nullptr), array_alloc_(alloc) {
            array_ = static_cast<pointer>(::operator new(NodeMaxSize * sizeof(T)));
        }

        ~Node() {
            for (size_type i = 0; i < size_; ++i) {
                alloc_traits::destroy(array_alloc_, array_ + i);
            }
            alloc_traits::deallocate(array_alloc_, array_, NodeMaxSize);
        }

        Node(T&&) = delete;
    };

   private:
    Node* head_;
    Node* tail_;
    size_type size_;
    size_type node_qt_;
    Allocator allocator_;
    NodeAllocator node_allocator_;

   public:
    template <typename ValueType, typename PointerType, typename ReferenceType>
    class iterator_base {
       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = ValueType;
        using difference_type = ptrdiff_t;
        using pointer = PointerType;
        using reference = ReferenceType;

       public:
        Node* current_node_;
        size_type current_index_;
        iterator_base(Node* node = nullptr, size_type index = 0)
            : current_node_(node), current_index_(index) {}

        reference operator*() const {
            return current_node_->array_[current_index_];
        }

        pointer operator->() const {
            return &current_node_->array_[current_index_];
        }

        iterator_base& operator++() {
            if (current_index_ + 1 < current_node_->size_) {
                ++current_index_;
            } else {
                current_node_ = current_node_->next_;
                current_index_ = 0;
            }
            return *this;
        }

        iterator_base operator++(int) {
            iterator_base temp = *this;
            ++(*this);
            return temp;
        }

        iterator_base& operator--() {
            if (current_index_ > 0) {
                --current_index_;
            } else {
                current_node_ = current_node_->prev_;
                current_index_ = current_node_ ? current_node_->size_ - 1 : 0;
            }
            return *this;
        }

        iterator_base operator--(int) {
            iterator_base temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const iterator_base& other) const {
            return current_node_ == other.current_node_ && current_index_ == other.current_index_;
        }

        bool operator!=(const iterator_base& other) const {
            return !(*this == other);
        }
    };

    using iterator = iterator_base<T, T*, T&>;
    using const_iterator = iterator_base<const T, const T*, const T&>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() { return iterator(head_, 0); }
    iterator end() { return iterator(nullptr, 0); }
    const_iterator begin() const { return const_iterator(head_, 0); }
    const_iterator end() const { return const_iterator(nullptr, 0); }
    const_iterator cbegin() const { return const_iterator(head_, 0); }
    const_iterator cend() const { return const_iterator(nullptr, 0); }
    reverse_iterator rbegin() {
        if (tail_ == nullptr) {
            return reverse_iterator(end());
        }
        return reverse_iterator(iterator(tail_, tail_->size_));
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const {
        if (tail_ == nullptr) {
            return const_reverse_iterator(cend());
        }
        return const_reverse_iterator(const_iterator(tail_, tail_->size_));
    }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    reference front() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head_->array_[0];
    }

    const_reference front() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return head_->array_[0];
    }

    reference back() {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail_->array_[tail_->size_ - 1];
    }

    const_reference back() const {
        if (empty()) {
            throw std::out_of_range("List is empty");
        }
        return tail_->array_[tail_->size_ - 1];
    }

    unrolled_list()
        : head_(nullptr), tail_(nullptr), size_(0), node_qt_(1) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        NodeAllocatorTraits::construct(node_allocator_, ptr);
        head_ = ptr;
        tail_ = ptr;
    }

    template <typename InputIterator>
    unrolled_list(InputIterator first, InputIterator last, const Allocator& alloc = Allocator())
        : allocator_(alloc), node_allocator_(alloc) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        try {
            NodeAllocatorTraits::construct(node_allocator_, ptr, allocator_);
            head_ = ptr;
            tail_ = ptr;
            size_ = 0;
            node_qt_ = 1;
            for (; first != last; ++first) {
                if (tail_->size_ < NodeMaxSize) {
                    new (tail_->array_ + tail_->size_) T(*first);
                    tail_->size_++;
                } else {
                    Node* new_node = NodeAllocatorTraits::allocate(node_allocator_, 1);
                    try {
                        NodeAllocatorTraits::construct(node_allocator_, new_node, allocator_);
                        new (new_node->array_) T(*first);
                        new_node->size_ = 1;
                        new_node->prev_ = tail_;
                        tail_->next_ = new_node;
                        tail_ = new_node;
                        node_qt_++;
                    } catch (...) {
                        NodeAllocatorTraits::deallocate(node_allocator_, new_node, 1);
                        throw;
                    }
                }
                size_++;
            }
        } catch (...) {
            if (head_) {
                clear();
            }
            throw;
        }
    }

    unrolled_list(size_type count, const T& value, const Allocator& alloc = Allocator())
        : allocator_(alloc), node_allocator_(alloc) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        NodeAllocatorTraits::construct(node_allocator_, ptr, allocator_);
        head_ = ptr;
        tail_ = ptr;
        size_ = 0;
        node_qt_ = 1;

        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    unrolled_list(const Allocator& alloc)
        : head_(nullptr), tail_(nullptr), size_(0), node_qt_(1), allocator_(alloc), node_allocator_(alloc) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        NodeAllocatorTraits::construct(node_allocator_, ptr, allocator_);
        head_ = ptr;
        tail_ = ptr;
    }

    unrolled_list(T obj, T&& rvalue, const Allocator& alloc)
        : head_(nullptr), tail_(nullptr), size_(0), node_qt_(0), allocator_(alloc), node_allocator_(alloc) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        NodeAllocatorTraits::construct(node_allocator_, ptr, allocator_);
        head_ = ptr;
        tail_ = ptr;
    }

    unrolled_list(const unrolled_list& other)
        : unrolled_list(alloc_traits::select_on_container_copy_construction(other.allocator_)) {
        for (const auto& item : other) {
            push_back(item);
        }
    }

    unrolled_list(const unrolled_list& other, const Allocator& alloc)
        : unrolled_list(alloc) {
        for (const auto& item : other) {
            push_back(item);
        }
    }

    unrolled_list(unrolled_list&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_), node_qt_(other.node_qt_), allocator_(std::move(other.allocator_)), node_allocator_(std::move(other.node_allocator_)) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
        other.node_qt_ = 0;
    }

    unrolled_list(unrolled_list&& other, const Allocator& alloc)
        : allocator_(alloc), node_allocator_(alloc) {
        if (alloc == other.get_allocator()) {
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            node_qt_ = other.node_qt_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
            other.node_qt_ = 0;
        } else {
            for (auto&& item : other) {
                push_back(std::move(item));
            }
        }
    }

    unrolled_list(std::initializer_list<T> il, const Allocator& alloc = Allocator())
        : allocator_(alloc), node_allocator_(alloc) {
        Node* ptr = NodeAllocatorTraits::allocate(node_allocator_, 1);
        NodeAllocatorTraits::construct(node_allocator_, ptr, allocator_);
        head_ = ptr;
        tail_ = ptr;
        size_ = 0;
        node_qt_ = 1;
        for (const auto& item : il) {
            push_back(item);
        }
    }

    ~unrolled_list() noexcept {
        clear();
    }

    unrolled_list& operator=(std::initializer_list<T> il) {
        clear();
        for (const auto& item : il) {
            push_back(item);
        }
        return *this;
    }

    unrolled_list& operator=(const unrolled_list& other) {
        if (this != &other) {
            clear();
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }

    unrolled_list& operator=(unrolled_list&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            node_qt_ = other.node_qt_;
            allocator_ = std::move(other.allocator_);
            node_allocator_ = std::move(other.node_allocator_);
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
            other.node_qt_ = 0;
        }
        return *this;
    }

    allocator_type get_allocator() const noexcept {
        return allocator_;
    }

    bool empty() const {
        return size_ == 0;
    }

    size_type size() const {
        return size_;
    }

    size_type max_size() {
        return node_qt_ * NodeMaxSize;
    }

    void clear() noexcept {
        while (head_) {
            Node* next = head_->next_;
            NodeAllocatorTraits::destroy(node_allocator_, head_);
            NodeAllocatorTraits::deallocate(node_allocator_, head_, 1);
            head_ = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
        node_qt_ = 0;
    }

    void push_back(const T& value) {
        if (tail_->size_ < NodeMaxSize) {
            alloc_traits::construct(allocator_, tail_->array_ + tail_->size_, value);
            tail_->size_++;
        } else {
            Node* new_node = NodeAllocatorTraits::allocate(node_allocator_, 1);
            try {
                NodeAllocatorTraits::construct(node_allocator_, new_node, allocator_);
                alloc_traits::construct(allocator_, new_node->array_, value);
                new_node->size_ = 1;
                new_node->prev_ = tail_;
                tail_->next_ = new_node;
                tail_ = new_node;
                node_qt_++;
            } catch (...) {
                NodeAllocatorTraits::deallocate(node_allocator_, new_node, 1);
                throw;
            }
        }
        size_++;
    }

    void pop_back() noexcept {
        if (tail_->size_ > 1) {
            alloc_traits::destroy(allocator_, tail_->array_ + tail_->size_ - 1);
            tail_->size_--;
        } else {
            Node* prev = tail_->prev_;
            NodeAllocatorTraits::destroy(node_allocator_, tail_);
            NodeAllocatorTraits::deallocate(node_allocator_, tail_, 1);

            if (prev) {
                prev->next_ = nullptr;
                tail_ = prev;
            } else {
                head_ = tail_ = nullptr;
            }
            node_qt_--;
        }
        size_--;
    }

    void push_front(const T& value) {
        if (head_->size_ < NodeMaxSize) {
            for (size_type i = head_->size_; i > 0; --i) {
                alloc_traits::construct(allocator_, head_->array_ + i, std::move(*(head_->array_ + i - 1)));
                alloc_traits::destroy(allocator_, head_->array_ + i - 1);
            }
            alloc_traits::construct(allocator_, head_->array_, value);
            head_->size_++;
        } else {
            Node* new_node = NodeAllocatorTraits::allocate(node_allocator_, 1);
            try {
                NodeAllocatorTraits::construct(node_allocator_, new_node, allocator_);
                alloc_traits::construct(allocator_, new_node->array_, value);
                new_node->size_ = 1;
                new_node->next_ = head_;
                head_->prev_ = new_node;
                head_ = new_node;
                node_qt_++;
            } catch (...) {
                NodeAllocatorTraits::deallocate(node_allocator_, new_node, 1);
                throw;
            }
        }
        size_++;
    }

    void pop_front() noexcept {
        if (head_->size_ > 1) {
            alloc_traits::destroy(allocator_, head_->array_);
            for (size_type i = 1; i < head_->size_; ++i) {
                alloc_traits::construct(allocator_, head_->array_ + i - 1, std::move(*(head_->array_ + i)));
                alloc_traits::destroy(allocator_, head_->array_ + i);
            }
            head_->size_--;
        } else {
            Node* next = head_->next_;
            NodeAllocatorTraits::destroy(node_allocator_, head_);
            NodeAllocatorTraits::deallocate(node_allocator_, head_, 1);

            if (next) {
                next->prev_ = nullptr;
                head_ = next;
            } else {
                head_ = tail_ = nullptr;
            }
            node_qt_--;
        }
        size_--;
    }
    void insert(size_type index, const T& value) {
        if (index > size_) {
            throw std::out_of_range("Index out of range");
        }

        Node* current = head_;
        size_type node_index = 0;

        if (index == 0) {
            push_front(value);
            return;
        }

        while (current && index >= node_index + current->size_) {
            node_index += current->size_;
            current = current->next_;
        }

        if (!current) {
            throw std::out_of_range("Index out of range");
        }

        size_type pos_in_node = index - node_index;

        if (current->size_ < NodeMaxSize) {
            for (size_type i = current->size_; i > pos_in_node; --i) {
                alloc_traits::construct(allocator_, current->array_ + i, std::move(*(current->array_ + i - 1)));
                alloc_traits::destroy(allocator_, current->array_ + i - 1);
            }
            alloc_traits::construct(allocator_, current->array_ + pos_in_node, value);
            current->size_++;
        } else {
            Node* new_node = NodeAllocatorTraits::allocate(node_allocator_, 1);
            try {
                NodeAllocatorTraits::construct(node_allocator_, new_node, allocator_);

                size_type split_point = NodeMaxSize / 2;

                for (size_type i = split_point, j = 0; i < NodeMaxSize; ++i, ++j) {
                    alloc_traits::construct(allocator_, new_node->array_ + j, std::move(*(current->array_ + i)));
                    alloc_traits::destroy(allocator_, current->array_ + i);
                }

                new_node->size_ = NodeMaxSize - split_point;
                current->size_ = split_point;

                if (pos_in_node < split_point) {
                    for (size_type i = current->size_; i > pos_in_node; --i) {
                        alloc_traits::construct(allocator_, current->array_ + i, std::move(*(current->array_ + i - 1)));
                        alloc_traits::destroy(allocator_, current->array_ + i - 1);
                    }
                    alloc_traits::construct(allocator_, current->array_ + pos_in_node, value);
                    current->size_++;
                } else {
                    size_type new_pos = pos_in_node - split_point;
                    for (size_type i = new_node->size_; i > new_pos; --i) {
                        alloc_traits::construct(allocator_, new_node->array_ + i, std::move(*(new_node->array_ + i - 1)));
                        alloc_traits::destroy(allocator_, new_node->array_ + i - 1);
                    }
                    alloc_traits::construct(allocator_, new_node->array_ + new_pos, value);
                    new_node->size_++;
                }

                new_node->next_ = current->next_;
                if (current->next_) {
                    current->next_->prev_ = new_node;
                }
                current->next_ = new_node;
                new_node->prev_ = current;

                if (current == tail_) {
                    tail_ = new_node;
                }
                node_qt_++;
            } catch (...) {
                NodeAllocatorTraits::deallocate(node_allocator_, new_node, 1);
                throw;
            }
        }

        size_++;
    }

    iterator insert(iterator pos, const T& value) {
        size_type index = std::distance(begin(), pos);
        insert(index, value);
        return iterator(pos.current_node_, pos.current_index_);
    }
    iterator insert(const_iterator pos, const T& value) {
        size_type index = std::distance(cbegin(), pos);
        insert(index, value);
        return iterator(pos.current_node_, pos.current_index_);
    }
    iterator insert(const_iterator pos, T&& value) {
        size_type index = std::distance(cbegin(), pos);
        insert(index, std::move(value));
        return iterator(pos.current_node_, pos.current_index_ + 1);
    }

    iterator insert(const_iterator pos, size_type count, const T& value) {
        size_type index = std::distance(cbegin(), pos);
        for (size_type i = 0; i < count; ++i) {
            insert(index + i, value);
        }
        return iterator(pos.current_node_, pos.current_index_ + count);
    }

    template <class InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type index = std::distance(cbegin(), pos);
        size_type offset = 0;
        for (auto it = first; it != last; ++it, ++offset) {
            insert(index + offset, *it);
        }
        return iterator(pos.current_node_, pos.current_index_ + offset);
    }

    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        size_type index = std::distance(cbegin(), pos);
        size_type offset = 0;
        for (const auto& value : ilist) {
            insert(index + offset, value);
            ++offset;
        }
        return iterator(pos.current_node_, pos.current_index_ + offset);
    }

    void erase(size_type index) noexcept {
        if (index >= size_) {
            return;
        }

        if (index == 0) {
            pop_front();
            return;
        } else if (index == size_ - 1) {
            pop_back();
            return;
        }

        Node* current = head_;
        size_type node_index = 0;
        while (current && index >= node_index + current->size_) {
            node_index += current->size_;
            current = current->next_;
        }

        if (!current) {
            return;
        }

        size_type pos_in_node = index - node_index;

        alloc_traits::destroy(allocator_, current->array_ + pos_in_node);
        for (size_type i = pos_in_node + 1; i < current->size_; ++i) {
            alloc_traits::construct(allocator_, current->array_ + i - 1, std::move(*(current->array_ + i)));
            alloc_traits::destroy(allocator_, current->array_ + i);
        }
        current->size_--;

        if (current->size_ == 0 && current != head_) {
            if (current->prev_) {
                current->prev_->next_ = current->next_;
            }
            if (current->next_) {
                current->next_->prev_ = current->prev_;
            }
            if (current == tail_) {
                tail_ = current->prev_;
            }
            NodeAllocatorTraits::destroy(node_allocator_, current);
            NodeAllocatorTraits::deallocate(node_allocator_, current, 1);
            node_qt_--;
        }

        size_--;
    }
    iterator erase(const_iterator pos) {
        size_type index = 0;
        Node* current = head_;
        while (current && !(pos.current_node_ == current && pos.current_index_ == index)) {
            index += current->size_;
            current = current->next_;
        }
        if (current) {
            index += pos.current_index_;
        }
        erase(index);
        return iterator(current, pos.current_index_);
    }

    iterator erase(const_iterator first, const_iterator last) {
        size_type start_index = 0, end_index = 0;
        Node* start_node = head_;
        Node* end_node = head_;

        while (start_node && !(first.current_node_ == start_node && first.current_index_ == start_index)) {
            start_index += start_node->size_;
            start_node = start_node->next_;
        }
        if (start_node) {
            start_index += first.current_index_;
        }

        while (end_node && !(last.current_node_ == end_node && last.current_index_ == end_index)) {
            end_index += end_node->size_;
            end_node = end_node->next_;
        }
        if (end_node) {
            end_index += last.current_index_;
        }

        for (size_type i = start_index; i < end_index; ++i) {
            erase(start_index);
        }
        return iterator(start_node, first.current_index_);
    }
    void print() const {
        Node* current = head_;
        while (current) {
            for (size_type i = 0; i < current->size_; ++i) {
                std::cout << *(current->array_ + i) << " ";
            }
            current = current->next_;
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};

template <typename T, size_t NodeMaxSize, typename Allocator>
bool operator==(const unrolled_list<T, NodeMaxSize, Allocator>& lhs,
                const unrolled_list<T, NodeMaxSize, Allocator>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }

    auto lhs_it = lhs.begin();
    auto rhs_it = rhs.begin();
    for (; lhs_it != lhs.end() && rhs_it != rhs.end(); ++lhs_it, ++rhs_it) {
        if (*lhs_it != *rhs_it) {
            return false;
        }
    }

    return true;
}