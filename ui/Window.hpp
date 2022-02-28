#ifndef _UI_WINDOW_H
#define _UI_WINDOW_H
#include "View.hpp"
#include "Application.hpp"
#include "Skia.hpp"
#include "../wayland/Surface.hpp"
#include "../wayland/Pointer.hpp"

namespace UI
{
    class View;
    class Window;

    class WindowDelegate
    {
    public:
        virtual void did_finish_launching(Window *window)
        {
            // std::cout << "Finished launching\n";
        }
    };

    class Window : public Wayland::PointerDelegate
    {
    protected:
        void setup_egl();

    public:
        Wayland::Surface *surface;
        Wayland::Seat *seat;
        Wayland::Pointer *pointer;
        EGLSurface egl_surface;
        wl_egl_window *egl_window;
        Skia skia;

        WindowDelegate *delegate = nullptr;

        bool needs_layout = false;
        bool needs_redraw = false;

        View *root_view = nullptr;
        View *hovered_view = nullptr;

        Window();

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
        virtual void on_mouse_scroll(bool discrete, int delta, bool is_scrolling){};

        virtual void add_root_view(View *view);

        virtual void draw();
        virtual void render(View *view, SkPoint origin);

        virtual int get_width() = 0;
        virtual int set_width(int width) = 0;

        virtual int get_height() = 0;
        virtual int set_height(int height) = 0;

        virtual void on_resize(int width, int height);
        virtual void flush_and_submit();
    };
};

#endif