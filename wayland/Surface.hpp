#ifndef _WAYLAND_SURFACE_H
#define _WAYLAND_SURFACE_H
#include <wayland-client.h>

namespace Wayland
{
    class Surface
    {
    protected:
    public:
        struct wl_surface *wl_surface;
        Surface(struct wl_compositor *compositor);
        ~Surface();

        void commit();
    };
};

#endif