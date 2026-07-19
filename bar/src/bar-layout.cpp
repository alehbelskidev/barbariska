#include "bar-layout.hpp"

#include <iostream>
#include <regex>

#include "bar-renderer.hpp"

void create_blocks_of_widgets(const std::vector<Widget> &in,
                              std::vector<block> &out)
{
    for (const auto &it : in) {
        out.push_back(block{.gap = it.gap,
                            .hoverable = it.hoverable,
                            .t = it.t,
                            .format = it.format});
    }
}

UI::UI(BarRenderer &r, std::vector<Widget> &left, std::vector<Widget> &center,
       std::vector<Widget> &right, Widget &root, float root_width)
    : r(r), root(root), root_width(root_width)
{
    create_blocks_of_widgets(left, lblocks);
    create_blocks_of_widgets(center, cblocks);
    create_blocks_of_widgets(right, rblocks);
}

void UI::draw(core::State &s)
{
    r.theme_draw_rect(core::Rect{
        .x = 0,
        .y = 0,
        .width = root_width,
        .height = root.height,
    });

    prepare_container(anchor::LEFT, lstart, lblocks, s);
    prepare_container(anchor::CENTER, cstart, cblocks, s);
    prepare_container(anchor::RIGHT, rstart, rblocks, s);

    draw_container(lstart, lblocks);
    draw_container(cstart, cblocks);
    draw_container(rstart, rblocks);
}

void mark_blocks_as_hovered(std::vector<block> &blocks,
                            const core::vec2 &mouse_pos)
{
    for (auto &it : blocks) {
        it.hovered = it.x <= mouse_pos.x && mouse_pos.x <= it.x + it.width;
    }
}

void UI::hover(core::vec2 mouse_pos)
{
    mark_blocks_as_hovered(lblocks, mouse_pos);
    mark_blocks_as_hovered(cblocks, mouse_pos);
    mark_blocks_as_hovered(rblocks, mouse_pos);
}

void reset_all_blocks_hover(std::vector<block> &blocks)
{
    for (auto &it : blocks) {
        it.hovered = false;
    }
}

void UI::reset_hover()
{
    reset_all_blocks_hover(lblocks);
    reset_all_blocks_hover(cblocks);
    reset_all_blocks_hover(rblocks);
}

/// TODO:: Trim text to 54-54 max based on format, 108 max in general
/// FIXME:: Check daemon cuz, this should be trimmed on daemon side :(
std::string format_window_text(std::optional<std::string> &format,
                               std::string &active_window_class,
                               std::string &active_window_title)
{
    std::string result;

    if (format.has_value()) {
        result = format.value();
        result = std::regex_replace(result, std::regex("\\%class"),
                                    active_window_class);
        result = std::regex_replace(result, std::regex("\\%title"),
                                    active_window_title);
    } else {
        result = std::string(active_window_class + " - " + active_window_title);
    }

    return result;
}

void UI::prepare_container(anchor a, float &start, std::vector<block> &blocks,
                           const core::State &s)
{
    float total_width = 0;

    for (auto &it : blocks) {
        switch (it.t) {
            case WidgetType::WINDOW: {
                auto active_window_class =
                    std::string(s.hypr.active_window_class);
                auto active_window_title =
                    std::string(s.hypr.active_window_title);
                auto formatted_text = format_window_text(
                    it.format, active_window_class, active_window_title);

                prepare_text_block(it, formatted_text);

                total_width += it.width;
                break;
            }
            case WidgetType::UNKNOWN: {
                prepare_text_block(it, "UNKNOWN_WIDGET");
                break;
            }
            case WidgetType::ROOT:
            case WidgetType::WORKSPACES:
            case WidgetType::CLOCK:
            case WidgetType::WIFI:
            case WidgetType::VOLUME:
            case WidgetType::DISKS:
            case WidgetType::PROC:
            case WidgetType::RAM:
            case WidgetType::SYSTEM:
                break;
        }
    }

    switch (a) {
        case anchor::LEFT:
            start = root.gap;
            break;
        case anchor::CENTER:
            start = (root_width - root.gap) / 2 - total_width / 2;
            break;
        case anchor::RIGHT:
            start = root_width - root.gap - total_width;
            break;
    }
}

void UI::draw_container(const float &start, std::vector<block> &blocks)
{
    float caret = start;

    for (auto &it : blocks) {
        switch (it.t) {
            case WidgetType::WINDOW:
            case WidgetType::UNKNOWN:
                draw_text_block(caret, it);
                caret += it.width;
                break;
            case WidgetType::ROOT:
            case WidgetType::WORKSPACES:
            case WidgetType::CLOCK:
            case WidgetType::WIFI:
            case WidgetType::VOLUME:
            case WidgetType::DISKS:
            case WidgetType::PROC:
            case WidgetType::RAM:
            case WidgetType::SYSTEM:
                break;
        }
    }
}

void UI::prepare_text_block(block &b, std::string text)
{
    auto text_ext = r.theme_measure_text(text);

    b.width = text_ext.width + b.gap * 2;
    b.height = root.height;
    b.text_width = text_ext.width;
    b.text_height = text_ext.height;
    b.text = text;
}

void UI::draw_text_block(float &caret, block &b)
{
    b.x = caret;
    b.y = 0;
    b.text_x = b.x + b.gap;
    b.text_y = b.height / 2 - b.text_height / 2;

    r.theme_draw_rect(
        core::Rect{
            .x = b.x,
            .y = b.y,
            .width = b.width,
            .height = b.height,
        },
        b.hovered && b.hoverable);

    r.theme_draw_text(b.text,
                      core::Rect{
                          .x = b.text_x,
                          .y = b.text_y,
                          .width = b.text_width,
                          .height = b.text_height,
                      },
                      b.hovered && b.hoverable);
}
