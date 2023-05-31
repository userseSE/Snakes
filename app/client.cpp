#include "client.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "flecs.h"
#include "food.hpp"
#include "input.hpp"
#include "map.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include "server.hpp"
#include "snake.hpp"
#include "system_helper.hpp"
#include "utils.hpp"
#include <iostream>
#include <memory>
#include <stdio.h>
#include <tuple>
#include <utility>
#include <vector>

void setup(flecs::world &ecs) {
  // 设置图块地图和游戏地图的初始状态
  auto tilemap = TileMap{100, 100};
  auto gamemap = GameMap{};
  TileMapBundle a =
      TileMapBundle{TileMap{tilemap}, TileMapStorage{}, TileSize{8, 8}};
  ecs.set(OccupiedTiles{});
  auto food_spawn =
      ecs.entity().set(FoodSpawner{tilemap, 5000}).set(TileMapStorage{});
  auto entity = a.spawn(ecs);

  gamemap.walls = entity;
  gamemap.foods = food_spawn;
  ecs.set(gamemap);

  auto entity_ref = ecs.entity(entity);
  auto &storage = *entity_ref.get_mut<TileMapStorage>();
  auto wall_fill = [](flecs::entity &e) { e.set<TileType>(TileType::WALL); };

  draw_line_with_func(ecs, entity_ref, storage, TilePos{0, 0},
                      TilePos{0, tilemap.height}, wall_fill);
  draw_line_with_func(ecs, entity_ref, storage, TilePos{0, 0},
                      TilePos{tilemap.width, 0}, wall_fill);
  draw_line_with_func(ecs, entity_ref, storage, TilePos{0, tilemap.height},
                      TilePos{tilemap.width, tilemap.height}, wall_fill);
  draw_line_with_func(ecs, entity_ref, storage, TilePos{tilemap.width, 0},
                      TilePos{tilemap.width, tilemap.height}, wall_fill);
}

void init_color(flecs::iter &it, TilePos *pos, TileType *type,
                const TileSize *tilesize) {
  it.world().defer_begin();
  for (int i = 0; i < it.count(); i++) {
    auto e = it.entity(i);
    auto size = tilesize[i].x;
    switch (type[i]) {

    case TileType::EMPTY:
      break;
    case TileType::WALL:
      raylib::Rectangle rect(pos[i].x * size, pos[i].y * size, size, size);
      e.set(rect);

      break;
    }
  }
  it.world().defer_end();
}

void draw_rects(flecs::iter &it, raylib::Rectangle *rects,
                raylib::Color *colors) {
  for (int i = 0; i < it.count(); i++) {
    auto color = raylib::Color::Black();
    if (colors) {
      color = colors[i];
    }
    rects[i].Draw(color);
  }
}

int main(int argc, char *argv[]) {
  // Initialization
  //--------------------------------------------------------------------------------------
  // init world

  flecs::world ecs;

  ZmqClientPlugin client;
  printf("test\n");
  ecs.set<ZmqClientRef>(
      std::move(ZmqClientRef{std::make_shared<ZmqClient>(2)}));
  printf("test\n");
  ecs.set<ServerAddress>({"tcp://127.0.0.1:5551"});
  client.build(ecs);

  auto controller = input_system(ecs); // input

  controller.depends_on(flecs::PreUpdate);
  // 在每次更新之前，flecs::PreUpdate系统都会被调用
  int screenWidth = 1600;
  int screenHeight = 900;
  raylib::Color textColor = raylib::Color::LightGray();
  raylib::Window window(screenWidth, screenHeight, "贪吃蛇");

  ecs.entity()
      .set<SnakeSpawn>(
          SnakeSpawn{{TilePos{1, 3}, TilePos{1, 2}, TilePos{1, 1}}})
      .set<Direction>(Direction::RIGHT)
      .set<SnakeController>({987});

  // snake_system.depends_on(flecs::OnStart);

  // system.depends_on(flecs::OnStart);
  printf("start");
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!window.ShouldClose()) { // Detect window close button or ESC key
    // Update (ECS世界以每秒60次的速率更新)
    // ecs.progress(1.0/60);

    //----------------------------------------------------------------------------------
    // Update your variables here
    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    
    ecs.progress(); // 更新ecs世界

    // 获取所有具有raylib::Rectangle和raylib::Color组件的实体
    auto queryRect = ecs.query<raylib::Rectangle, raylib::Color>();

    // 清除屏幕
    BeginDrawing();
    { window.ClearBackground(RAYWHITE); }

    // 遍历每个实体并绘制矩形
    queryRect.each([&](raylib::Rectangle &rect, raylib::Color &color) {
      DrawRectangleRec(rect, color);
    });

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  return 0;
}
