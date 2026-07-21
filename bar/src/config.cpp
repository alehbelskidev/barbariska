#include "config.hpp"

#include <stdio.h>

#include <iostream>
#include <string_view>

using namespace std::string_view_literals;

void Config::parse_widget(std::vector<WidgetVariant> &target, std::string key)
{
    auto section = t[key];

    switch (key) {
        case "workspaces": {
            std::array<std::string, 9> icons;
            if (auto *arr = section["icons"].as_array()) {
                size_t i = 0;
                for (auto &el : *arr) {
                    if (i >= 9) break;
                    icons[i++] =
                        el.value<std::string>().value_or(std::string_view(i));
                }
            }

            std::array<std::string, 6> active_icons;
            if (auto *arr = section["active_icons"].as_array()) {
                size_t i = 0;
                for (auto &el : *arr) {
                    if (i >= 9) break;
                    active_icons[i++] =
                        el.value<std::string>().value_or(std::string_view(i));
                }
            }

            target.emplace_back(WorkspacesWidget{
                .t = WidgetType::WORKSPACES,
                .padding_x = section["padding_x"].value_or(0.0),
                .hoverable = section["hover"].value_or(false),
                .gap = section["gap"].value_or(4.0),
                .icons = icons,
                .active_icons = icons,
            });
            break;
        }
        case "window": {
            target.emplace_back(WindowWidget{
                .t = WidgetType::WINDOW,
                .padding_x = section["padding_x"].value_or(0.0),
                .hoverable = section["hover"].value_or(false),
                .format = section["format"].value_or("%title"sv),
            });
            break;
        }
        case "clock": {
            target.emplace_back(ClockWidget{
                .t = WidgetType::CLOCK,
                .padding_x = section["padding_x"].value_or(0.0),
                .hoverable = section["hover"].value_or(false),
                .format = section["format"].value_or("%H:%M:%S, %d.%m.%Y"sv),
            });
            break;
        }
        case "wifi": {
            std::array<std::string, 6> levels;
            if (auto *arr = section["levels"].as_array()) {
                size_t i = 0;
                for (auto &el : *arr) {
                    if (i >= 6) break;
                    levels[i++] = el.value<std::string>().value_or(""sv);
                }
            }

            target.emplace_back(WifiWidget{
                .t = WidgetType::WIFI,
                .padding_x = section["padding_x"].value_or(0.0),
                .hoverable = section["hover"].value_or(false),
                .levels = levels,
                .format = section["format"].value_or("%signal%ssid"sv),
            });
            break;
        }
            // case "volume":
            //     return WidgetType::VOLUME;
            // case "disks":
            //     return WidgetType::DISKS;
            // case "proc":
            //     return WidgetType::PROC;
            // case "ram":
            //     return WidgetType::RAM;
            // case "system":
            //     return WidgetType::SYSTEM;
        default: {
            target.emplace_back(UNKNOWN_WIDGET{
                .t = WidgetType::UNKNOWN,
                .padding_x = section["padding_x"].value_or(0.0),
                .hoverable = section["hover"].value_or(false),
            });
            break;
        }
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

        left_gaps = t["gaps"]["left"].value_or(4.0);
        center_gaps = t["gaps"]["center"].value_or(4.0);
        right_gaps = t["gaps"]["right"].value_or(4.0);

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
