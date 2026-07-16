#pragma once

#include <optional>
#include <string>
#include <vector>

#include "core.hpp"
#include "font.hpp"
#include "math.hpp"
#include "toml.hpp"

#define BAR_CONFIG_PATH "/.config/barbariska/bar.toml"

struct Theme {
    core::RGBA bg, fg, accent;
};
// const char *homedir = getenv("HOME");
// snprintf(path, size, "%s%s", homedir, configdir);

enum class WidgetType {
    ROOT,
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

struct Widget {
    WidgetType t;
    float gap, roundness, height;
    bool hoverable;
    std::optional<std::string> format;
    std::optional<std::array<std::string, 6>> levels;
};

class Config {
private:
    Widget root;

    core::Font font;
    Theme theme;
    std::vector<Widget> left, center, right;

    toml::parse_result t;

    void parse_font();
    void parse_theme();
    Widget parse_widget(std::string key);
    void parse_modules(std::vector<Widget> &target, std::string tkey);

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
    const Widget &get_root() const
    {
        return root;
    }
    const std::vector<Widget> &get_left() const
    {
        return left;
    }
    const std::vector<Widget> &get_center() const
    {
        return center;
    }
    const std::vector<Widget> &get_right() const
    {
        return right;
    }

    void _DEBUG_print() const;
};
