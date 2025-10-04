#include <iostream>
#include <memory>

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

struct BaseControlBlock {
  size_t shared_count = 0;
  size_t weak_count = 0;

  BaseControlBlock(size_t shared_count, size_t weak_count)
      : shared_count(shared_count), weak_count(weak_count) {}

  virtual ~BaseControlBlock() = default;

  virtual void destroy() = 0;
  virtual void deallocate() = 0;
};

template <typename U, typename Deleter = std::default_delete<U>, typename Alloc = std::allocator<U>>
struct ControlBlockRegular : BaseControlBlock, private Deleter, private Alloc {
  using Allocator = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockRegular<U, Deleter, Alloc>>;
  U* ptr;

  ControlBlockRegular(Alloc alloc, Deleter del, U* ptr)
      : BaseControlBlock(1, 0), Deleter(std::move(del)), Alloc(std::move(alloc)), ptr(ptr) {}

  void destroy() override {
    static_cast<Deleter&>(*this)(ptr);
  }

  void deallocate() override {
    Allocator alloc(static_cast<Alloc&>(*this));
    std::allocator_traits<Allocator>::deallocate(alloc, this, 1);
  }
};


template <typename U, typename Alloc>
struct ControlBlockMakeShared : BaseControlBlock {
  Alloc alloc;
  std::aligned_storage_t<sizeof(U), alignof(U)> storage;

  template <typename... Args>
  ControlBlockMakeShared(Alloc alloc, Args&&... args) : BaseControlBlock(1, 0), alloc(alloc) {
    std::allocator_traits<Alloc>::construct(alloc, reinterpret_cast<U*>(&storage), std::forward<Args>(args)...);
  }

  U* getPtr() {
    return reinterpret_cast<U*>(&storage);
  }

  void destroy() override {
    std::allocator_traits<Alloc>::destroy(alloc, getPtr());
  }

  void deallocate() override {
    using AllocCB = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared<U, Alloc>>;
    AllocCB alloc_cb(alloc);
    std::allocator_traits<AllocCB>::deallocate(alloc_cb, this, 1);
  }
};

template <typename T>
class SharedPtr {
  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;

  template <typename U, typename Alloc, typename... Args>
  friend SharedPtr<U> allocateShared(Alloc alloc, Args&&... args);

private:

  T* ptr = nullptr;
  BaseControlBlock* p_count = nullptr;

  template <typename U, typename Alloc>
  explicit SharedPtr(ControlBlockMakeShared<U, Alloc>* cb) : ptr(cb->getPtr()), p_count(cb) {}

  void dec() {
    if (p_count == nullptr) {
      return;
    }
    if (--(p_count->shared_count) == 0) {
      p_count->destroy();
    }
    if (p_count->shared_count == 0 && p_count->weak_count == 0) {
      p_count->deallocate();
      p_count = nullptr;
      ptr = nullptr;
    }
  }

public:
  SharedPtr() = default;

  SharedPtr(const SharedPtr<T>& other) : ptr(other.ptr), p_count(other.p_count) {
    if (p_count != nullptr) {
      ++(p_count->shared_count);
    }
  }

  SharedPtr(SharedPtr<T>&& sp) : ptr(sp.ptr), p_count(sp.p_count) {
    sp.ptr = nullptr;
    sp.p_count = nullptr;
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& other)
      : ptr(other.ptr),
        p_count(static_cast<BaseControlBlock*>(other.p_count)) {
    if (p_count != nullptr) {
      ++(p_count->shared_count);
    }
  }

  template <typename U>
  SharedPtr(SharedPtr<U>&& sp) : ptr(sp.ptr), p_count(sp.p_count) {
    sp.ptr = nullptr;
    sp.p_count = nullptr;
  }

  SharedPtr(const WeakPtr<T>& weak_ptr) : ptr(weak_ptr.ptr), p_count(weak_ptr.p_count) {
    if (p_count != nullptr) {
      ++(p_count->shared_count);
    }
  }

  template <typename Y, typename Deleter = std::default_delete<Y>, typename Alloc = std::allocator<Y>>
  SharedPtr(Y* ptr, Deleter del = Deleter(), Alloc alloc = Alloc())
      : ptr(static_cast<T*>(ptr)) {
    using AllocCB = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockRegular<Y, Deleter, Alloc>>;
    AllocCB alloc_cb(alloc);
    ControlBlockRegular<Y, Deleter, Alloc>* cb = std::allocator_traits<AllocCB>::allocate(alloc_cb, 1);
    new(cb) ControlBlockRegular<Y, Deleter, Alloc>(alloc, std::move(del), ptr);
    p_count = cb;
  }

  template <class Y>
  SharedPtr(const SharedPtr<Y>& other, T* ptr) noexcept : ptr(ptr), p_count(other.p_count) {
    if (p_count) {
      ++(p_count->shared_count);
    }
  }

  ~SharedPtr() {
    dec();
  }

  T* get() const noexcept {
    return ptr;
  }

