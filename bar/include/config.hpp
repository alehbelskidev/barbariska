#pragma once

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
    float width, height, roundness;
    core::vec2 padding;
};

struct BaseWidget {
    WidgetType t;
    float padding_x;
    bool hoverable;
};

struct WithFormatWidget : BaseWidget {
    std::string format;
};

struct WifiWidget : WithFormatWidget {
    std::array<std::string, 6> levels;
};

struct WorkspacesWidget : BaseWidget {
    float gap;
    std::array<std::string, 9> icons, active_icons;
};

struct WindowWidget : WithFormatWidget {};

struct ClockWidget : WithFormatWidget {};

struct UNKNOWN_WIDGET : BaseWidget {};

using WidgetVariant = std::variant<WifiWidget, WorksapcesWidget, WindowWidget,
                                   ClockWidget, UNKNOWN_WIDGET>;

class Config {
private:
    core::Font font;
    Theme theme;
    Root root;
    std::vector<WidgetVariant> left, center, right;
    float left_gaps, center_gaps, right_gaps;

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
    const float get_left_gaps() const
    {
        return left_gaps;
    }
    const float get_center_gaps() const
    {
        return center_gaps;
    }
    const float get_right_gaps() const
    {
        return right_gaps;
    }

    void _DEBUG_print() const;
};
