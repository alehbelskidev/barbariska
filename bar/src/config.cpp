#include "config.hpp"

#include <stdio.h>

#include <iostream>
#include <string_view>

using namespace std::string_view_literals;

WidgetType get_wtype_by_str(std::string str)
{
    if (str == "workspaces")
        return WidgetType::WORKSPACES;
    else if (str == "window")
        return WidgetType::WINDOW;
    else if (str == "clock")
        return WidgetType::CLOCK;
    else if (str == "wifi")
        return WidgetType::WIFI;
    else if (str == "volume")
        return WidgetType::VOLUME;
    else if (str == "disks")
        return WidgetType::DISKS;
    else if (str == "proc")
        return WidgetType::PROC;
    else if (str == "ram")
        return WidgetType::RAM;
    else if (str == "system")
        return WidgetType::SYSTEM;
    else
        return WidgetType::UNKNOWN;
}

/*

[root]
height = 30
padding = {x = 12, y = 4}

[theme]
bg = "#1e1e2e"
fg = "#cdd6f4"
accent = "#89b4fa"

[font]
family = "Iosevka Nerd Font"
size = 20

[modules]
left = [ "workspaces", "window"  ]
center = [ "clock", ]
right = [ "wifi", "volume", "disks", "proc",  "ram", "system"]

[wifi]
levels = ["󰤭", "󰤯", "󰤟", "󰤢", "󰤥", "󰤨"]
gap = 4
padding = {x = 4, y = 4}

[workspaces]
gap = 4
padding = {x = 4, y = 4}
roundness = 0.2

[window]
format = "%class - %title"
gap = 8
padding = {x = 4, y = 4}
roundness = 0.2
hover = false

[clock]
format = "󰃰 %H:%M:%S, %d.%m.%Y"
gap = 8
padding = {x = 4, y = 4}
roundness = 0.2
hover = false
*/

void Config::parse_font()
{
    font.family = t["font"]["family"].value_or(""sv);
    font.size = t["font"]["size"].value_or(14);
}

void Config::parse_theme()
{
    theme.bg = core::RGBA(t["theme"]["bg"].value_or("#000000"sv));
    theme.fg = core::RGBA(t["theme"]["fg"].value_or("#ffffff"sv));
    theme.accent = core::RGBA(t["theme"]["accent"].value_or("#000fff"sv));
}

Widget Config::parse_widget(std::string key)
{
    auto section = t[key];
    auto wt = get_wtype_by_str(key);
    Widget w;
    w.t = wt;

    if (wt == WidgetType::CLOCK || wt == WidgetType::WINDOW) {
        w.format = section["format"].value_or(""sv);
    }
    if (wt == WidgetType::WIFI) {
        if (auto *arr = section["levels"].as_array()) {
            std::array<std::string, 6> tmp;
            size_t i = 0;
            for (auto &el : *arr) {
                if (i >= 6) break;
                tmp[i++] = el.value<std::string>().value_or("");
            }
            w.levels = tmp;
        }
    }

    w.gap = section["gap"].value_or(0);
    w.roundness = section["roundness"].value_or(0.0);
    w.hoverable = section["hover"].value_or(false);

    return w;
}

void Config::parse_modules(std::vector<Widget> &target, std::string tkey)
{
    if (auto *arr = t["modules"][tkey].as_array()) {
        for (auto &el : *arr) {
            auto key = el.value<std::string>().value_or("");
            auto widget = parse_widget(key);
            target.push_back(widget);
        }
    }
}

Config::Config()
{
    char configpath[108];
    snprintf(configpath, 108, "%s%s", getenv("HOME"), BAR_CONFIG_PATH);

    try {
        t = toml::parse_file(configpath);

        root.height = t["root"]["height"].value_or(30.0);
        root.gap = t["root"]["gap"].value_or(12.0);

        parse_font();
        parse_theme();

        parse_modules(left, "left");
        parse_modules(center, "center");
        parse_modules(right, "right");
    } catch (const toml::parse_error &err) {
        std::cerr << err << "\n";
    }
}

void Config::_DEBUG_print() const
{
    std::cout << "=== Config ===\n";
    std::cout << "font: " << font.family << " " << font.size << "px\n";

    auto print_widgets = [](const std::string &name,
                            const std::vector<Widget> &widgets) {
        std::cout << "\n[" << name << "]\n";
        for (const auto &w : widgets) {
            if (w.format) std::cout << "    format=" << *w.format << "\n";
            if (w.levels) {
                std::cout << "    levels=[ ";
                for (const auto &l : *w.levels) std::cout << l << " ";
                std::cout << "]\n";
            }
        }
    };

    print_widgets("left", left);
    print_widgets("center", center);
    print_widgets("right", right);
}
