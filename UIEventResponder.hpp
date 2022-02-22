#ifndef _EVENTRESPONDER_H
#define _EVENTRESPONDER_H

namespace UI
{
    class EventResponder
    {
    public:
        EventResponder *next;

        virtual void respond();

        virtual void on_mouse_click();
        virtual void on_mouse_up(int x, int y);

        virtual ~EventResponder(){};
    };
};

#endif
