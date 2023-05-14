#pragma once
#include "Color.hpp"
#include "Vector2.hpp"
#include "bundle.hpp"
#include "flecs/addons/cpp/world.hpp"
#include "map.hpp"
#include "raylib-cpp.hpp"
#include "system_helper.hpp"
#include <optional>
#include <stdint.h>
#include <stdio.h>
#include <unordered_map>
#include <utility>

enum class TileType : int { EMPTY, WALL };
struct GameMap{
    ecs_entity_t walls;
    ecs_entity_t foods;
};
struct TileColor {
  raylib::Color color = raylib::Color::Gray();
};

struct TilePos {
  int x, y;
};
struct TileMap {
  int width;
  int height;
};

struct TileSize {
  int x, y;
};

struct PairHash {
  std::size_t operator()(const std::pair<int, int> &p) const {
    return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
  }
};

struct TileMapStorage : public std::unordered_map<std::pair<int, int>,
                                                  flecs::entity_t, PairHash> {
  std::optional<flecs::entity_t> get_tile(int x, int y) const{
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

struct MapPlugin {
  void build(flecs::world &world);
};

struct OccupiedTiles
    : public std::unordered_map<std::pair<int, int>, bool, PairHash> {

  bool is_occupied(int x, int y) const {
    const auto key = std::make_pair(x, y);
    auto it = this->find(key);
    if (it == this->end()) {
      return false;
    }
    return this->at(key);
  }

  void set_occupied(int x, int y) {
    const auto key = std::make_pair(x, y);
    (*this)[key] = true;
  }
  void reset_occupied(int x, int y) {
    const auto key = std::make_pair(x, y);
    (*this)[key] = false;
  }
};

struct OccupiedTilePlugin{
  void build(flecs::world & world);
};
using TileMapBundle = basic::Bundle<TileMap, TileMapStorage, TileSize>;
