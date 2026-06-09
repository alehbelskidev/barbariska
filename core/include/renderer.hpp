#pragma once

#include <cairo/cairo.h>

#include <functional>

#include "surface.hpp"

namespace Core {
using CommitFn = std::function<void()>;

class Renderer {
private:
    // cairo_image_surface - wraper around shm_data
    // Cairo doesn't know about Wayland, it accepts only mem address and
    // dimensions Cairo draws into that memory - shm_data, COMPOSITOR reads from
    // same region as - buffer
    cairo_surface_t *cairo_surface;
    // cr - is a pen.
    cairo_t *cr;
    CommitFn on_commit;
    void flush();

public:
    Renderer(Surface::Dimensions surface_dimensions, void *shm_data, int stride,
             CommitFn commit_cb);
    ~Renderer();

    void draw_bg(Surface::Dimensions surface_dimensions);
    void draw_test();
};
}  // namespace Core
