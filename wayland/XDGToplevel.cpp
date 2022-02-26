#include "XDGToplevel.hpp"
#include <iostream>

using namespace Wayland;

static void
xdg_toplevel_configure_handler(void *data,
                               struct xdg_toplevel *xdg_toplevel,
                               int32_t width,
                               int32_t height,
                               struct wl_array *states)
{
    XDGToplevel *toplevel = (XDGToplevel *)data;
    toplevel->listener->configure(toplevel, width, height);
}

static void xdg_toplevel_close_handler(void *data, struct xdg_toplevel *xdg_toplevel)
{
    XDGToplevel *toplevel = (XDGToplevel *)data;
    toplevel->listener->close();
}

static const struct xdg_toplevel_listener xdg_top_level_listener = {
    .configure = xdg_toplevel_configure_handler,
    .close = xdg_toplevel_close_handler,
};

XDGToplevel::XDGToplevel(struct xdg_wm_base *wm_base, struct wl_compositor *compositor) : XDGSurface(wm_base, compositor)
{
    std::cout << "Created XDGToplevel\n";
    this->xdg_toplevel = xdg_surface_get_toplevel(this->xdg_surface);
    xdg_toplevel_add_listener(this->xdg_toplevel, &xdg_top_level_listener, this);
}

void XDGToplevel::set_title(const char *title)
{
    xdg_toplevel_set_title(this->xdg_toplevel, title);
}

XDGToplevel::~XDGToplevel()
{
    xdg_toplevel_destroy(this->xdg_toplevel);
}