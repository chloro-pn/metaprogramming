#include <iostream>

namespace meta_prom {

template <typename T, T... values>
struct integral_sequence {
  using value_type = T;
};

// 将T类型的值data添加到datas前，并将结果作为新的类型返回
template <typename T, T data, typename datas>
struct push_front_t {};

template <typename T, T data, T... datas>
struct push_front_t<T, data, integral_sequence<T, datas...>> {
  using type = integral_sequence<T, data, datas...>;
};

// 将T类型的值data添加到datas后，并将结果作为新的类型返回
template <typename T, T data, typename datas>
struct push_back_t {};

template <typename T, T data, T... datas>
struct push_back_t<T, data, integral_sequence<T, datas...>> {
  using type = integral_sequence<T, datas..., data>;
};

template <bool con, typename T, typename F>
struct conditional {};

template <typename T, typename F>
struct conditional<true, T, F> {
  using type = T;
};

template <typename T, typename F>
struct conditional<false, T, F> {
  using type = F;
};

template <typename T, template<T, T> typename Filter, T compare, typename integral_sequence>
struct filter {};

template<typename T, template<T, T> typename Filter, T compare>
struct filter<T, Filter, compare, integral_sequence<T>> {
  using type = integral_sequence<T>;
};

template <typename T, template<T, T> typename Filter, T compare, T head, T... tails>
struct filter<T, Filter, compare, integral_sequence<T, head, tails...>> {
  using type = typename conditional<
    Filter<head, compare>::value, 
    typename push_front_t<T, head, typename filter<T, Filter, compare, integral_sequence<T, tails...>>::type>::type,
    typename filter<T, Filter, compare, integral_sequence<T, tails...>>::type>::type;
};

template<typename seq1, typename seq2>
struct concat {};

template<typename sequence, typename T>
struct concat<sequence, integral_sequence<T>> {
  using type = sequence;
};

// sequence, head, tails...
template<typename sequence, typename T, T head, T... tails>
struct concat<sequence, integral_sequence<T, head, tails...>> {
  using type = typename concat<typename push_back_t<T, head, sequence>::type, integral_sequence<T, tails...>>::type;
};

template <typename T>
struct CompareTrait {};

#define ConstructCompare(type) \
template <> \
struct CompareTrait<type> { \
template <type i, type j> \
struct big { \
  static constexpr bool value = (i > j); \
}; \
 \
template <type i, type j> \
struct low { \
  static constexpr bool value = (i <= j); \
}; \
 \
};

ConstructCompare(int64_t)
ConstructCompare(uint64_t)
ConstructCompare(int32_t)
ConstructCompare(uint32_t)
ConstructCompare(float)
ConstructCompare(double)


template <typename integer_sequence>
struct quick_sort {};

template <typename T>
struct quick_sort<integral_sequence<T>> {
  using type = integral_sequence<T>;
};

template<typename T, T head, T ...tails>
struct quick_sort<integral_sequence<T, head, tails...> >{
  using type=typename concat<
      typename quick_sort<typename filter<T, CompareTrait<T>::template low, head,integral_sequence<T, tails...>>::type>::type,
      typename concat<
        integral_sequence<T, head>, 
        typename quick_sort<typename filter<T, CompareTrait<T>::template big, head,integral_sequence<T, tails...>>::type>::type>
        ::type>
      ::type;
};

template <typename seq>
struct print {};

template <typename T, T start, T... values>
struct print<integral_sequence<T, start, values...>> {
  static void print_integer() {
    std::cout << start << " ";
    print<integral_sequence<T, values...>>::print_integer();
  }
};

template<typename T>
struct print<integral_sequence<T>> {
  static void print_integer() {
    std::cout << std::endl;
  }
};

}

using namespace meta_prom;

int main() {
  using int_type = quick_sort<integral_sequence<int, 3, 0, 8, 9, 2, 8, 7>>::type;
  print<int_type>::print_integer();
  
  using double_type = quick_sort<integral_sequence<double, 2.4, 3.3, 1.0>>::type;
  print<double_type>::print_integer();
  return 0;
}