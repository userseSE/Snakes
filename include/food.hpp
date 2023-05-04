#pragma once

#include <flecs.h>
#include "map.hpp"
#include "snake.hpp"


struct Food{
    TilePos pos;
};

//在地图上生成食物
void spawn_food(flecs::world &ecs, TileMap &tile_map, Snake &snake);

//在不与蛇身体碰撞的情况下随机选择食物的位置
TilePos random_food_pos(flecs::world &ecs,TileMap &tile_map, Snake &snake);
