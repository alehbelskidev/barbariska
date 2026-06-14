#include <iostream>

#include "client.hpp"
#include "renderer.hpp"
#include "shm-buffer.hpp"
#include "surface.hpp"
#include "wayland-context.hpp"

int main()
{
    Core::WaylandContext wctx;
    Core::Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920,
                          30);

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();

    Core::ShmBuffer shm_buffer(surface_dimensions, wctx.get_shm());
    Core::Renderer r(
        surface_dimensions, shm_buffer.get_shm_data(), shm_buffer.get_stride(),
        [&surface, &shm_buffer]() { surface.commit(shm_buffer.get_buffer()); });
    Core::Client c([&r, &surface_dimensions](Core::State *state) {
        std::cout << "on_update: " << state->hypr.active_window << "\n";
        r.draw_bg(surface_dimensions);
        r.draw_test(state);
        r.flush();
    });

    r.draw_bg(surface_dimensions);
    r.draw_test(c.get_state());

    auto print_state = [&c]() {
        const Core::State *s = c.get_state();
        std::cout << "version: " << s->version << "\n";
        std::cout << "hypr.active_window: " << s->hypr.active_window << "\n";
        std::cout << "hypr.active_wsid: " << s->hypr.active_wsid << "\n";
        std::cout << "hypr.ws_count: " << s->hypr.ws_count << "\n";
        for (int i = 0; i < s->hypr.ws_count; i++) {
            std::cout << "hypr.wss[" << i << "].id: " << s->hypr.wss[i].id
                      << "\n";
            std::cout << "hypr.wss[" << i << "].name: " << s->hypr.wss[i].name
                      << "\n";
        }
        std::cout << "network.essid: " << s->network.essid << "\n";
        std::cout << "network.dmb: " << s->network.dmb << "\n";
        std::cout << "network.connected: " << s->network.connected << "\n";
        std::cout << std::endl;
    };

    print_state();

    while (wctx.should_dispatch()) {
        c.listen();
    }

    return 0;
}
