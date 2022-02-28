#ifndef _WAYLAND_SHELL_SURFACE_H
#define _WAYLAND_SHELL_SURFACE_H

#include "../wlr-layer-shell-unstable-v1.h"
#include "Surface.hpp"
#include <iostream>
#include <assert.h>

namespace Wayland
{
    class ShellSurface : public Surface
    {
    public:
        struct zwlr_layer_surface_v1 *layer_surface;

        ShellSurface(struct zwlr_layer_shell_v1 *layer_shell, struct wl_output *wl_output, struct wl_compositor *compositor);
        ~ShellSurface();
    };
};

#endif