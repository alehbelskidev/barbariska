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
    // Core::Client c([&r, &surface_dimensions](Core::State *state) {
    //     r.draw_bg(surface_dimensions);
    //     r.draw_test(state);
    //     r.flush();
    // });

    r.draw_bg(surface_dimensions);
    // r.draw_test(c.get_state());

    while (wctx.should_dispatch()) {
        // c.listen();
    }

    return 0;
}
