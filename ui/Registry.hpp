#ifndef _UI_REGISTRY_H
#define _UI_REGISTRY_H
#include <vector>
#include "../wayland/Registry.hpp"
#include "../xdg-shell-client-protocol.h"
#include "../wlr-layer-shell-unstable-v1.h"

namespace UI
{
    class Registry : public Wayland::Registry
    {
    public:
        struct xdg_wm_base *xdg_wm_base;
        struct wl_compositor *wl_compositor;
        struct wl_seat *wl_seat;
        struct wl_shm *wl_shm;
        struct zwlr_layer_shell_v1 *layer_shell;
        std::vector<struct wl_output *> wl_outputs;

        virtual void global(struct wl_registry *wl_registry,
                            uint32_t name, const char *interface, uint32_t version);
    };
};

#endif