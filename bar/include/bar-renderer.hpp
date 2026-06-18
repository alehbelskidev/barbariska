#pragma once

#include "config.hpp"
#include "renderer.hpp"
#include "surface.hpp"

class BarRenderer : public core::Renderer {
private:
    Config &config;

public:
    BarRenderer(void *shm_data, core::CommitFn commit_cb,
                core::Surface::Dimensions surface_dimensions, int stride,
                core::State &state, Config &config)
        : Renderer(shm_data, commit_cb, surface_dimensions, stride, state),
          config(config)
    {
    }
    ~BarRenderer() = default;
};
