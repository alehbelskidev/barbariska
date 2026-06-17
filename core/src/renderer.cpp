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
