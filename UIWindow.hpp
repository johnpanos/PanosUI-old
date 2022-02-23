#ifndef _UIWINDOW_H
#define _UIWINDOW_H

#include <wayland-egl.h>
#include <EGL/egl.h>
#include <include/core/SkSurface.h>
#include "WaylandRegistry.hpp"
#include "WaylandPointer.hpp"
#include "UIView.hpp"

namespace UI
{
    class Window;

    class WindowDelegate
    {
    public:
        virtual void did_finish_launching(Window *window)
        {
            // std::cout << "Finished launching\n";
        }
    };

    class Window : public WaylandPointerDelegate
    {
    public:
        WindowDelegate *delegate;

        EGLSurface egl_surface;
        SkSurface *surface;
        wl_egl_window *egl_window;

        WaylandXDG *xdg;
        WaylandXDGSurfaceToplevel *toplevel;

        WaylandSeat *seat;
        WaylandPointer *pointer;

        SkRect frame;

        bool needsRepaint;

        bool needs_layout;

        View *root_view;

        View *hovered_view;
        View *clicked_view;
        SkPoint global_clicked_at;
        SkPoint local_clicked_at;

        Window(const char *title, SkRect frame);
        ~Window();

        void on_resize(int width, int height);
        void draw();
        void run();

        virtual void on_mouse_motion(int x, int y);
        virtual void on_mouse_click();
        virtual void on_mouse_up();
    };
};

#endif