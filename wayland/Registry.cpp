#include "Registry.hpp"

using namespace Wayland;

static void
registry_global(void *data, struct wl_registry *wl_registry,
                uint32_t name, const char *interface, uint32_t version)
{
    Registry *registry = (Registry *)(data);
    registry->global(wl_registry, name, interface, version);
}

static void
registry_global_remove(void *data,
                       struct wl_registry *wl_registry, uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = registry_global_remove,
};

void Registry::set_registry(struct wl_registry *registry)
{
    if (registry == nullptr)
    {
        throw std::runtime_error("Could not get Wayland registry");
    }
    this->wl_registry = registry;
    wl_registry_add_listener(this->wl_registry, &registry_listener, this);
}

void Registry::remove()
{
    std::cout << "removing registry\n";
    if (this->wl_registry != nullptr)
    {
        wl_registry_destroy(this->wl_registry);
    }
}