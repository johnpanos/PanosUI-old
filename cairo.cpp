#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include "xdg-shell-client-protocol.h"

/* Wayland code */
struct client_state
{
    /* Globals */
    struct wl_display *wl_display;
    struct wl_registry *wl_registry;
    struct wl_shm *wl_shm;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_wm_base;
    /* Objects */
    struct wl_surface *wl_surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
};

static void
wl_buffer_release(void *data, struct wl_buffer *wl_buffer)
{
    /* Sent by the compositor when it's no longer using this buffer */
    wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
    .release = wl_buffer_release,
};

static void
xdg_surface_configure(void *data,
                      struct xdg_surface *xdg_surface, uint32_t serial)
{
    struct client_state *state = static_cast<struct client_state *>(data);
    xdg_surface_ack_configure(xdg_surface, serial);

    // struct wl_buffer *buffer = draw_frame(state);
    // wl_surface_attach(state->wl_surface, buffer, 0, 0);
    wl_surface_commit(state->wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure,
};

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping,
};

class RegistryProvider
{
public:
    struct wl_registry *wl_registry;
    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_wm_base;

    void setup();
};

static void
registry_global(void *data, struct wl_registry *wl_registry,
                uint32_t name, const char *interface, uint32_t version)
{
    RegistryProvider *provider = static_cast<RegistryProvider *>(data);
    if (strcmp(interface, wl_compositor_interface.name) == 0)
    {
        provider->wl_compositor = static_cast<wl_compositor *>(
            wl_registry_bind(wl_registry, name, &wl_compositor_interface, 4));
    }
    else if (strcmp(interface, xdg_wm_base_interface.name) == 0)
    {
        provider->xdg_wm_base = static_cast<xdg_wm_base *>(
            wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1));
        xdg_wm_base_add_listener(provider->xdg_wm_base,
                                 &xdg_wm_base_listener, provider);
    }
}

static void
registry_global_remove(void *data,
                       struct wl_registry *wl_registry, uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener wl_registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

void RegistryProvider::setup()
{
    {
        this->wl_display = wl_display_connect(NULL);

        if (this->wl_display == NULL)
        {
            std::cout << "Failed to connect to Wayland display\n";
            exit(1);
        }
        std::cout << "Found display\n";

        this->wl_registry = wl_display_get_registry(this->wl_display);
        wl_registry_add_listener(this->wl_registry, &wl_registry_listener, static_cast<void *>(this));

        std::cout << "Listening to registry\n";
        wl_display_dispatch(this->wl_display);
        wl_display_roundtrip(this->wl_display);

        if (this->wl_compositor == nullptr || this->xdg_wm_base == nullptr)
        {
            std::cerr << "Could not find compositor or xdg_wm_base\n";
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Start\n";
    RegistryProvider *provider = new RegistryProvider;
    provider->setup();

    // state.wl_surface = wl_compositor_create_surface(state.wl_compositor);

    // if (state.wl_surface == NULL)
    // {
    //     std::cerr << "Could not create surface\n";
    //     return 1;
    // }

    // state.xdg_surface = xdg_wm_base_get_xdg_surface(state.xdg_wm_base, state.wl_surface);
    // xdg_surface_add_listener(state.xdg_surface, &xdg_surface_listener, NULL);

    // cleanup_wayland(&state);
    return 0;
}