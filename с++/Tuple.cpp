#include <iostream>

template <typename T>
using is_copy_list_initializable = std::is_constructible<T, std::initializer_list<int>>;

template<typename... Types>
class Tuple;

template<>
class Tuple<> {
public:
  constexpr Tuple() = default;
};

template<typename Head, typename... Tail>
class Tuple<Head, Tail...> {
public:
  Head head;
  Tuple<Tail...> tail;

  template<typename H = Head,
            std::enable_if_t<std::conjunction_v<std::is_default_constructible<H>, std::is_default_constructible<Tail>...>, int> = 0>
  explicit(!std::conjunction_v<is_copy_list_initializable<H>, is_copy_list_initializable<Tail>...>)
      Tuple() : head(), tail() {}

  explicit(!std::conjunction_v<std::is_convertible<const Head&, Head>, std::is_convertible<const Tail&, Tail>...>)
      Tuple(const Head& h, const Tail&... t)
    requires(sizeof(Head) + sizeof...(Tail) >= 1 && std::conjunction_v<std::is_copy_constructible<Head>, std::is_copy_constructible<Tail>...>)
      : head(h), tail(t...) {}

  template<typename UHead, typename... UTail,
            std::enable_if_t<(sizeof...(UTail) == sizeof...(Tail)) && (sizeof...(Tail) + sizeof(Head) >= 1) &&
                                 std::conjunction_v<std::is_constructible<Head, UHead&&>, std::is_constructible<Tail, UTail&&>...>, int> = 0>
  explicit(!std::conjunction_v<std::is_convertible<UHead&&, Head>, std::is_convertible<UTail&&, Tail>...>)
      Tuple(UHead&& uhead, UTail&&... utail)
      : head(std::forward<UHead>(uhead)), tail(std::forward<UTail>(utail)...) {
  }

  template<typename U1, typename U2,
            std::enable_if_t<std::is_constructible_v<Head, U1> && (std::is_constructible_v<Tail, U2> && ...), int> = 0>
  explicit(!std::conjunction_v<std::is_convertible<U1&&, Head>, std::is_convertible<U2&&, Tail>...>)
      Tuple(U1&& u1, U2&& u2)
      : head(std::forward<U1>(u1)), tail(std::forward<U2>(u2)) {}

  template<typename UHead, typename... UTail>
    requires (sizeof...(UTail) == sizeof...(Tail)) && (sizeof(Head) + sizeof...(Tail) >= 1) &&
                std::is_constructible_v<Head, decltype(get<0>(std::forward<Tuple<UHead>>(std::declval<Tuple<UHead>>())))> &&
                (std::is_constructible_v<Tail, decltype(get<1 + sizeof...(Tail) - sizeof...(UTail)>(std::forward<Tuple<UHead, UTail...>>(std::declval<Tuple<UHead, UTail...>>())))> && ...) &&
                (sizeof(UHead) + sizeof...(UTail) != 1 ||
                 (!std::is_convertible_v<Tuple<UHead, UTail...>, Head> &&
                  !std::is_constructible_v<Head, Tuple<UHead, UTail...>> &&
                  !std::is_same_v<Head, UHead> && !std::is_same_v<Tail..., UTail...>))
  explicit(!std::conjunction_v<std::is_convertible<const UHead&, Head>, std::is_convertible<const UTail&, Tail>...>)
      Tuple(const Tuple<UHead, UTail...>& other)
      : head(other.head), tail(other.tail) {}

  template<typename UHead, typename... UTail>
    requires ((sizeof...(UTail) == sizeof...(Tail)) && (sizeof(UHead) + sizeof...(UTail) >= 1)) &&
                (std::is_constructible_v<Head&&, decltype(get<0>(std::forward<Tuple<UHead>>(std::declval<Tuple<UHead>>())))>) &&
                //By tests is shows same results if it was "is_contructible_v<Tail, UTail&&>"... but I believe in cppref wisdom
                (std::is_constructible_v<Tail, decltype(get<1 + sizeof...(Tail) - sizeof...(UTail)>(std::forward<Tuple<UHead, UTail...>>(std::declval<Tuple<UHead, UTail...>>())))> && ...) &&
                (sizeof(Head) + sizeof...(Tail) != 1 ||
                 (!std::is_convertible_v<Tuple<UHead, UTail...>, Head> &&
                  !std::is_constructible_v<Head, Tuple<UHead, UTail...>> &&
                  !std::is_same_v<Head, UHead> && !std::is_same_v<Tail..., UTail...>))
  explicit(!std::conjunction_v<std::is_convertible<UHead&&, Head>, std::is_convertible<UTail&&, Tail>...>)
      Tuple(Tuple<UHead, UTail...>&& other)
      : head(std::forward<UHead>(other.head)), tail(std::forward<Tuple<UTail...>>(other.tail))
  {}

