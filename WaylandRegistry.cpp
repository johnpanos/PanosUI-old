#include "WaylandRegistry.hpp"
#include <functional>
#include <iostream>
#include <cstring>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"
#include "wlr-layer-shell-unstable-v1.h"

static void
registry_global(void *data, struct wl_registry *wl_registry,
                uint32_t name, const char *interface, uint32_t version)
{
    WaylandRegistry *wayland_registry = (WaylandRegistry *)(data);
    wayland_registry->global(data, wl_registry, name, interface, version);
}

static void
registry_global_remove(void *data,
                       struct wl_registry *wl_registry, uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

WaylandRegistry::WaylandRegistry(struct wl_display *display)
{
    this->display = display;
    this->registry = wl_display_get_registry(this->display);
    wl_registry_add_listener(this->registry, &wl_registry_listener, this);
}

void WaylandRegistry::global(void *data, struct wl_registry *wl_registry,
                             uint32_t name, const char *interface, uint32_t version)
{
    std::cout << interface << "\n";
}

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

void WaylandXDGRegistry::global(void *data, struct wl_registry *wl_registry,
                                uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        this->wl_compositor = static_cast<struct wl_compositor *>(
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4));
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0)
    {
        this->wl_shm = static_cast<struct wl_shm *>(wl_registry_bind(wl_registry, name, &wl_shm_interface, 1));
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0)
    {
        this->wl_seat = static_cast<struct wl_seat *>(wl_registry_bind(wl_registry, name, &wl_seat_interface, 7));
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        this->xdg_wm_base = static_cast<struct xdg_wm_base *>(
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(this->xdg_wm_base,
                                 &xdg_wm_base_listener, this);
    }
    else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0)
    {
        this->zwlr_layer_shell_v1 = static_cast<struct zwlr_layer_shell_v1 *>(wl_registry_bind(registry, name,
                                                                                               &zwlr_layer_shell_v1_interface, version < 4 ? version : 4));
    }
}

WaylandXDG::WaylandXDG(struct wl_display *display)
{
    this->registry = new WaylandXDGRegistry(display);
}

static void
xdg_surface_configure(void *data,
                      struct xdg_surface *xdg_surface, uint32_t serial)
{
    WaylandXDGSurface *wayland_xdg_surface = static_cast<WaylandXDGSurface *>(data);
    wayland_xdg_surface->configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

WaylandXDGSurface::WaylandXDGSurface(WaylandXDGRegistry *registry) : WaylandSurface(registry)
{
    this->xdg_surface = xdg_wm_base_get_xdg_surface(registry->xdg_wm_base, this->wl_surface);
    xdg_surface_add_listener(this->xdg_surface, &xdg_surface_listener, this);
}

static void
xdg_toplevel_configure_handler(void *data,
                               struct xdg_toplevel *xdg_toplevel,
                               int32_t width,
                               int32_t height,
                               struct wl_array *states)
{
    WaylandXDGSurfaceToplevel *toplevel = (WaylandXDGSurfaceToplevel *)data;
    toplevel->configure(xdg_toplevel, width, height, states);
}

static const struct xdg_toplevel_listener xdg_top_level_listener = {
    .configure = xdg_toplevel_configure_handler,
};

WaylandXDGSurfaceToplevel::WaylandXDGSurfaceToplevel(WaylandXDGRegistry *registry) : WaylandXDGSurface(registry)
{
    this->xdg_toplevel = this->get_toplevel();
    xdg_toplevel_add_listener(this->xdg_toplevel, &xdg_top_level_listener, this);
}