#include <iostream>
#include <stdexcept>
template<typename T>
class Deque{
private:
  template<bool isConst>
  class le_iterator {
    friend class Deque;
  public:
    using value_type = typename std::conditional<isConst, const T, T>::type;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = int;

    le_iterator() = default;
    le_iterator(T** cont, size_t out_ind, size_t in_ind, size_t cont_cap);
    le_iterator(const le_iterator<false>& other);

    value_type& operator*() const;
    value_type* operator->() const;

    le_iterator<isConst>& operator++();
    le_iterator<isConst> operator++(int);
    le_iterator<isConst>& operator--();
    le_iterator<isConst> operator--(int);
    le_iterator<isConst>& operator+=(int n);
    le_iterator<isConst> operator+(int n) const;
    le_iterator<isConst>& operator-=(int n);
    le_iterator<isConst> operator-(int n) const;
    le_iterator<isConst>& operator=(const le_iterator<false>& other);
    int operator-(const le_iterator<isConst>& other) const;

    bool operator==(const le_iterator<isConst>& other) const;
    bool operator!=(const le_iterator<isConst>& other) const;
    bool operator<(const le_iterator<isConst>& other) const;
    bool operator>(const le_iterator<isConst>& other) const;
    bool operator<=(const le_iterator<isConst>& other) const;
    bool operator>=(const le_iterator<isConst>& other) const;
  private:
    T** cont = nullptr;
    size_t out_ind = 0;
    size_t in_ind = 0;
    size_t cont_cap = 0;
    value_type* current = nullptr;
    void increment();
    void decrement();
    void update_current();
  };

public:
  using iterator = le_iterator<false> ;
  using const_iterator = le_iterator<true> ;
  using reverse_iterator = std::reverse_iterator<iterator> ;
  using const_reverse_iterator = std::reverse_iterator<const_iterator> ;

  Deque();
  Deque(size_t sz);
  Deque(const Deque& other);
  Deque(size_t sz, const T& elem);
  ~Deque();

  size_t size() const;

  void push_back(const T& elem);
  void push_front(const T& elem);
  void pop_back();
  void pop_front();

  iterator erase(iterator it);
  iterator insert(iterator it, const T &elem);

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  const_iterator  cbegin() const;
  const_iterator cend() const;

  reverse_iterator rbegin();
  reverse_iterator rend();

  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

  T& at(size_t ind);
  const T& at(size_t ind) const;

  T& operator[](size_t ind);
  const T& operator[](size_t ind) const;

  Deque& operator=(const Deque& other);

private:
  void extend_outer();

  static constexpr size_t block_sz_ = 16;
  T** out_arr_ = nullptr;
  size_t sz_deque_ = 0;
  size_t out_cap_ = 0;
  iterator begin_;
  iterator end_;
};

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>::le_iterator(T** cont, size_t out_ind, size_t in_ind, size_t cont_cap)
    : cont(cont), out_ind(out_ind), in_ind(in_ind), cont_cap(cont_cap), current(cont[out_ind]) {}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>::le_iterator(const le_iterator<false>& other)
    : cont(other.cont), out_ind(other.out_ind), in_ind(other.in_ind), cont_cap(other.cont_cap), current(other.current) {}

template <typename T>
template <bool isConst>
void Deque<T>::le_iterator<isConst>::increment() {
  ++in_ind;
  if (in_ind == block_sz_) {
    ++out_ind;
    in_ind = 0;
    update_current();
  }
}

template <typename T>
template <bool isConst>
void Deque<T>::le_iterator<isConst>::decrement() {
  if (in_ind == 0) {
    --out_ind;
    in_ind = block_sz_ - 1;
    update_current();
  } else {
    --in_ind;
  }
}

template <typename T>
template <bool isConst>
void Deque<T>::le_iterator<isConst>::update_current() {
  if (out_ind < cont_cap) {
    current = cont[out_ind];
  }
}

template <typename T>
template <bool isConst>
std::conditional<isConst, const T, T>::type&
Deque<T>::le_iterator<isConst>::operator*() const {
  return *(current + in_ind);
}

