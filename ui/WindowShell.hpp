#ifndef _WAYLAND_WINDOW_SHELL_H
#define _WAYLAND_WINDOW_SHELL_H

#include "../wayland/ShellSurface.hpp"
#include "Window.hpp"

namespace UI
{
    class WindowShell : public Window
    {
    public:
        Wayland::ShellSurface *shell_surface;

        int width;
        int height;

        WindowShell(struct wl_output *wl_output);

        virtual int get_width();
        virtual void set_width(int width);

        virtual int get_height();
        virtual void set_height(int height);
    };
};

#endif