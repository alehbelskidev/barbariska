#pragma once

#include "wlr-layer-shell-unstable-v1-client-protocol.h"

class Surface {
public:
    struct Dimensions {
        int bar_width;
        int bar_height;
    };

private:
    //----
    // SURFACE = canvas. Empty rect recognized by COMPOSITOR.
    // w/o SURFACE there is nothing - no pos, no size, no pixels
    wl_surface *surface;

    // LAYER_SURFACE - extension to surface specific for wlroots.
    // It tells COMPOSITOR that we have layer object instead of regular window
    // nullptr - output (screen), nullptr as arg in our case means COMPOSITOR
    // will pick itself ZWLR_LAYER_SHELL_V1_LAYER_TOP - layer above regular
    // windows, below overlays. "bar" - namespace name, serves as id
    zwlr_layer_surface_v1 *layer_surface;

    // TODO: width comes from configure callback - `layer_surface_configure_cb`
    // for now hardcode
    Dimensions dimensions;

    // COMPOSITOR sends `configure` with real sizes when ready to show
    // `surface`.
    static void layer_surface_configure_cb(void *data,
                                           zwlr_layer_surface_v1 *layer_surface,
                                           uint32_t serial, uint32_t width,
                                           uint32_t height);

    // COMPOSITOR sends `closed` when surface will be removed,
    // .e.g display turned off
    static void layer_surface_closed_cb(void *data,
                                        zwlr_layer_surface_v1 *layer_surface);
    const zwlr_layer_surface_v1_listener layer_surface_listener = {
        .configure = layer_surface_configure_cb,
        .closed = layer_surface_closed_cb,
    };

public:
    Surface(wl_compositor *compositor, zwlr_layer_shell_v1 *layer_shell,
            int default_w, int default_h);

    // Telling SURFACE which buffer to show and commit
    // New pixels will be AVAILABLE for COMPOSER only after commit
    void commit(wl_buffer *buffer);

    Dimensions get_dimensions() const
    {
        return dimensions;
    }
};
