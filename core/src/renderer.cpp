#include "renderer.hpp"

Core::Renderer::Renderer(void *shm_data, CommitFn commit_cb,
                         Surface::Dimensions surface_dimensions, int stride,
                         State &state)
    : on_commit(commit_cb), surface_dimensions(surface_dimensions), state(state)
{
    cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char *>(shm_data), CAIRO_FORMAT_ARGB32,
        surface_dimensions.bar_width, surface_dimensions.bar_height, stride);

    cr = cairo_create(cairo_surface);
}

Core::Renderer::~Renderer()
{
    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);
}

void Core::Renderer::flush()
{
    // Flush - cleaning up and drawing new
    cairo_surface_flush(cairo_surface);
    on_commit();
}

void Core::Renderer::draw_bg()
{
    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0);
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0.8, 0.2, 0.2, 1.0);
    cairo_rectangle(cr, 0, 0, surface_dimensions.bar_width,
                    surface_dimensions.bar_height);
    cairo_fill(cr);
}
void Core::Renderer::draw_test()
{
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14.0);
    cairo_move_to(cr, 15, 18);
    cairo_show_text(cr, state.hypr.active_window);
}

void Core::Renderer::render()
{
    draw_bg();
    draw_test();
    flush();
}
