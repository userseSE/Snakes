#include "snake.hpp"
#include "Color.hpp"
#include "bundle.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/iter.hpp"
#include "map.hpp"
#include <algorithm>
#include <stdio.h>

using SnakeBodyBundle = basic::Bundle<SnakeBody, TilePos, TileSize, raylib::Color>;
void init_snake_bodies(flecs::iter &it, SnakeSpawn *snakes) {


  auto snake = Snake{};
  for (int i = 0; i < it.count(); i++) {
    auto snakedata = snakes[i];

    SnakeBodyBundle body = SnakeBodyBundle{
        SnakeBody{SnakeBody::BODY}, TilePos{*snakedata.pos.begin()},
        TileSize{8, 8}, raylib::Color{raylib::Color::Orange()}};
    auto e = body.spawn(it.world());
    snake.body.emplace_back(e);

    std::for_each(
        snakedata.pos.begin() + 1, snakedata.pos.end(), [&](TilePos &pos) {
          SnakeBodyBundle body =
              SnakeBodyBundle{SnakeBody{SnakeBody::BODY}, TilePos{pos},
                              TileSize{8, 8}, raylib::Color{raylib::Color::Red()}};

          auto e = body.spawn(it.world());
        
          snake.body.emplace_back(e);
          
        });
 
    it.entity(i).remove<SnakeSpawn>();
    it.entity(i).set(snake);
    
    
  }
}
void init_snake_graphic(flecs::iter &it, Snake *snakes) {
 
  for (int i = 0; i < it.count(); i++) {
    auto snake = snakes[i];
    std::for_each(snake.body.begin(), snake.body.end(), [&](flecs::entity_t e) {

      auto pos = it.world().entity(e).get<TilePos>();
      auto tilesize = it.world().entity(e).get<TileSize>();
      auto size = tilesize->x;
      raylib::Rectangle rect(pos->x * size, pos->y * size, size, size);
      it.world().entity(e).set(rect);
    });
  }
}
