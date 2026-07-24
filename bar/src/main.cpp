#include <poll.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <array>
#include <csignal>
#include <iostream>
#include <memory>
#include <utility>

#include "bar-layout.hpp"
#include "bar-renderer.hpp"
#include "core.hpp"

static bool running = true;

void on_signal(int)
{
    running = false;
}

int main()
{
    signal(SIGTERM, on_signal);
    signal(SIGINT, on_signal);

    int efd = eventfd(0, EFD_CLOEXEC);

    Config config;
    core::State state;

    strncpy(state.hypr.active_window_class, "", 108);
    strncpy(state.hypr.active_window_title, "", 108);

    core::WaylandContext wctx;
    core::InputContext ictx(wctx.get_seat());
    core::Surface surface(
        wctx.get_compositor(), wctx.get_layer_shell(), 1920,
        config.get_root().height + config.get_root().padding.y * 2, [efd]() {
            uint64_t val = 1;
            write(efd, &val, sizeof(val));
        });

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    config.set_root_width(surface_dimensions.bar_width);

    core::ShmBuffer shm_buffer(surface_dimensions, wctx.get_shm());
    BarRenderer r(
        shm_buffer.get_shm_data(),
        [&surface, &shm_buffer]() { surface.commit(shm_buffer.get_buffer()); },
        surface_dimensions, shm_buffer.get_stride(), config);

    bool redraw_pending = false;

    core::Client c([&state, efd, &redraw_pending](core::State &new_state) {
        state = new_state;
        if (!redraw_pending) {
            redraw_pending = true;
            uint64_t val = 1;
            write(efd, &val, sizeof(val));
        }
    });

    pollfd fds[4];
    fds[0].fd = wl_display_get_fd(wctx.get_display());
    fds[0].events = POLLIN;
    fds[1].fd = efd;
    fds[1].events = POLLIN;
    fds[2].fd = c.get_readfd();
    fds[2].events = POLLIN;

    auto theme = config.get_theme();

    auto ui = UI(r, config.get_left(), config.get_center(), config.get_right(),
                 config.get_root());

    auto finish_draw = [&r, &wctx]() {
        r.draw_finish();
        wl_display_flush(wctx.get_display());
        wl_display_dispatch_pending(wctx.get_display());
    };

    while (running) {
        wl_display_flush(wctx.get_display());
        poll(fds, 3, -1);

        if (ictx.get_is_hovering_surface()) {
            auto pointer_pos = ictx.get_pointer_position();
            ui.hover(pointer_pos);
            ui.draw(state);
            finish_draw();

            if (ictx.is_button_pressed(0)) {
                std::cout << "button presed 0" << std::endl;
            }
        } else {
            ui.reset_hover();
        }

        if (fds[0].revents & POLLIN) wl_display_dispatch(wctx.get_display());
        if (fds[1].revents & POLLIN) {
            uint64_t val;
            read(efd, &val, sizeof(val));
            redraw_pending = false;
            ui.draw(state);
            finish_draw();
        }

        if (fds[2].revents & POLLIN) {
            c.poll_state();
        }
    }

    close(efd);
    return 0;
}
