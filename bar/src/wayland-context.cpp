#include "wayland-context.hpp"

#include <iostream>

void WaylandContext::registry_global_cb(void *data, wl_registry *reg,
                                        uint32_t name, const char *interface,
                                        uint32_t version)
{
    auto *ctx = static_cast<WaylandContext *>(data);
    // Comparing interface names and biding what we need
    // wl_registry_bind ask object of a certain type from COMPOSITOR
    if (std::string_view(interface) == wl_compositor_interface.name) {
        ctx->compositor = static_cast<wl_compositor *>(
            wl_registry_bind(reg, name, &wl_compositor_interface, 4));
    } else if (std::string_view(interface) == wl_shm_interface.name) {
        ctx->shm = static_cast<wl_shm *>(
            wl_registry_bind(reg, name, &wl_shm_interface, 1));
    } else if (std::string_view(interface) ==
               zwlr_layer_shell_v1_interface.name)
    {
        ctx->layer_shell = static_cast<zwlr_layer_shell_v1 *>(
            wl_registry_bind(reg, name, &zwlr_layer_shell_v1_interface, 1));
    }
}

void WaylandContext::registry_global_remove_cb(void *data, wl_registry *reg,
                                               uint32_t name)
{
}

WaylandContext::WaylandContext()
{
    // Opening COMPOSITOR connection through WAYLAND_DISPLAY from env,
    // usually wayland-1? doubt it
    display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Failed to connect to wayland\n";
    }

    // Getting registry and attaching listener to get all available interfaces
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, this);

    // Waiting for COMPOSITOR to send all global objects
    // after that "roundtrip" `registry_global` will be called
    // for each *interface
    wl_display_roundtrip(display);

    if (!compositor || !shm || !layer_shell) {
        std::cerr << "Required interfaces not available\n";
    }

    std::cout << "All interfaces bound\n";
}

WaylandContext::~WaylandContext()
{
    wl_display_disconnect(display);
}

void WaylandContext::roundtrip()
{
    wl_display_roundtrip(display);
}

bool WaylandContext::should_dispatch()
{
    return wl_display_dispatch(display) != -1;
}
