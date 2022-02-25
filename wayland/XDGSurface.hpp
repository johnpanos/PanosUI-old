#ifndef _WAYLAND_XDG_SURFACE_H
#define _WAYLAND_XDG_SURFACE_H

#include <wayland-client.h>
#include <assert.h>
#include "../xdg-shell-client-protocol.h"
#include "Surface.hpp"

namespace Wayland
{
    class XDGSurface : public Surface
    {
    public:
        struct xdg_surface *xdg_surface;

        XDGSurface(struct xdg_wm_base *wm_base, struct wl_compositor *compositor);
        ~XDGSurface();
    };
};

#endif