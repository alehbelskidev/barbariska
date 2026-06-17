#pragma once

#include <cairo/cairo.h>

#include <functional>

#include "state.hpp"
#include "surface.hpp"

namespace Core {
using CommitFn = std::function<void()>;

class Renderer {
private:
    /// cairo_image_surface - wraper around shm_data
    /// Cairo doesn't know about Wayland, it accepts only mem address and
    /// dimensions Cairo draws into that memory - shm_data, COMPOSITOR reads
    /// from same region as - buffer
    cairo_surface_t *cairo_surface;
    CommitFn on_commit;

protected:
    Surface::Dimensions surface_dimensions;
    /// cr - is a pen.
    /// shm always synced
    cairo_t *cr;
    /// Holding ref on state
    State &state;
    void flush();

public:
    Renderer(void *shm_data, CommitFn commit_cb,
             Surface::Dimensions surface_dimensions, int stride, State &state);
    virtual ~Renderer();
};
}  // namespace Core
