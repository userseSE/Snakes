#include "server.hpp"
#include "Color.hpp"
#include "flecs.h"
#include "food.hpp"
#include "input.hpp"
#include "map.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include "snake.hpp"
#include "system_helper.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <tuple>
#include <utility>
#include <vector>


using CollisionQuery = flecs::query<const Rectangle, const Color>;

void setup(flecs::world &ecs) {
  auto tilemap = TileMap{100, 100};
  auto gamemap = GameMap{};
  TileMapBundle a =
      TileMapBundle{TileMap{tilemap}, TileMapStorage{}, TileSize{8, 8}};
  ecs.set(OccupiedTiles{});
  auto food_spawn =
      ecs.entity().set(FoodSpawner{tilemap, 3}).set(TileMapStorage{});
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
      e.set(raylib::Color::Black());
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
  OccupiedTilePlugin p1;
  ZmqServerPlugin server;
  server.build(ecs);
  setup(ecs); // setup tilemap

  auto system =
      ecs.system<TilePos, TileType, const TileSize>().term_at(3).parent().iter(
          init_color); // init color

  IntoSystemBuilder builder(init_snake_bodies);   // init snake
  IntoSystemBuilder builder2(init_snake_graphic); // init snake graphic
  IntoSystemBuilder move_snake_system(move_snake);  // move snake
  IntoSystemBuilder update_render_snake_system(update_render_snake);  // update and render

  auto spawn_food = spawn_food_system(ecs); // spawn food
  auto map_food = food_to_map_system(ecs);  // map food
  spawn_food.depends_on(flecs::PostUpdate); // spawn food depends on postupdate
  map_food.depends_on(spawn_food);        // map food depends on spawn food

  p1.build(ecs);                          // build occupied tile plugin
  auto snake_system = builder.build(ecs); // build snake system
  snake_system.set_name("init_system_bodies");
  auto snake_system2 = builder2.build(ecs);
  auto snake_system3 = move_snake_system.build(ecs);
  auto snake_system4 = update_render_snake_system.build(ecs);
  // auto draw_rect =
  //     ecs.system<raylib::Rectangle,
  //     raylib::Color>().term_at(2).optional().iter(
  //         draw_rects);                            // draw rect

  snake_system3.interval(0.1);
  snake_system.depends_on(flecs::PreUpdate);
  snake_system3.depends_on(flecs::PreUpdate);
  snake_system4.depends_on(flecs::OnUpdate);

  // int screenWidth = 1600;
  // int screenHeight = 900;
  // raylib::Color textColor = raylib::Color::LightGray();
  // raylib::Window window(screenWidth, screenHeight, "贪吃蛇服务端");

  system.depends_on(flecs::OnStart);
  ecs.set_target_fps(60);
  // SetTargetFPS(60);

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (ecs.progress()) { // Detect window close button or ESC key
    // Update (ECS世界以每秒60次的速率更新)
    // ecs.progress(1.0/60);

    //----------------------------------------------------------------------------------
    // Update your variables here
    //----------------------------------------------------------------------------------
    // Draw
    //----------------------------------------------------------------------------------
    // BeginDrawing();

    // { window.ClearBackground(RAYWHITE); }

    // std::cout << "Before ecs.progress()" << std::endl;

    // std::cout << "After ecs.progress()" << std::endl;

    //  EndDrawing();
    //----------------------------------------------------------------------------------
  }

  return 0;
}
