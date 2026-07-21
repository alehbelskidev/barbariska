#pragma once

#include <string>

#include "bar-renderer.hpp"
#include "core.hpp"

enum class anchor { LEFT, CENTER, RIGHT };

struct block {
    vec2 padding;
    float x, y, width, height, gap;
    float text_x, text_y, text_width, text_height;
    float roundness;
    bool hoverable;
    bool hovered;
    WidgetType t;
    std::string text;
    std::optional<std::string> format;
};

class UI {
public:
    UI(BarRenderer &r, std::vector<WidgetVariant> &left,
       std::vector<WidgetVariant> &center, std::vector<WidgetVariant> &right,
       Root &root);

    void draw(core::State &s);
    void hover(core::vec2 mouse_pos);
    void reset_hover();
    /// TODO: click

private:
    BarRenderer &r;
    Widget &root;
    float root_width;

    std::vector<block> lblocks;
    float lstart;
    std::vector<block> cblocks;
    float cstart;
    std::vector<block> rblocks;
    float rstart;

    void prepare_container(anchor a, float &start, std::vector<block> &blocks,
                           const core::State &s);
    void draw_container(const float &start, std::vector<block> &blocks);

    void prepare_text_block(block &b, std::string text);
    void draw_text_block(float &caret, block &b);
};
