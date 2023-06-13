#include "food.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "bundle.hpp"
#include "flecs/addons/cpp/c_types.hpp"
#include "flecs/addons/cpp/world.hpp"
#include "map.hpp"
#include "snake.hpp"
#include "system_helper.hpp"
#include "utils.hpp"
#include <algorithm>
#include <chrono> //时间库
#include <flecs.h>
#include <random>
#include <stdio.h>

// 使用别名（alias）定义一个叫FoodBundle的新模板类，
// 四个模板参数：Food、TilePos、TileSize 和 raylib::Color
using FoodBundle =
    basic::Bundle<Food, TilePos, TileSize, raylib::Color, raylib::Rectangle>;
TilePos random_food_pos(const FoodSpawner &food_spawner,
                        const OccupiedTiles &occupied_tiles);
void spawn_food(flecs::iter &it, FoodSpawner *food_spawner);

void spawn_food(flecs::iter &it, FoodSpawner *food_spawner) {

  auto occupied_tiles = it.world().get_mut<OccupiedTiles>();
  for (int i = 0; i < it.count(); i++) {
    int count = 0;
    it.entity(i).children([&](flecs::entity_t e) { count++; });

    if (count >= food_spawner[i].food_number) {

      continue;
    }
    int current_food_number = count;
    while (current_food_number < food_spawner[i].food_number) {
      current_food_number++;
      auto pos = random_food_pos(food_spawner[i], *occupied_tiles);
      FoodBundle food = FoodBundle{
          Food{}, TilePos{pos}, TileSize{8, 8},
          raylib::Color{raylib::Color::Green()},
          raylib::Rectangle{raylib::Rectangle(pos.x * 8, pos.y * 8, 8, 8)}};
      auto entity = food.spawn(it.world());
      occupied_tiles->set_occupied(pos.x, pos.y);
      it.world().entity(entity).child_of(it.entity(i));
    }
  }
}
void food_to_map(flecs::iter &it, FoodSpawner *food_spawner,
                 TileMapStorage *storage) {
  for (int i = 0; i < it.count(); i++) {
    storage[i].clear();
    it.entity(i).children([&](flecs::entity_t e) {
      auto pos = it.world().entity(e).get<TilePos>();
      storage->set_tile(pos->x, pos->y, e);
    });
  }
}
auto food_to_map_system(flecs::world &world) -> flecs::system {
  auto sig = IntoSystemBuilder(food_to_map);
  return sig.build(world);
}
auto spawn_food_system(flecs::world &world) -> flecs::system {
  auto sig = IntoSystemBuilder(spawn_food);
  return sig.build(world);
}
inline TilePos random_food_pos(const FoodSpawner &food_spawner,
                               const OccupiedTiles &occupied_tiles) {

  std::random_device rd;  // 随机数种子
  std::mt19937 gen(rd()); // 随机数生成器
  std::uniform_int_distribution<> dis_x(0, food_spawner.tile_map.width - 1); // 均匀分布
  std::uniform_int_distribution<> dis_y(0, food_spawner.tile_map.height - 1);

  TilePos food_pos;

  bool valid_pos = false; // 是否是有效的位置

  while (!valid_pos) {
    food_pos.x = dis_x(gen); // 生成随机位置
    food_pos.y = dis_y(gen);

    // 检查食物是否与蛇身体重叠
    valid_pos = !occupied_tiles.is_occupied(food_pos.x, food_pos.y);
  }
  return food_pos;
}
