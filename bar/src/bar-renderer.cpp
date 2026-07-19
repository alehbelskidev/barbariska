#include "bar-renderer.hpp"

#include "surface.hpp"

void BarRenderer::theme_draw_rect(core::Rect rect, bool inverted)
{
    auto theme = config.get_theme();
    auto font = config.get_font();
    draw_rect(rect, inverted ? theme.fg : theme.bg);
}

void BarRenderer::theme_draw_text(std::string text, core::Rect r, bool inverted)
{
    auto theme = config.get_theme();
    auto font = config.get_font();
    auto config_root = config.get_root();
    draw_text(text.c_str(), font, inverted ? theme.bg : theme.fg, r);
}

core::Size BarRenderer::theme_measure_text(std::string text)
{
    auto font = config.get_font();
    return measure_text(text.c_str(), font);
}
