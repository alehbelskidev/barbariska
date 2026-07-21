#pragma once

#include <cairo/cairo.h>

#include <functional>

#include "colors.hpp"
#include "font.hpp"
#include "math.hpp"
#include "shapes.hpp"
#include "surface.hpp"

namespace core {
using CommitFn = std::function<void()>;
/// TODO: REmove this shit
struct Size {
    float width, height;
};

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
    void flush();
    cairo_font_face_t *font_face;

public:
    Renderer(void *shm_data, CommitFn commit_cb,
             core::Surface::Dimensions surface_dimensions, int stride);
    virtual ~Renderer();

    void draw_rect(core::Rect r, core::rgba bg);
    void draw_rect_rounded(core::Rect r, core::rgba bg, float roundness);
    void draw_text(const char *text, core::Font font, core::rgba fg,
                   core::Rect r);
    core::Size measure_text(const char *text, core::Font font);

    /// Should always been called last!!!
    void draw_finish()
    {
        flush();
    }
};
}  // namespace core