  size_t use_count() const {
    if (p_count != nullptr) {
      return p_count->shared_count;
    }
    return 0;
  }

  void swap(SharedPtr& other) noexcept {
    std::swap(ptr, other.ptr);
    std::swap(p_count, other.p_count);
  }

  template <typename U>
  void reset(U* other) noexcept {
    SharedPtr<U> temp(other);
    swap(temp);
  }

  void reset() noexcept {
    dec();
    ptr = nullptr;
    p_count = nullptr;
  }

  SharedPtr<T>& operator=(const SharedPtr<T>& other) noexcept {
    if(&other == this) {
      return *this;
    }
    auto copy = other;
    swap(copy);
    return *this;
  }

  SharedPtr<T>& operator=(SharedPtr&& other) noexcept {
    if(&other == this) {
      return *this;
    }
    dec();
    swap(other);
    other.ptr = nullptr;
    other.p_count = nullptr;
    return *this;
  }

  T& operator*() const noexcept {
    return *get();
  }

  T* operator->() const noexcept {
    return get();
  }
};


template <typename T>
class WeakPtr {
  template <typename U>
  friend class WeakPtr;

  template <typename U>
  friend class SharedPtr;
private:
  T* ptr = nullptr;
  BaseControlBlock* p_count = nullptr;
public:
  WeakPtr() = default;

  template <typename U>
  WeakPtr(const SharedPtr<U>& sp)
      : ptr(static_cast<T*>(sp.ptr)),
        p_count(sp.p_count) {
    if (p_count != nullptr) {
      ++(p_count->weak_count);
    }
  }

  template <typename U>
  WeakPtr(SharedPtr<U>&& sp)
      : ptr(static_cast<T*>(std::move(sp.ptr))),
        p_count(static_cast<typename SharedPtr<U>::BaseControlBlock*>(std::move(sp.p_count))) {
    if (p_count != nullptr) {
      ++(p_count->weak_count);
    }
  }

  template<typename U>
  WeakPtr(const WeakPtr<U>& other)
      : ptr(static_cast<T*>(other.ptr)),
        p_count(static_cast<BaseControlBlock*>(other.p_count)) {
    if (p_count != nullptr) {
      ++(p_count->weak_count);
    }
  }

  template<typename U>
  WeakPtr(WeakPtr<U>&& other) noexcept
      : ptr(static_cast<T*>(std::move(other.ptr))),
        p_count(static_cast<BaseControlBlock*>(std::move(other.p_count))) {
    other.ptr = nullptr;
    other.p_count = nullptr;
  }

  WeakPtr(const WeakPtr& other)
      : ptr(other.ptr),
        p_count(other.p_count) {
    if (p_count != nullptr) {
      ++(p_count->weak_count);
    }
  }

  void swap(WeakPtr& other) noexcept {
    std::swap(ptr, other.ptr);
    std::swap(p_count, other.p_count);
  }

  template <typename U>
  WeakPtr& operator=(const SharedPtr<U>& other) {
    WeakPtr copy(other);
    swap(copy);
    return *this;
  }

  template <typename U>
  WeakPtr& operator=(SharedPtr<U>&& other) {
    swap(other);
    other.p_count = nullptr;
    other.ptr = nullptr;
    return *this;
  }

  size_t use_count() const {
    if (p_count) {
      return p_count->shared_count;
    }
    return 0;
  }

  bool expired() const {
    if (p_count == nullptr || p_count->shared_count == 0) {
      return true;
    }
    return false;
  }

  SharedPtr<T> lock() const {
    if (p_count == nullptr) {
      throw;
    }
    return SharedPtr<T>(*this);
  }

  void dec() {
    if (p_count == nullptr) {
      return;
    }
    --(p_count->weak_count);
    if (p_count->shared_count == 0 && p_count->weak_count == 0) {
      p_count->deallocate();
      p_count = nullptr;
      ptr = nullptr;
    }
  }

  ~WeakPtr() {
    dec();
  }
};


template <typename T>
class EnableSharedFromThis {
  friend class SharedPtr<T>;
public:
  WeakPtr<T> weak;

protected:
  EnableSharedFromThis() = default;

public:
  SharedPtr<T> shared_from_this() const {
    return weak.lock();
  }
};


template <typename T, typename Alloc, typename... Args>
SharedPtr<T> allocateShared(Alloc alloc, Args&&... args) {
  using AllocCB = typename std::allocator_traits<Alloc>::template rebind_alloc<ControlBlockMakeShared<T, Alloc>>;
  AllocCB alloc_cb(alloc);
  ControlBlockMakeShared<T, Alloc>* cb = std::allocator_traits<AllocCB>::allocate(alloc_cb, 1);
  //std::allocator_traits<AllocCB>::construct(alloc_cb, cb, alloc, std::forward<Args>(args)...);
  new(cb) ControlBlockMakeShared<T, Alloc>(alloc, std::forward<Args>(args)...);
  SharedPtr<T> sp(cb);
  if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
    cb->getPtr()->weak = sp; //weak from enableShared
  }
  return sp;
}

template <typename T, typename... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}
