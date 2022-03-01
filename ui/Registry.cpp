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
        this->wl_compositor = static_cast<struct wl_compositor *>(
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, version));
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        this->xdg_wm_base = static_cast<struct xdg_wm_base *>(
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(this->xdg_wm_base,
                                 &xdg_wm_base_listener, this);
    }
    else if (strcmp(interface, wl_seat_interface.name) == 0)
    {
        std::cout << "Found seat\n";
        this->wl_seat = static_cast<struct wl_seat *>(wl_registry_bind(wl_registry, name, &wl_seat_interface, 7));
    }
    else if (strcmp(interface, wl_shm_interface.name) == 0)
    {
        this->wl_shm = static_cast<struct wl_shm *>(
            wl_registry_bind(wl_registry, name, &wl_shm_interface, 1));
    }
    else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0)
    {
        this->layer_shell = static_cast<struct zwlr_layer_shell_v1 *>(wl_registry_bind(wl_registry, name,
                                                                                       &zwlr_layer_shell_v1_interface, version < 4 ? version : 4));
    }
    else if (strcmp(interface, "wl_output") == 0)
    {
        std::cout << "Output found\n";
        this->wl_outputs.emplace_back(static_cast<struct wl_output *>(wl_registry_bind(wl_registry, name,
                                                                                       &wl_output_interface, 4)));
    }
}