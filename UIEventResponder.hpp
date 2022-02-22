#ifndef _EVENTRESPONDER_H
#define _EVENTRESPONDER_H

namespace UI
{
    class EventResponder
    {
    public:
        EventResponder *next;

        virtual void respond();

        virtual ~EventResponder(){};
    };
};

#endif
