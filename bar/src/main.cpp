#include <cairo/cairo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wayland-client.h>

#include <iostream>

#include "wlr-layer-shell-unstable-v1-client-protocol.h"

// SHM buffer pipeline:
// shm_open -> ftruncate -> mmap -> wl_shm_pool -> wl_buffer
// COMPOSITOR reads pixels directly from that memory region

//----PROTOCOL OBJECTS(interfaces)
// DISPLAY - object as entrypoint for wayland
wl_display *display = nullptr;
// REGISTRY - is the list of all global object provided by COMPOSITOR.
wl_registry *registry = nullptr;
// COMPOSITOR - creates new surfaces (canvases on which we drew).
// wl_surface will be created with COMPOSITOR help
wl_compositor *compositor = nullptr;
// SHARED_MEMORY - allocating buf in memory to draw in it with Cairo.
// COMPOSITOR reads from that buffer and displaying on a screen.
wl_shm *shm = nullptr;
// LAYER_SHELL - wlroots ext. which allows to anchor window to screen edge.
// Can't make bar w/o it, cuz regular Wayland windows can'f fix themselves
zwlr_layer_shell_v1 *layer_shell = nullptr;
///-----

// COMPOSITOR calling that function as callback for each available global object
static void registry_global_cb(void *data, wl_registry *reg, uint32_t name,
                               const char *interface, uint32_t version)
{
    // Comparing interface names and biding what we need
    // wl_registry_bind ask object of a certain type from COMPOSITOR
    if (std::string_view(interface) == wl_compositor_interface.name) {
        compositor = static_cast<wl_compositor *>(
            wl_registry_bind(reg, name, &wl_compositor_interface, 4));
    } else if (std::string_view(interface) == wl_shm_interface.name) {
        shm = static_cast<wl_shm *>(
            wl_registry_bind(reg, name, &wl_shm_interface, 1));
    } else if (std::string_view(interface) ==
               zwlr_layer_shell_v1_interface.name)
    {
        layer_shell = static_cast<zwlr_layer_shell_v1 *>(
            wl_registry_bind(reg, name, &zwlr_layer_shell_v1_interface, 1));
    }
}

// COMPOSITOR calling this function when global object is GONE
// GONE=means COMPOSITOR removed interface during runtime, e.g.
// disabled display and `wl_output` disapeared for that display.
static void registry_global_remove_cb(void *data, wl_registry *reg,
                                      uint32_t name)
{
}

// Callback table for registry
// Wayland using them exactly that way
static const wl_registry_listener registry_listener = {
    .global = registry_global_cb,
    .global_remove = registry_global_remove_cb,
};

// COMPOSITOR sends `configure` with real sizes when ready to show `surface`.
static void layer_surface_configure_cb(void *data,
                                       zwlr_layer_surface_v1 *layer_surface,
                                       uint32_t serial, uint32_t width,
                                       uint32_t height)
{
    zwlr_layer_surface_v1_ack_configure(layer_surface, serial);
    std::cout << "configured: " << width << "x" << height << "\n";
}

// COMPOSITOR sends `closed` when surface will be removed,
// .e.g display turned off
static void layer_surface_closed_cb(void *data,
                                    zwlr_layer_surface_v1 *layer_surface)
{
    std::cout << "layer surface closed\n";
}

static const zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure_cb,
    .closed = layer_surface_closed_cb,
};

