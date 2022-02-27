#ifndef _UI_REGISTRY_H
#define _UI_REGISTRY_H
#include "../wayland/Registry.hpp"
#include "../xdg-shell-client-protocol.h"

namespace UI
{
    class Registry : public Wayland::Registry
    {
    public:
        struct xdg_wm_base *xdg_wm_base;
        struct wl_compositor *wl_compositor;
        struct wl_seat *wl_seat;
        struct wl_shm *wl_shm;

        virtual void global(struct wl_registry *wl_registry,
                            uint32_t name, const char *interface, uint32_t version);
    };
};

#endif