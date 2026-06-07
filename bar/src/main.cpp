#include <cairo/cairo.h>
#include <wayland-client.h>

#include "shm-buffer.hpp"
#include "surface.hpp"
#include "wayland-context.hpp"

int main()
{
    WaylandContext wctx;
    Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920, 30);

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    ShmBuffer shm_buffer(surface_dimensions.bar_width,
                         surface_dimensions.bar_height, wctx.get_shm());

    // cairo_image_surface - wraper around shm_data
    // Cairo doesn't know about Wayland, it accepts only mem address and
    // dimensions Cairo draws into that memory - shm_data, COMPOSITOR reads from
    // same region as - buffer
    cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char *>(shm_buffer.get_shm_data()),
        CAIRO_FORMAT_ARGB32, surface_dimensions.bar_width,
        surface_dimensions.bar_height, shm_buffer.get_stride());

    // cr - is a pen.
    cairo_t *cr = cairo_create(cairo_surface);

    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0);
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0.8, 0.2, 0.2, 1.0);
    cairo_rectangle(cr, 0, 0, surface_dimensions.bar_width,
                    surface_dimensions.bar_height);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14.0);
    cairo_move_to(cr, 15, 18);
    cairo_show_text(cr, "Hello World");

    // Flush - cleaning up and drawing new
    cairo_surface_flush(cairo_surface);

    surface.commit(shm_buffer.get_buffer());

    while (wctx.should_dispatch()) {
    }

    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);

    return 0;
}