int main()
{
    // Opening COMPOSITOR connection through WAYLAND_DISPLAY from env,
    // usually wayland-1? doubt it
    display = wl_display_connect(nullptr);
    if (!display) {
        std::cerr << "Failed to connect to wayland\n";
        return 1;
    }

    // Getting registry and attaching listener to get all available interfaces
    registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, nullptr);

    // Waiting for COMPOSITOR to send all global objects
    // after that "roundtrip" `registry_global` will be called
    // for each *interface
    wl_display_roundtrip(display);

    if (!compositor || !shm || !layer_shell) {
        std::cerr << "Required interfaces not available\n";
        return 1;
    }

    std::cout << "All interfaces bound\n";

    //----
    // SURFACE = canvas. Empty rect recognized by COMPOSITOR.
    // w/o SURFACE there is nothing - no pos, no size, no pixels
    wl_surface *surface = wl_compositor_create_surface(compositor);

    // LAYER_SURFACE - extension to surface specific for wlroots.
    // It tells COMPOSITOR that we have layer object instead of regular window
    // nullptr - output (screen), nullptr as arg in our case means COMPOSITOR
    // will pick itself ZWLR_LAYER_SHELL_V1_LAYER_TOP - layer above regular
    // windows, below overlays. "bar" - namespace name, serves as id
    zwlr_layer_surface_v1 *layer_surface =
        zwlr_layer_shell_v1_get_layer_surface(layer_shell, surface, nullptr,
                                              ZWLR_LAYER_SHELL_V1_LAYER_TOP,
                                              "bar");
    // TODO: height should come from config->daemon
    auto mock_height = 30;
    // 0 - full width
    // 30 - height
    zwlr_layer_surface_v1_set_size(layer_surface, 0, mock_height);

    // Anchoring bar to the top
    // TODO: get from config!
    zwlr_layer_surface_v1_set_anchor(layer_surface,
                                     ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP |
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                                         ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT);

    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, mock_height);

    zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener,
                                       nullptr);

    // Wayland bufferizing all changes and applies them only after commit
    wl_surface_commit(surface);

    // Waiting for COMPOSITOR to send us `configure event`.
    // LAYER_SURFACE won't activate w/o it
    wl_display_roundtrip(display);

    // TODO: width comes from configure callback - `layer_surface_configure_cb`
    // for now hardcode
    int bar_width = 2560;
    int bar_height = mock_height;
    int stride = bar_width * 4;
    int shm_size = stride * bar_height;

    // shm_open creates anon file in RAM
    // COMPOSITOR and our process share this memory region
    int fd = shm_open("/barbariska-bar", O_RDWR | O_CREAT, 0600);
    shm_unlink("/barbariska-bar");  // delete name , keep file descriptor alive
    ftruncate(fd, shm_size);        // set filesize to fit pixels

    // mmap maps that file inot our address space
    // we write pixels here - COMPOSITOR reads from same physical memory
    void *shm_data =
        mmap(nullptr, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // wl_shm_pool wraps our fd for COMPOSITOR
    // COMPOSITOR now knows about this memory region
    wl_shm_pool *pool = wl_shm_create_pool(shm, fd, shm_size);

    // wl_buffer is a view into the pool
    // ARGB8888 - 4 bytes per pixel, blue 1st in mem
    wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, bar_width, bar_height, stride, WL_SHM_FORMAT_ARGB8888);

    // pool is useless after creating buffer
    wl_shm_pool_destroy(pool);
    // fd as well, mmap keeps memory
    close(fd);

    // cairo_image_surface - wraper around shm_data
    // Cairo doesn't know about Wayland, it accepts only mem address and
    // dimensions Cairo draws into that memory - shm_data, COMPOSITOR reads from
    // same region as - buffer
    cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char *>(shm_data), CAIRO_FORMAT_ARGB32, bar_width,
        bar_height, stride);

    // cr - is a pen.
    cairo_t *cr = cairo_create(cairo_surface);

    cairo_set_source_rgba(cr, 0.1, 0.1, 0.1, 0);
    cairo_paint(cr);

    cairo_set_source_rgba(cr, 0.8, 0.2, 0.2, 1.0);
    cairo_rectangle(cr, 0, 0, bar_width, bar_height);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_select_font_face(cr, "monospace", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 14.0);
    cairo_move_to(cr, 15, 18);
    cairo_show_text(cr, "Hello World");

    // Flush - cleaning up and drawing new
    cairo_surface_flush(cairo_surface);

    // Telling SURFACE which buffer to show and commit
    // New pixels will be AVAILABLE for COMPOSER only after commit
    wl_surface_attach(surface, buffer, 0, 0);
    wl_surface_damage(surface, 0, 0, bar_width, bar_height);
    wl_surface_commit(surface);

    // wl_display_dispatch blocks process until new COMPOSITOR events came up
    // reads events, calling callbacks, blocks again
    while (wl_display_dispatch(display) != -1) {
    }

    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);
    wl_display_disconnect(display);

    return 0;
}
