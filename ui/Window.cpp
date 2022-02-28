#include "Window.hpp"

using namespace UI;

Window::Window()
{
    Application *app = Application::get_instance();

    this->seat = new Wayland::Seat(app->registry->wl_seat);
    this->pointer = new Wayland::Pointer(app->registry->wl_compositor, app->registry->wl_shm);
    this->pointer->delegate = this;
    this->seat->add_listener(this->pointer);
}

void Window::setup_egl()
{
    Application *app = Application::get_instance();

    this->egl_window = wl_egl_window_create(this->surface->wl_surface, this->get_width(), this->get_height());
    this->egl_surface = eglCreateWindowSurface(app->egl_provider.egl_display, app->egl_provider.egl_config, this->egl_window, nullptr);

    if (this->egl_surface == EGL_NO_SURFACE)
    {
        std::cerr << "Failed to create EGL window surface\n";
    }
}

void Window::add_root_view(View *view)
{
    this->root_view = view;
    this->root_view->window = this;

    UI::Animation::Transaction::begin();
    this->root_view->view_did_load();
    this->root_view->layout_if_needed();
    UI::Animation::Transaction::flush();
}

void Window::draw()
{
    Application *app = Application::get_instance();

    if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    if (this->root_view != nullptr)
    {
        UI::Animation::Transaction::begin();

        if (this->needs_layout)
        {
            this->root_view->layout_if_needed();
            this->needs_layout = false;
        }

        UI::Animation::Transaction::flush();
        if (UI::Animation::AnimationCore::tick())
        {
            // std::cout << "Tick\n";
            this->render(this->root_view, SkPoint::Make(this->root_view->frame.x(), this->root_view->frame.y()));
            this->flush_and_submit();
        }
    }
}

void Window::render(View *view, SkPoint origin)
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

void Window::on_resize(int width, int height)
{
    if (this->get_width() == width && this->get_height() == height)
        return;

    Application *app = Application::get_instance();
    if (eglMakeCurrent(app->egl_provider.egl_display, this->egl_surface, this->egl_surface, app->egl_provider.egl_context) == EGL_FALSE)
    {
        std::cerr << "Failed to make EGL context current\n";
    }

    this->set_width(width);
    this->set_height(height);

    wl_egl_window_resize(this->egl_window, width, height, 0, 0);

    if (this->root_view != nullptr)
    {
        UI::Animation::Transaction::begin();
        this->root_view->set_frame(UI::Shape::Rect(0, 0, width, height));
        UI::Animation::Transaction::flush();
    }

    this->skia.setup(width, height);

    this->needs_redraw = true;
}

void Window::flush_and_submit()
{
    Application *app = Application::get_instance();
    this->skia.surface->flushAndSubmit();
    if (eglSwapBuffers(app->egl_provider.egl_display, this->egl_surface) == EGL_FALSE)
    {
        std::cerr << eglGetError() << " Failed to swap buffers\n";
    }
}