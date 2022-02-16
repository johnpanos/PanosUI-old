#include "egl.hpp"
#include <iostream>
#include <GL/gl.h>

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

void EGLProvider::create_window(struct wl_display *display, struct wl_surface *surface)
{
    this->egl_window = wl_egl_window_create(surface, 500, 500);

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

    this->egl_display = eglGetPlatformDisplay(EGL_PLATFORM_WAYLAND_EXT, display, NULL);

    EGLint major = 0;
    EGLint minor = 0;
    if (eglInitialize(this->egl_display, &major, &minor) == EGL_FALSE)
    {
        std::cerr << "Failed to initialize EGL\n";
    }
    else
    {
        std::cout << major << " " << minor << ": Initialized EGL\n";
        if (!((major == 1 && minor >= 4) || major >= 2))
        {
            std::cerr << "Too old\n";
            return;
        }
    }

    EGLint matched = 0;
    if (!eglChooseConfig(this->egl_display, config_attributes, &this->egl_config, 1, &matched))
    {
        std::cerr << "eglChooseConfig failed\n";
        return;
    }

    this->egl_context = eglCreateContext(this->egl_display, this->egl_config, EGL_NO_CONTEXT, context_attributes);

    if (this->egl_context == EGL_NO_CONTEXT)
    {
        std::cerr << "Failed to create EGL context\n";
        return;
    }

    this->egl_surface = eglCreateWindowSurface(this->egl_display, this->egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
        return;
    }

    if (eglMakeCurrent(this->egl_display, this->egl_surface, this->egl_surface, egl_context) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Brokey\n";
    }

    glClearColor(10, 10, 10, 0.5F);
    glClear(GL_COLOR_BUFFER_BIT);

    if (eglSwapBuffers(this->egl_display, this->egl_surface) == EGL_FALSE)
    {
    }
}