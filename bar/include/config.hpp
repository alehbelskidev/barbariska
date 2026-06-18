#pragma once

#include <optional>
#include <string>
#include <vector>

#include "colors.hpp"
#include "math.hpp"
#include "toml.hpp"

#define BAR_CONFIG_PATH "/.config/barbariska/bar.toml"

struct Font {
    std::string family;
    int size;
};

struct Theme {
    Colors::RGBA bg, fg, accent;
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

struct Widget {
    WidgetType t;
    double gap, roundness;
    bool hoverable;
    Math::Vec2 padding;
    std::optional<std::string> format;
    std::optional<std::array<std::string, 6>> levels;
};

class Config {
private:
    Font font;
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

    Font get_font() const
    {
        return font;
    }
    Theme get_theme() const
    {
        return theme;
    }

    void _DEBUG_print() const;
};
