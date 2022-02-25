#include "Application.hpp"
#include "Registry.hpp"

using namespace UI;

Application *Application::get_instance()
{
    static Application *shared_instance = new Application;
    return shared_instance;
}

Application::Application()
{
    this->display.connect();
    this->registry = new UI::Registry();
    this->display.registry = this->registry;

    this->display.connect();
    this->display.get_registry();
    this->windows = std::vector<Window *>();
}

Application::~Application()
{
    for (Window *window : this->windows)
    {
        delete window;
    }

    this->display.disconnect();
}

void Application::add_window(Window *window)
{
    this->windows.push_back(window);
}

void Application::remove_window(Window *window)
{
    auto it = std::find(this->windows.begin(), this->windows.end(), window);
    if (it != this->windows.end())
    {
        this->windows.erase(it);
    }
}