#ifndef _UI_WINDOW_TOPLEVEL_H
#define _UI_WINDOW_TOPLEVEL_H

#include "../wayland/XDGToplevel.hpp"
#include "../wayland/Pointer.hpp"
#include <wayland-egl.h>
#include <GL/gl.h>
#include <EGL/egl.h>
#include "Application.hpp"
#include "Window.hpp"

namespace UI
{
    class WindowToplevel : public Window, public Wayland::XDGToplevelListener
    {
    public:
        virtual void on_mouse_motion(int x, int y)
        {
            this->x = x;
            this->y = y;
        }
        virtual void on_mouse_click()
        {
            std::cout << "Click!\n";
        };
        virtual void on_mouse_up(){};
        virtual void on_mouse_scroll(bool up){};

        Wayland::XDGToplevel *toplevel;
        EGLSurface egl_surface;
        wl_egl_window *egl_window;

        WindowToplevel(const char *title, int width, int height);

        bool needs_redraw;

        int width;
        int height;

        Wayland::Seat *seat;
        Wayland::Pointer *pointer;

        virtual void add_root_view(View *view);

        virtual void draw();
        virtual void render(View *view, SkPoint origin);

        virtual int get_width();
        virtual int get_height();

        virtual void on_resize(int width, int height);

        virtual void configure(Wayland::XDGToplevel *toplevel, int width, int height);
        virtual void close();
    };
};

#endif