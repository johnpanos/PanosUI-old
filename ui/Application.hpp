#ifndef _UI_APPLICATION_H
#define _UI_APPLICATION_H

#include <vector>
#include <functional>
#include "../egl.hpp"
#include "../wayland/Wayland.hpp"
#include "Window.hpp"
#include "Registry.hpp"

namespace UI
{
    class Application
    {
        std::vector<Window *> windows;

    public:
        Wayland::Display display;
        UI::Registry *registry;
        EGLProvider egl_provider;

        static Application *get_instance();
        Application();
        ~Application();

        void add_window(Window *window);
        void remove_window(Window *window);

        void run();
    };
};

#endif