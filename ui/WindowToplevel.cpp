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
    WindowToplevel *window = (WindowToplevel *)data;
    // std::cout << "frame\n";

    window->needs_redraw = true;

    struct wl_callback *cb = wl_surface_frame(window->toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, window);
}

void WindowToplevel::configure(Wayland::XDGToplevel *toplevel, int width, int height)
{
    if (width > 0 && height > 0)
    {
        this->on_resize(width, height);
    }
}

void WindowToplevel::close()
{
    Application *app = Application::get_instance();
    app->remove_window(this);
    delete this->toplevel;
}

WindowToplevel::WindowToplevel(const char *title, int width, int height) : width(width), height(height)
{
    std::cout << "Created WindowToplevel " << width << " " << height << "\n";
    Application *app = Application::get_instance();
    this->toplevel = new Wayland::XDGToplevel(app->registry->xdg_wm_base, app->registry->wl_compositor);
    this->toplevel->listener = this;
    this->toplevel->set_title(title);

    struct wl_callback *cb = wl_surface_frame(toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, this);

    this->toplevel->commit();
    app->display.round_trip();

    this->egl_window = wl_egl_window_create(toplevel->wl_surface, width, height);

    this->egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }

    this->needs_redraw = true;
    this->draw();
}

void WindowToplevel::draw()
{
    if (this->needs_redraw)
    {
        Application *app = Application::get_instance();

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

        this->needs_redraw = false;
    }
}

int WindowToplevel::get_width()
{
    return this->width;
}

int WindowToplevel::get_height()
{
    return this->height;
}

void WindowToplevel::on_resize(int width, int height)
{
    if (this->width == width && this->height == height)
        return;

    this->width = width;
    this->height = height;

    wl_egl_window_resize(this->egl_window, width, height, 0, 0);

    this->needs_redraw = true;
    this->draw();
}