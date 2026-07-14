#pragma once

#include <string>

#include "config.hpp"
#include "core.hpp"

/// Definitions

enum class Anchor { LEFT, CENTER, RIGHT, FULL, AUTO };

class Container : public core::Node {
public:
    std::string t_str = "Container";
    Anchor anchor;
    Container(const core::NodeData &node_data, Anchor anchor);

    void measure(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr) override;
};

class WindowWidget : public core::Node {
public:
    WidgetType t = WidgetType::WINDOW;
    std::string t_str = "Window";
    WindowWidget(const core::NodeData &node_data, std::string format);

    WidgetType type = WidgetType::WINDOW;
    std::string format;
    core::Size text_ext;
    std::string last_measured_text;

    void set_text(void *renderer_ptr, std::string input);
    void measure(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr) override {};
};

class UnknownWidget : public core::Node {
public:
    WidgetType t = WidgetType::UNKNOWN;
    std::string t_str = "Unknown";
    UnknownWidget(const core::NodeData &node_data);

    WidgetType type = WidgetType::UNKNOWN;
    core::Size text_ext;
    std::string last_measured_text;

    void measure(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr, const core::Node &parent) override;
    void render(void *renderer_ptr) override {};
};

/// Helpers
inline std::unique_ptr<core::Node> container(
    core::NodeData node_data,
    std::vector<std::unique_ptr<core::Node>> children = {},
    Anchor anchor = Anchor::AUTO)
{
    auto node = std::make_unique<Container>(node_data, anchor);
    for (auto &child : children) {
        node->add_child(std::move(child));
    }
    return node;
}

inline std::unique_ptr<core::Node> window_widget(core::NodeData node_data,
                                                 std::string format)
{
    return std::make_unique<WindowWidget>(node_data, format);
}

inline std::unique_ptr<core::Node> unknown_widget(core::NodeData node_data)
{
    return std::make_unique<UnknownWidget>(node_data);
}

inline void parse_config_group(const std::vector<Widget> &config_group,
                               std::unique_ptr<core::Node> &container)
{
    for (auto &config_widget : config_group) {
        switch (config_widget.t) {
            case WidgetType::WINDOW:
                container->add_child(window_widget(
                    core::NodeData{
                        .x = (float)config_widget.gap,
                        .padding = config_widget.padding,
                    },
                    config_widget.format.value()));
                break;
            case WidgetType::WORKSPACES:
            case WidgetType::CLOCK:
            case WidgetType::WIFI:
            case WidgetType::VOLUME:
            case WidgetType::DISKS:
            case WidgetType::PROC:
            case WidgetType::RAM:
            case WidgetType::SYSTEM:
            case WidgetType::UNKNOWN:
                container->add_child(unknown_widget(core::NodeData{
                    .x = (float)config_widget.gap,
                    .padding = config_widget.padding,
                }));
                break;
        }
    }
}

// inline std::unique_ptr<core::Node> find_widget_by_type_in(
//     std::unique_ptr<core::Node> root, WidgetType t)
//{
// }

/// Builder
