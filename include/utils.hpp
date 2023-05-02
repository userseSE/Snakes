#include "bundle.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "map.hpp"
template <class F>
inline void draw_line_with_func(flecs::world &ecs, flecs::entity &entity_ref,
                         TileMapStorage &storage, const TilePos &start,
                         const TilePos &end, F &&func) {
  auto [start_x, start_y] = std::make_tuple(start.x, start.y);
  auto [end_x, end_y] = std::make_tuple(end.x, end.y);
  int dx = std::abs(end_x - start_x);
  int dy = std::abs(end_y - start_y);
  int x = start_x;
  int y = start_y;
  int sx = start_x < end_x ? 1 : -1;
  int sy = start_y < end_y ? 1 : -1;
  int err = dx - dy;

  while (true) {
    TileBundle b = TileBundle{TileType{TileType::WALL}, TilePos{x, y}};

    if (storage.get_tile(x, y).has_value()) {
      auto e = storage.get_tile(x, y).value();
      ecs.entity(e).clear();
      ecs.entity(e).child_of(entity_ref);
      b.insert(ecs, e);
    } else {
      auto e = b.spawn(ecs);
      ecs.entity(e).child_of(entity_ref);
      storage.set_tile(x, y, e);
    }
    auto e = storage.get_tile(x, y).value();
    auto eref = ecs.entity(e);
    std::forward<F>(func)(eref);

    if (x == end_x && y == end_y)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}

inline void draw_line(flecs::world &ecs, flecs::entity &entity_ref,
               TileMapStorage &storage, const TilePos &start,
               const TilePos &end) {
  auto [start_x, start_y] = std::make_tuple(start.x, start.y);
  auto [end_x, end_y] = std::make_tuple(end.x, end.y);
  int dx = std::abs(end_x - start_x);
  int dy = std::abs(end_y - start_y);
  int x = start_x;
  int y = start_y;
  int sx = start_x < end_x ? 1 : -1;
  int sy = start_y < end_y ? 1 : -1;
  int err = dx - dy;

  while (true) {
    TileBundle b = TileBundle{TileType{TileType::EMPTY}, TilePos{x, y}};

    if (storage.get_tile(x, y).has_value()) {
      auto e = storage.get_tile(x, y).value();
      ecs.entity(e).child_of(entity_ref);
      b.insert(ecs, e);
    } else {
      auto e = b.spawn(ecs);
      ecs.entity(e).child_of(entity_ref);
      storage.set_tile(x, y, e);
    }

    if (x == end_x && y == end_y)
      break;
    int e2 = 2 * err;
    if (e2 > -dy) {
      err -= dy;
      x += sx;
    }
    if (e2 < dx) {
      err += dx;
      y += sy;
    }
  }
}