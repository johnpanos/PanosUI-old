#include "WaylandPointer.hpp"

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

    pointer->on_motion(cur_x, cur_y);
}

static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
                              uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
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