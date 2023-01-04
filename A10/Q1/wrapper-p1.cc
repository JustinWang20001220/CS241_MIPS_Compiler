#include <iostream>
#include <cstdint>
#include "a10p1.h"

// === Insert any helper functions here

int64_t wain(int64_t* a, int64_t n) {
  // Insert mainline code here
  int64_t* p = arena();
  int64_t cur_max_count = 0;
  for (int64_t i = 0; i < n; ++i) {
    p[a[i]] = 0;
  }
  for (int64_t i = 0; i < n; ++i) {
    p[a[i]] += 1;
    if (p[a[i]] > cur_max_count) {
      cur_max_count = p[a[i]];
    }
  }
  return cur_max_count;
}

// Do not modify the code below.
int main(int argc, char *argv[]) {
  int64_t l, c;
  int64_t* a;
  std::cout << "Enter length of array: ";
  std::cin >> l; 
  a = new int64_t[l];
  for(int64_t i = 0; i < l; ++i) {
    std::cout << "Enter value of array element " << i << " ";
    std::cin >> a[i];
  }
  c = wain(a,l);
  delete [] a;
  std::cerr << c << std::endl;
  return c;
}
