#include "WindowToplevel.hpp"

using namespace UI;

static void frame_handle_done(void *data, struct wl_callback *callback,
                              uint32_t time);

static const struct wl_callback_listener frame_listener = {
    .done = frame_handle_done,
};

static void frame_handle_done(void *data, struct wl_callback *callback,
                              uint32_t time)
{
    struct wl_surface *surface = (struct wl_surface *)data;
    std::cout << "frame\n";

    struct wl_callback *cb = wl_surface_frame(surface);
    wl_callback_add_listener(cb, &frame_listener, surface);
}

void WindowToplevel::configure(Wayland::XDGToplevel *toplevel, int width, int height)
{
    if (width > 0 && height > 0)
    {
        this->on_resize(width, height);
    }
}

WindowToplevel::WindowToplevel(int width, int height) : width(width), height(height)
{
    std::cout << "Created WindowToplevel " << width << " " << height << "\n";
    Application *app = Application::get_instance();
    this->toplevel = new Wayland::XDGToplevel(app->registry->xdg_wm_base, app->registry->wl_compositor);
    this->toplevel->listener = this;
    this->toplevel->set_title("Hello World");

    struct wl_callback *cb = wl_surface_frame(toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, toplevel->wl_surface);

    this->toplevel->commit();

    this->egl_window = wl_egl_window_create(toplevel->wl_surface, width, height);
    app->egl_provider.setup(app->display.wl_display);
    this->egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }

    if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    glClearColor(255, 255, 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Failed to swap buffers\n";
    }
}

int WindowToplevel::get_width()
{
    return 0;
}

int WindowToplevel::get_height()
{
    return 0;
}

void WindowToplevel::on_resize(int width, int height)
{
    if (this->width == width && this->height == height)
        return;

    this->width = width;
    this->height = height;
    wl_egl_window_resize(this->egl_window, width, height, 0, 0);

    Application *app = Application::get_instance();

    glClearColor(255, 255, 255, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Failed to swap buffers\n";
    }
}