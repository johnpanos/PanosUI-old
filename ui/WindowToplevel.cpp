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

WindowToplevel::WindowToplevel(const char *title, int width, int height)
{
    std::cout << "Created WindowToplevel " << width << " " << height << "\n";
    Application *app = Application::get_instance();

    this->seat = new Wayland::Seat(app->registry->wl_seat);
    this->pointer = new Wayland::Pointer(app->registry->wl_compositor, app->registry->wl_shm);
    this->pointer->delegate = this;
    this->seat->add_listener(this->pointer);

    this->toplevel = new Wayland::XDGToplevel(app->registry->xdg_wm_base, app->registry->wl_compositor);
    this->toplevel->listener = this;
    this->toplevel->set_title(title);
    this->toplevel->commit();

    app->display.round_trip();

    this->egl_window = wl_egl_window_create(toplevel->wl_surface, width, height);
    this->egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }

    struct wl_callback *cb = wl_surface_frame(toplevel->wl_surface);
    wl_callback_add_listener(cb, &frame_listener, this);

    this->on_resize(width, height);
    this->draw();
}

void WindowToplevel::add_root_view(View *view)
{
    this->root_view = view;

    UI::Animation::Transaction::begin();
    this->root_view->view_did_load();
    this->root_view->layout_if_needed();
    UI::Animation::Transaction::flush();
}

void WindowToplevel::draw()
{
    Application *app = Application::get_instance();
    if (!this->finished_launching)
    {
        if (this->delegate == nullptr)
        {
            if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
            {
                std::cerr << "Failed to make EGL context current\n";
            }
            this->skia.surface->getCanvas()->clear(SK_ColorWHITE);
            this->skia.surface->flushAndSubmit();
            if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
            {
                std::cerr << eglGetError() << " Failed to swap buffers\n";
            }
            return;
        }
        else
        {
            this->delegate->did_finish_launching(this);
            this->finished_launching = true;
            UI::Animation::Transaction::begin();
            if (!root_view->loaded)
            {
                root_view->view_did_load();
            }
            this->root_view->layout_if_needed();
            UI::Animation::Transaction::flush();
        }
    }
    if (true)
    {
        if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
        {
            std::cerr << "Failed to make EGL context current\n";
        }

        if (UI::Animation::AnimationCore::tick())
        {
            // std::cout << "Tick\n";
            this->skia.surface->getCanvas()->clear(SK_ColorWHITE);
            this->render(this->root_view, SkPoint::Make(this->root_view->frame.x(), this->root_view->frame.y()));
            this->skia.surface->flushAndSubmit();

            if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
            {
                std::cerr << eglGetError() << " Failed to swap buffers\n";
            }
        }

        this->needs_redraw = false;
    }
}

void WindowToplevel::render(View *view, SkPoint origin)
{
    if (!view->loaded)
    {
        view->view_did_load();
    }

    SkPoint new_origin = origin;
    new_origin.offset(view->layer->frame.x(), view->layer->frame.y());

    if (view->parent != nullptr)
    {
        new_origin.offset(view->parent->layer->bounds.x(), view->parent->layer->bounds.y());
    }

    SkRect layer_frame = SkRect::MakeXYWH(view->layer->frame.x(), view->layer->frame.y(), view->layer->frame.width(), view->layer->frame.height());

    if (view->frame.width() != view->layer->frame.width() ||
        view->frame.height() != view->layer->frame.height() ||
        view->opacity != view->layer->opacity.get() ||
        view->background_radius != view->layer->background_radius.get() ||
        view->layer->needs_repaint || true)
    {
        view->layer->needs_repaint = true;
        view->layer->draw();
        view->layer->needs_repaint = false;
    }

    SkCanvas *canvas = this->skia.surface->getCanvas();

    // if (view->drop_shadow)
    // {
    //     SkPaint paint;
    //     paint.setColor(SK_ColorBLACK);
    //     paint.setAntiAlias(true);
    //     paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, 5.0f));
    //     canvas->drawRRect(SkRRect::MakeRectXY(layer_frame, view->background_radius, view->background_radius), paint);
    // }

    if (view->clip_to_bounds)
    {
        SkRRect clip_rect = SkRRect::MakeRectXY(layer_frame, view->background_radius, view->background_radius);
        canvas->save();
        canvas->clipRRect(clip_rect, SkClipOp::kIntersect, true);
        view->layer->backing_surface->draw(this->skia.surface->getCanvas(), new_origin.x(), new_origin.y());
        for (UI::View *view : view->children)
        {
            this->render(view, new_origin);
        }
        canvas->restore();
    }
    else
    {
        view->layer->backing_surface->draw(this->skia.surface->getCanvas(), new_origin.x(), new_origin.y());
        for (UI::View *view : view->children)
        {
            this->render(view, new_origin);
        }
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

    Application *app = Application::get_instance();
    if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    this->width = width;
    this->height = height;

    wl_egl_window_resize(this->egl_window, width, height, 0, 0);

    this->skia.setup(width, height);

    this->needs_redraw = true;
    this->draw();
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
    wl_egl_window_destroy(this->egl_window);
    eglDestroySurface(app->egl_provider.egl_display, this->egl_surface);

    delete this->toplevel;
}