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

    this->egl_provider.setup(this->display.wl_display);
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
    std::cout << "remove window\n";
    auto it = std::find(this->windows.begin(), this->windows.end(), window);
    if (it != this->windows.end())
    {
        this->windows.erase(it);
    }
}

void Application::run()
{
    while (this->display.round_trip())
    {
        if (this->windows.empty())
            break;
        for (Window *window : this->windows)
        {
            window->draw();
        }
    }
}