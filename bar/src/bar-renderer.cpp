#include "bar-renderer.hpp"

void BarRenderer::draw_bg()
{
    // cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0);
    // cairo_paint(cr);
    auto t = config.get_theme();

    cairo_set_source_rgba(cr, t.bg.r, t.bg.g, t.bg.b, t.bg.a);
    cairo_rectangle(cr, 0, 0, surface_dimensions.bar_width,
                    surface_dimensions.bar_height);
    cairo_fill(cr);
}
void BarRenderer::draw_text()
{
    auto t = config.get_theme();
    auto f = config.get_font();

    cairo_set_source_rgba(cr, t.fg.r, t.fg.g, t.fg.b, t.fg.a);
    cairo_select_font_face(cr, f.family.c_str(), CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, f.size);

    cairo_text_extents_t ext;
    cairo_text_extents(cr, state.hypr.active_window, &ext);

    cairo_move_to(cr, 15,
                  (surface_dimensions.bar_height / 2.0 - ext.height / 2.0) -
                      ext.y_bearing);

    cairo_show_text(cr, state.hypr.active_window);
}

void BarRenderer::draw()
{
    draw_bg();
    draw_text();
    flush();
}
