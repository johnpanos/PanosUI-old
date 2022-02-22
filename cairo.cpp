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
#include "WaylandPointer.hpp"

#include "xdg-shell-client-protocol.h"

// int main(int argc, char *argv[])
// {

//     struct wl_display *display = wl_display_connect(nullptr);
//     if (display == nullptr)
//     {
//         // std::cout << "display is null\n";
//     }

//     WaylandRegistryWrapper<struct wl_display *> wrapper;
//     wrapper.setValue(display);

//     WaylandXDG xdg(display);
//     wl_display_dispatch(display);
//     WaylandSeat seat(xdg.registry->wl_seat);

//     bool mouse_changed = true;
//     int x_g = 0;
//     int y_g = 0;
//     WaylandPointer *pointer = new WaylandPointer;
//     pointer->on_motion = [&x_g, &y_g, &mouse_changed](int x, int y)
//     {
//         // // std::cout << "X: " << x << "Y: " << y << "\n";
//         x_g = x;
//         y_g = y;
//         mouse_changed = true;
//     };
//     seat.add_listener(pointer);

//     WaylandXDGSurfaceToplevel toplevel(xdg.registry);
//     toplevel.width = 500;
//     toplevel.height = 500;

//     toplevel.set_title("Hello World");
//     toplevel.commit();
//     xdg.registry->round_trip();

//     EGLProvider *egl_provider = new EGLProvider;
//     egl_provider->setup(display, toplevel.wl_surface, toplevel.height, toplevel.width);
//     egl_provider->on_resize(toplevel.height, toplevel.width);
//     egl_provider->draw(x_g, y_g);

//     while (true)
//     {
//         wl_display_dispatch(display);

//         if (toplevel.resized)
//         {
//             egl_provider->on_resize(toplevel.width, toplevel.height);
//             egl_provider->draw(x_g, y_g);
//             toplevel.resized = false;
//         }
//         if (mouse_changed)
//         {
//             egl_provider->draw(x_g, y_g);
//             mouse_changed = false;
//         }
//         sleep(.16);
//     }

//     return 0;
// }