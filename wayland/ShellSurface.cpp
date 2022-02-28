#include "ShellSurface.hpp"

using namespace Wayland;

static void layer_surface_configure(void *data,
                                    struct zwlr_layer_surface_v1 *surface,
                                    uint32_t serial, uint32_t w, uint32_t h)
{
    zwlr_layer_surface_v1_ack_configure(surface, serial);
}

static void layer_surface_closed(void *data,
                                 struct zwlr_layer_surface_v1 *surface)
{
    // eglDestroySurface(egl_display, egl_surface);
    // wl_egl_window_destroy(egl_window);
    zwlr_layer_surface_v1_destroy(surface);
    // wl_surface_destroy(wl_surface);
    // run_display = false;
}

struct zwlr_layer_surface_v1_listener layer_surface_listener = {
    .configure = layer_surface_configure,
    .closed = layer_surface_closed,
};

ShellSurface::ShellSurface(struct zwlr_layer_shell_v1 *layer_shell, struct wl_output *wl_output, struct wl_compositor *compositor) : Surface::Surface(compositor)
{
    std::cout << "Created ShellSurface\n";
    assert(layer_shell != nullptr);
    assert(wl_output != nullptr);
    assert(compositor != nullptr);
    this->layer_surface = zwlr_layer_shell_v1_get_layer_surface(layer_shell, this->wl_surface, wl_output, ZWLR_LAYER_SHELL_V1_LAYER_BOTTOM, "PanosUI2");
    zwlr_layer_surface_v1_set_size(layer_surface, 100, 100);
    zwlr_layer_surface_v1_set_anchor(layer_surface, ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, 72);
    zwlr_layer_surface_v1_set_margin(layer_surface,
                                     0, 0, 0, 0);
    zwlr_layer_surface_v1_set_keyboard_interactivity(
        layer_surface, ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_NONE);

    zwlr_layer_surface_v1_add_listener(this->layer_surface, &layer_surface_listener, this);
}

ShellSurface::~ShellSurface()
{
    zwlr_layer_surface_v1_destroy(this->layer_surface);
    this->layer_surface = nullptr;
}