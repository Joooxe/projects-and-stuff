template<size_t N>
class StackStorage {
private:
  char storage[N];
public:
  size_t weight = 0;

  StackStorage() = default;
  StackStorage(const StackStorage&) = delete;

  uintptr_t get_start_address();
};

template<typename T, size_t N = 10'000>
class StackAllocator {
public:
  using value_type = T;

  StackStorage<N>* alloc_storage;

  template <class U>
  struct rebind;

  template <typename U>
  constexpr StackAllocator(const StackAllocator<U, N>& other) noexcept;
  explicit StackAllocator(StackStorage<N>& storage);
  ~StackAllocator() = default;

  constexpr T* allocate(size_t size);
  constexpr void deallocate(T*, size_t) noexcept;

  template<typename... Args>
  static void construct(T* ptr, const Args&... args);
  static void destroy(T* ptr);

  static constexpr StackAllocator<T, N> select_on_container_copy_construction(const StackAllocator& other);

  template <typename U>
  StackAllocator<T, N>& operator=(const StackAllocator<U, N>& other);

  template <typename U>
  bool operator==(const StackAllocator<U, N>& other) const noexcept;
};

template<typename T, typename Alloc = std::allocator<T>>
class List {
private:
  struct BaseNode;
  struct Node;

  template<bool isConst>
  class BidirIter {
  public:
    using value_type = typename std::conditional<isConst, const T, T>::type;
    using node_type = typename std::conditional<isConst, const BaseNode*, BaseNode*>::type;
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;

    BaseNode* current;

    BidirIter() = default;
    BidirIter(node_type node);
    BidirIter(const BidirIter<false>& other);

    value_type& operator*() const;
    value_type* operator->() const noexcept;

    BidirIter& operator++();
    BidirIter<isConst> operator++(int);
    BidirIter& operator--();
    BidirIter<isConst> operator--(int);

    bool operator==(const BidirIter& other) const;
    bool operator!=(const BidirIter& other) const;

    BidirIter& operator=(const BidirIter<false>& other);

    Node* get_node() const;
    BidirIter base() const;
  };

public:
  using iterator = BidirIter<false>;
  using const_iterator = BidirIter<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using NodeAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
  using AllocTraits = std::allocator_traits<NodeAlloc>;

  List();
  List(const Alloc& allocator);
  List(const List<T, Alloc>& other);
  List(size_t size, const T& elem, const Alloc& allocator = Alloc());
  List(size_t size, const Alloc& allocator = Alloc());
  ~List() noexcept;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  size_t size() const;

  void insert(const_iterator pos, const T& elem);

  void push_back(const T& elem);

  void push_front(const T& elem);

  void erase(const_iterator pos);

  void pop_back();

  void pop_front();

  List& operator=(const List<T, Alloc>& other);

  Alloc get_allocator() const;
private:
  BaseNode fakeNode;
  size_t sz = 0;
  NodeAlloc alloc;
};

template<size_t N>
uintptr_t StackStorage<N>::get_start_address() {
  return reinterpret_cast<uintptr_t>(&storage);
}

template<typename T, size_t N>
template <class U>
struct StackAllocator<T, N>::rebind {
  using other = StackAllocator<U, N>;
};
//TODO----------------------------------------------

template<typename T, size_t N>
template <typename U>
constexpr StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& other) noexcept : alloc_storage(other.alloc_storage) {}

template<typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& storage) : alloc_storage(&storage) {}

template<typename T, size_t N>
constexpr T* StackAllocator<T, N>::allocate(size_t size) {
  uintptr_t st_address = alloc_storage->get_start_address();
  size_t free_space = N - alloc_storage->weight;
  size_t offset = alignof(T) - (st_address + alloc_storage->weight) & (alignof(T) - 1); //& (alignof(T) - 1) same as % alignof(T) but for degree of 2
  if (offset + size * sizeof(T) > free_space) {
    std::cout << "Stack Overflow\n";
    return nullptr;
  }
  T* ptr = reinterpret_cast<T*>(st_address + offset + alloc_storage->weight);
  alloc_storage->weight += offset + size * sizeof(T);
  return ptr;
}

template<typename T, size_t N>
constexpr void StackAllocator<T, N>::deallocate(T*, size_t) noexcept {
  //nothing..?
}

template<typename T, size_t N>
template<typename... Args>
void StackAllocator<T, N>::construct(T* ptr, const Args&... args) {
  ::new(ptr) T(args...);
}

