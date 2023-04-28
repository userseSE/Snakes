#include "flecs.h"
#include <vector>

struct Food
{
    int type; // 表示食物类型的整数，有不同的分值
};

// 食物生成系统, iter: Flecs ECS 框架中的迭代器，用于访问当前系统所匹配的实体集合
// iter.term<T>(int index): 返回一个包含组件类型 T 的容器，其中 index 是组件在查询中的位置。
// iter.entity(int index): 返回当前迭代器中指定索引处的实体。
void food_generation_system(flecs::iter &iter)
{
    auto snake_head = iter.term<Position>(1);
    auto food = iter.term<Food>(2);
    auto food_position = iter.term<Position>(3);

    // i为蛇头实体的索引，j为食物实体的索引
    for (auto i : iter)
    {
        for (auto j : iter)
        {
            // 检查蛇头和食物是否有碰撞
            if (snake_head[i].x == food_position[j].x && snake_head[i].y == food_position[j].y)
            {
                // 增加蛇的长度（将蛇尾的旧位置推入蛇身体实体的向量中，实现身体长度增加1）
                Position old_tail_position = snake_body_entities.back().get<Position>(); // 获取蛇尾的旧位置
                auto new_snake_body = iter.world().entity()
                    .set<Snake_body>({1, snake_color[i]})
                    .set<Position>(old_tail_position); // 使用蛇尾的旧位置作为新的蛇身体部分的位置
                // 将新的蛇身体实体添加到相应的蛇身体实体列表中
                snake_body_entities.push_back(new_snake_body);

                // 增加玩家的分数
                iter.entity(i).get_mut<Score>()->value += food[j].type;

                // 如果有碰撞，将食物实体删除
                iter.entity(j).destruct();

                // 生成新的食物实体，iter.world()返回一个指向当前迭代器所在的ECS世界的引用。
                // 将这个世界作为参数传递给flecs::entity构造函数，在这个世界中创建一个新的实体。
                flecs::entity(iter.world())
                    .set<Food>({1})
                    .set<Position>({rand() % (screenWidth / gridSize), rand() % (screenHeight / gridSize)});
            }
        }
    }
}