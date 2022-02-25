#ifndef _WAYLAND_DISPLAY_H
#define _WAYLAND_DISPLAY_H
#include <wayland-client.h>
#include <iostream>

#include "Registry.hpp"

namespace Wayland
{
    class Display
    {
    private:
    public:
        struct wl_display *wl_display = nullptr;
        Registry *registry = nullptr;

        Display();
        ~Display();

        void connect();
        void disconnect();
        void get_registry();

        bool round_trip();
    };
};

#endif