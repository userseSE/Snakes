#include "food.hpp"
#include "Color.hpp"
#include "Rectangle.hpp"
#include "bundle.hpp"
#include "map.hpp"
#include "snake.hpp"
#include <flecs.h>
#include <random>
#include "utils.hpp"
#include <algorithm>
#include <chrono>   //时间库



//使用别名（alias）定义一个叫FoodBundle的新模板类，
//四个模板参数：Food、TilePos、TileSize 和 raylib::Color
using FoodBundle=basic::Bundle<Food, TilePos, TileSize, raylib::Color>;

TilePos random_food_pos(flecs::world &ecs, TileMap &tile_map, Snake &snake){
    std::random_device rd;  //随机数种子
    std::mt19937 gen(rd()); //随机数生成器
    std::uniform_int_distribution<> dis_x(0, tile_map.width - 1); //均匀分布
    std::uniform_int_distribution<> dis_y(0, tile_map.height - 1);

    TilePos food_pos;

    bool valid_pos = false; //是否是有效的位置
    while (!valid_pos) {
        food_pos.x = dis_x(gen); //生成随机位置
        food_pos.y = dis_y(gen);

        //检查食物是否与蛇身体重叠
        valid_pos = true;
        for (auto &snake_part : snake.body) {   //遍历蛇身体
            const TilePos *snake_part_pos=ecs.get<TilePos>(snake_part); //获取蛇身体的位置
            if(snake_part_pos->x==food_pos.x && snake_part_pos->y==food_pos.y){
                valid_pos=false;
                break;
            }
        }
    }
    return food_pos;
}

void spawn_food(flecs::world &ecs, TileMap &tile_map, Snake &snake){
    TilePos food_pos = random_food_pos(ecs,tile_map, snake);

    raylib::Color food_color = raylib::Color::Green();
    FoodBundle food_bundle = {
        Food{food_pos}, 
        std::move(food_pos),  //强制转换成右值引用
        TileSize{8, 8}, 
        std::move(food_color)
        };

    food_bundle.spawn(ecs);
}

void update_render_food(flecs::iter &it, Food *foods, TilePos *pos, raylib::Color *color, 
                        TileSize *size, raylib::Rectangle *rects){

    for (int i = 0; i < it.count(); i++) {  //遍历所有食物实体
        rects[i].SetPosition(size[i].x*pos[i].x, size[i].y*pos[i].y); //更新矩形的位置
    }

}

void init_food_graphic(flecs::iter &it, Food *foods, TilePos *pos, raylib::Color *color, TileSize *size){
    for (int i = 0; i < it.count(); i++) {  //遍历所有食物实体
        raylib::Rectangle rect = {
            static_cast<float>(pos[i].x * size[i].x), 
            static_cast<float>(pos[i].y * size[i].y), 
            static_cast<float>(size[i].x), 
            static_cast<float>(size[i].y)
            };  //创建一个矩形表示食物

        it.entity(i).set(rect); //为食物实体设置矩形组件
    }
}

