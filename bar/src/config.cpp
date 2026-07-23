#include "config.hpp"

#include <stdio.h>

#include <iostream>
#include <string_view>

using namespace std::string_view_literals;

void Config::parse_widget(std::vector<WidgetVariant> &target, std::string key)
{
    auto section = t[key];

    if (key == "workspaces") {
        std::array<std::string, 9> icons;
        if (auto *arr = section["icons"].as_array()) {
            size_t i = 0;
            for (auto &el : *arr) {
                if (i >= 9) break;
                icons[i++] =
                    el.value<std::string>().value_or(std::to_string(i));
            }
        }

        std::array<std::string, 9> active_icons;
        if (auto *arr = section["active_icons"].as_array()) {
            size_t i = 0;
            for (auto &el : *arr) {
                if (i >= 9) break;
                active_icons[i++] =
                    el.value<std::string>().value_or(std::to_string(i));
            }
        }

        target.emplace_back(WorkspacesWidget{
            WidgetType::WORKSPACES, section["padding_x"].value_or(0.0f),
            section["hover"].value_or(false), section["gap"].value_or(4.0f),
            icons, active_icons});
    } else if (key == "window") {
        target.emplace_back(WindowWidget{WidgetType::WINDOW,
                                         section["padding_x"].value_or(0.0f),
                                         section["hover"].value_or(false),
                                         section["format"].value_or("%title")});
    } else if (key == "clock") {
        target.emplace_back(
            ClockWidget{WidgetType::CLOCK, section["padding_x"].value_or(0.0f),
                        section["hover"].value_or(false),
                        section["format"].value_or("%H:%M:%S, %d.%m.%Y")});
    } else if (key == "wifi") {
        std::array<std::string, 6> levels;
        if (auto *arr = section["levels"].as_array()) {
            size_t i = 0;
            for (auto &el : *arr) {
                if (i >= 6) break;
                levels[i++] = el.value<std::string>().value_or("");
            }
        }

        target.emplace_back(
            WifiWidget{WidgetType::WIFI, section["padding_x"].value_or(0.0f),
                       section["hover"].value_or(false),
                       section["format"].value_or("%signal%ssid"), levels});
    } else {
        target.emplace_back(UNKNOWN_WIDGET{WidgetType::UNKNOWN,
                                           section["padding_x"].value_or(0.0f),
                                           section["hover"].value_or(true)});
    }
}

void Config::parse_modules(std::vector<WidgetVariant> &target, std::string tkey)
{
    if (auto *arr = t["modules"][tkey].as_array()) {
        for (auto &el : *arr) {
            auto key = el.value<std::string>().value_or("");
            parse_widget(target, key);
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
        root.roundness = t["root"]["roundness"].value_or(0.0);
        root.padding.x = t["root"]["padding"]["x"].value_or(16.0);
        root.padding.y = t["root"]["padding"]["y"].value_or(0.0);
        root.gaps = t["root"]["gaps"].value_or(4.0);

        font.family = t["font"]["family"].value_or(""sv);
        font.size = t["font"]["size"].value_or(14);

        theme.bg = core::rgba(t["theme"]["bg"].value_or("#000000"sv));
        theme.fg = core::rgba(t["theme"]["fg"].value_or("#ffffff"sv));
        theme.accent = core::rgba(t["theme"]["accent"].value_or("#000fff"sv));

        parse_modules(left, "left");
        parse_modules(center, "center");
        parse_modules(right, "right");
    } catch (const toml::parse_error &err) {
        std::cerr << err << "\n";
    }
}
