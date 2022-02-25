#ifndef _UI_WINDOW_H
#define _UI_WINDOW_H

namespace UI
{
    class Window
    {
        virtual int get_width() = 0;
        virtual int get_height() = 0;

        virtual void on_resize(int width, int height) = 0;
    };
};

#endif