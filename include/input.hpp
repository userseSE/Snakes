#pragma once
#include "flecs.h"
#include <optional>
#include <raylib-cpp.hpp>

enum class Direction { LEFT, RIGHT, UP, DOWN };

struct SnakeController {
  int player_id;
};

auto input_system(flecs::world &world) -> flecs::system;
auto handle_input() -> std::optional<Direction>;
