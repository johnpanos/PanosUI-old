#include "WindowShell.hpp"

using namespace UI;

static void output_geometry(void *data, struct wl_output *output, int x, int y,
                            int physical_width, int physical_height,
                            int subpixel, const char *make, const char *model,
                            int transform)
{
    (void)data;
    (void)output;
    (void)x;
    (void)y;
    (void)physical_width;
    (void)physical_height;
    (void)subpixel;
    (void)make;
    (void)model;
    (void)transform;
}
static void output_mode(void *data, struct wl_output *output, uint32_t flags,
                        int width, int height, int refresh)
{
    (void)data;
    (void)output;
    (void)flags;
    (void)width;
    (void)height;
    (void)refresh;
    std::cout << "width: " << width << "\n";
    std::cout << "height: " << height << "\n";
    std::cout << "refresh: " << refresh << "\n";
    WindowShell *window = static_cast<WindowShell *>(data);
    window->set_width(width);
    window->set_height(72);
}
static void output_done(void *data, struct wl_output *output)
{
    (void)data;
    (void)output;
}
static void output_scale(void *data, struct wl_output *wl_output,
                         int32_t scale_factor)
{
    std::cout << "Scale factor: " << scale_factor << "\n";
}

static const struct wl_output_listener output_listener = {
    .geometry = output_geometry,
    .mode = output_mode,
    .done = output_done,
    .scale = output_scale};

WindowShell::WindowShell(struct wl_output *wl_output) : Window()
{
    Application *app = Application::get_instance();

    this->width = 100;
    this->height = 100;

    this->shell_surface = new Wayland::ShellSurface(app->registry->layer_shell, wl_output, app->registry->wl_compositor);
    this->surface = this->shell_surface;
    this->shell_surface->commit();

    wl_output_add_listener(wl_output, &output_listener, this);

    app->display.round_trip();

    this->setup_egl();

    this->on_resize(this->get_width(), this->get_height());
    this->flush_and_submit();
}

void WindowShell::set_width(int width)
{
    this->width = width;
}

int WindowShell::get_width()
{
    return width;
}

void WindowShell::set_height(int height)
{
    this->height = height;
}

int WindowShell::get_height()
{
    return height;
}