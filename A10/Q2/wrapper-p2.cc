#include <iostream>
#include <cstdint>
#include "a10p2.h"
using namespace std;

// === Insert any helper functions here

int64_t* reverse(int64_t* head) {
  int64_t* n = head;
  int64_t* result = nullptr;
  char c;
  while (n) {
    c = (char) car(n);
    result = cons(c, result);
    n = cdr(n);
  }
  return result;
}

void print_list(int64_t* head) {
  int64_t* n = head;
  char c;
  while (n) {
    c = (char) car(n);
    cout << c;
    n = cdr(n);
  }
}

void delete_list(int64_t* head) {
  int64_t* cur = head;
  int64_t* next = nullptr;
  while (cur) {
    next = cdr(cur);
    snoc(cur);
    cur = next;
  }
}

int64_t wain(int64_t*, int64_t) {
  cin >> std::noskipws;
  int64_t char_count = 0;
  int64_t* head = nullptr;
  char c;
  while (cin >> c) {
    head = cons(c, head);
    char_count += 1;
  }
  int64_t* reversed_head = reverse(head);
  print_list(reversed_head);
  print_list(reversed_head);
  delete_list(head);
  delete_list(reversed_head);
  return char_count;
}

// Do not modify the code below.
int main(int argc, char *argv[]) {
  int ret = wain(0,0);
  std::cerr << ret << std::endl;
  return ret;
}
