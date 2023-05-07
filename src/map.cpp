#include "map.hpp"

inline void clear_occupied(flecs::iter &it) {
  auto occupied_tiles = it.world().get_ref<OccupiedTiles>();
  occupied_tiles->clear();
}
inline void print_occupied(flecs::iter &it) {

  auto occupied_tiles = it.world().get_ref<OccupiedTiles>();
  printf("%d\n", occupied_tiles->size());
}
inline void detect_occupied(flecs::iter &it, TilePos *pos) {

  auto occupied_tiles = it.world().get_ref<OccupiedTiles>();

  for (int i = 0; i < it.count(); i++) {
    occupied_tiles->set_occupied(pos[i].x, pos[i].y);
  }
}
inline auto print_occupied_system(flecs::world &world) -> flecs::system {

  IntoSystemBuilder system(print_occupied);
  return system.build(world);
}
inline auto clear_occupied_system(flecs::world &world) -> flecs::system {

  IntoSystemBuilder system(clear_occupied);
  return system.build(world);
}
inline auto detect_occupied_system(flecs::world &world) -> flecs::system {

  IntoSystemBuilder system(detect_occupied);
  return system.build(world);
}

void OccupiedTilePlugin::build(flecs::world &ecs) {
  auto clear_system = clear_occupied_system(ecs);
  auto detect_system = detect_occupied_system(ecs);
  //auto print_system = print_occupied_system(ecs);
  detect_system.depends_on(clear_system);
  //print_system.depends_on(detect_system);
}
