#pragma once
#include "flecs.h"
#include "input.hpp"
#include "map.hpp"
#include <deque>
#include <vector>

struct Snake {
  std::deque<flecs::entity_t> body;
  //std::deque是一种双端队列容器，允许在其首尾两端快速插入及删除。
  //double-ended queue
};

struct SnakeSpawn {
  std::vector<TilePos> pos;
};
enum class SnakeBody { HEAD, BODY, TAIL };

struct MoveTo {
  int x, y;
};

void init_snake_bodies(flecs::iter &it, SnakeSpawn *snakes);
void init_snake_graphic(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                        raylib::Color *color, TileSize *size);

void move_snake(flecs::iter &it, Snake *snakes, Direction *dirs);
void update_render_snake(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                         raylib::Color *color, TileSize *size,
                         raylib::Rectangle *rects);