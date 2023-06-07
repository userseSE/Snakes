#pragma once

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>

#if __cplusplus >= 202002L

template <typename F>
concept is_callable = requires { &F::operator(); };

template <typename F> struct function_argument_types;

template <typename R, typename... Args>
struct function_argument_types<R(Args...)> {
  using type = std::tuple<Args...>;
};

template <typename R, typename... Args>
struct function_argument_types<R (*)(Args...)> {
  using type = std::tuple<Args...>;
};

template <typename F>
  requires is_callable<F>
struct function_argument_types<F>
    : function_argument_types<decltype(&F::operator())> {};

template <typename F, typename R, typename... Args>
struct function_argument_types<R (F::*)(Args...) const> {
  using type = std::tuple<Args...>;
};

template <typename F>
using function_argument_types_t =
    typename function_argument_types<std::decay_t<F>>::type;

#else
template <typename F> struct function_argument_types;

// For free functions
template <typename R, typename... Args>
struct function_argument_types<R(Args...)> {
  using type = std::tuple<Args...>;
};

// For function pointers
template <typename R, typename... Args>
struct function_argument_types<R (*)(Args...)> {
  using type = std::tuple<Args...>;
};

// For member function pointers
template <typename Lambda, typename R, typename... Args>
struct function_argument_types<R (Lambda::*)(Args...) const> {
  using type = std::tuple<Args...>;
};

// For functors
template <typename F>
struct function_argument_types
    : function_argument_types<decltype(&F::operator())> {};

// Alias for convenience
template <typename F>
using function_argument_types_t =
    typename function_argument_types<std::decay_t<F>>::type;
#endif