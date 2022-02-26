#ifndef _UI_WINDOW_TOPLEVEL_H
#define _UI_WINDOW_TOPLEVEL_H

#include "../wayland/XDGToplevel.hpp"
#include <wayland-egl.h>
#include <GL/gl.h>
#include <EGL/egl.h>
#include "Application.hpp"
#include "Window.hpp"
#include "Skia.hpp"

namespace UI
{
    class WindowToplevel : public Window, public Wayland::XDGToplevelListener
    {
    public:
        Wayland::XDGToplevel *toplevel;
        EGLSurface egl_surface;
        wl_egl_window *egl_window;

        Skia skia;

        WindowToplevel(const char *title, int width, int height);

        bool needs_redraw;

        int width;
        int height;

        virtual void draw();

        virtual int get_width();
        virtual int get_height();

        virtual void on_resize(int width, int height);

        virtual void configure(Wayland::XDGToplevel *toplevel, int width, int height);
        virtual void close();
    };
};

#endif