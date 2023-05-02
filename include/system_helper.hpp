#pragma once


#include <flecs.h>
#include "function_argument_helper.hpp"

#include <utility>

template <typename... Args>
struct system_arguments
{
 private:
  template <typename Tuple, size_t... Is>
  static auto tuple_tail_elements_impl(std::index_sequence<Is...>)
  {

    return std::tuple<std::tuple_element_t<Is + 1, Tuple>...>{};
  }

 public:
  using types = std::tuple<Args...>;

  template <typename Tuple>
  static auto tuple_tail_elements()
  {
    return tuple_tail_elements_impl<Tuple>(
        std::make_index_sequence<std::tuple_size_v<Tuple> - 1>());
  }
};

template <typename F, size_t... Is>
constexpr auto get_system_arguments_impl(std::index_sequence<Is...>)
{
  using arg_tuple       = function_argument_types_t<F>;
  using remaining_types = std::conditional_t<
      std::is_same_v<std::remove_cvref_t<std::tuple_element_t<0, arg_tuple>>,
                     flecs::iter>,
      decltype(system_arguments<>::template tuple_tail_elements<arg_tuple>()),
      arg_tuple>;

  return system_arguments<std::remove_pointer_t<
      std::remove_cvref_t<std::tuple_element_t<Is, remaining_types>>>...>{};
}

template <typename F>
constexpr auto get_system_arguments(F&&)
{
  using arg_tuple       = function_argument_types_t<F>;
  using remaining_types = std::conditional_t<
      std::is_same_v<std::remove_cvref_t<std::tuple_element_t<0, arg_tuple>>,
                     flecs::iter>,
      decltype(system_arguments<>::template tuple_tail_elements<arg_tuple>()),
      arg_tuple>;

  constexpr size_t num_args = std::tuple_size_v<remaining_types>;

  return get_system_arguments_impl<F>(std::make_index_sequence<num_args>{});
}
template <typename F>
constexpr auto get_system_arguments()
{
  using arg_tuple       = function_argument_types_t<F>;
  using remaining_types = std::conditional_t<
      std::is_same_v<std::remove_cvref_t<std::tuple_element_t<0, arg_tuple>>,
                     flecs::iter>,
      decltype(system_arguments<>::template tuple_tail_elements<arg_tuple>()),
      arg_tuple>;

  constexpr size_t num_args = std::tuple_size_v<remaining_types>;

  return get_system_arguments_impl<F>(std::make_index_sequence<num_args>{});
}

template <typename... Args>
constexpr auto create_system(flecs::world& world, system_arguments<Args...>)
{
  return world.system<Args...>();
}

template <typename... Args>
constexpr auto create_query(flecs::world& world, system_arguments<Args...>)
{
  return world.query<Args...>();
}

template <typename F>
struct IntoSystemBuilder
{
  using args = decltype(get_system_arguments<F>());
  using SystemBuilder =
      decltype(create_system(std::declval<flecs::world&>(), args{}));

 public:
  explicit IntoSystemBuilder( F&& function):
        function_(std::forward<F>(function))
  {
  }
  auto build(flecs::world& world){
    SystemBuilder system_builder (create_system(world, args{}));
    return system_builder.iter(function_);
  }
 private:
 
  F&& function_;
  ;
};

class WorldWrapper
{
 public:
  explicit WorldWrapper(flecs::world& world) : world_(world) {}

  template <typename F>
  void add_system(F&& f)
  {
    add_system(flecs::world & world, Func && f);
  }

  operator flecs::world&() { return world_; }

 private:
  flecs::world& world_;
};

