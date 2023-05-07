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
 //初始化蛇的身体
void init_snake_bodies(flecs::iter &it, SnakeSpawn *snakes) {

  auto snake = Snake{}; // Snake component
  for (int i = 0; i < it.count(); i++) {  // For each entity with SnakeSpawn
    auto snakedata = snakes[i];          // Get SnakeSpawn component

    SnakeBodyBundle body = SnakeBodyBundle{ // Create SnakeBodyBundle
        SnakeBody{SnakeBody::HEAD},     // SnakeBody
        TilePos{*snakedata.pos.begin()},  // TilePos
        TileSize{8, 8},             // TileSize
        raylib::Color{raylib::Color::Orange()}};    // Color
    auto e = body.spawn(it.world());  // Spawn entity
    snake.body.emplace_back(e);      // Add entity to snake body (其功能和push_back()相同，都是在vector容器的尾部添加一个元素)

    std::for_each(snakedata.pos.begin() + 1, snakedata.pos.end(),   
                  [&](TilePos &pos) {
                    SnakeBodyBundle body = SnakeBodyBundle{
                        SnakeBody{SnakeBody::BODY}, 
                        TilePos{pos},
                        TileSize{8, 8}, 
                        raylib::Color{raylib::Color::Red()}}; 
                        //for_each遍历容器中的每个元素，并对每个元素执行相同的操作

                    auto e = body.spawn(it.world());  // Spawn entity
                    it.world().entity(e).child_of(it.entity(i));  // Set entity as child of snake
                    snake.body.emplace_back(e); // Add entity to snake body
                  });

    //从具有 SnakeSpawn 组件的实体中移除 SnakeSpawn 组件，因为已经初始化了蛇的身体。
    it.entity(i).remove<SnakeSpawn>();  // Remove SnakeSpawn component
    //将 Snake 组件添加到具有 SnakeSpawn 组件的实体，这将在后续的系统中使用它。
    it.entity(i).set(snake);          // Set Snake component
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
 //根据具有 SnakeBody 组件的实体初始化贪吃蛇的图形
void init_snake_graphic(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                        raylib::Color *color, TileSize *size) { 

  for (int i = 0; i < it.count(); i++) {  // For each entity with SnakeBody
    switch (snakes[i]) {  // Set color based on SnakeBody type

    case SnakeBody::HEAD: // If head, set color to orange
      color[i] = raylib::Color::Orange();
      break;
    case SnakeBody::TAIL:
    case SnakeBody::BODY:
      color[i] = raylib::Color::Red();
      break;
    }
    raylib::Rectangle rect(pos[i].x * size[i].x, pos[i].y * size[i].y,
                           size[i].x, size[i].y); // Create rectangle

    it.entity(i).set(rect); // Set rectangle component
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
 //更新和渲染具有 SnakeBody 组件的实体
 //根据实体的 SnakeBody 类型设置颜色
 //根据实体的 TilePos 和 TileSize 更新实体的矩形位置
void update_render_snake(flecs::iter &it, SnakeBody *snakes, TilePos *pos,
                         raylib::Color *color, TileSize *size,
                         raylib::Rectangle *rects) {

  for (int i = 0; i < it.count(); i++) {  // For each entity with SnakeBody

    switch (snakes[i]) {  // Set color based on SnakeBody type

    case SnakeBody::HEAD:
      color[i] = raylib::Color::Orange();
      break;
    case SnakeBody::TAIL:
    case SnakeBody::BODY:
      color[i] = raylib::Color::Red();
      break;
    }
    rects[i].SetPosition(size[i].x * pos[i].x, size[i].y * pos[i].y); // Set rectangle position
  }
}

/**

@brief Move the snake based on its current direction.
@param it Flecs iterator containing the entities with Snake components.
@param snakes Pointer to Snake components.
@param dirs Pointer to Direction components.
*/
//移动蛇
//根据蛇当前的方向移动具有 Snake 组件的实体
void move_snake(flecs::iter &it, Snake *snakes, Direction *dirs) {

 
  //遍历实体，对于每个具有 Snake 组件的实体：
  //a. 获取实体的方向组件。
  //b. 获取蛇头部实体的位置。
  //c. 将蛇头部实体的类型设置为身体。
  //d. 计算蛇头部实体在当前方向上的新位置。
  //e. 获取蛇尾部实体并从蛇的身体中删除。
  //f. 创建一个新的蛇头部实体，并将其插入到蛇的身体中。
  for (int i = 0; i < it.count(); i++) {  // For each entity with Snake
    auto dir = dirs[i];                  // Get Direction component
    auto &snake = snakes[i];
    flecs::entity_t head = *snake.body.begin(); // Get snake head entity
    auto pos = it.world().entity(head).get<TilePos>();  // Get snake head position
    auto body_type = it.world().entity(head).get_mut<SnakeBody>();  // Get snake head body type
    *body_type = SnakeBody::BODY; // Set snake head body type to body
    auto dir_vector = GetDirectionVector(dir);  // Get direction vector
    auto new_pos =
        TilePos{pos->x + dir_vector.first, pos->y + dir_vector.second}; // Calculate new position

    auto tail = snake.body.back();  // Get snake tail entity
    // printf("%d %d\n",tail,head);
    snake.body.pop_back();  // Remove tail from snake body


    SnakeBodyBundle body =
        SnakeBodyBundle{SnakeBody{SnakeBody::HEAD}, TilePos{new_pos},
                        TileSize{8, 8}, raylib::Color{raylib::Color::Orange()}}; // Create new head  

    body.insert(it.world(), tail);  

    snake.body.push_front(tail);  


  }
}

/*
  蛇是由一系列实体组成的，每个实体代表蛇的一个部分（头部、身体或尾部）。
  这些实体具有多个组件，它们包括：

  1.SnakeBody：表示蛇的部分类型（头部、身体或尾部）。
  2.TilePos：表示蛇的部分在地图上的位置，用网格坐标表示。
  3.TileSize：表示蛇部分的大小，以像素为单位。
  4.raylib::Color：表示蛇的部分的颜色。

  通过组合这些组件，可以在游戏中表示和控制蛇的不同部分。
  例如，可以用TilePos组件来确定蛇的头部在地图上的位置，以便在蛇移动时更新其位置。
  可以使用raylib::Color组件在渲染时为蛇的各个部分设置颜色。

  另外，贪吃蛇游戏中的蛇实体还具有以下组件：

  1.Snake：这个组件包含了一个列表（std::vector<flecs::entity_t>），它存储了组成蛇身体的实体。使得可以轻松地访问和修改蛇的各个部分。
  2.Direction：表示蛇当前的移动方向。它可以是上、下、左或右。在每个游戏帧中，根据这个方向移动蛇。

  通过这些组件，可以在游戏中创建、更新和控制蛇。
  例如，当蛇移动时，可以通过Direction组件计算蛇头部的新位置，并根据需要添加或删除身体部分。
  在渲染过程中，可以使用raylib::Rectangle组件为蛇的各个部分绘制矩形。
*/
