#include <algorithm>
#include <cstring>
#include <iostream>

class String {
private:
  char* ptr = nullptr;
  size_t sz = 0;
  size_t cap = 0;

  size_t uni_find(const String& substr, bool isReverse) const;

  void set_capacity(size_t amount);

  void extend(size_t amount);

  explicit String(size_t count);

public:
  String();

  String(const char* c_style);

  String(size_t count, char sym);

  String(const String& other);

  void swap(String& other);

  size_t size() const;

  size_t length() const;

  size_t capacity() const;

  bool empty() const;

  void push_back(char elem);

  void pop_back();

  void clear();

  void shrink_to_fit();

  char& front();
  const char& front() const;

  char& back();
  const char& back() const;

  char* data();
  const char* data() const;

  String substr(size_t start, size_t cnt) const;

  size_t find(const String& substr) const;

  size_t rfind(const String& substr) const;

  String& operator=(const String& other);

  String& operator+=(const String& other);

  String& operator+=(char sym);

  char& operator[](size_t index);
  const char& operator[](size_t index) const;

  ~String();
};

String operator+(const String& left, const String& right) {
  String result(left);
  result += right;
  return result;
}

String operator+(const String& left, char right) {
  String result(left);
  result.push_back(right);
  return result;
}

String operator+(char left, const String& right) {
  String result(1, left);
  result += right;
  return result;
}

bool operator==(const String& left, const String& right) {
  return left.size() == right.size() &&
         memcmp(left.data(), right.data(), left.size()) == 0;
}

bool operator!=(const String& left, const String& right) {
  return !(left == right);
}

bool operator<(const String& left, const String& right) {
  size_t cnt = std::min(left.size(), right.size());
  int res = memcmp(left.data(), right.data(), cnt);
  return res < 0 || left.size() < right.size();
}

bool operator>(const String& left, const String& right) {
  return right < left;
}

bool operator<=(const String& left, const String& right) {
  return !(left > right);
}

bool operator>=(const String& left, const String& right) {
  return !(left < right);
}

std::istream& operator>>(std::istream& in, String& str) {
  char sym;

  while (in.get(sym) && isspace(sym)) continue;
  str.push_back(sym);

  while (in.get(sym)) {
    if (isspace(sym)) {
      break;
    }
    str.push_back(sym);
  }
  return in;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    out << str[i];
  }
  return out;
}

void String::extend(size_t amount) {
  if (amount > cap) {
    set_capacity(2 * amount);
  }
}

void String::set_capacity(size_t amount) {
  char* new_buffer = new char[amount + 1];
  std::copy(ptr, ptr + sz, new_buffer);
  delete[] ptr;
  cap = amount;
  ptr = new_buffer;
  ptr[sz] = '\0';
}

String::String(size_t count) : ptr(new char[count + 1]), sz(count), cap(count) {
  ptr[sz] = '\0';
}

String::String() : String(size_t(0)) {}

String::String(const char* c_style) : String(strlen(c_style)) {
  std::copy(c_style, c_style + sz, ptr);
}

String::String(size_t count, char sym) : String(count) {
  std::fill(ptr, ptr + count, sym);
}

String::String(const String& other) : String(other.sz) {
  std::copy(other.ptr, other.ptr + sz, ptr);
}

String::~String() { delete[] ptr; }

void String::swap(String& other) {
  std::swap(ptr, other.ptr);
  std::swap(sz, other.sz);
  std::swap(cap, other.cap);
}

size_t String::size() const { return sz; }

size_t String::length() const { return sz; }

size_t String::capacity() const { return cap; }

bool String::empty() const { return sz == 0; }

void String::push_back(char elem) {
  extend(sz + 1);
  ptr[sz] = elem;
  ptr[++sz] = '\0';
}

void String::pop_back() {
  ptr[--sz] = '\0';
}

void String::clear() {
  sz = 0;
  ptr[0] = '\0';
}

void String::shrink_to_fit() {
  if (sz < cap) {
    set_capacity(sz);
  }
}

char& String::front() { return ptr[0]; }

const char& String::front() const { return ptr[0]; }

char& String::back() { return ptr[sz - 1]; }

const char& String::back() const { return ptr[sz - 1]; }

char* String::data() { return ptr; }

const char* String::data() const { return ptr; }

String String::substr(size_t start, size_t cnt) const {
  cnt = std::min(cnt, sz - start);
  String substr(cnt);

  std::copy(ptr + start, ptr + start + cnt, substr.ptr);
  return substr;
}

size_t String::uni_find(const String& substr, bool isReverse) const {
  if (substr.sz > sz) return length();

  size_t end = isReverse ? size_t(-1) : sz - substr.sz + 1;
  size_t start = isReverse ?  sz - substr.sz : 0;
  size_t step = isReverse ? size_t(-1) : 1;

  for (size_t i = start; i != end; i += step) {
    if (memcmp(ptr + i, substr.ptr, substr.sz) == 0) {
      return i;
    }
  }

  return length();
}

size_t String::find(const String& substr) const {
  return uni_find(substr, false);
}

size_t String::rfind(const String& substr) const {
  return uni_find(substr, true);
}

String& String::operator=(const String& other) {
  String copy(other);
  swap(copy);
  return *this;
}

String& String::operator+=(const String& other) {
  extend(sz + other.sz);
  std::copy(other.ptr, other.ptr + other.sz, ptr + sz);
  sz += other.sz;
  return *this;
}

String& String::operator+=(char sym) {
  push_back(sym);
  return *this;
}

char& String::operator[](size_t index) { return ptr[index]; }

const char& String::operator[](size_t index) const { return ptr[index]; }
