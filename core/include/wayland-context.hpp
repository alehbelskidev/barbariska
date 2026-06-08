#pragma once

#include <wayland-client.h>

#include "wlr-layer-shell-unstable-v1-client-protocol.h"

namespace Core {
class WaylandContext {
private:
    ///----PROTOCOL OBJECTS(interfaces)
    /// DISPLAY - object as entrypoint for wayland
    wl_display *display = nullptr;
    /// REGISTRY - is the list of all global object provided by COMPOSITOR.
    wl_registry *registry = nullptr;
    /// COMPOSITOR - creates new surfaces (canvases on which we drew).
    /// wl_surface will be created with COMPOSITOR help
    wl_compositor *compositor = nullptr;
    /// SHARED_MEMORY - allocating buf in memory to draw in it with Cairo.
    /// COMPOSITOR reads from that buffer and displaying on a screen.
    wl_shm *shm = nullptr;
    /// LAYER_SHELL - wlroots ext. which allows to anchor window to screen edge.
    /// Can't make bar w/o it, cuz regular Wayland windows can'f fix themselves
    zwlr_layer_shell_v1 *layer_shell = nullptr;

    /// COMPOSITOR calling that function as callback for each available global
    static void registry_global_cb(void *data, wl_registry *reg, uint32_t name,
                                   const char *interface, uint32_t version);

    /// COMPOSITOR calling this function when global object is GONE
    /// GONE=means COMPOSITOR removed interface during runtime, e.g.
    /// disabled display and `wl_output` disapeared for that display.
    static void registry_global_remove_cb(void *data, wl_registry *reg,
                                          uint32_t name);

    /// Callback table for registry
    /// Wayland using them exactly that way
    const wl_registry_listener registry_listener = {
        .global = registry_global_cb,
        .global_remove = registry_global_remove_cb,
    };

public:
    WaylandContext();
    ~WaylandContext();

    /// wl_display_dispatch blocks process until new COMPOSITOR events came up
    /// reads events, calling callbacks, blocks again
    bool should_dispatch();
    /// Waiting for COMPOSITOR to send us `configure event`.
    /// LAYER_SURFACE won't activate w/o it
    void roundtrip();

    wl_display *get_display()
    {
        return display;
    };

    wl_compositor *get_compositor()
    {
        return compositor;
    }

    wl_shm *get_shm()
    {
        return shm;
    }

    zwlr_layer_shell_v1 *get_layer_shell()
    {
        return layer_shell;
    }
};
}  // namespace Core
