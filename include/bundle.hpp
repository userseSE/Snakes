#pragma once

#include <algorithm>
#include <flecs.h>
#include <tuple>
#include <utility>

/// \namespace basic
/// \brief
/// 基本的命名空间，有着实用的工具函数和类，就像美丽的花朵绽放在往世乐土一样哦~♪
namespace basic {

/// \brief 编译时for循环，像小精灵一样在编译时为你跳舞~♪
/// \tparam Start 起始值，舞蹈的开始
/// \tparam End 结束值，舞蹈的终点
/// \tparam Inc 递增值，每一步跳跃的力量
/// \tparam F 函数对象类型，跳舞的精灵
template <auto Start, auto End, auto Inc, class F>
constexpr void constexpr_for(F &&f) {
  if constexpr (Start < End) {
    f(std::integral_constant<decltype(Start), Start>());
    constexpr_for<Start + Inc, End, Inc>(f);
  }
}
/// \brief 将元组中的元素插入到flecs实体，就像将星星点缀在夜空中一样美丽~♪
/// \tparam Tuple 元组类型，闪耀的星星
/// \param tuple 元组实例，璀璨的星空
/// \param world flecs世界实例，美妙的舞台
/// \param e flecs实体ID，接受星星的礼物
template <class Tuple>
constexpr void tuple_insert(const Tuple &tuple, flecs::world &world,
                            flecs::entity_t e) {
  constexpr size_t cnt = std::tuple_size_v<std::decay_t<Tuple>>;
  constexpr_for<size_t(0), cnt, size_t(1)>([&](const auto i) {
    auto a = std::get<i>(tuple);
    world.id(e).entity().emplace<decltype(a)>(std::move(a));
  });
}

/// \brief 从元组创建新的flecs实体，就像在往世乐土中遇见新朋友一样快乐~♪
/// \tparam Tuple 元组类型，我们的共同记忆
/// \param tuple 元组实例，珍贵的回忆
/// \param world flecs世界实例，相遇的奇妙世界
template <class Tuple>
constexpr auto tuple_spawn(const Tuple &tuple, flecs::world &world) {
  auto e = world.entity().id();
  tuple_insert(tuple, world, e);
  return e;
}
/// \brief 组件包装类，就像把我们的心紧紧捆在一起的彩带~♪
/// \tparam Components 组件类型列表，五彩斑斓的心意
template <typename... Components> struct ComponentBundle {
  using Tuple = std::tuple<Components...>;
  Tuple tuple;
  static constexpr auto build(Components &&...Args) {
    return std::make_tuple(Args...);
  }
  ComponentBundle() {}
  ComponentBundle(Components &&...components)
      : tuple(std::forward<Components>(components)...) {}

  constexpr void insert(flecs::world &world, flecs::entity_t e) const {
    constexpr size_t cnt = std::tuple_size_v<std::decay_t<Tuple>>;
    constexpr_for<size_t(0), cnt, size_t(1)>([&](const auto i) {
      auto a = std::get<i>(tuple);
      world.entity(e).set<decltype(a)>(std::move(a));
    });
  }
  constexpr void insert(const flecs::world &world, flecs::entity_t e) const {
    constexpr size_t cnt = std::tuple_size_v<std::decay_t<Tuple>>;
    constexpr_for<size_t(0), cnt, size_t(1)>([&](const auto i) {
      auto a = std::get<i>(tuple);
      world.entity(e).set<decltype(a)>(std::move(a));
    });
  }
  template <typename... OtherComponents>
  constexpr auto merge(OtherComponents... otherComponents) const {
    auto other_tuple = std::make_tuple(otherComponents...);
    auto new_tuple = std::tuple_cat(tuple, other_tuple);
    return createBundleFromTuple(
        new_tuple,
        std::index_sequence_for<Components..., OtherComponents...>());
  }

  constexpr auto spawn(flecs::world &world) const {
    auto e = world.entity().id();
    insert(world, e);
    return e;
  }

  constexpr auto spawn(const flecs::world &world) const {
    auto e = world.entity().id();
    insert(world, e);
    return e;
  }
  template <typename... OtherComponents>
  constexpr ComponentBundle<Components..., OtherComponents...>
  merge(const ComponentBundle<OtherComponents...> &other) const {
    auto mergedTuple = std::tuple_cat(tuple, other.tuple);
    return ComponentBundle<Components..., OtherComponents...>{mergedTuple};
  }
};
/// \brief Bundle类型别名，简化使用，让我们的心跳更轻松愉快~♪
template <typename... Components> using Bundle = ComponentBundle<Components...>;

/// \brief 合并多个ComponentBundle的结构体，让友谊之花熠熠生辉~♪
/// \tparam Bundles 要合并的ComponentBundle类型列表，共同编织的彩带
template <typename... Bundles> struct MergeBundles;

template <typename... Components>
struct MergeBundles<ComponentBundle<Components...>> {
  using Type = ComponentBundle<Components...>;
};

template <typename... Components, typename... OtherComponents,
          typename... OtherBundles>
struct MergeBundles<ComponentBundle<Components...>,
                    ComponentBundle<OtherComponents...>, OtherBundles...> {
  using Type =
      typename MergeBundles<ComponentBundle<Components..., OtherComponents...>,
                            OtherBundles...>::Type;
};

template <typename... Components, typename... OtherBundles>
struct MergeBundles<ComponentBundle<Components...>, OtherBundles...> {
  using Type = typename ComponentBundle<Components...>::template merge<
      typename MergeBundles<OtherBundles...>::Type>;
};

/// \brief 方便地使用MergeBundles的类型别名，让我们的友情之花熠熠生辉~♪
/// \tparam Bundles 要合并的ComponentBundle类型列表，共同编织的彩带
template <typename... Bundles>
using MergeBundles_t = typename MergeBundles<Bundles...>::Type;

} // namespace basic