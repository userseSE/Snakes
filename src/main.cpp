#include <vector>
#include "flecs.h"
#include "raylib-cpp.hpp"


int main(int argc, char* argv[])
{

    
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;
    raylib::Color textColor = raylib::Color::LightGray();
    raylib::Window window(screenWidth, screenHeight, "贪吃蛇");

    //init world
    flecs::world ecs;

    //register components
    //实体在游戏地图上的坐标。位置组件可用于蛇、食物和地图边界等实体
    ecs.component<Position>();
    //实体的移动速度和方向。在贪吃蛇游戏中，速度组件主要用于蛇头实体，用于控制蛇的移动
    ecs.component<Velocity>();
    //标识实体为蛇的一部分（蛇头或蛇身）。蛇组件可以包含蛇的长度、颜色等属性
    ecs.component<Snake>();
    //标识实体为食物。食物组件可以包含食物的类型、分值等属性
    ecs.component<Food>();
    //标识实体为玩家。玩家组件可以包含玩家的id、输入状态等属性
    ecs.component<Player>();
    //表示玩家的得分。分数组件可以包含玩家当前的分数值
    ecs.component<Score>();
    //表示实体为游戏地图。地图组件可以包含地图的大小、边界等属性
    ecs.component<map>();

    //register systems
    //移动系统
    ecs.system<Position, Velocity>().each(snake_movement_system);
    //碰撞检测系统
    ecs.system<Position, Snake>().each(collision_system);
    //食物生成系统
    ecs.system<Food>().each(food_generation_system);
    
    //init entities
    //snake1:
    auto snake_head1=ecs.entity()
        .set<Snake_head>({RED,RIGHT})
        .set<Position>({0,0})
        .set<Velocity>({1,0});
    //创建并存储玩家1的蛇身体实体
    std::vector<flecs::entity> snake_body1_entities;
    auto snake_body1=ecs.entity()
        .set<Snake_body>({3,RED})
        .set<Position>({1,0})
        .set<Velocity>({1,0});
    //将蛇身体实体添加到实体vector中
    snake_body1_entities.push_back(snake_body1);
    auto snake_tail1=ecs.entity()
        .set<Snake_tail>({RED})
        .set<Position>({4,0})
        .set<Velocity>({1,0});

    //snake2:
    auto snake_head2=ecs.entity()
        .set<Snake_head>({BLUE,LEFT})
        .set<Position>({10,10})
        .set<Velocity>({1,0});
    //创建并存储玩家2的蛇身体实体
    std::vector<flecs::entity> snake_body2_entities;
    auto snake_body2=ecs.entity()
        .set<Snake_body>({3,BLUE})
        .set<Position>({11,10})
        .set<Velocity>({1,0});
    //将蛇身体实体添加到实体vector中
    snake_body2_entities.push_back(snake_body2);
    auto snake_tail2=ecs.entity()
        .set<Snake_tail>({BLUE})
        .set<Position>({14,10})
        .set<Velocity>({1,0});
    auto food=ecs.entity().set<Position>({5,5}).set<Food>({1});
    auto map=ecs.entity().set<Map>({20,20});

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose()) {   // Detect window close button or ESC key
        // Update (ECS世界以每秒60次的速率更新)
        ecs.progress(1.0/60)
        //----------------------------------------------------------------------------------
        // Update your variables here
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            textColor.DrawText("双人贪吃蛇游戏", 190, 200, 20);
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    //clean up (清理资源)
    ecs.deinit();
    return 0;
}
