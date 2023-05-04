#include "input.hpp"
    

    auto handle_input() -> std::optional<Direction>
    {
         std::optional<Direction> dir = std::nullopt;
        if (IsKeyDown(KEY_RIGHT))
        {
            if (dir != Direction::LEFT)
                dir = Direction::RIGHT;
        }

        else if (IsKeyDown(KEY_LEFT))
        {
            if (dir != Direction::RIGHT)
                dir = Direction::LEFT;
        }

        else if (IsKeyDown(KEY_UP))
        {
            if (dir != Direction::DOWN)
                dir = Direction::UP;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            if (dir != Direction::UP)
                dir = Direction::DOWN;
        }
        return dir;
    }