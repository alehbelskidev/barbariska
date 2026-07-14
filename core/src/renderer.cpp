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
void core::Renderer::draw_text(const char *text, core::Font font, core::RGBA fg,
                               core::Rect r)
{
    cairo_set_source_rgba(cr, fg.r, fg.g, fg.b, fg.a);
    cairo_select_font_face(cr, font.family.c_str(), CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, font.size);

    cairo_text_extents_t ext;
    cairo_text_extents(cr, text, &ext);

    cairo_move_to(cr, r.x, r.y - ext.y_bearing);

    cairo_show_text(cr, text);
}

core::Size core::Renderer::measure_text(const char *text, core::Font font)
{
    cairo_surface_t *temp_surface =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1);
    cairo_t *temp_cr = cairo_create(temp_surface);

    cairo_select_font_face(temp_cr, font.family.c_str(),
                           CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(temp_cr, font.size);

    cairo_text_extents_t ext;
    cairo_text_extents(temp_cr, text, &ext);

    cairo_destroy(temp_cr);
    cairo_surface_destroy(temp_surface);

    return core::Size{.width = (float)ext.width, .height = (float)ext.height};
}
