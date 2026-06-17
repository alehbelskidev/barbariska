#pragma once

#include "config.hpp"
#include "renderer.hpp"
#include "surface.hpp"

class BarRenderer : public Core::Renderer {
private:
    Config &config;

    void draw_bg();
    void draw_text();

public:
    BarRenderer(void *shm_data, Core::CommitFn commit_cb,
                Core::Surface::Dimensions surface_dimensions, int stride,
                Core::State &state, Config &config)
        : Renderer(shm_data, commit_cb, surface_dimensions, stride, state),
          config(config)
    {
    }
    ~BarRenderer() = default;

    void draw();
};
