#include <poll.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <csignal>
#include <iostream>

#include "bar-renderer.hpp"
#include "client.hpp"
#include "shm-buffer.hpp"
#include "surface.hpp"
#include "wayland-context.hpp"

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
    config._DEBUG_print();
    Core::State state;
    strncpy(state.hypr.active_window, "", 108);
    Core::WaylandContext wctx;
    Core::Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920,
                          30, [efd]() {
                              uint64_t val = 1;
                              write(efd, &val, sizeof(val));
                          });

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    Core::ShmBuffer shm_buffer(surface_dimensions, wctx.get_shm());
    BarRenderer r(
        shm_buffer.get_shm_data(),
        [&surface, &shm_buffer]() { surface.commit(shm_buffer.get_buffer()); },
        surface_dimensions, shm_buffer.get_stride(), state, config);

    Core::Client c([&state, efd](Core::State &new_state) {
        state = new_state;
        uint64_t val = 1;
        write(efd, &val, sizeof(val));
    });

    pollfd fds[3];
    fds[0].fd = wl_display_get_fd(wctx.get_display());
    fds[0].events = POLLIN;
    fds[1].fd = efd;
    fds[1].events = POLLIN;
    fds[2].fd = c.get_readfd();
    fds[2].events = POLLIN;

    while (running) {
        wl_display_flush(wctx.get_display());
        poll(fds, 3, -1);
        if (fds[0].revents & POLLIN) wl_display_dispatch(wctx.get_display());
        if (fds[1].revents & POLLIN) {
            uint64_t val;
            read(efd, &val, sizeof(val));
            r.draw();
            wl_display_flush(wctx.get_display());
            wl_display_dispatch_pending(wctx.get_display());
        }
        if (fds[2].revents & POLLIN) c.poll_state();
    }

    close(efd);
    return 0;
}
