#include "Wayland.hpp"

using namespace Wayland;

Display::Display()
{
}

Display::~Display()
{
    std::cout << "removing display\n";
    if (this->wl_display != nullptr)
    {
        this->disconnect();
    }
}

void Display::connect()
{
    this->wl_display = wl_display_connect(nullptr);
    if (this->wl_display == nullptr)
    {
        throw std::runtime_error("Could not connect to Wayland display");
    }
}

void Display::disconnect()
{
    if (this->registry != nullptr)
    {
        this->registry->remove();
        delete this->registry;
    }

    wl_display_flush(this->wl_display);
    wl_display_disconnect(this->wl_display);
    this->wl_display = nullptr;
}

void Display::get_registry()
{
    this->registry->set_registry(wl_display_get_registry(this->wl_display));
    wl_display_dispatch(this->wl_display);
}

bool Display::round_trip()
{
    wl_display_dispatch(this->wl_display);
    return wl_display_roundtrip(this->wl_display);
}