#include <tuple>
#include <vector>
#include "flecs.h"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/iter.hpp"
#include "map.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"

void draw_line(flecs::world &ecs, TileMap &tilemap, flecs::entity &entity_ref,
               TileMapStorage *&storage) {
  for (int i = 0; i < tilemap.height; i++) {
    auto [x, y] = std::make_tuple(0, i);
    TileBundle b = TileBundle{TileType{TileType::WALL}, TilePos{x, y}};
    auto e = b.spawn(ecs);
    ecs.entity(e).add<Parent>(entity_ref);
    storage->set_tile(x, y, e);
  }
}
void setup(flecs::world &ecs) {
  auto tilemap = TileMap{100, 100};
  TileMapBundle a =
      TileMapBundle{TileMap{tilemap}, TileMapStorage{}, TileSize{64, 64}};

  auto entity = a.spawn(ecs);
  auto entity_ref = ecs.entity(entity);
  auto storage = entity_ref.get_mut<TileMapStorage>();
  draw_line(ecs, tilemap, entity_ref, storage);
  for (int i = 0; i < tilemap.width; i++) {
    auto [x, y] = std::make_tuple(i, 0);
    TileBundle b = TileBundle{TileType{TileType::WALL}, TilePos{x, y}};
    auto e = b.spawn(ecs);
    ecs.entity(e).add<Parent>(entity_ref);
    storage->set_tile(x, y, e);
  }
}
void init_color(flecs::iter &it, TilePos * pos,TileType * type){
    for(int i=0;i<it.count();i++){

        switch (type[i]) {

        case TileType::EMPTY:
          break;
        case TileType::WALL:
             raylib::Rectangle rect(pos[i].x*64, pos[i].y*64, 64, 64);
            rect.Draw(raylib::Color::Red());
          break;
        }
    }

}

int main(int argc, char* argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    //init world
    flecs::world ecs;
    setup(ecs);
    auto system = ecs.system<TilePos,TileType >().iter( init_color);

    int screenWidth = 1600;
    int screenHeight = 900;
    raylib::Color textColor = raylib::Color::LightGray();
    raylib::Window window(screenWidth, screenHeight, "贪吃蛇");






    //SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose()) {   // Detect window close button or ESC key
        // Update (ECS世界以每秒60次的速率更新)
       // ecs.progress(1.0/60);

        //----------------------------------------------------------------------------------
        // Update your variables here
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
         system.run();
        {
            window.ClearBackground(RAYWHITE);
            textColor.DrawText("双人贪吃蛇游戏", 190, 200, 20);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }


    return 0;
}
