#include "XDGSurface.hpp"
#include <iostream>

using namespace Wayland;

static void
xdg_surface_configure(void *data,
                      struct xdg_surface *xdg_surface, uint32_t serial)
{
    XDGSurface *wayland_xdg_surface = static_cast<XDGSurface *>(data);
    std::cout << "xdg surface configure\n";

    xdg_surface_ack_configure(xdg_surface, serial);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

XDGSurface::XDGSurface(struct xdg_wm_base *wm_base, struct wl_compositor *compositor) : Surface::Surface(compositor)
{
    std::cout << "Created XDGSurface\n";
    assert(wm_base != nullptr);
    assert(compositor != nullptr);
    this->xdg_surface = xdg_wm_base_get_xdg_surface(wm_base, this->wl_surface);
    xdg_surface_add_listener(this->xdg_surface, &xdg_surface_listener, this);
}

XDGSurface::~XDGSurface()
{
    xdg_surface_destroy(this->xdg_surface);
    this->xdg_surface = nullptr;
}