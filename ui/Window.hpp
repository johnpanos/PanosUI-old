#ifndef _UI_WINDOW_H
#define _UI_WINDOW_H
#include "View.hpp"
#include "Skia.hpp"

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

    class Window
    {
    public:
        WindowDelegate *delegate = nullptr;
        Skia skia;

        View *root_view = nullptr;

        virtual void
        draw() = 0;

        virtual int get_width() = 0;
        virtual int get_height() = 0;

        virtual void on_resize(int width, int height) = 0;
    };
};

#endif