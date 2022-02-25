#ifndef _WAYLAND_REGISTRY_H
#define _WAYLAND_REGISTRY_H
#include <iostream>
#include <wayland-client.h>
#include <cstring>

namespace Wayland
{
    class Registry
    {
    protected:
        struct wl_registry *wl_registry = nullptr;

    public:
        void set_registry(struct wl_registry *registry);
        void remove();

        virtual void global(struct wl_registry *wl_registry,
                            uint32_t name, const char *interface, uint32_t version)
        {
            std::cout << "base " << interface << "\n";
        };
    };

    class CompositorRegistry : public Registry
    {
    private:
        struct wl_compositor *wl_compositor;

    public:
        virtual void global(struct wl_registry *wl_registry,
                            uint32_t name, const char *interface, uint32_t version)
        {
            if (strcmp(interface, wl_compositor_interface.name) == 0)
            {
                this->wl_compositor = static_cast<struct wl_compositor *>(
                    wl_registry_bind(wl_registry, name, &wl_compositor_interface, version));
            }
            else
            {
                Registry::global(wl_registry, name, interface, version);
            }
        }
    };
};

#endif