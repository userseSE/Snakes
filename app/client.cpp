#include "client.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "flecs.h"
#include "flecs/addons/cpp/world.hpp"
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
#include <string>
#include <tuple>
#include <utility>
#include <vector>
#include <fstream>

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

//获取ip
std::string init_ip(flecs::world &ecs) {
  // 构建JSON文件路径，相对于exe文件所在目录
  std::string jsonFilePath = "config_client.json";

  // 读取JSON文件
  std::ifstream file(jsonFilePath);

  // 解析JSON文件内容
  json data = json::parse(file);

  // 将解析后的JSON对象传递给UserDatabase，将其转换为UserDatabase对象
  std::string ip = data["ip"];

  std::cout << ip << std::endl;

  file.close();

  return ip;
}

int main(int argc, char *argv[]) {
  // Initialization
  //--------------------------------------------------------------------------------------
  // init world

  flecs::world ecs;

  ZmqClientPlugin client;

  client.build(ecs);

  ecs.set<ZmqClientRef>(
      ZmqClientRef{std::make_shared<ZmqClient>(2)});
  
  std::string ip=init_ip(ecs);
  ecs.set<ServerAddress>({ip});

  std::cout << ecs.get<ServerAddress>() << std::endl;


  auto controller = input_system(ecs); // input

  controller.depends_on(flecs::PreUpdate);
  // 在每次更新之前，flecs::PreUpdate系统都会被调用
  int screenWidth = 1600;
  int screenHeight = 900;

  raylib::Window window(screenWidth, screenHeight, "贪吃蛇");

  // snake_system.depends_on(flecs::OnStart);
  // system.depends_on(flecs::OnStart);
  printf("start");
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------
  // 获取所有具有raylib::Rectangle和raylib::Color组件的实体
  auto queryRect = ecs.query<raylib::Rectangle, raylib::Color>();
  // Main game loop
  while (!window.ShouldClose()) { // Detect window close button or ESC key
    // Update (ECS世界以每秒60次的速率更新)w

    ecs.progress(); // 更新ecs世界
    //----------------------------------------------------------------------------------
    // Update your variables here
    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------



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
