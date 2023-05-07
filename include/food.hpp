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



//初始化食物图形
void init_food_graphic(flecs::iter &it, Food *foods, TilePos *pos,  TileSize *size);
