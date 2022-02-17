#include <iostream>
#include <errno.h>
#include <stdbool.h>
#include <cstring>
#include <unistd.h>
#include <GLES2/gl2.h>
#include <wayland-client.h>
#include <wayland-egl.h>

#include <EGL/egl.h>

#include <egl.hpp>

#include "WaylandRegistry.hpp"

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
    struct xdg_toplevel *toplevel;
};

class RegistryProvider
{
public:
    struct wl_registry *wl_registry;
    struct wl_display *wl_display;
    struct wl_compositor *wl_compositor;
    struct xdg_wm_base *xdg_wm_base;

    struct wl_surface *wl_surface;
    struct xdg_surface *xdg_surface;

    struct xdg_toplevel *toplevel;

    int32_t width;
    int32_t height;

    void setup();
    void setup_toplevel();
};

static void
xdg_surface_configure(void *data,
                      struct xdg_surface *xdg_surface, uint32_t serial)
{
    RegistryProvider *provider = static_cast<RegistryProvider *>(data);
    xdg_surface_ack_configure(xdg_surface, serial);

    // struct wl_buffer *buffer = draw_frame(state);
    // wl_surface_attach(state->wl_surface, buffer, 0, 0);
    provider->setup_toplevel();
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

static void
xdg_toplevel_configure_handler(void *data,
                               struct xdg_toplevel *xdg_toplevel,
                               int32_t width,
                               int32_t height,
                               struct wl_array *states)
{
    printf("configure: %dx%d\n", width, height);
    // RegistryProvider *provider = static_cast<RegistryProvider *>(data);
    // if (width > 0 && height > 0)
    // {
    //     provider->width = width;
    //     provider->height = height;
    // }
}

static const struct xdg_toplevel_listener xdg_top_level_listener = {
    .configure = xdg_toplevel_configure_handler,
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

        this->wl_surface = wl_compositor_create_surface(this->wl_compositor);
        this->xdg_surface = xdg_wm_base_get_xdg_surface(this->xdg_wm_base, this->wl_surface);

        this->toplevel = xdg_surface_get_toplevel(this->xdg_surface);
        xdg_toplevel_set_title(this->toplevel, "Hello World");

        xdg_surface_add_listener(this->xdg_surface, &xdg_surface_listener, this);
        xdg_toplevel_add_listener(this->toplevel, &xdg_top_level_listener, this);

        wl_surface_commit(this->wl_surface);
        wl_display_roundtrip(this->wl_display);
    }
}

void RegistryProvider::setup_toplevel()
{
    EGLProvider *egl_provider = new EGLProvider;
    egl_provider->create_window(this->width, this->height, this->wl_display, this->wl_surface);
}

int main(int argc, char *argv[])
{
    struct wl_display *display = wl_display_connect(nullptr);
    if (display == nullptr)
    {
        std::cout << "display is null\n";
    }
    WaylandXDG xdg(display);
    xdg.registry->round_trip();

    if (xdg.registry->zwlr_layer_shell_v1 == nullptr)
    {
        std::cerr << "did not load layer_shell\n";
        return -1;
    }

    WaylandXDGSurface xdg_surf(xdg.registry);
    WaylandXDGSurfaceToplevel toplevel(&xdg_surf);

    toplevel.width = 500;
    toplevel.height = 500;

    toplevel.set_title("Hello World");
    toplevel.commit();
    xdg.registry->round_trip();

    EGLProvider *egl_provider = new EGLProvider;
    egl_provider->create_window(toplevel.width, toplevel.height, xdg.registry->display, toplevel.wayland_xdg_surface->wayland_surface->wl_surface);

    while (true)
    {
        wl_display_dispatch(display);
        if (toplevel.resized)
        {
            egl_provider->create_window(toplevel.width, toplevel.height, xdg.registry->display, toplevel.wayland_xdg_surface->wayland_surface->wl_surface);
            toplevel.resized = false;
        }
    }

    return 0;
}