#ifndef _UI_WINDOW_H
#define _UI_WINDOW_H
#include "View.hpp"
#include "Skia.hpp"
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
        virtual void on_mouse_scroll(bool discrete, int delta, bool is_scrolling){};

        WindowDelegate *delegate = nullptr;
        Skia skia;

        bool finished_launching = false;
        bool needs_layout = false;

        View *root_view = nullptr;
        View *hovered_view = nullptr;

        virtual void add_root_view(View *view) = 0;

        virtual void draw() = 0;
        virtual void render(View *view, SkPoint origin) = 0;

        virtual int get_width() = 0;
        virtual int get_height() = 0;

        virtual void on_resize(int width, int height) = 0;
    };
};

#endif