#include <cstring>
#include <iostream>

struct Node {
  char* top_element = nullptr;
  Node* prev = nullptr;
};

char* reader(char*& request, size_t& cap, bool is_do_copy) {
  char sym = getchar();
  size_t sz = 0;
  while (!isspace(sym)) {
    if (sz >= cap) {
      char* new_buffer = new char[2 * sz + 1];
      std::copy(request, request + sz, new_buffer);
      delete[] request;
      cap = 2 * sz;
      request = new_buffer;
    }
    request[sz] = sym;
    sym = getchar();
    ++sz;
  }
  request[sz] = '\0';

  if (is_do_copy) {
    char* new_val = new char[sz + 1];
    std::copy(request, request + sz, new_val);
    new_val[sz] = '\0';
    return new_val;
  }
  return request;
}

void push(Node*& p_current_top, char* value, size_t& sz) {
  Node* t_node = new Node{value, p_current_top};
  p_current_top = t_node;
  ++sz;
  std::cout << "ok\n";
}

void pop(Node*& p_current_top, size_t& sz) {
  if (sz != 0) {
    std::cout << p_current_top->top_element << '\n';
    Node* t_node = p_current_top->prev;
    delete[] p_current_top->top_element;
    delete p_current_top;
    p_current_top = t_node;
    --sz;
  } else {
    std::cout << "error\n";
  }
}

void back(Node* p_current_top) {
  std::cout << (p_current_top != nullptr ? p_current_top->top_element : "error")
            << '\n';
}

void size(size_t sz) { std::cout << sz << '\n'; }

void private_clear(Node*& p_current_top, size_t& sz) {
  while (sz != 0) {
    Node* t_node = p_current_top->prev;
    delete[] p_current_top->top_element;
    delete p_current_top;
    p_current_top = t_node;
    --sz;
  }
}

void clear(Node*& p_current_top, size_t& sz) {
  private_clear(p_current_top, sz);
  std::cout << "ok\n";
}

void exit(Node*& p_current_top, size_t& sz) {
  private_clear(p_current_top, sz);
  std::cout << "bye\n";
}

int main() {
  Node* p_current_top = nullptr;
  size_t sz = 0;
  char* request = new char[2];
  size_t cap = 1;
  while (true) {
    request = reader(request, cap, false);
    if (strcmp(request, "push") == 0) {
      char* str_value;
      str_value = reader(request, cap, true);
      push(p_current_top, str_value, sz);
    } else if (strcmp(request, "back") == 0) {
      back(p_current_top);
    } else if (strcmp(request, "pop") == 0) {
      pop(p_current_top, sz);
    } else if (strcmp(request, "clear") == 0) {
      clear(p_current_top, sz);
    } else if (strcmp(request, "size") == 0) {
      size(sz);
    } else if (strcmp(request, "exit") == 0) {
      exit(p_current_top, sz);
      break;
    }
  }
  delete p_current_top;
  delete[] request;
}