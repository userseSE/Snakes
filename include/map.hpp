#pragma once
#include "Color.hpp"
#include "Vector2.hpp"
#include "bundle.hpp"
#include <optional>
#include <unordered_map>
#include <utility>
#include "map.hpp"
#include "raylib-cpp.hpp"


enum class TileType : int { EMPTY, WALL };
struct TileColor{
    raylib::Color color = raylib::Color::Gray();
};
struct Parent{};
struct Children{};
struct TilePos {
    int x,y;
    
};
struct TileMap {
  int width;
  int height;
};

struct TileSize {
 int x,y;
};

struct PairHash {
  std::size_t operator()(const std::pair<int, int> &p) const {
    return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
  }
};

struct TileMapStorage : public std::unordered_map<std::pair<int, int>,
                                                  flecs::entity_t, PairHash> {
  std::optional<flecs::entity_t> get_tile(int x, int y) {
    const auto key = std::make_pair(x, y);
    auto it = this->find(key);
    if (it != this->end()) {
      return it->second;
    }
    return std::nullopt;
  }

  flecs::entity_t set_tile(int x, int y, flecs::entity_t e) {
    const auto key = std::make_pair(x, y);
    (*this)[key] = e;
    return e;
  }
};
using TileBundle = basic::Bundle<TileType, TilePos>;
using TileMapBundle = basic::Bundle<TileMap, TileMapStorage,TileSize>;

struct MapPlugin {
  void build(flecs::world &world);
};