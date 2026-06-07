#include <cairo/cairo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wayland-client.h>

#include <iostream>

#include "surface.hpp"
#include "wayland-context.hpp"
#include "wlr-layer-shell-unstable-v1-client-protocol.h"

// SHM buffer pipeline:
// shm_open -> ftruncate -> mmap -> wl_shm_pool -> wl_buffer
// COMPOSITOR reads pixels directly from that memory region

int main()
{
    WaylandContext wctx;
    Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920, 30);

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    int stride = surface_dimensions.bar_width * 4;
    int shm_size = stride * surface_dimensions.bar_height;

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
    wl_shm_pool *pool = wl_shm_create_pool(wctx.get_shm(), fd, shm_size);

    // wl_buffer is a view into the pool
    // ARGB8888 - 4 bytes per pixel, blue 1st in mem
    wl_buffer *buffer = wl_shm_pool_create_buffer(
        pool, 0, surface_dimensions.bar_width, surface_dimensions.bar_height,
        stride, WL_SHM_FORMAT_ARGB8888);

    // pool is useless after creating buffer
    wl_shm_pool_destroy(pool);
    // fd as well, mmap keeps memory
    close(fd);

    // cairo_image_surface - wraper around shm_data
    // Cairo doesn't know about Wayland, it accepts only mem address and
    // dimensions Cairo draws into that memory - shm_data, COMPOSITOR reads from
    // same region as - buffer
    cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
        static_cast<unsigned char *>(shm_data), CAIRO_FORMAT_ARGB32,
        surface_dimensions.bar_width, surface_dimensions.bar_height, stride);

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
    surface.commit(buffer);

    while (wctx.should_dispatch()) {
    }

    cairo_destroy(cr);
    cairo_surface_destroy(cairo_surface);

    return 0;
}
