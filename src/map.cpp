#include "map.hpp"
    




void MapPlugin::build(flecs::world &world) {
    world.system<>()
      .kind(flecs::OnSet)
      .each([](flecs::entity e, const TileMap &tile_map, TileMapStorage &storage, const TileSize &tile_size) {
        int width = tile_map.width;
        int height = tile_map.height;
        int tile_size_x = tile_size.x;
        int tile_size_y = tile_size.y;

        e.set<TileMap>({width, height});
        e.set<TileMapStorage>(TileMapStorage{});
        e.set<TileSize>({tile_size_x, tile_size_y});
        e.set<OccupiedTiles>(OccupiedTiles{});
      });

}
