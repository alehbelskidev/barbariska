#include "surface.hpp"

#include <iostream>

Core::Surface::Surface(wl_compositor *compositor,
                       zwlr_layer_shell_v1 *layer_shell, int default_w,
                       int default_h, std::function<void()> on_configure_cb)
    : on_configure(on_configure_cb)
{
    dimensions = {
        .bar_width = default_w,
        .bar_height = default_h,
    };

    surface = wl_compositor_create_surface(compositor);
    layer_surface = zwlr_layer_shell_v1_get_layer_surface(
        layer_shell, surface, nullptr, ZWLR_LAYER_SHELL_V1_LAYER_TOP, "bar");

    zwlr_layer_surface_v1_set_size(layer_surface, 0, dimensions.bar_height);

    // Anchoring bar to the top
    // TODO: get from config!
    zwlr_layer_surface_v1_set_anchor(layer_surface,
                                     ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);

    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface,
                                             dimensions.bar_height);

    zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener,
                                       this);

    // Wayland bufferizing all changes and applies them only after commit
    wl_surface_commit(surface);
}

void Core::Surface::layer_surface_configure_cb(
    void *data, zwlr_layer_surface_v1 *layer_surface, uint32_t serial,
    uint32_t width, uint32_t height)
{
    auto *surface = static_cast<Surface *>(data);
    zwlr_layer_surface_v1_ack_configure(layer_surface, serial);
    surface->dimensions.bar_width = width;
    surface->dimensions.bar_height = height;
    surface->on_configure();
}

void Core::Surface::layer_surface_closed_cb(
    void *data, zwlr_layer_surface_v1 *layer_surface)
{
    std::cout << "layer surface closed\n";
}

void Core::Surface::commit(wl_buffer *buffer)
{
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, dimensions.bar_width,
                      dimensions.bar_height);
    wl_surface_commit(surface);
}
