#include "bar-layout.hpp"

#include <regex>

#include "bar-renderer.hpp"

/// Container
Container::Container(const core::NodeData &node_data) : core::Node(node_data)
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
        data.width += child_data.width + child_data.padding.x;
        if (child_data.height > data.height) data.height = child_data.height;
    }
}
void Container::render(void *renderer_ptr, const core::Node &parent)
{
    if (children.empty()) return;
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);

    data.width = 0;
    data.height = 0;
    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto &child = *it;
        child->measure(renderer_ptr, *this);
        auto &child_data = child->get_data();
        data.width += child_data.width + child_data.padding.x;
        if (child_data.height > data.height) data.height = child_data.height;
    }

    r.theme_draw_rect(core::Rect{
        .x = data.x,
        .y = data.y,
        .width = data.width,
        .height = data.height,
    });

    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto &child = *it;
        child->render(renderer_ptr, *this);
    }
}
void Container::render(void *renderer_ptr)
{
    if (children.empty()) return;
    auto &r = *static_cast<BarRenderer *>(renderer_ptr);

    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto &child = *it;
        child->measure(renderer_ptr, *this);
    }

    r.theme_draw_rect(core::Rect{
        .x = data.x,
        .y = data.y,
        .width = data.width,
        .height = data.height,
    });

    for (auto it = children.rbegin(); it != children.rend(); ++it) {
        auto &child = *it;
        child->render(renderer_ptr, *this);
    }
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
    auto container =
        core::Rect{.x = parent_data.x + data.x,
                   .y = parent_data.y + data.y,
                   .width = text_ext.width + parent_data.padding.x,
                   .height = text_ext.height + parent_data.padding.y};

    r.theme_draw_rect(container);
    std::string text_val =
        data.text.has_value() ? data.text.value() : "WINDOW WIDGET (notext)";
    r.theme_draw_text(text_val,
                      core::Rect{.x = container.x + parent_data.padding.x,
                                 .y = container.y + parent_data.padding.y,
                                 .width = text_ext.width,
                                 .height = text_ext.height});
};
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
    auto container =
        core::Rect{.x = parent_data.x + data.x,
                   .y = parent_data.y + data.y,
                   .width = text_ext.width + parent_data.padding.x,
                   .height = text_ext.height + parent_data.padding.y};

    std::string text_val =
        data.text.has_value() ? data.text.value() : "UNKOWN WIDGET (notext)";
    text_ext = r.theme_measure_text(text_val);
    r.theme_draw_rect(container);
    r.theme_draw_text(text_val,
                      core::Rect{.x = container.x + parent_data.padding.x,
                                 .y = container.y + parent_data.padding.y,
                                 .width = text_ext.width,
                                 .height = text_ext.height});
}
/// End Unknown
