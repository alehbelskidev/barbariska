#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "colors.hpp"
#include "math.hpp"

namespace core {

/// TODO: remove, appeared just to not to use cairo text ext
struct Size {
    float width, height;
};

struct NodeData {
    float width = 0, height = 0, x = 0, y = 0;
    core::V2 padding{};
    std::optional<std::string> text;
};

class Node {
protected:
    NodeData data;
    std::vector<std::unique_ptr<Node>> children;

public:
    Node(const NodeData &node_data) : data(node_data)
    {
    }
    virtual ~Node() = default;

    virtual void update(const NodeData &node_data)
    {
        data = node_data;
    }
    virtual void measure(void *renderer_ptr, const Node &parent) = 0;
    /// FYI: passing parent to properly calc padding and offsets
    /// because, actual block offset is parent.padding.x[OR]y + this.x[OR]y
    virtual void render(void *renderer_ptr, const Node &parent) = 0;
    virtual void render(void *renderer_ptr) = 0;

    const NodeData &get_data() const
    {
        return data;
    }
    const std::vector<std::unique_ptr<Node>> &get_children() const
    {
        return children;
    }
    void add_child(std::unique_ptr<Node> child)
    {
        children.push_back(std::move(child));
    }
};
};  // namespace core
