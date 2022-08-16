#pragma once

#include <cstddef>
#include <cstdint>

namespace meta_prom {

template <typename T, T... values> struct integral_sequence {
  using value_type = T;
};

template <typename integral_sequence> struct length {};

template <typename T, T... data> struct length<integral_sequence<T, data...>> {
  constexpr static size_t value = sizeof...(data);
};

// 将T类型的值data添加到datas前，并将结果作为新的类型返回
template <typename T, T data, typename datas> struct push_front_t {};

template <typename T, T data, T... datas>
struct push_front_t<T, data, integral_sequence<T, datas...>> {
  using type = integral_sequence<T, data, datas...>;
};

// 将T类型的值data添加到datas后，并将结果作为新的类型返回
template <typename T, T data, typename datas> struct push_back_t {};

template <typename T, T data, T... datas>
struct push_back_t<T, data, integral_sequence<T, datas...>> {
  using type = integral_sequence<T, datas..., data>;
};

template <bool con, typename T, typename F> struct conditional {};

template <typename T, typename F> struct conditional<true, T, F> {
  using type = T;
};

template <typename T, typename F> struct conditional<false, T, F> {
  using type = F;
};

template <typename T, template <T, T> typename Filter, T compare,
          typename integral_sequence>
struct filter {};

template <typename T, template <T, T> typename Filter, T compare>
struct filter<T, Filter, compare, integral_sequence<T>> {
  using type = integral_sequence<T>;
};

template <typename T, template <T, T> typename Filter, T compare, T head,
          T... tails>
struct filter<T, Filter, compare, integral_sequence<T, head, tails...>> {
  using type = typename conditional<
      Filter<head, compare>::value,
      typename push_front_t<
          T, head,
          typename filter<T, Filter, compare,
                          integral_sequence<T, tails...>>::type>::type,
      typename filter<T, Filter, compare,
                      integral_sequence<T, tails...>>::type>::type;
};

template <typename seq1, typename seq2> struct concat {};

template <typename sequence, typename T>
struct concat<sequence, integral_sequence<T>> {
  using type = sequence;
};

// sequence, head, tails...
template <typename sequence, typename T, T head, T... tails>
struct concat<sequence, integral_sequence<T, head, tails...>> {
  using type = typename concat<typename push_back_t<T, head, sequence>::type,
                               integral_sequence<T, tails...>>::type;
};

template <typename T> struct CompareTrait {};

#define ConstructCompare(type)                                                 \
  template <> struct CompareTrait<type> {                                      \
    template <type i, type j> struct big {                                     \
      static constexpr bool value = (i > j);                                   \
    };                                                                         \
                                                                               \
    template <type i, type j> struct low {                                     \
      static constexpr bool value = (i <= j);                                  \
    };                                                                         \
  };

ConstructCompare(int64_t);
ConstructCompare(uint64_t);
ConstructCompare(int32_t);
ConstructCompare(uint32_t);
ConstructCompare(float);
ConstructCompare(double);

template <typename integer_sequence> struct quick_sort {};

template <typename T> struct quick_sort<integral_sequence<T>> {
  using result = integral_sequence<T>;
};

template <typename T, T head, T... tails>
struct quick_sort<integral_sequence<T, head, tails...>> {
  using result = typename concat<
      typename quick_sort<
          typename filter<T, CompareTrait<T>::template low, head,
                          integral_sequence<T, tails...>>::type>::result,
      typename concat<
          integral_sequence<T, head>,
          typename quick_sort<typename filter<
              T, CompareTrait<T>::template big, head,
              integral_sequence<T, tails...>>::type>::result>::type>::type;
};

template <typename integer_sequence, size_t index> struct nth {};

template <typename T, T head, T... tails>
struct nth<integral_sequence<T, head, tails...>, 0> {
  static constexpr T value = head;
};

template <typename T, size_t index> struct nth<integral_sequence<T>, index> {
  // index越界
};

template <typename T, size_t index, T head, T... tails>
struct nth<integral_sequence<T, head, tails...>, index> {
  static constexpr T value =
      nth<integral_sequence<T, tails...>, index - 1>::value;
};

template <typename integer_sequence, size_t n> struct prefix {};

template <typename T, T... datas>
struct prefix<integral_sequence<T, datas...>, 0> {
  using type = integral_sequence<T>;
};

template <typename T, T head, T... tails>
struct prefix<integral_sequence<T, head, tails...>, 1> {
  using type = integral_sequence<T, head>;
};

template <typename T, size_t n, T... datas>
struct prefix<integral_sequence<T, datas...>, n> {
  using sequ = integral_sequence<T, datas...>;
  using type = typename push_back_t<T, nth<sequ, n - 1>::value,
                                    typename prefix<sequ, n - 1>::type>::type;
};

template <typename integer_sequence, size_t n> struct remain {};

template <typename T, T head, T... tails, size_t n>
struct remain<integral_sequence<T, head, tails...>, n> {
  using type = typename remain<integral_sequence<T, tails...>, n - 1>::type;
};

template <typename T, T head, T... tails>
struct remain<integral_sequence<T, head, tails...>, 0> {
  using type = integral_sequence<T, head, tails...>;
};

template <typename T, size_t n> struct remain<integral_sequence<T>, n> {
  // 越界
};

template <typename integer_sequence> struct cut {
  constexpr static size_t cut_position = length<integer_sequence>::value / 2;
  using type1 = typename prefix<integer_sequence, cut_position>::type;
  using type2 = typename remain<integer_sequence, cut_position>::type;
};

template <typename sequ0, typename sequ1, typename sequ2> struct merge_impl {};

template <typename sequ0, typename T, T head, T... datas>
struct merge_impl<sequ0, integral_sequence<T>,
                  integral_sequence<T, head, datas...>> {
  using type =
      typename concat<sequ0, integral_sequence<T, head, datas...>>::type;
};

template <typename sequ0, typename T, T head, T... datas>
struct merge_impl<sequ0, integral_sequence<T, head, datas...>,
                  integral_sequence<T>> {
  using type =
      typename concat<sequ0, integral_sequence<T, head, datas...>>::type;
};

template <typename sequ0, typename T>
struct merge_impl<sequ0, integral_sequence<T>, integral_sequence<T>> {
  using type = sequ0;
};

template <typename sequ0, typename T, T head, T... tails, T head2, T... tails2>
struct merge_impl<sequ0, integral_sequence<T, head, tails...>,
                  integral_sequence<T, head2, tails2...>> {
  using type = typename conditional<
      (head > head2),
      typename merge_impl<typename push_back_t<T, head2, sequ0>::type,
                          integral_sequence<T, head, tails...>,
                          integral_sequence<T, tails2...>>::type,
      typename merge_impl<typename push_back_t<T, head, sequ0>::type,
                          integral_sequence<T, tails...>,
                          integral_sequence<T, head2, tails2...>>::type>::type;
};

template <typename T, typename sequ1, typename sequ2>
using merge = merge_impl<integral_sequence<T>, sequ1, sequ2>;

template <typename integer_sequence> struct merge_sort {};

template <typename T> struct merge_sort<integral_sequence<T>> {
  using result = integral_sequence<T>;
};

template <typename T, T data> struct merge_sort<integral_sequence<T, data>> {
  using result = integral_sequence<T, data>;
};

template <typename T, T data1, T data2>
struct merge_sort<integral_sequence<T, data1, data2>> {
  using result =
      conditional<(data1 < data2), integral_sequence<T, data1, data2>,
                  integral_sequence<T, data2, data1>>::type;
};

template <typename T, T... datas>
struct merge_sort<integral_sequence<T, datas...>> {
  using cut_type1 = typename cut<integral_sequence<T, datas...>>::type1;
  using cut_type2 = typename cut<integral_sequence<T, datas...>>::type2;
  using result = typename merge<T, typename merge_sort<cut_type1>::result,
                                typename merge_sort<cut_type2>::result>::type;
};

template <typename seq> struct print {};

template <typename T, T start, T... values>
struct print<integral_sequence<T, start, values...>> {
  static void print_integer() {
    std::cout << start << " ";
    print<integral_sequence<T, values...>>::print_integer();
  }
};

template <typename T> struct print<integral_sequence<T>> {
  static void print_integer() { std::cout << std::endl; }
};

} // namespace meta_prom