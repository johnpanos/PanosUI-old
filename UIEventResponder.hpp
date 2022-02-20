#ifndef _EVENTRESPONDER_H
#define _EVENTRESPONDER_H

namespace UI
{
    class EventResponder
    {
        EventResponder *next;

        virtual void respond();
    };
};

#endif
