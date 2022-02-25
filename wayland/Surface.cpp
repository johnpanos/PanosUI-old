#include "Surface.hpp"
#include <iostream>

using namespace Wayland;

Surface::Surface(struct wl_compositor *compositor)
{
    std::cout << "Created Surface\n";
    this->wl_surface = wl_compositor_create_surface(compositor);
}

void Surface::commit()
{
    wl_surface_commit(this->wl_surface);
}

Surface::~Surface()
{
    if (this->wl_surface != nullptr)
    {
        wl_surface_destroy(this->wl_surface);
    }
}