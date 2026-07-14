#include "bar-layout.hpp"

#include <iostream>
#include <regex>

#include "bar-renderer.hpp"

void debug_node(const core::Node &n, std::string t_str)
{
    auto d = n.get_data();
    std::cout << "DEBUG::[" << t_str << "]" << std::endl;
    std::cout << "width=" << d.width << ". height=" << d.height << "."
              << std::endl;
    std::cout << "x=" << d.x << ". y=" << d.y << "." << std::endl;
    std::cout << "padding.x=" << d.padding.x << ". padding.y=" << d.padding.y
              << "." << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
}

/// Container
Container::Container(const core::NodeData &node_data, Anchor anchor)
    : core::Node(node_data), anchor(anchor)
{
}

void Container::measure(void *renderer_ptr, const core::Node &parent)
{
    data.width = 0;
    data.height = 0;
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto &child = *it;
        child->measure(renderer_ptr, *this);
        auto &child_data = child->get_data();
        data.width += child_data.x + child_data.width;
        if (child_data.height > data.height) data.height = child_data.height;
    }
}

void Container::render(void *renderer_ptr, const core::Node &parent)
{
    if (children.empty()) return;
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto parent_data = parent.get_data();

    for (auto &child : children) {
        child->measure(renderer_ptr, *this);
    }

    data.width = 0;
    data.height = 0;
    for (auto &child : children) {
        auto &child_data = child->get_data();
        data.width += child_data.x + child_data.width;
        if (child_data.height > data.height) data.height = child_data.height;
    }

    float offset_x = 0;
    switch (anchor) {
        case Anchor::LEFT:
            offset_x = parent_data.x + parent_data.padding.x;
            break;
        case Anchor::CENTER:
            offset_x = parent_data.x + (parent_data.width - data.width) / 2;
            break;
        case Anchor::RIGHT:
            offset_x = parent_data.x + parent_data.width -
                       parent_data.padding.x - data.width;
            break;
        case Anchor::AUTO:
            offset_x = parent_data.x + data.x;
            break;
        default:
            offset_x = parent_data.x + data.x;
            break;
    }
    data.x = offset_x;

    r.theme_draw_rect(core::Rect{
        .x = data.x,
        .y = parent_data.padding.y,
        .width = data.width,
        .height = data.height,
    });

    for (auto &child : children) {
        child->render(renderer_ptr, *this);
    }
    debug_node(*this, t_str);
}

void Container::render(void *renderer_ptr)
{
    if (children.empty()) return;
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);

    r.theme_draw_rect(core::Rect{
        .x = 0,
        .y = 0,
        .width = data.width,
        .height = data.height,
    });

    for (auto &child : children) {
        child->render(renderer_ptr, *this);
    }

    debug_node(*this, t_str);
}
/// END Container

/// Window
WindowWidget::WindowWidget(const core::NodeData &node_data, std::string format)
    : core::Node(node_data), format(format)
{
    /// FYI: should probably set text here, but won't
}

void WindowWidget::set_text(void *renderer_ptr, std::string input)
{
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto pos = input.find('\x1F');
    auto class_name = input.substr(0, pos);
    auto title = input.substr(pos + 1);

    std::string result(format);
    result = std::regex_replace(result, std::regex("\\%class"), class_name);
    result = std::regex_replace(result, std::regex("\\%title"), title);

    data.text = result;
    text_ext = r.theme_measure_text(result);
}

void WindowWidget::measure(void *renderer_ptr, const core::Node &parent)
{
    if (!data.text.has_value()) {
        data.width = 0;
        data.height = 0;
        return;
    }
    if (last_measured_text == data.text.value()) {
        return;
    }
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto parent_data = parent.get_data();
    text_ext = r.theme_measure_text(data.text.value());

    data.width = text_ext.width + parent_data.padding.x * 2;
    data.height = text_ext.height + parent_data.padding.y * 2;
    last_measured_text = data.text.value();
}

void WindowWidget::render(void *renderer_ptr, const core::Node &parent)
{
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto parent_data = parent.get_data();

    float abs_x = parent_data.x + data.x;
    float abs_y = parent_data.y;

    core::Rect container{.x = abs_x,
                         .y = abs_y,
                         .width = text_ext.width + parent_data.padding.x * 2,
                         .height = text_ext.height + parent_data.padding.y * 2};

    r.theme_draw_rect(container);

    std::string text_val =
        data.text.has_value() ? data.text.value() : "WINDOW WIDGET (notext)";
    r.theme_draw_text(text_val,
                      core::Rect{.x = container.x + parent_data.padding.x,
                                 .y = container.y + parent_data.padding.y,
                                 .width = text_ext.width,
                                 .height = text_ext.height});
    debug_node(*this, t_str);
}
/// END Window

/// Unknown
UnknownWidget::UnknownWidget(const core::NodeData &node_data)
    : core::Node(node_data)
{
    data.text = "UNKNOWN";
}

void UnknownWidget::measure(void *renderer_ptr, const core::Node &parent)
{
    if (!data.text.has_value()) {
        data.width = 0;
        data.height = 0;
        return;
    }
    if (last_measured_text == data.text.value()) {
        return;
    }
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto parent_data = parent.get_data();
    text_ext = r.theme_measure_text(data.text.value());

    data.width = text_ext.width + parent_data.padding.x * 2;
    data.height = text_ext.height + parent_data.padding.y * 2;
    last_measured_text = data.text.value();
}
void UnknownWidget::render(void *renderer_ptr, const core::Node &parent)
{
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);
    auto parent_data = parent.get_data();

    float abs_x = parent_data.x + data.x;
    float abs_y = parent_data.y + data.y;

    core::Rect container{.x = abs_x,
                         .y = abs_y,
                         .width = text_ext.width + parent_data.padding.x * 2,
                         .height = text_ext.height + parent_data.padding.y * 2};

    r.theme_draw_rect(container);

    core::Rect text_rect{.x = container.x + parent_data.padding.x,
                         .y = container.y + parent_data.padding.y +
                              (container.height / 2 - text_ext.height / 2),
                         .width = text_ext.width,
                         .height = text_ext.height};

    r.theme_draw_text(data.text.value(), text_rect);
    debug_node(*this, t_str);
}
/// End Unknown
