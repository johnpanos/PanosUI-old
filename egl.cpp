#include "egl.hpp"
#include <iostream>

const EGLint config_attributes[] = {
    EGL_SURFACE_TYPE,
    EGL_WINDOW_BIT,
    EGL_RED_SIZE,
    8,
    EGL_GREEN_SIZE,
    8,
    EGL_BLUE_SIZE,
    8,
    EGL_ALPHA_SIZE,
    8,
    EGL_RENDERABLE_TYPE,
    EGL_OPENGL_ES2_BIT,
    EGL_NONE,
};

const EGLint context_attributes[] = {
    EGL_CONTEXT_CLIENT_VERSION,
    2,
    EGL_NONE,
};

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

    EGLint matched = 0;
    if (!eglChooseConfig(this->egl_display, config_attributes,
                         (EGLConfig *)&this->egl_config, 1, &matched))
    {
        std::cerr << "eglChooseConfig failed\n";
        return;
    }

    this->egl_context = (EGLContext *)(eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, context_attributes));

    if (this->egl_context == EGL_NO_CONTEXT)
    {
        std::cerr << "Failed to create EGL context\n";
    }
}