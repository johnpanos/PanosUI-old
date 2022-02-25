#ifndef _WAYLAND_TOPLEVEL_H
#define _WAYLAND_TOPLEVEL_H

#include "XDGSurface.hpp"
#include "../xdg-shell-client-protocol.h"

namespace Wayland
{
    class XDGToplevel;

    class XDGToplevelListener
    {
    public:
        virtual void configure(XDGToplevel *toplevel, int width, int height) = 0;
    };

    class XDGToplevel : public XDGSurface
    {
    public:
        struct xdg_toplevel *xdg_toplevel;
        XDGToplevelListener *listener;

        XDGToplevel(struct xdg_wm_base *wm_base, struct wl_compositor *compositor);

        void set_title(const char *title);
    };
};

#endif