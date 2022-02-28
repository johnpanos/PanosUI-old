#include "ui/Application.hpp"
#include "ui/Window.hpp"
#include "ui/WindowToplevel.hpp"
#include "ui/View.hpp"
#include "ui/widget/Label.hpp"

#include "wayland/ShellSurface.hpp"

#include <iostream>
#include <thread>
#include <queue>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <random>

#include <include/core/SkFont.h>
#include <include/core/SkTextBlob.h>

class HoverView : public UI::View
{
    using UI::View::View;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->opacity = 150;
        this->layer->opacity.set(150);
    }

    int x_before = 0;

    virtual void on_mouse_click()
    {
        this->x_before = this->frame.x();
    }

    virtual void on_mouse_up(int x, int y)
    {

        UI::View::animate(250, [this]()
                          {
        UI::Shape::Rect rect = this->frame;
        rect.set_x(this->x_before);
        this->set_frame(rect); });
    }

    virtual void on_mouse_drag(SkPoint delta)
    {
        UI::Shape::Rect rect = this->frame;
        rect.set_x(x_before + delta.x());
        this->set_frame(rect);
    }

    virtual void on_mouse_enter()
    {
        UI::View::animate(250, [this]()
                          { this->set_opacity(255); });
    }

    virtual void on_mouse_exit()
    {
        UI::View::animate(500, [this]()
                          { this->set_opacity(150); });
    }
};

class ShellView : public UI::View
{
    using UI::View::View;

    UI::Label *time_label;
    HoverView *menu_button;
    HoverView *program_view;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->background_color = SkColorSetARGB(230, 47, 53, 69);

        menu_button = new HoverView(UI::Shape::Rect(8, 4, 24, 24));
        this->add_subview(menu_button);
        menu_button->background_color = SkColorSetARGB(255, 150, 150, 150);
        menu_button->set_background_radius(8);

        program_view = new HoverView(UI::Shape::Rect(this->menu_button->frame.width() + this->menu_button->frame.x() + 12, 4, 130, 24));
        this->add_subview(program_view);
        program_view->background_color = SkColorSetARGB(255, 150, 150, 150);
        program_view->set_background_radius(8);

        time_label = new UI::Label();
        time_label->set_contents("5:00 AM");
        time_label->color = SK_ColorWHITE;
        this->add_subview(time_label);
    }

    virtual void layout_subviews()
    {
        auto t = std::time(nullptr);
        auto tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%I:%M %p");
        std::string str = oss.str();

        if (str.front() == '0')
        {
            std::cout << "begins with 0\n";
            str.erase(0, 1);
        }

        this->time_label->set_contents(str);
        this->time_label->size_to_fit();

        int width = this->time_label->frame.width();
        int height = this->time_label->frame.height();
        int x = this->frame.width() - width - 12;
        int y = this->frame.height() / 2 - height / 2;
        this->time_label->set_frame(UI::Shape::Rect(x, y, width, height));
    }
};

class ScrollView : public UI::View
{
    using UI::View::View;

    UI::View *scroll_indicator;

    virtual void view_did_load()
    {
        UI::View::view_did_load();

        this->scroll_indicator = new UI::View(0, 0, 10, 10);
        this->scroll_indicator->background_radius = 16;

        this->clip_to_bounds = true;
        this->drop_shadow = true;
        this->background_radius = 32;

        std::random_device rd;                         // obtain a random number from hardware
        std::mt19937 gen(rd());                        // seed the generator
        std::uniform_int_distribution<> distr(0, 255); // define the range

        for (int i = 0; i < 10; i++)
        {
            int r = distr(gen);
            int b = distr(gen);
            int g = distr(gen);
            UI::View *new_view = new UI::View(0, 0, 10, 10);
            new_view->background_color = SkColorSetRGB(r, g, b);

            this->add_subview(new_view);
        }

        this->add_subview(this->scroll_indicator);
    }

    int prev_y = 0;
    const static int height = 64;

