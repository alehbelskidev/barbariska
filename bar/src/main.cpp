#include <poll.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

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
    config._DEBUG_print();
    auto config_root = config.get_root();
    auto bar_height = config_root.height + config_root.padding.y * 2;

    core::State state;
    strncpy(state.hypr.active_window, "", 108);
    core::WaylandContext wctx;
    core::Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920,
                          bar_height, [efd]() {
                              uint64_t val = 1;
                              write(efd, &val, sizeof(val));
                          });

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    core::ShmBuffer shm_buffer(surface_dimensions, wctx.get_shm());
    BarRenderer r(
        shm_buffer.get_shm_data(),
        [&surface, &shm_buffer]() { surface.commit(shm_buffer.get_buffer()); },
        surface_dimensions, shm_buffer.get_stride(), state, config);

    core::Client c([&state, efd](core::State &new_state) {
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

    auto theme = config.get_theme();

    auto left = container(core::NodeData{}, {}, Anchor::LEFT);
    auto center = container(core::NodeData{}, {}, Anchor::CENTER);
    auto right = container(core::NodeData{}, {}, Anchor::RIGHT);

    parse_config_group(config.get_left(), left);
    parse_config_group(config.get_center(), center);
    parse_config_group(config.get_right(), right);

    std::vector<std::unique_ptr<core::Node>> groups;
    groups.push_back(std::move(left));
    groups.push_back(std::move(center));
    groups.push_back(std::move(right));

    auto tree = container(
        core::NodeData{
            .width = (float)surface_dimensions.bar_width,
            .height = (float)surface_dimensions.bar_height,
            .x = 0,
            .y = 0,
            .padding = config_root.padding,
        },
        std::move(groups), Anchor::FULL);

    while (running) {
        wl_display_flush(wctx.get_display());
        poll(fds, 3, -1);
        if (fds[0].revents & POLLIN) wl_display_dispatch(wctx.get_display());
        if (fds[1].revents & POLLIN) {
            uint64_t val;
            read(efd, &val, sizeof(val));

            tree->render(&r);

            r.draw_finish();

            wl_display_flush(wctx.get_display());
            wl_display_dispatch_pending(wctx.get_display());
        }
        if (fds[2].revents & POLLIN) c.poll_state();
    }

    close(efd);
    return 0;
}
