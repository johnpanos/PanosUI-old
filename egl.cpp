#include "egl.hpp"
#include <iostream>

void EGLProvider::create_window(struct wl_display *display)
{
    const char *client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if (client_extensions == nullptr)
    {
        if (eglGetError() == EGL_BAD_DISPLAY)
        {
            std::cerr << "EGL_EXT_client_extensions not supported\n";
        }
        else
        {
            std::cerr << "Failed to query EGL client extensions\n";
        }
    }

    // TODO: Query EGL client extensions for EGL_PLATFORM_WAYLAND_EXT instead of just using it

    this->egl_display = static_cast<EGLDisplay *>(eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_EXT, display, NULL));

    if (eglInitialize(this->egl_display, NULL, NULL) == EGL_FALSE)
    {
        std::cerr << "Failed to initialize EGL\n";
    }
    else
    {
        std::cout << "Initialized EGL\n";
    }
}