    virtual void layout_subviews()
    {
        for (int i = 0; i < 10; i++)
        {
            UI::View *view = children.at(i);
            view->layer->opacity.set(150);
            view->set_frame(UI::Shape::Rect(0, height * i, this->frame.width(), 64));
        }

        int width = this->frame.width();
        int height = this->frame.height();
        this->scroll_indicator->set_frame(UI::Shape::Rect(width - 16, 0, 16, 64));
    }

    int scroll_end()
    {
        int d = std::min(0, (int)(this->layer->bounds.y()));
        d = std::max(-(int)(height * children.size()) + this->frame.height(), d);
        this->set_bounds(UI::Shape::Rect(0, d, this->bounds.width(), this->bounds.height()));
    }

    virtual void on_mouse_up(int x, int y)
    {
        // UI::View::animate(250, [this]()
        //                   { this->scroll_end(); });
    }

    virtual void on_mouse_drag(SkPoint delta)
    {
        // this->layer->bounds.set_y(this->bounds.y() + delta.y());
    }

    virtual void on_mouse_scroll(bool discrete, int delta, bool is_scrolling)
    {
        if (!discrete)
        {
            if (is_scrolling)
            {
                this->layer->bounds.set_y(this->layer->bounds.y() + (delta / 64));
            }
            else
            {
                std::cout << "Animate scroll\n";
                UI::View::animate(250, [this]()
                                  { this->scroll_end(); });
            }
        }
        else
        {
            int d = (delta < 0) ? 64 : -64;
            this->set_bounds(UI::Shape::Rect(0, this->bounds.y() + d, this->bounds.width(), this->bounds.height()));
        }
    }
};

class RootView : public UI::View
{
    using UI::View::View;

    UI::View *test_view;
    ShellView *shell_view;
    ScrollView *scroll_view;

    UI::Label *label;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->background_color = SkColorSetRGB(33, 33, 33);

        this->label = new UI::Label();
        this->label->set_contents("Edging.");
        this->label->set_font_size(48);
        this->label->color = SK_ColorWHITE;
        this->add_subview(label);

        this->shell_view = new ShellView(0, 0, 10, 10);
        this->add_subview(shell_view);

        this->scroll_view = new ScrollView(0, 0, 100, 100);
        this->add_subview(scroll_view);
    }

    virtual void layout_subviews()
    {
        int width = this->frame.width();
        int height = this->frame.height();
        std::cout << width << "\n";

        this->label->size_to_fit();
        int label_width = this->label->frame.width();
        int label_height = this->label->frame.height();
        int x = width / 2 - label_width / 2;
        int y = 32;
        this->label->set_frame(UI::Shape::Rect(x, y, label_width, label_height));

        this->shell_view->set_frame(UI::Shape::Rect(0, height - 32, width, 32));
        this->scroll_view->set_frame(UI::Shape::Rect(width / 4, height / 4, width / 2, height / 2));
    }
};

class MyWindowDelegate : public UI::WindowDelegate
{
    virtual void did_finish_launching(UI::Window *window)
    {
        window->add_root_view(new RootView(0, 0, 500, 500));
    }
};

int main()
{
    UI::Application *app = UI::Application::get_instance();
    Wayland::ShellSurface *shell_surface = new Wayland::ShellSurface(app->registry->layer_shell, app->registry->wl_outputs.at(0), app->registry->wl_compositor);
    shell_surface->commit();
    app->display.round_trip();

    UI::Skia skia;
    EGLSurface egl_surface;
    wl_egl_window *egl_window;

    egl_window = wl_egl_window_create(shell_surface->wl_surface, 100, 100);
    egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, egl_window, nullptr);

    if (egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }

    if (eglMakeCurrent(app->egl_provider.egl_display, egl_surface, egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    skia.setup(100, 100);

    skia.surface->getCanvas()->clear(SK_ColorWHITE);
    skia.surface->flushAndSubmit();

    if (eglSwapBuffers(app->egl_provider.egl_display, egl_surface) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Failed to swap buffers\n";
    }

    while (app->display.round_trip())
        ;

    // UI::WindowToplevel *window = new UI::WindowToplevel("PanosUI", 500, 500);
    // window->delegate = new MyWindowDelegate();

    // app->add_window(window);
    // app->run();
}