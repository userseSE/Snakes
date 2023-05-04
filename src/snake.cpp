/**
 * @file snake.hpp
 * @brief Snake game implementation.
 * 
 * This file contains the implementation of the snake game using flecs and raylib.
 */

#include "snake.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "bundle.hpp"
#include "food.hpp"
#include "map.hpp"
#include "utils.hpp"
#include <algorithm>
#include <stdio.h>

using SnakeBodyBundle =
    basic::Bundle<SnakeBody, TilePos, TileSize, raylib::Color>;

/**
 * @brief Initialize snake bodies.
 * 
 * @param it Flecs iterator containing the entities with SnakeSpawn components.
 * @param snakes Pointer to SnakeSpawn components.
 */
void init_snake_bodies(flecs::iter &it, SnakeSpawn *snakes) {

  auto snake = Snake{};
  for (int i = 0; i < it.count(); i++) {
    auto snakedata = snakes[i];

    SnakeBodyBundle body = SnakeBodyBundle{
        SnakeBody{SnakeBody::HEAD}, TilePos{*snakedata.pos.begin()},
        TileSize{8, 8}, raylib::Color{raylib::Color::Orange()}};
    auto e = body.spawn(it.world());
    snake.body.emplace_back(e);

    std::for_each(snakedata.pos.begin() + 1, snakedata.pos.end(),
                  [&](TilePos &pos) {
                    SnakeBodyBundle body = SnakeBodyBundle{
                        SnakeBody{SnakeBody::BODY}, TilePos{pos},
                        TileSize{8, 8}, raylib::Color{raylib::Color::Red()}};

                    auto e = body.spawn(it.world());
                    it.world().entity(e).child_of(it.entity(i));
                    snake.body.emplace_back(e);
                  });

    it.entity(i).remove<SnakeSpawn>();
    it.entity(i).set(snake);
  }
}

/**
 * @brief Initialize snake graphics.
 * 
 * @param it Flecs iterator containing the entities with SnakeBody components.
 * @param snakes Pointer to SnakeBody components.
 * @param pos Pointer to TilePos components.
 * @param color Pointer to raylib::Color components.
 * @param size Pointer to TileSize components.
 */
void init_snake_graphic(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                        raylib::Color *color, TileSize *size) {

  for (int i = 0; i < it.count(); i++) {
    switch (snakes[i]) {

    case SnakeBody::HEAD:
      color[i] = raylib::Color::Orange();
      break;
    case SnakeBody::TAIL:
    case SnakeBody::BODY:
      color[i] = raylib::Color::Red();
      break;
    }
    raylib::Rectangle rect(pos[i].x * size[i].x, pos[i].y * size[i].y,
                           size[i].x, size[i].y);

    it.entity(i).set(rect);
  }
}

/**
 * @brief Update and render the snake.
 * 
 * @param it Flecs iterator containing the entities with SnakeBody components.
 * @param snakes Pointer to SnakeBody components.
 * @param pos Pointer to TilePos components.
 * @param color Pointer to raylib::Color components.
 * @param size Pointer to TileSize components.
 * @param rects Pointer to raylib::Rectangle components.
 */
void update_render_snake(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                         raylib::Color *color, TileSize *size,
                         raylib::Rectangle *rects) {

  for (int i = 0; i < it.count(); i++) {

    switch (snakes[i]) {

    case SnakeBody::HEAD:
      color[i] = raylib::Color::Orange();
      break;
    case SnakeBody::TAIL:
    case SnakeBody::BODY:
      color[i] = raylib::Color::Red();
      break;
    }
    rects[i].SetPosition(size[i].x * pos[i].x, size[i].y * pos[i].y);
  }
}

/**

@brief Move the snake based on its current direction.
@param it Flecs iterator containing the entities with Snake components.
@param snakes Pointer to Snake components.
@param dirs Pointer to Direction components.
*/
void move_snake(flecs::iter &it, Snake *snakes, Direction *dirs) {

  for (int i = 0; i < it.count(); i++) {
    auto dir = dirs[i];
    auto &snake = snakes[i];
    flecs::entity_t head = *snake.body.begin();
    auto pos = it.world().entity(head).get<TilePos>();
    auto body_type = it.world().entity(head).get_mut<SnakeBody>();
    *body_type = SnakeBody::BODY;
    auto dir_vector = GetDirectionVector(dir);
    auto new_pos =
        TilePos{pos->x + dir_vector.first, pos->y + dir_vector.second};

    auto tail = snake.body.back();
    // printf("%d %d\n",tail,head);
    snake.body.pop_back();

    SnakeBodyBundle body =
        SnakeBodyBundle{SnakeBody{SnakeBody::HEAD}, TilePos{new_pos},
                        TileSize{8, 8}, raylib::Color{raylib::Color::Orange()}};

    body.insert(it.world(), tail);

    snake.body.push_front(tail);
    // {
    //   flecs::entity_t head = *snake.body.begin();
    //   auto pos = it.world().entity(head).get<TilePos>();
    //   printf("%d %d\n",pos->x,pos->y);
    // }
  }
}
