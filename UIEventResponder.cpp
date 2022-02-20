#include "UIEventResponder.hpp"

using namespace UI;

void EventResponder::respond()
{
    if (this->next != nullptr)
    {
        this->next->respond();
    }
}