template<typename T, size_t N>
constexpr StackAllocator<T, N> StackAllocator<T, N>::select_on_container_copy_construction(const StackAllocator& other) {
  return other;
}

template<typename T, size_t N>
void StackAllocator<T, N>::destroy(T* ptr) {
  ptr->~T();
}

template<typename T, size_t N>
template <typename U>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(const StackAllocator<U, N>& other) {
  if (this == &other) {
    return *this;
  }
  alloc_storage = other.storage;
  return *this;
}

template<typename T, size_t N>
template <typename U>
bool StackAllocator<T, N>::operator==(const StackAllocator<U, N>& other) const noexcept {
  return alloc_storage == other.storage;
}

template<typename T, typename Alloc>
struct List<T, Alloc>::BaseNode {
  BaseNode* prev = nullptr;
  BaseNode* next = nullptr;

  BaseNode() = default;

  BaseNode(BaseNode* prev, BaseNode* next) : prev(prev), next(next){};
};

template<typename T, typename Alloc>
struct List<T, Alloc>::Node : List<T, Alloc>::BaseNode {
  T value;

  Node() : BaseNode() {}

  Node(BaseNode* prev, BaseNode* next) : BaseNode(prev, next) {}

  Node(BaseNode* prev, BaseNode* next, const T& value) : BaseNode(prev, next), value(value) {}
};

template<typename T, typename Alloc>
template<bool isConst>
List<T, Alloc>::BidirIter<isConst>::BidirIter(List<T, Alloc>::BidirIter<isConst>::node_type node) : current(node->prev->next) {}

template<typename T, typename Alloc>
template<bool isConst>
List<T, Alloc>::BidirIter<isConst>::BidirIter(const BidirIter<false>& other) : current(other.current) {}

template<typename T, typename Alloc>
template<bool isConst>
List<T, Alloc>::BidirIter<isConst>::value_type&
List<T, Alloc>::BidirIter<isConst>::operator*() const {
  return static_cast<Node*>(current)->value;
}

template<typename T, typename Alloc>
template<bool isConst>
List<T, Alloc>::BidirIter<isConst>::value_type*
List<T, Alloc>::BidirIter<isConst>::operator->() const noexcept {
  return &(static_cast<Node*>(current)->value);
}

template <typename T, typename Alloc>
template <bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>&
List<T, Alloc>::BidirIter<isConst>::operator++() {
  current = current->next;
  return *this;
}

template <typename T, typename Alloc>
template <bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>
List<T, Alloc>::BidirIter<isConst>::operator++(int) {
  BidirIter<isConst> copy(*this);
  current = current->next;
  return copy;
}

template<typename T, typename Alloc>
template<bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>&
List<T, Alloc>::BidirIter<isConst>::operator--() {
  current = current->prev;
  return *this;
}

template<typename T, typename Alloc>
template<bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>
List<T, Alloc>::BidirIter<isConst>::operator--(int) {
  BidirIter<isConst> copy(*this);
  current = current->prev;
  return copy;
}


template<typename T, typename Alloc>
template<bool isConst>
bool List<T, Alloc>::BidirIter<isConst>::operator==(const BidirIter& other) const {
  return current == other.current;
}

template<typename T, typename Alloc>
template<bool isConst>
bool List<T, Alloc>::BidirIter<isConst>::operator!=(const BidirIter& other) const {
  return !(current == other.current);
}

template<typename T, typename Alloc>
template<bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>&
List<T, Alloc>::BidirIter<isConst>::operator=(const BidirIter<false>& other) {
  if (this == &other) {
    return *this;
  }
  current = other.current;
  return *this;
}


template<typename T, typename Alloc>
template<bool isConst>
List<T, Alloc>::Node*
List<T, Alloc>::BidirIter<isConst>::get_node() const {
  return static_cast<Node*>(current);
}

template<typename T, typename Alloc>
template<bool isConst>
typename List<T, Alloc>::template BidirIter<isConst>
List<T, Alloc>::BidirIter<isConst>::base() const {
  return BidirIter(current);
}

template<typename T, typename Alloc>
List<T, Alloc>::List() : fakeNode{&fakeNode, &fakeNode}, sz(0) {};

template<typename T, typename Alloc>
List<T, Alloc>::List(const Alloc& allocator) : fakeNode{&fakeNode, &fakeNode}, alloc(AllocTraits::select_on_container_copy_construction(allocator)) {}

