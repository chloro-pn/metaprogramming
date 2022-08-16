#include <iostream>

#include "sort.h"

using namespace meta_prom;

int main() {
  using nums = integral_sequence<int, 3, 0, 8, 9, 2, 8, 7>;
  using merge_result = merge_sort<nums>::result;
  std::cout << " merge sort : ";
  print<merge_result>::print_integer();
  std::cout << " quick sort : ";
  using quick_result = quick_sort<nums>::result;
  print<quick_result>::print_integer();
  return 0;
}