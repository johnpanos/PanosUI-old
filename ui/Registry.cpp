#include "Registry.hpp"
#include <iostream>

using namespace UI;

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    std::cout << "ping\n";
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

void Registry::global(struct wl_registry *wl_registry,
                      uint32_t name, const char *interface, uint32_t version)
{
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        std::cout << "compositor\n";
        this->wl_compositor = static_cast<struct wl_compositor *>(
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, version));
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        std::cout << "wm_base\n";
        this->xdg_wm_base = static_cast<struct xdg_wm_base *>(
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(this->xdg_wm_base,
                                 &xdg_wm_base_listener, this);
    }
}