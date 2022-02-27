#ifndef _WAYLAND_POINTER_H
#define _WAYLAND_POINTER_H

#include <iostream>
#include <vector>
#include <functional>
#include <wayland-client.h>
#include <wayland-cursor.h>

namespace Wayland
{
    class SeatListener
    {
    public:
        virtual void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps){};
        void handle_name(struct wl_seat *wl_seat, const char *name){};

        virtual ~SeatListener() = default;
    };

    class Pointer;

    class PointerDelegate
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
        virtual void on_mouse_scroll(bool up){};
    };

    class Pointer : public SeatListener
    {
    public:
        PointerDelegate *delegate;
        struct wl_compositor *wl_compositor;
        struct wl_shm *wl_shm;

        Pointer(struct wl_compositor *wl_compositor, struct wl_shm *wl_shm);

        struct wl_pointer *wl_pointer;
        struct wl_surface *cursor_surface;

        virtual void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps);

        void handle_name(struct wl_seat *wl_seat, const char *name)
        {
        }
    };

    class Seat
    {
        struct wl_seat *wl_seat;
        std::vector<SeatListener *> listeners;

    public:
        Seat(struct wl_seat *wl_seat);

        void handle_capabilities(struct wl_seat *wl_seat, uint32_t caps)
        {
            for (SeatListener *listener : this->listeners)
            {
                listener->handle_capabilities(wl_seat, caps);
            }
        }

        void handle_name(struct wl_seat *wl_seat, const char *name)
        {
            // std::cout << "Name: " << name << "\n";
        }

        void add_listener(SeatListener *listener)
        {
            // std::cout << "Adding listener\n";
            this->listeners.emplace_back(listener);
        }
    };
};

#endif