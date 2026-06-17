#include "config.hpp"

#include <stdio.h>

#include <iostream>
#include <string_view>

using namespace std::string_view_literals;

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
    theme.bg = UI::RGBA(t["theme"]["bg"].value_or("#000000"sv));
    theme.fg = UI::RGBA(t["theme"]["fg"].value_or("#ffffff"sv));
    theme.accent = UI::RGBA(t["theme"]["accent"].value_or("#000fff"sv));
}

Config::Config()
{
    char configpath[108];
    snprintf(configpath, 108, "%s%s", getenv("HOME"), BAR_CONFIG_PATH);

    try {
        t = toml::parse_file(configpath);
        parse_font();
        parse_theme();
    } catch (const toml::parse_error &err) {
        std::cerr << err << "\n";
    }
}
