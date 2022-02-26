#ifndef _EVENT_RESPONDER_H
#define _EVENT_RESPONDER_H

#include <include/core/SkPoint.h>

namespace UI
{
    class EventResponder
    {
    public:
        EventResponder *next = nullptr;

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
