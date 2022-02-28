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
    window->needs_redraw = true;
    struct wl_callback *cb = wl_surface_frame(window->toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, window);
}

WindowToplevel::WindowToplevel(const char *title, int width, int height) : Window()
{
    std::cout << "Created WindowToplevel " << width << " " << height << "\n";
    Application *app = Application::get_instance();

    this->width = width;
    this->height = height;

    this->toplevel = new Wayland::XDGToplevel(app->registry->xdg_wm_base, app->registry->wl_compositor);
    this->toplevel->listener = this;
    this->toplevel->set_title(title);
    this->toplevel->commit();

    this->surface = this->toplevel;
    app->display.round_trip();
    this->setup_egl();

    struct wl_callback *cb = wl_surface_frame(toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, this);

    this->on_resize(width, height);

    this->skia.surface->getCanvas()->clear(SK_ColorWHITE);
    this->flush_and_submit();
}

void WindowToplevel::on_mouse_motion(int x, int y)
{
    UI::View *new_hovered_view = this->root_view->hit_test(SkPoint::Make(x, y));
    if (new_hovered_view != nullptr)
    {
        if (new_hovered_view != hovered_view)
        {
            if (hovered_view != nullptr)
            {
                hovered_view->on_mouse_exit();
            }
            new_hovered_view->on_mouse_enter();
            hovered_view = new_hovered_view;
        }
    }
}

void WindowToplevel::on_mouse_scroll(bool discrete, int delta, bool is_scrolling)
{
    std::cout << "window\n";
    std::cout << "discrete " << discrete << "\n";
    std::cout << "delta " << delta << "\n";
    std::cout << "is_scrolling " << is_scrolling << "\n";
    if (hovered_view != nullptr)
    {
        UI::Animation::Transaction::begin();
        hovered_view->on_mouse_scroll(discrete, delta, is_scrolling);
    }
    else
    {
        // std::cout << "No view found";
    }
}

void WindowToplevel::configure(Wayland::XDGToplevel *toplevel, int width, int height)
{
    if (width > 0 && height > 0)
    {
        this->on_resize(width, height);
    }
}

void WindowToplevel::set_width(int width)
{
    this->width = width;
}

int WindowToplevel::get_width()
{
    return width;
}

void WindowToplevel::set_height(int height)
{
    this->height = height;
}

int WindowToplevel::get_height()
{
    return height;
}

void WindowToplevel::close()
{
    Application *app = Application::get_instance();

    app->remove_window(this);
    wl_egl_window_destroy(this->egl_window);
    eglDestroySurface(app->egl_provider.egl_display, this->egl_surface);

    delete this->toplevel;
}