#pragma once

#include <string>

#include "config.hpp"
#include "core.hpp"

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

    void theme_draw_rect(core::Rect rect);
    void theme_draw_text(std::string text, core::Rect r);
    core::Size theme_measure_text(std::string text);
};
