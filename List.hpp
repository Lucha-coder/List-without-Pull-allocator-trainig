#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

template <typename T>
class List {
   private:
    struct BaseNode_ {
        BaseNode_* prev;
        BaseNode_* next;

        BaseNode_() : prev(nullptr), next(nullptr) {}

        virtual ~BaseNode_() = default;
    };

    struct Node : BaseNode_ {
        T value_;

        template <typename... Args>
        explicit Node(Args&&... args) : BaseNode_(), value_(std::forward<Args>(args)...) {}
    };

   private:
    size_t sz_;
    BaseNode_* head_;
    BaseNode_* tail_;

   private:
    template <bool IsConst>
    class base_iterator {
       public:
        using value_type = T;
        using pointer_type = std::conditional_t<IsConst, const BaseNode_*, BaseNode_*>;
        using reference_type = std::conditional_t<IsConst, const BaseNode_&, BaseNode_&>;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

       private:
        friend class List;

        template <bool>
        friend class base_iterator;

        pointer_type ptr;

        using pointer_value_type = std::conditional_t<IsConst, const T*, T*>;
        using reference_value_type = std::conditional_t<IsConst, const T&, T&>;

        using node_pointer = std::conditional_t<IsConst, const Node*, Node*>;

       public:
        base_iterator() : ptr(nullptr) {}

        base_iterator(const base_iterator& other) : ptr(other.ptr) {}

        template <bool otherconst>
            requires(IsConst && !otherconst)
        explicit base_iterator(const base_iterator<otherconst>& other) : ptr(other.ptr) {}

        explicit base_iterator(pointer_type ptr) : ptr(ptr) {}

        base_iterator& operator=(const base_iterator& other) {
            ptr = other.ptr;
            return *this;
        }

        template <bool otherconst>
        bool operator==(const base_iterator<otherconst>& other) const noexcept {
            return ptr == other.ptr;
        }

        template <bool otherconst>
        bool operator!=(const base_iterator<otherconst>& other) const noexcept {
            return ptr != other.ptr;
        }

        base_iterator& operator++() {
            ptr = ptr->next;
            return *this;
        }

        base_iterator& operator--() {
            ptr = ptr->prev;
            return *this;
        }

        base_iterator operator++(int) {
            base_iterator copy(*this);
            ptr = ptr->next;
            return copy;
        }

        base_iterator operator--(int) {
            base_iterator copy(*this);
            ptr = ptr->prev;
            return copy;
        }

        pointer_value_type operator->() const {
            return std::addressof(static_cast<node_pointer>(ptr)->value_);
        }

        reference_value_type operator*() const {
            return static_cast<node_pointer>(ptr)->value_;
        }
    };

   public:
    using iterator = base_iterator<false>;
    using const_iterator = base_iterator<true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() {
        return iterator(head_);
    }

    iterator end() {
        return iterator(tail_);
    }

    const_iterator begin() const {
        return const_iterator(head_);
    }

    const_iterator end() const {
        return const_iterator(tail_);
    }

    const_iterator cbegin() const {
        return const_iterator(head_);
    }

    const_iterator cend() const {
        return const_iterator(tail_);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(cbegin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(cbegin());
    }

   public:
    List() : sz_(0), head_(new BaseNode_()), tail_(head_) {}

    explicit List(size_t n);
    List(size_t n, const T& obj);

    List(const List& other);
    List(List&& other);  // NOLINT

    List& operator=(const List& other);
    List& operator=(List&& other);  // NOLINT

    void PushBack(const T& obj);
    void PushBack(T&& obj);
    void PushFront(const T& obj);
    void PushFront(T&& obj);

    template <typename... Args>
    void EmplaceBack(Args&&... args);

    template <typename... Args>
    void EmplaceFront(Args&&... args);

    void swap(List& other) noexcept;

    ~List() noexcept;
};

template <typename T>
template <typename... Args>
void List<T>::EmplaceBack(Args&&... args) {
    Node* newNode = new Node(std::forward<Args>(args)...);
    if (sz_ == 0) {
        tail_->prev = newNode;
        newNode->next = tail_;
        head_ = newNode;
        ++sz_;
        return;
    }
    tail_->prev->next = newNode;
    newNode->prev = tail_->prev;
    newNode->next = tail_;
    tail_->prev = newNode;
    ++sz_;
}

template <typename T>
template <typename... Args>
void List<T>::EmplaceFront(Args&&... args) {
    Node* newNode = new Node(std::forward<Args>(args)...);
    if (sz_ == 0) {
        tail_->prev = newNode;
        newNode->next = tail_;
        head_ = newNode;
        ++sz_;
        return;
    }
    head_->prev = newNode;
    newNode->next = head_;
    head_ = newNode;
    ++sz_;
}

template <typename T>
void List<T>::PushBack(const T& obj) {
    EmplaceBack(obj);
}

template <typename T>
void List<T>::PushBack(T&& obj) {
    EmplaceBack(std::move(obj));
}

template <typename T>
void List<T>::PushFront(const T& obj) {
    EmplaceFront(obj);
}

template <typename T>
void List<T>::PushFront(T&& obj) {
    EmplaceFront(std::move(obj));
}

template <typename T>
void List<T>::swap(List<T>& other) noexcept {
    std::swap(sz_, other.sz_);
    std::swap(head_, other.head_);
    std::swap(tail_, other.tail_);
}

template <typename T>
List<T>::List(size_t n) : List() {
    for (size_t i = 0; i < n; ++i) {
        EmplaceBack();
    }
}

template <typename T>
List<T>::List(size_t n, const T& obj) : List() {
    for (size_t i = 0; i < n; ++i) {
        PushBack(obj);
    }
}

template <typename T>
List<T>::List(const List& other) : List() {
    auto it = other.begin();
    while (it != other.end()) {
        PushBack(*it);
        ++it;
    }
}

template <typename T>
List<T>::List(List&& other) : List() {  // NOLINT
    swap(other);
}

template <typename T>
List<T>& List<T>::operator=(const List& other) {
    if (this == &other) {
        return *this;
    }
    List copy(other);
    swap(copy);
    return *this;
}

template <typename T>
List<T>& List<T>::operator=(List&& other) {  // NOLINT
    if (this != &other) {
        List moved(std::move(other));
        swap(moved);
    }
    return *this;
}

template <typename T>
List<T>::~List() noexcept {
    BaseNode_* current = head_;

    while (current != tail_) {
        BaseNode_* nextNode = current->next;
        delete current;
        current = nextNode;
    }

    delete tail_;
}
