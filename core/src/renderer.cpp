#include "renderer.hpp"

#include "core.hpp"

core::Renderer::Renderer(void *shm_data, CommitFn commit_cb,
                         Surface::Dimensions surface_dimensions, int stride,
                         State &state)
    : on_commit(commit_cb), surface_dimensions(surface_dimensions), state(state)
{
    cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char *>(shm_data), CAIRO_FORMAT_ARGB32,
        surface_dimensions.bar_width, surface_dimensions.bar_height, stride);

    cr = cairo_create(cairo_surface);
}

core::Renderer::~Renderer()
{
    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);
}

void core::Renderer::flush()
{
    // Flush - cleaning up and drawing new
    cairo_surface_flush(cairo_surface);
    on_commit();
}

void core::Renderer::draw_rect(core::Rect r, core::RGBA bg)
{
    cairo_set_source_rgba(cr, bg.r, bg.g, bg.b, bg.a);
    cairo_rectangle(cr, r.x, r.y, r.width, r.height);
    cairo_fill(cr);
}
void core::Renderer::draw_text(char *text, core::Font font, core::RGBA fg,
                               core::V2 padding)
{
    cairo_set_source_rgba(cr, fg.r, fg.g, fg.b, fg.a);
    cairo_select_font_face(cr, font.family.c_str(), CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, font.size);

    cairo_text_extents_t ext;
    cairo_text_extents(cr, text, &ext);

    cairo_move_to(cr, padding.x,
                  (surface_dimensions.bar_height / 2.0 - ext.height / 2.0) -
                      ext.y_bearing);

    cairo_show_text(cr, text);
}
