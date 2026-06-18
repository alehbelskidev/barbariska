#pragma once

#include <functional>
#include <vector>

#include "math.hpp"
#include "shapes.hpp"

class Component {
private:
    Shapes::Rect bound;
    core::V2 padding;
    std::vector<Component> children;
    std::function<void()> on_render;

public:
    Component(std::vector<Component> children, std::function<void()> on_render)
        : children(children), on_render(on_render)
    {
    }
    Shapes::Rect get_bounds() const
    {
        return bounds;
    }
};
