#include <vector>
#include "flecs.h"

struct Snake_head {
    enum Color {RED, BLUE} color;
    enum Direction { UP, DOWN, LEFT, RIGHT } direction;
};
struct Snake_body {
    int length;
    enum Color {RED, BLUE} color;
};
struct Snake_tail {
    enum Color {RED, BLUE} color;
};
struct Velocity {
    int dx;
    int dy;
};

void snake_movement_system(flecs::iter &iter){
    auto snake_head = iter.term<Snake_head>(1);
    auto snake_body = iter.term<Snake_body>(2);
    auto snake_tail = iter.term<Snake_tail>(3);
    
    //根据用户的输入或AI逻辑更新蛇头的移动方向
    update_snake_direction_based_on_input(snake_head[iter]);

    //更新蛇头的位置

    //更新蛇身体部分的位置

    //更新蛇尾部的位置

    //检查蛇头是否与地图边界或蛇身体的其他部分发生碰撞

    //蛇头是否与另一条蛇发生碰撞



}

//根据用户输入更新蛇头的移动方向
void update_snake_direction_based_on_input(Snake_head& snake_head) {
    if (IsKeyPressed(KEY_UP) && snake_head.direction != Snake_head::DOWN) {
        snake_head.direction = Snake_head::UP;
    } else if (IsKeyPressed(KEY_DOWN) && snake_head.direction != Snake_head::UP) {
        snake_head.direction = Snake_head::DOWN;
    } else if (IsKeyPressed(KEY_LEFT) && snake_head.direction != Snake_head::RIGHT) {
        snake_head.direction = Snake_head::LEFT;
    } else if (IsKeyPressed(KEY_RIGHT) && snake_head.direction != Snake_head::LEFT) {
        snake_head.direction = Snake_head::RIGHT;
    }
}
