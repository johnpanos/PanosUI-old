#include <linux/input-event-codes.h>
#include "WaylandPointer.hpp"

WaylandPointer::WaylandPointer(WaylandXDGRegistry *registry)
{
    this->cursor_surface = wl_compositor_create_surface(registry->wl_compositor);
    this->registry = registry;
}

static void seat_handle_capabilities(void *data, struct wl_seat *wl_seat, uint32_t caps)
{
    WaylandSeat *wayland_seat = static_cast<WaylandSeat *>(data);
    wayland_seat->handle_capabilities(wl_seat, caps);
}

static void seat_handle_name(void *data, struct wl_seat *wl_seat, const char *name)
{
    WaylandSeat *wayland_seat = static_cast<WaylandSeat *>(data);
    wayland_seat->handle_name(wl_seat, name);
}

const struct wl_seat_listener seat_listener = {
    .capabilities = seat_handle_capabilities,
    .name = seat_handle_name,
};

WaylandSeat::WaylandSeat(struct wl_seat *wl_seat)
{
    this->wl_seat = wl_seat;
    wl_seat_add_listener(this->wl_seat, &seat_listener, this);
}

static void wl_pointer_enter(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface,
                             wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    WaylandPointer *pointer = static_cast<WaylandPointer *>(data);
    struct wl_cursor_theme *cursor_theme = wl_cursor_theme_load(NULL, 16, pointer->registry->wl_shm);
    struct wl_cursor_image *image = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr")->images[0];
    ;
    wl_surface_attach(pointer->cursor_surface,
                      wl_cursor_image_get_buffer(image), 0, 0);
    wl_surface_damage(pointer->cursor_surface, 1, 0,
                      image->width, image->height);
    wl_surface_commit(pointer->cursor_surface);
    wl_pointer_set_cursor(wl_pointer, serial, pointer->cursor_surface,
                          image->hotspot_x, image->hotspot_y);
}

static void wl_pointer_leave(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface)
{
}

static void wl_pointer_motion(void *data, struct wl_pointer *wl_pointer,
                              uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    WaylandPointer *pointer = static_cast<WaylandPointer *>(data);
    int cur_x = wl_fixed_to_int(surface_x);
    int cur_y = wl_fixed_to_int(surface_y);

    pointer->delegate->on_mouse_motion(cur_x, cur_y);
}

static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
                              uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
    WaylandPointer *pointer = static_cast<WaylandPointer *>(data);
    if (button == BTN_LEFT && state == 1)
    {
        pointer->delegate->on_mouse_click();
    }
    else if (button == BTN_LEFT && state == 0)
    {
        pointer->delegate->on_mouse_up();
    }
}

static void wl_pointer_axis(void *data, struct wl_pointer *wl_pointer,
                            uint32_t time, uint32_t axis, wl_fixed_t value)
{
    // Who cares
}

static void wl_pointer_frame(void *data, struct wl_pointer *wl_pointer)
{
    // Who cares
}

static void wl_pointer_axis_source(void *data, struct wl_pointer *wl_pointer,
                                   uint32_t axis_source)
{
    // Who cares
}

static void wl_pointer_axis_stop(void *data, struct wl_pointer *wl_pointer,
                                 uint32_t time, uint32_t axis)
{
    // Who cares
}

static void wl_pointer_axis_discrete(void *data, struct wl_pointer *wl_pointer,
                                     uint32_t axis, int32_t discrete)
{
    // Who cares
}

struct wl_pointer_listener pointer_listener = {
    .enter = wl_pointer_enter,
    .leave = wl_pointer_leave,
    .motion = wl_pointer_motion,
    .button = wl_pointer_button,
    .axis = wl_pointer_axis,
    .frame = wl_pointer_frame,
    .axis_source = wl_pointer_axis_source,
    .axis_stop = wl_pointer_axis_stop,
    .axis_discrete = wl_pointer_axis_discrete,
};

void WaylandPointer::handle_capabilities(struct wl_seat *wl_seat, uint32_t caps)
{
    if (caps & WL_SEAT_CAPABILITY_POINTER)
    {
        std::cout << "Has pointer\n";
        this->wl_pointer = wl_seat_get_pointer(wl_seat);
        wl_pointer_add_listener(this->wl_pointer, &pointer_listener, this);
    }
    else if (this->wl_pointer != nullptr)
    {
        wl_pointer_release(this->wl_pointer);
        this->wl_pointer = nullptr;
    }
}