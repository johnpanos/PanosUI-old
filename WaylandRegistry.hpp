#ifndef _WAYLANDREGISTRY_H
#define _WAYLANDREGISTRY_H
#endif

#include <iostream>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "wlr-layer-shell-unstable-v1.h"

class WaylandRegistry
{
private:
    WaylandRegistry();

public:
    struct wl_display *display;
    struct wl_registry *registry;

    WaylandRegistry(struct wl_display *display);

    virtual void global(void *data, struct wl_registry *wl_registry,
                        uint32_t name, const char *interface, uint32_t version);

    void round_trip()
    {
        wl_display_dispatch(this->display);
        wl_display_roundtrip(this->display);
    }
};

class WaylandXDGRegistry : public WaylandRegistry
{
private:
    WaylandXDGRegistry();

public:
    WaylandXDGRegistry(struct wl_display *display) : WaylandRegistry(display){};

    struct wl_compositor *wl_compositor = nullptr;
    struct xdg_wm_base *xdg_wm_base = nullptr;
    struct zwlr_layer_shell_v1 *zwlr_layer_shell_v1 = nullptr;

    virtual void global(void *data, struct wl_registry *wl_registry,
                        uint32_t name, const char *interface, uint32_t version);
};

class WaylandXDG
{
public:
    WaylandXDGRegistry *registry;

    WaylandXDG(struct wl_display *display);
};

class WaylandSurface
{
public:
    struct wl_surface *wl_surface;
    WaylandSurface(WaylandXDGRegistry *registry)
    {
        this->wl_surface = wl_compositor_create_surface(registry->wl_compositor);
    }
};

class WaylandLayerShellSurface
{
public:
    WaylandLayerShellSurface(WaylandXDGRegistry);
};

class WaylandXDGSurface
{
public:
    WaylandSurface *wayland_surface;
    struct xdg_surface *xdg_surface;

    WaylandXDGSurface(WaylandXDGRegistry *registry);

    struct xdg_toplevel *get_toplevel()
    {
        return xdg_surface_get_toplevel(this->xdg_surface);
    }

    void configure(struct xdg_surface *xdg_surface, uint32_t serial)
    {
        xdg_surface_ack_configure(xdg_surface, serial);
    }
};

class WaylandXDGSurfaceToplevel
{
public:
    WaylandXDGSurface *wayland_xdg_surface;
    struct xdg_toplevel *xdg_toplevel;

    int32_t width;
    int32_t height;
    bool resized;

    WaylandXDGSurfaceToplevel(WaylandXDGSurface *surf);

    void set_title(char *str)
    {
        xdg_toplevel_set_title(this->xdg_toplevel, str);
    }

    bool CheckIfWlArrayHasValue(struct wl_array *wl_array, uint32_t value)
    {
        // wl_array_for_each has a bug in upstream. It tries to assign void* to
        // uint32_t *, which is not allowed in C++. Explicit cast should be
        // performed. In other words, one just cannot assign void * to other pointer
        // type implicitly in C++ as in C. We can't modify wayland-util.h, because
        // it is fetched with gclient sync. Thus, use own loop.
        uint32_t *data = reinterpret_cast<uint32_t *>(wl_array->data);
        size_t array_size = wl_array->size / sizeof(uint32_t);
        for (size_t i = 0; i < array_size; i++)
        {
            std::cout << data[i];
            if (data[i] == value)
                return true;
        }
        return false;
    }

    void configure(struct xdg_toplevel *xdg_toplevel,
                   int32_t width,
                   int32_t height,
                   struct wl_array *states)
    {
        printf("XDGTOPLEVEL configure: %dx%d\n", width, height);
        if (CheckIfWlArrayHasValue(states, XDG_TOPLEVEL_SHOW_WINDOW_MENU))
        {
            if (width > 0 && height > 0)
            {
                this->width = width;
                this->height = height;
                this->resized = true;
            }
        }
    }

    void commit()
    {
        wl_surface_commit(this->wayland_xdg_surface->wayland_surface->wl_surface);
    }
};
