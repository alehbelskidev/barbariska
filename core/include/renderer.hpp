#pragma once

#include <cairo/cairo.h>

#include <functional>

#include "colors.hpp"
#include "font.hpp"
#include "shapes.hpp"
#include "state.hpp"
#include "surface.hpp"

namespace core {
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
    core::Surface::Dimensions surface_dimensions;
    /// cr - is a pen.
    /// shm always synced
    cairo_t *cr;
    /// Holding ref on state
    State &state;
    void flush();

public:
    Renderer(void *shm_data, CommitFn commit_cb,
             core::Surface::Dimensions surface_dimensions, int stride,
             State &state);
    virtual ~Renderer();

    void draw_rect(core::Rect r, core::RGBA bg);
    void draw_text(char *text, core::Font font, core::RGBA fg);

    /// Should always been call last!!!
    void draw_finish()
    {
        flush();
    }
};
}  // namespace core
