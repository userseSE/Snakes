#include "input.hpp"
#include "flecs/addons/cpp/iter.hpp"
#include "system_helper.hpp"

auto handle_input() -> std::optional<Direction> {
  std::optional<Direction> dir = std::nullopt;
  if (IsKeyDown(KEY_RIGHT)) {
    if (dir != Direction::LEFT)
      dir = Direction::RIGHT;
  }

  else if (IsKeyDown(KEY_LEFT)) {
    if (dir != Direction::RIGHT)
      dir = Direction::LEFT;
  }

  else if (IsKeyDown(KEY_UP)) {
    if (dir != Direction::DOWN)
      dir = Direction::UP;
  } else if (IsKeyDown(KEY_DOWN)) {
    if (dir != Direction::UP)
      dir = Direction::DOWN;
  }
  return dir;
}

Direction oppositeDirection(Direction dir) {
  switch (dir) {
  case Direction::LEFT:
    return Direction::RIGHT;
  case Direction::RIGHT:
    return Direction::LEFT;
  case Direction::UP:
    return Direction::DOWN;
  case Direction::DOWN:
    return Direction::UP;
  }
}

inline void controller(flecs::iter &it, SnakeController *controller) {
  for (int i = 0; i < it.count(); i++) {
    auto s = handle_input();
    if (!s.has_value()) {
      continue;
    }

    auto newDirection = s.value();
    auto currentDirection = it.entity(i).get_mut<Direction>();
    if (!currentDirection) {
      it.entity(i).set(newDirection);
      continue;
    }

    if (newDirection != oppositeDirection(*currentDirection)) {
      *currentDirection = newDirection;
    }
  }
}
auto input_system(flecs::world &world) -> flecs::system {
  auto sig = IntoSystemBuilder(controller);
  return sig.build(world);
}