#pragma once

#include "map.hpp"
#include "snake.hpp"
#include <flecs.h>

struct Food {
  TilePos pos;
};

struct FoodSpawner {
  TileMap tile_map;
  int food_number;
};

auto spawn_food_system(flecs::world &world) -> flecs::system;
auto food_to_map_system(flecs::world &world) -> flecs::system;