template<typename T, typename Alloc>
List<T, Alloc>::List(const List<T, Alloc>& other) : List(other.get_allocator()) {
  try {
    for (auto it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
  } catch(...) {
    while (sz > 0) {
      pop_back();
    }
    throw;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t size, const T& elem, const Alloc& allocator) : alloc(allocator) {
  size_t i = 0;
  try {
    for (; i < size; ++i) {
      push_back(elem);
    }
  } catch(...) {
    while (sz > 0) {
      pop_back();
    }
    throw;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t size, const Alloc& allocator) : List(allocator) {
  size_t i = 0;
  try {
    for (; i < size; ++i) {
      Node* node = AllocTraits::allocate(alloc, 1);
      try {
        AllocTraits::construct(alloc, node, fakeNode.prev, &fakeNode);
        fakeNode.prev->next = node;
        fakeNode.prev = node;
        ++sz;
      } catch (...) {
        AllocTraits::deallocate(alloc, node, 1);
        throw;
      }
    }
  } catch(...) {
    for(size_t j = i; j != 0; --j) {
      pop_back();
    }
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::~List() noexcept {
  while (sz > 0) {
    pop_back();
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::iterator
List<T, Alloc>::begin() {
  return iterator(fakeNode.next);
}

template<typename T, typename Alloc>
List<T, Alloc>::iterator
List<T, Alloc>::end() {
  return iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator
List<T, Alloc>::begin() const {
  return const_iterator(fakeNode.next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator
List<T, Alloc>::end() const {
  return const_iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator
List<T, Alloc>::cbegin() const {
  return const_iterator(fakeNode.next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_iterator
List<T, Alloc>::cend() const {
  return const_iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::reverse_iterator
List<T, Alloc>::rbegin() {
  return reverse_iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::reverse_iterator
List<T, Alloc>::rend() {
  return reverse_iterator(fakeNode.next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator
List<T, Alloc>::rbegin() const {
  return const_reverse_iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator
List<T, Alloc>::rend() const {
  return const_reverse_iterator(fakeNode.next);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator
List<T, Alloc>::crbegin() const {
  return const_reverse_iterator(&fakeNode);
}

template<typename T, typename Alloc>
List<T, Alloc>::const_reverse_iterator
List<T, Alloc>::crend() const {
  return const_reverse_iterator(fakeNode.next);
}

template<typename T, typename Alloc>
size_t List<T, Alloc>::size() const {
  return sz;
}

template<typename T, typename Alloc>
void List<T, Alloc>::insert(const_iterator pos, const T& elem) {
  Node* new_node = AllocTraits::allocate(alloc, 1);
  try {
    AllocTraits::construct(alloc, new_node, pos.get_node()->prev, pos.get_node(), elem);
    pos.get_node()->prev->next = new_node;
    pos.get_node()->prev = new_node;
    ++sz;
  } catch(...) {
    AllocTraits::deallocate(alloc, new_node, 1);
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_back(const T& elem) {
  try {
    insert(end(), elem);
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::push_front(const T& elem) {
  try {
    insert(begin(), elem);
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::erase(const_iterator pos) {
  Node* to_del = pos.get_node();
  //  to_del->prev->next = to_del->next;
  //  to_del->next->prev = to_del->prev;
  auto prev_node = to_del->prev;
  auto next_node = to_del->next;
  try {
    AllocTraits::destroy(alloc, to_del);
    prev_node->next = next_node;  // upd pointers only if destroy done well
    next_node->prev = prev_node;
    AllocTraits::deallocate(alloc, to_del, 1);
    --sz;
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_back() {
  try {
    erase(--end());
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
void List<T, Alloc>::pop_front() {
  try {
    erase(begin());
  } catch(...) {
    throw;
  }
}

template<typename T, typename Alloc>
List<T, Alloc>&
List<T, Alloc>::operator=(const List<T, Alloc>& other) {
  if (this == &other) {
    return *this;
  }
  if (AllocTraits::propagate_on_container_copy_assignment::value) {
    alloc = other.alloc;
  }
  while (sz > 0) {
    (*this).pop_back();
  }
  int cnt = 0;
  try {
    for (const auto &elem : other) {
      push_back(elem);
      ++cnt;
    }
  } catch(...) {
    for (int i = 0; i < cnt; ++i) {
      pop_back();
    }
  }
  return *this;
}

template<typename T, typename Alloc>
Alloc List<T, Alloc>::get_allocator() const {
  return alloc;
}