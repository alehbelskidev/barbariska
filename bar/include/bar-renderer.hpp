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
                Config &config)
        : Renderer(shm_data, commit_cb, surface_dimensions, stride),
          config(config)
    {
    }
    ~BarRenderer() = default;

    void theme_draw_rect(core::Rect rect, bool inverted = false);
    void theme_draw_rect_rounded(core::Rect rect, bool inverted = false,
                                 float roundness = 0);
    void theme_draw_text(std::string text, core::Rect r, bool inverted = false);
    core::Size theme_measure_text(std::string text);
};
