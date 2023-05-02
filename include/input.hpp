#pragma once
#include <raylib-cpp.hpp>
#include <optional>
enum class Direction { LEFT, RIGHT, UP, DOWN };
auto handle_input() -> std::optional<Direction>;
