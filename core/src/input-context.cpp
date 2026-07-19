#include "input-context.hpp"

const wl_pointer_listener core::InputContext::pointer_listener = {
    .enter = pointer_enter_cb,
    .leave = pointer_leave_cb,
    .motion = pointer_motion_cb,
    .button = pointer_button_cb,
    .axis = pointer_axis_cb};

core::InputContext::InputContext(wl_seat *seat) : seat(seat)
{
    pointer = wl_seat_get_pointer(seat);
    if (!pointer) {
        /// TODO:: throw something?
        return;
    }
    wl_pointer_add_listener(pointer, &pointer_listener, this);
}

core::InputContext::~InputContext()
{
    if (pointer) {
        wl_pointer_destroy(pointer);
        pointer = nullptr;
    }
}

void core::InputContext::pointer_enter_cb(void *data,
                                          struct wl_pointer *wl_pointer,
                                          uint32_t serial,
                                          struct wl_surface *surface,
                                          wl_fixed_t sx, wl_fixed_t sy)
{
    auto *self = static_cast<InputContext *>(data);
    self->handle_enter(surface, wl_fixed_to_double(sx), wl_fixed_to_double(sy));
}

void core::InputContext::pointer_leave_cb(void *data,
                                          struct wl_pointer *wl_pointer,
                                          uint32_t serial,
                                          struct wl_surface *surface)
{
    auto *self = static_cast<InputContext *>(data);
    self->handle_leave(surface);
}

void core::InputContext::pointer_motion_cb(void *data,
                                           struct wl_pointer *wl_pointer,
                                           uint32_t time, wl_fixed_t sx,
                                           wl_fixed_t sy)
{
    auto *self = static_cast<InputContext *>(data);
    self->handle_motion(wl_fixed_to_double(sx), wl_fixed_to_double(sy));
}

void core::InputContext::pointer_button_cb(void *data,
                                           struct wl_pointer *wl_pointer,
                                           uint32_t serial, uint32_t time,
                                           uint32_t button, uint32_t state)
{
    auto *self = static_cast<InputContext *>(data);
    bool pressed = (state == WL_POINTER_BUTTON_STATE_PRESSED);
    self->handle_button(button, pressed);
}

void core::InputContext::pointer_axis_cb(void *data,
                                         struct wl_pointer *wl_pointer,
                                         uint32_t time, uint32_t axis,
                                         wl_fixed_t value)
{
    auto *self = static_cast<InputContext *>(data);
    self->handle_axis(axis, wl_fixed_to_double(value));
}

void core::InputContext::handle_enter(wl_surface *surface, float x, float y)
{
    is_hovering_surface = true;
    pointer_pos.x = x;
    pointer_pos.y = y;
}

void core::InputContext::handle_leave(wl_surface *surface)
{
    is_hovering_surface = false;
}

void core::InputContext::handle_motion(float x, float y)
{
    pointer_pos.x = x;
    pointer_pos.y = y;
}

void core::InputContext::handle_button(uint32_t button, bool pressed)
{
    if (pressed) {
        button_mask |= (1u << button);
    } else {
        button_mask &= ~(1u << button);
    }
}

void core::InputContext::handle_axis(uint32_t axis, float value)
{
}

bool core::InputContext::is_button_pressed(uint32_t button) const
{
    return (button_mask & (1u << button)) != 0;
}
