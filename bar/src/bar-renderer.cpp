#include "bar-renderer.hpp"

void BarRenderer::draw_rect(Shapes::Rect r, Colors::RGBA bg)
{
    cairo_set_source_rgba(cr, bg.r, bg.g, bg.b, bg.a);
    cairo_rectangle(cr, r.x, r.y, r.width, r.height);
    cairo_fill(cr);
}
void BarRenderer::draw_text(char *text, Colors::RGBA fg)
{
    auto f = config.get_font();

    cairo_set_source_rgba(cr, fg.r, fg.g, fg.b, fg.a);
    cairo_select_font_face(cr, f.family.c_str(), CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, f.size);

    cairo_text_extents_t ext;
    cairo_text_extents(cr, text, &ext);

    cairo_move_to(cr, 0,
                  (surface_dimensions.bar_height / 2.0 - ext.height / 2.0) -
                      ext.y_bearing);

    cairo_show_text(cr, text);
}
