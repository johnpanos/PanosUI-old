#include "UIEventResponder.hpp"
#include <iostream>

using namespace UI;

void EventResponder::respond()
{
    if (this->next != nullptr)
    {
        this->next->respond();
    }
}

void EventResponder::on_mouse_click()
{
    std::cout << "Not handling mouse click\n";
    if (this->next != nullptr)
    {
        std::cout << "Passing mouse click\n";
        this->next->on_mouse_click();
    }
}

void EventResponder::on_mouse_up(int x, int y)
{
    std::cout << "Not handling mouse up\n";
    if (this->next != nullptr)
    {
        std::cout << "Passing mouse up\n";
        this->next->on_mouse_up(x, y);
    }
}