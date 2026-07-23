#pragma once

#include <array>
#include <string>
#include <variant>
#include <vector>

#include "core.hpp"

#define BAR_CONFIG_PATH "/.config/barbariska/bar.toml"

struct Theme {
    core::rgba bg, fg, accent;
};
// const char *homedir = getenv("HOME");
// snprintf(path, size, "%s%s", homedir, configdir);

enum class WidgetType {
    WORKSPACES,
    WINDOW,
    CLOCK,
    WIFI,
    VOLUME,
    DISKS,
    PROC,
    RAM,
    SYSTEM,
    UNKNOWN,
};

struct Root {
    float width, height, roundness, gaps;
    core::vec2 padding;
};
struct BaseWidget {
    WidgetType t;
    float padding_x;
    bool hoverable;
};

struct WithFormatWidget : public BaseWidget {
    std::string format;
};

struct WifiWidget : public WithFormatWidget {
    std::array<std::string, 6> levels;
};

struct WorkspacesWidget : public BaseWidget {
    float gap;
    std::array<std::string, 9> icons;
    std::array<std::string, 9> active_icons;
};

struct WindowWidget : public WithFormatWidget {};

struct ClockWidget : public WithFormatWidget {};

struct UNKNOWN_WIDGET : public BaseWidget {};

using WidgetVariant = std::variant<WifiWidget, WorkspacesWidget, WindowWidget,
                                   ClockWidget, UNKNOWN_WIDGET>;

class Config {
private:
    core::Font font;
    Theme theme;
    Root root;
    std::vector<WidgetVariant> left, center, right;

    toml::parse_result t;

    void parse_widget(std::vector<WidgetVariant> &target, std::string key);
    void parse_modules(std::vector<WidgetVariant> &target, std::string tkey);

public:
    Config();
    ~Config() = default;

    const core::Font &get_font() const
    {
        return font;
    }
    const Theme &get_theme() const
    {
        return theme;
    }
    const void set_root_width(float width)
    {
        root.width = width;
    }
    const Root &get_root() const
    {
        return root;
    }
    const std::vector<WidgetVariant> &get_left() const
    {
        return left;
    }
    const std::vector<WidgetVariant> &get_center() const
    {
        return center;
    }
    const std::vector<WidgetVariant> &get_right() const
    {
        return right;
    }
};
