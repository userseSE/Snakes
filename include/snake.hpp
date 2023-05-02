#pragma once
#include "flecs.h"
#include "map.hpp"
#include <deque>
#include <vector>

struct Snake{
    std::deque<flecs::entity_t> body;
};

struct SnakeSpawn{
    std::vector<TilePos> pos;
};
enum class SnakeBody{
    HEAD,
    BODY,
    TAIL
};

struct MoveTo{
    int x, y;
};

void init_snake_bodies(flecs::iter & it, SnakeSpawn * snakes);
void init_snake_graphic(flecs::iter &it, Snake *snakes);