  template<typename T1, typename T2>
    requires (sizeof...(Tail) == 1 && std::conjunction_v<std::is_constructible<Head, const T1&>, std::is_constructible<Tail..., const T2&>>)
  explicit(!std::conjunction_v<std::is_convertible<const T1&, Head>, std::is_convertible<const T2&, Tail>...>)
      Tuple(const std::pair<T1, T2>& p) : head(p.first), tail(p.second) {}

  template<typename T1, typename T2>
    requires (sizeof...(Tail) == 1 && std::conjunction_v<std::is_constructible<Head, T1&&>, std::is_constructible<Tail..., T2&&>>)
  explicit(!std::conjunction_v<std::is_convertible<T1&&, Head>, std::is_convertible<T2&&, Tail>...>)
      Tuple(std::pair<T1, T2>&& p) : head(std::move(p.first)), tail(std::move(p.second)) {}


  Tuple( const Tuple& other ) = default;
  Tuple( Tuple&& other ) = default;

  Tuple& operator=(const Tuple& other)
    requires (std::conjunction_v<std::is_copy_assignable<Head>, std::is_copy_assignable<Tail>...>) {
    head = other.head;
    tail = other.tail;
    return *this;
  }

  Tuple& operator=(Tuple&& other) noexcept
    requires (std::conjunction_v<std::is_move_assignable<Head>, std::is_move_assignable<Tail...>>) {
    head = std::forward<Head>(other.head);
    tail = std::forward<Tuple<Tail...>>(other.tail);
    return *this;
  }

  template<typename H = Head, typename... T,
            std::enable_if_t<std::conjunction_v<std::is_assignable<H&, const H&>, std::is_assignable<T&, const T&>...>, int> = 0>
  Tuple& operator=(const Tuple<H&, T&...>& other) {
    head = other.head;
    tail = other.tail;
    return *this;
  }

  template<typename UHead, typename... UTail>
  Tuple& operator=(const Tuple<UHead, UTail...>& other)
    requires ((sizeof...(UTail) == sizeof...(Tail)) &&
             std::conjunction_v<std::is_assignable<Head&, const UHead&>, std::is_assignable<Tail&, const UTail&>...>) {
    head = other.head;
    tail = other.tail;
    return *this;
  }

  template<typename UHead, typename... UTail>
  Tuple& operator=(Tuple<UHead, UTail...>&& other) noexcept
    requires ((sizeof...(UTail) == sizeof...(Tail)) &&
             std::conjunction_v<std::is_assignable<Head&, UHead>, std::is_assignable<Tail&, UTail>...>) {
    head = std::forward<UHead>(other.head);
    tail = std::forward<Tuple<UTail...>>(other.tail);
    return *this;
  }

  template<typename E1, typename E2>
  Tuple& operator=(const std::pair<E1, E2>& p)
    requires (sizeof...(Tail) == 1 &&
             std::conjunction_v<std::is_assignable<const Head&, const E1&>, std::is_assignable<const Tail&, const E2&>...>) {
    head = p.first;
    tail = Tuple<Tail...>(p.second);
    return *this;
  }

  template<typename E1, typename E2>
  Tuple& operator=(std::pair<E1, E2>&& p)
    requires (sizeof...(Tail) == 1 &&
             std::conjunction_v<std::is_assignable<Head&, E1>, std::is_assignable<Tail&, E2>...>) {
    head = std::move(p.first);
    tail = Tuple<Tail...>(std::move(p.second));
    return *this;
  }
};

template<typename... Types>
auto makeTuple(Types&&... args) {
  return Tuple<std::unwrap_ref_decay_t<Types>...>(std::forward<Types>(args)...);
}


template <typename T>
decltype(auto) to_lvalue(T&& t) {
  if constexpr (std::is_rvalue_reference_v<T>) {
    using Type = std::remove_reference_t<T>;
    return static_cast<Type&>(t);
  } else {
    return t;
  }
}

template<size_t Index, typename Head, typename... Tail>
decltype(auto) get(Tuple<Head, Tail...>& tuple) {
  if constexpr (Index == 0) {
    return to_lvalue(tuple.head);
  } else {
    return get<Index - 1>(tuple.tail);
  }
}

template<size_t Index, typename Head, typename... Tail>
decltype(auto) get(const Tuple<Head, Tail...>& tuple) {
  if constexpr (Index == 0) {
    return to_lvalue(tuple.head);
  } else {
    return get<Index - 1>(tuple.tail);
  }
}

