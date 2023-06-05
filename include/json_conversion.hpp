#pragma once
#include "Rectangle.hpp"
#include "raylib.h"
#include<nlohmann/json.hpp>
#include "Color.hpp"
using json = nlohmann::json;

inline void to_json(json& j, const raylib::Rectangle& rect) {
    j = json{{"x", rect.GetX()}, {"y", rect.GetY()}, {"width", rect.width}, {"height", rect.height}};
}

inline void to_json(json& j, const raylib::Color& color) {
    j = json{{"r", color.r}, {"g", color.g}, {"b", color.b}, {"a", color.a}};
}

inline void from_json(const json& j, raylib::Rectangle& rect) {
    j.at("x").get_to(rect.x);
    j.at("y").get_to(rect.y);
    j.at("width").get_to(rect.width);
    j.at("height").get_to(rect.height);
}

inline void from_json(const json& j, raylib::Color& color) {
    j.at("r").get_to(color.r);
    j.at("g").get_to(color.g);
    j.at("b").get_to(color.b);
    j.at("a").get_to(color.a);
}
