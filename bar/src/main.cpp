#include <cairo/cairo.h>

#include "renderer.hpp"
#include "shm-buffer.hpp"
#include "surface.hpp"
#include "wayland-context.hpp"

int main()
{
    WaylandContext wctx;
    Surface surface(wctx.get_compositor(), wctx.get_layer_shell(), 1920, 30);

    wctx.roundtrip();

    auto surface_dimensions = surface.get_dimensions();
    ShmBuffer shm_buffer(surface_dimensions, wctx.get_shm());
    Renderer r(
        surface_dimensions, shm_buffer.get_shm_data(), shm_buffer.get_stride(),
        [&surface, &shm_buffer]() { surface.commit(shm_buffer.get_buffer()); });

    while (wctx.should_dispatch()) {
    }

    return 0;
}
