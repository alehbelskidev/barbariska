#pragma once

#include <linux/input-event-codes.h>
#include <wayland-client.h>

#include "math.hpp"
#include "wlr-layer-shell-unstable-v1-client-protocol.h"

namespace core {

class InputContext {
public:
    explicit InputContext(wl_seat *seat);
    ~InputContext();

    vec2 get_pointer_position() const
    {
        return pointer_pos;
    }
    bool is_button_pressed(uint32_t button) const;
    bool get_is_hovering_surface() const
    {
        return is_hovering_surface;
    }

private:
    bool is_hovering_surface = false;
    wl_seat *seat = nullptr;
    wl_pointer *pointer = nullptr;
    vec2 pointer_pos = {0.0f, 0.0f};
    uint32_t button_mask = 0;

    static void pointer_enter_cb(void *data, struct wl_pointer *wl_pointer,
                                 uint32_t serial, struct wl_surface *surface,
                                 wl_fixed_t sx, wl_fixed_t sy);

    static void pointer_leave_cb(void *data, struct wl_pointer *wl_pointer,
                                 uint32_t serial, struct wl_surface *surface);

    static void pointer_motion_cb(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t time, wl_fixed_t sx, wl_fixed_t sy);

    static void pointer_button_cb(void *data, struct wl_pointer *wl_pointer,
                                  uint32_t serial, uint32_t time,
                                  uint32_t button, uint32_t state);

    static void pointer_axis_cb(void *data, struct wl_pointer *wl_pointer,
                                uint32_t time, uint32_t axis, wl_fixed_t value);

    void handle_enter(wl_surface *surface, float x, float y);
    void handle_leave(wl_surface *surface);
    void handle_motion(float x, float y);
    void handle_button(uint32_t button, bool pressed);
    void handle_axis(uint32_t axis, float value);

    static const wl_pointer_listener pointer_listener;
};

}  // namespace core
