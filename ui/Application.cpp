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
    this->surface_to_window[window->surface->wl_surface] = window;
    window->delegate->did_finish_launching(window);
}

void Application::remove_window(Window *window)
{
    auto it = std::find(this->windows.begin(), this->windows.end(), window);
    if (it != this->windows.end())
    {
        this->surface_to_window.erase(window->surface->wl_surface);
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