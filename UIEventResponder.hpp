#ifndef _EVENTRESPONDER_H
#define _EVENTRESPONDER_H

#include <include/core/SkPoint.h>

namespace UI
{
    class EventResponder
    {
    public:
        EventResponder *next;

        virtual void respond();

        virtual void on_mouse_click();
        virtual void on_mouse_up(int x, int y);

        virtual void on_mouse_enter();
        virtual void on_mouse_exit();

        virtual void on_mouse_drag(SkPoint delta);
        virtual void on_mouse_scroll(int delta);

        virtual ~EventResponder(){};
    };
};

#endif