template<size_t Index, typename Head, typename... Tail>
decltype(auto) get(Tuple<Head, Tail...>&& tuple) {
  if constexpr (Index == 0) {
    return std::forward<Head>(tuple.head);
  } else {
    return get<Index - 1>(std::forward<Tuple<Tail...>>(tuple.tail));
  }
}

template<size_t Index, typename Head, typename... Tail>
decltype(auto) get(const Tuple<Head, Tail...>&& tuple) {
  if constexpr (Index == 0) {
    return std::forward<const Head>(tuple.head);
  } else {
    return get<Index - 1>(std::forward<const Tuple<Tail...>>(tuple.tail));
  }
}

template<typename T, typename Head, typename... Tail>
decltype(auto) get(Tuple<Head, Tail...>& tuple) {
  if constexpr (std::is_same_v<T, Head>) {
    return to_lvalue(tuple.head);
  } else {
    return get<T>(tuple.tail);
  }
}

template<typename T, typename Head, typename... Tail>
decltype(auto) get(const Tuple<Head, Tail...>& tuple) {
  if constexpr (std::is_same_v<T, Head>) {
    return to_lvalue(tuple.head);
  } else {
    return get<T>(tuple.tail);
  }
}

template<typename T, typename Head, typename... Tail>
decltype(auto) get(Tuple<Head, Tail...>&& tuple) {
  if constexpr (std::is_same_v<T, Head>) {
    return std::forward<Head>(tuple.head);
  } else {
    return get<T>(std::forward<Tuple<Tail...>>(tuple.tail));
  }
}

template<typename T, typename Head, typename... Tail>
decltype(auto) get(const Tuple<Head, Tail...>&& tuple) {
  if constexpr (std::is_same_v<T, Head>) {
    return std::forward<const Head>(tuple.head);
  } else {
    return get<T>(std::forward<const Tuple<Tail...>>(tuple.tail));
  }
}

template<typename T1, typename T2>
Tuple(const std::pair<T1, T2>&) -> Tuple<T1, T2>;

template<typename T1, typename T2>
Tuple(std::pair<T1, T2>&&) -> Tuple<T1, T2>;

template <typename T>
struct tuple_size;

template <typename... Types>
struct tuple_size<Tuple<Types...>> : std::integral_constant<size_t, sizeof...(Types)> {
};

template <typename T>
constexpr size_t tuple_size_v = tuple_size<T>::value;


template <typename... Types>
constexpr auto forwardAsTuple(Types&&... ts) noexcept {
  return Tuple<Types&&...>{std::forward<Types>(ts)...};
}

struct tupleCatImpl {
  template <typename First, typename Second, typename... Rest>
  static constexpr auto concat(First&& frst, Second&& sec, Rest&&... ts) {
    return concat(concatPair<
                      std::make_index_sequence<tuple_size_v<std::remove_reference_t<First>>>,
                      std::make_index_sequence<tuple_size_v<std::remove_reference_t<Second>>>>::
                      f(std::forward<First>(frst), std::forward<Second>(sec)), std::forward<Rest>(ts)...);
  }

  template <typename Tuple>
  static constexpr auto concat(Tuple&& ret) {
    return makeFwdTuple<std::make_index_sequence<tuple_size_v<Tuple>>>::make_tuple_from_fwd_tuple(std::forward<Tuple>(ret));
  }

  template <typename Fwd_index_seq, typename Tuple_index_seq>  // only way to parse Indices (I guess)
  struct concatPair;

  template <size_t... First_indices, size_t... Second_indices>
  struct concatPair<std::index_sequence<First_indices...>, std::index_sequence<Second_indices...>> {
    template <typename First, typename Second>
    static constexpr auto f(First&& fwd, Second&& t) {
      return forwardAsTuple(get<First_indices>(std::forward<First>(fwd))..., get<Second_indices>(std::forward<Second>(t))...);
    }
  };

  template <typename Index_seq>  // only way to parse Indices (I guess)
  struct makeFwdTuple;

  template <size_t... Indices>
  struct makeFwdTuple<std::index_sequence<Indices...>> {
    template <typename Tuple>
    static constexpr auto make_tuple_from_fwd_tuple(Tuple &&fwd) {
      return makeTuple(get<Indices>(std::forward<Tuple>(fwd))...);
    }
  };

};

template <typename... Tuples>
constexpr auto tupleCat(Tuples&&... tuples) {
  return tupleCatImpl::concat(std::forward<Tuples>(tuples)...);
}