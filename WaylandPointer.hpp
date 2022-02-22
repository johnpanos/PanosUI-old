#ifndef _WAYLANDPOINTER_H
#define _WAYLANDPOINTER_H

#include <iostream>
#include <vector>
#include <functional>
#include <wayland-client.h>
#include <wayland-cursor.h>
#include <WaylandRegistry.hpp>

class WaylandSeatListener
{
public:
    virtual void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps){};
    void handle_name(struct wl_seat *wl_seat, const char *name){};

    virtual ~WaylandSeatListener() = default;
};

class WaylandPointer;

class WaylandPointerDelegate
{
public:
    int x, y;
    virtual void on_mouse_motion(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    virtual void on_mouse_click(){};
    virtual void on_mouse_up(){};
};

class WaylandPointer : public WaylandSeatListener
{
public:
    WaylandPointerDelegate *delegate;
    WaylandXDGRegistry *registry;

    WaylandPointer(WaylandXDGRegistry *registry);

    struct wl_pointer *wl_pointer;
    struct wl_surface *cursor_surface;

    virtual void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps);

    void handle_name(struct wl_seat *wl_seat, const char *name)
    {
    }
};

class WaylandSeat
{
    struct wl_seat *wl_seat;
    std::vector<WaylandSeatListener *> listeners;

public:
    WaylandSeat(struct wl_seat *wl_seat);

    void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps)
    {
        for (WaylandSeatListener *listener : this->listeners)
        {
            listener->handle_capabilities(wl_seat, caps);
        }
    }

    void handle_name(struct wl_seat *wl_seat, const char *name)
    {
        // std::cout << "Name: " << name << "\n";
    }

    void add_listener(WaylandSeatListener *listener)
    {
        // std::cout << "Adding listener\n";
        this->listeners.emplace_back(listener);
    }
};

#endif