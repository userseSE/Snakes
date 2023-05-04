#pragma once

#include <flecs.h>
#include "map.hpp"
#include "snake.hpp"


struct Food{
    TilePos pos;
};

struct FoodSpawner{
    TileMap tile_map;
    int food_number;
};

//在地图上生成食物
void spawn_food(flecs::world &ecs);

//在不与蛇身体碰撞的情况下随机选择食物的位置
TilePos random_food_pos(flecs::world &ecs);

//更新和渲染食物
void update_render_food(flecs::iter &it, Food *foods, TilePos *pos, raylib::Color *color, TileSize *size, raylib::Rectangle *rects);

//初始化食物图形
void init_food_graphic(flecs::iter &it, Food *foods, TilePos *pos, raylib::Color *color, TileSize *size);
