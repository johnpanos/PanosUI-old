#ifndef _WAYLANDPOINTER_H
#define _WAYLANDPOINTER_H
#endif

#include <iostream>
#include <vector>
#include <functional>
#include <wayland-client.h>

class WaylandSeatListener
{
public:
    virtual void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps){};
    void handle_name(struct wl_seat *wl_seat, const char *name){};

    virtual ~WaylandSeatListener() = default;
};

class WaylandPointer : public WaylandSeatListener
{
private:
    void motion(int x, int y)
    {
        this->on_motion(x, y);
    }

public:
    std::function<void(int, int)> on_motion;
    struct wl_pointer *wl_pointer;

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
        std::cout << "Name: " << name << "\n";
    }

    void add_listener(WaylandSeatListener *listener)
    {
        std::cout << "Adding listener\n";
        this->listeners.emplace_back(listener);
    }
};
