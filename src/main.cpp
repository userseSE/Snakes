#include "Color.hpp"
#include "Rectangle.hpp"
#include "flecs.h"
#include "flecs/addons/cpp/mixins/pipeline/decl.hpp"
#include "input.hpp"
#include "map.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include "snake.hpp"
#include "system_helper.hpp"
#include "utils.hpp"
#include <tuple>
#include <utility>
#include <vector>

void setup(flecs::world &ecs) {
  auto tilemap = TileMap{100, 100};
  TileMapBundle a =
      TileMapBundle{TileMap{tilemap}, TileMapStorage{}, TileSize{8, 8}};

  auto entity = a.spawn(ecs);
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
  setup(ecs);
  auto system =
      ecs.system<TilePos, TileType, const TileSize>().term_at(3).parent().iter(
          init_color);
  auto draw_rect =
      ecs.system<raylib::Rectangle, raylib::Color>().term_at(2).optional().iter(
          draw_rects);
  IntoSystemBuilder builder(init_snake_bodies);
  IntoSystemBuilder builder2(init_snake_graphic);
  IntoSystemBuilder move_snake_system(move_snake);
  IntoSystemBuilder update_render_snake_system(update_render_snake);
  auto snake_system = builder.build(ecs);
  auto snake_system2 = builder2.build(ecs);
  auto snake_system3 = move_snake_system.build(ecs);
  auto snake_system4 = update_render_snake_system.build(ecs);

  snake_system3.interval(0.1);
  snake_system3.depends_on(flecs::PreUpdate);
  snake_system4.depends_on(flecs::PostUpdate);
  int screenWidth = 1600;
  int screenHeight = 900;
  raylib::Color textColor = raylib::Color::LightGray();
  raylib::Window window(screenWidth, screenHeight, "贪吃蛇");
  ecs.entity()
      .set<SnakeSpawn>(
          SnakeSpawn{{TilePos{1, 3}, TilePos{1, 2}, TilePos{1, 1}}})
      .set<Direction>(Direction::DOWN);
  snake_system.depends_on(flecs::OnStart);

  system.depends_on(flecs::OnStart);

  SetTargetFPS(60);
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
    BeginDrawing();

    { window.ClearBackground(RAYWHITE); }
    ecs.progress();
    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  return 0;
}