template <typename T>
template <bool isConst>
std::conditional<isConst, const T, T>::type*
Deque<T>::le_iterator<isConst>::operator->() const {
  return current + in_ind;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>
Deque<T>::le_iterator<isConst>::operator++(int) {
  Deque<T>::le_iterator copy(*this);
  ++(*this);
  return copy;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>&
Deque<T>::le_iterator<isConst>::operator++() {
  increment();
  return *this;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>
Deque<T>::le_iterator<isConst>::operator--(int) {
  le_iterator<isConst> copy(*this);
  --(*this);
  return copy;
}


template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>&
Deque<T>::le_iterator<isConst>::operator--() {
  decrement();
  return *this;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>&
Deque<T>::le_iterator<isConst>::operator+=(int n) {
  if (n >= 0) {
    int offset = (n + in_ind) / block_sz_;
    out_ind += offset;
    if (offset != 0) {
      update_current();
    }
    in_ind = (in_ind + n % block_sz_) % block_sz_;
  } else {
    int offset = (block_sz_ - 1 - n - in_ind) / block_sz_;
    out_ind -= offset;
    if (offset != 0) {
      update_current();
    }
    in_ind = (in_ind + n) % block_sz_;
  }
  return *this;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>
Deque<T>::le_iterator<isConst>::operator+(int n) const {
  le_iterator<isConst> tmp(*this);
  return tmp += n;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>&
Deque<T>::le_iterator<isConst>::operator-=(int n) {
  return (*this) += -n;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>
Deque<T>::le_iterator<isConst>::operator-(int n) const {
  le_iterator<isConst> tmp(*this);
  return tmp -= n;
}

template <typename T>
template <bool isConst>
int Deque<T>::le_iterator<isConst>::operator-(const le_iterator<isConst>& other) const {
  return (in_ind - other.in_ind) + (out_ind - other.out_ind) * block_sz_;
}

template <typename T>
template <bool isConst>
Deque<T>::le_iterator<isConst>&
Deque<T>::le_iterator<isConst>::operator=(const le_iterator<false>& other) {
  if (this == &other) {
    return *this;
  }
  cont = other.cont;
  out_ind = other.out_ind;
  in_ind = other.in_ind;
  cont_cap = other.cont_cap;
  update_current();
  return *this;
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator==(const le_iterator<isConst>& other) const {
  return out_ind == other.out_ind && in_ind == other.in_ind;
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator!=(const le_iterator<isConst>& other) const {
  return !(*this == other);
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator<(const le_iterator<isConst>& other) const {
  return (out_ind < other.out_ind) || (out_ind == other.out_ind && in_ind < other.in_ind);
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator>(const le_iterator<isConst>& other) const {
  return !(*this < other) && *this != other;
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator<=(const le_iterator<isConst>& other) const {
  return !(*this > other);
}

template <typename T>
template <bool isConst>
bool Deque<T>::le_iterator<isConst>::operator>=(const le_iterator<isConst>& other) const {
  return !(*this < other);
}

template <typename T>
Deque<T>::Deque() {
  sz_deque_ = 0;
  out_cap_ = sz_deque_ / block_sz_ + 1;
  try {
    out_arr_ = new T* [out_cap_];
  } catch(...) {
    delete[] out_arr_;
    throw;
  }
  try {
    out_arr_[0] = reinterpret_cast<T*>(new char[block_sz_ * sizeof(T)]);
    begin_ = iterator(out_arr_, 0, 0, out_cap_);
    end_ = iterator(out_arr_, 0, 0, out_cap_);
  } catch(...) {
    delete[] reinterpret_cast<char*>(out_arr_[0]);
    throw;
  }
}

template <typename T>
Deque<T>::Deque(size_t sz) : Deque(sz, T()) {}


template <typename T>
Deque<T>::Deque(const Deque& other) : sz_deque_(other.sz_deque_), out_cap_(other.out_cap_) {
  size_t i = 0;
  try {
    out_arr_ = new T* [out_cap_];
    for (; i < out_cap_; ++i) {
      out_arr_[i] = reinterpret_cast<T*>(new char[block_sz_ * sizeof(T)]);
    }

    size_t j = 0;
    try {
      for (size_t out_ind = other.begin_.out_ind; out_ind <= other.end_.out_ind; ++out_ind) {
        size_t in_ind_start = (out_ind == other.begin_.out_ind) ? other.begin_.in_ind : 0;
        size_t in_ind_end = (out_ind == other.end_.out_ind) ? other.end_.in_ind : block_sz_;

        for (size_t in_ind = in_ind_start; in_ind < in_ind_end; ++in_ind, ++j) {
          new (out_arr_[out_ind] + in_ind) T(other.out_arr_[out_ind][in_ind]);
        }
      }
    } catch(...) {
      for (size_t k = 0; k < j; ++k) {
        size_t out_ind = other.begin_.out_ind + k / block_sz_;
        size_t in_ind = k % block_sz_;
        (out_arr_[out_ind] + in_ind)->~T();
      }
      throw;
    }

  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      delete[] reinterpret_cast<char*>(out_arr_[j]);
    }
    delete[] out_arr_;
    throw;
  }

  begin_ = iterator(out_arr_, other.begin_.out_ind, other.begin_.in_ind, other.out_cap_);
  end_ = begin_ + sz_deque_;
}


template <typename T>
Deque<T>::Deque(size_t sz, const T& elem) {
  sz_deque_ = sz;
  out_cap_ = (sz + block_sz_ - 1) / block_sz_;
  size_t i = 0;
  size_t total_initialized = 0;
  try {
    out_arr_ = new T*[out_cap_];
    for (; i < out_cap_; ++i) {
      out_arr_[i] = reinterpret_cast<T*>(new char[block_sz_ * sizeof(T)]);
      size_t j = 0;
      for (; j < block_sz_ && total_initialized < sz; ++j, ++total_initialized) {
        new (out_arr_[i] + j) T(elem);
      }
    }
  } catch(...) {
    for (size_t k = 0; k <= i; ++k) {
      if (out_arr_[k] != nullptr) {
        size_t limit = (k == i) ? total_initialized % block_sz_ : block_sz_;
        for (size_t j = 0; j < limit; ++j) {
          (out_arr_[k] + j)->~T();
        }
        delete[] reinterpret_cast<char*>(out_arr_[k]);
      }
    }
    delete[] out_arr_;
    throw;
  }
  begin_ = iterator(out_arr_, 0, 0, out_cap_);
  end_ = begin_ + sz_deque_;
}


template <typename T>
Deque<T>::~Deque() {
  for (auto it = begin(); it < end(); ++it) {
    it->~T();
  }
  for (size_t i = 0; i < out_cap_; ++i) {
    delete[] reinterpret_cast<char*>(out_arr_[i]);
  }
  delete[] out_arr_;
}

template <typename T>
size_t Deque<T>::size() const {
  return end_ - begin_;
}

template <typename T>
void Deque<T>::push_back(const T& elem) {
  if (end_.out_ind + 1 >= out_cap_) {
    extend_outer();
  }

  iterator new_end = end_;
  T* target = nullptr;
  T temp(elem);
  target = out_arr_[new_end.out_ind] + new_end.in_ind;
  new (target) T(std::move(temp));
  ++new_end;
  ++sz_deque_;
  end_ = new_end;
}


template <typename T>
void Deque<T>::push_front(const T& elem) {
  if (sz_deque_ == 0) {
    push_back(elem);
    return;
  }
  if (begin_.out_ind == 0) {
    extend_outer();
  }

  iterator new_begin = begin_;
  new_begin.decrement();
  T* target = nullptr;
  T temp(elem);
  target = out_arr_[new_begin.out_ind] + new_begin.in_ind;
  new (target) T(std::move(temp));
  begin_ = new_begin;
  ++sz_deque_;
}

template <typename T>
void Deque<T>::pop_back() {
  if (sz_deque_ == 0) {
    throw std::out_of_range("Deque is empty");
  }
  --end_;
  end_->~T();
  --sz_deque_;
}

template <typename T>
void Deque<T>::pop_front() {
  if (sz_deque_ == 0) {
    throw std::out_of_range("Deque is empty");
  }
  begin_->~T();
  ++begin_;
  --sz_deque_;
}

template <typename T>
Deque<T>::iterator
Deque<T>::erase(iterator it) {
  if (it == end() - 1) {
    pop_back();
    return end();
  }
  if (it == begin()) {
    pop_front();
    return begin();
  }
  for (iterator swp = it; swp < end(); ++swp) {
    std::swap(*swp, *(swp + 1));
  }
  pop_back();
  return it;
}

template <typename T>
Deque<T>::iterator
Deque<T>::insert(iterator it, const T& elem) {
  if (it == end()) {
    push_back(elem);
    return end() - 1;
  }
  if (it == begin()) {
    push_front(elem);
    return begin();
  }
  size_t offset = it - begin_;
  Deque tmp(*this);  // copy for strong guarantee
  tmp.push_back(elem);
  it = tmp.begin_ + offset;
  for (iterator swp = tmp.end() - 1; swp != it; --swp) {
    std::swap(*swp, *(swp - 1));
  }
  (*this) = tmp;
  return it;
}

template <typename T>
Deque<T>::iterator
Deque<T>::begin() {
  return begin_;
}

template <typename T>
Deque<T>::iterator
Deque<T>::end() {
  return end_;
}

template <typename T>
Deque<T>::const_iterator
Deque<T>::begin() const {
  return const_iterator(begin_);
}

template <typename T>
Deque<T>::const_iterator
Deque<T>::end() const {
  return const_iterator(end_);
}

template <typename T>
Deque<T>::const_iterator
Deque<T>::cbegin() const {
  return const_iterator(begin_);
}

template <typename T>
Deque<T>::const_iterator
Deque<T>::cend() const {
  return const_iterator(end_);
}

template<typename T>
Deque<T>::reverse_iterator
Deque<T>::rbegin() {
  return reverse_iterator(end_);
}

template<typename T>
Deque<T>::reverse_iterator
Deque<T>::rend() {
  return reverse_iterator(begin_);
}

template<typename T>
Deque<T>::const_reverse_iterator
Deque<T>::crbegin() const {
  return const_reverse_iterator(begin_);
}

template<typename T>
Deque<T>::const_reverse_iterator
Deque<T>::crend() const {
  return const_reverse_iterator(end_);
}

template<typename T>
T& Deque<T>::at(size_t ind) {
  if (ind >= sz_deque_) {
    throw std::out_of_range("bro out of range");
  }
  return (*this)[ind];
}

template<typename T>
const T& Deque<T>::at(size_t ind) const {
  if (ind >= sz_deque_) {
    throw std::out_of_range("bro out of range");
  }
  return (*this)[ind];
}

template<typename T>
T& Deque<T>::operator[](size_t ind) {
  iterator tmp(out_arr_, begin_.out_ind, begin_.in_ind, out_cap_);
  return *(tmp + ind);
}

template<typename T>
const T& Deque<T>::operator[](size_t ind) const {
  const_iterator tmp(out_arr_, begin_.out_ind, begin_.in_ind, out_cap_);
  return *(tmp + ind);
}

template <typename T>
Deque<T>&
Deque<T>::operator=(const Deque& other) {
  if (this == &other) {
    return *this;
  }
  Deque tmp = other; // only here exception may appear
  std::swap(out_arr_, tmp.out_arr_);
  std::swap(sz_deque_, tmp.sz_deque_);
  std::swap(out_cap_, tmp.out_cap_);
  std::swap(begin_, tmp.begin_);
  std::swap(end_, tmp.end_);
  return *this;
}

template <typename T>
void Deque<T>::extend_outer() {
  size_t new_out_cap = out_cap_ * 3;
  T** new_out_arr = new T*[new_out_cap];

  size_t i = 0;
  try {
    for (; i < out_cap_; ++i) {
      new_out_arr[i] = reinterpret_cast<T*>(new char[block_sz_ * sizeof(T)]);
    }
  } catch(...) {
    for(size_t j = 0; j < i; ++j) {
      delete[] reinterpret_cast<char*>(new_out_arr[j]);
    }
    delete[] new_out_arr;
    throw;
  }

  i = out_cap_ * 2;
  try {
    for (; i < out_cap_ * 3; ++i) {
      new_out_arr[i] = reinterpret_cast<T*>(new char[block_sz_ * sizeof(T)]);
    }
  } catch(...) {
    for(size_t j = out_cap_ * 2; j < i; ++j) {
      delete[] reinterpret_cast<char*>(new_out_arr[j]);
    }
    delete[] new_out_arr;
    throw;
  }

  for (size_t i = out_cap_; i < out_cap_ * 2; ++i) {
    new_out_arr[i] = out_arr_[i - out_cap_];
  }

  delete[] out_arr_;
  begin_ = iterator(new_out_arr, begin_.out_ind, begin_.in_ind, new_out_cap);
  begin_ += out_cap_ * block_sz_;
  end_ = begin_ + sz_deque_;
  out_arr_ = new_out_arr;
  out_cap_ = new_out_cap;
}