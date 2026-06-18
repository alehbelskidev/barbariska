#pragma once

#include "config.hpp"
#include "renderer.hpp"
#include "shapes.hpp"
#include "surface.hpp"

class BarRenderer : public Core::Renderer {
private:
    Config &config;

public:
    BarRenderer(void *shm_data, Core::CommitFn commit_cb,
                Core::Surface::Dimensions surface_dimensions, int stride,
                Core::State &state, Config &config)
        : Renderer(shm_data, commit_cb, surface_dimensions, stride, state),
          config(config)
    {
    }
    ~BarRenderer() = default;

    void draw_rect(Shapes::Rect r, Colors::RGBA bg);
    void draw_text(char *text, Colors::RGBA fg);

    /// Should always been call last!!!
    void draw_finish()
    {
        flush();
    }
};
