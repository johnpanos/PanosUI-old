#define SK_GL 1
#include "UIApplication.hpp"
#include "UIAnimation.hpp"
#include "UIView.hpp"
#include <include/effects/SkBlurMaskFilter.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace UI;

Application::Application()
{
    this->display = wl_display_connect(nullptr);
    this->frame_pending = false;
}

GrDirectContext *Application::getSkiaContext()
{
    if (this->interface == nullptr && this->context == nullptr)
    {
        // std::cout << "making skia context\n";
        GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
        {
            return eglGetProcAddress(name);
        };
        this->interface = GrGLMakeAssembledGLESInterface(this, get_proc);
        this->context = GrDirectContext::MakeGL(interface).release();
    }

    return context;
}

Application *Application::getInstance()
{
    static Application *sharedInstance = new Application;
    return sharedInstance;
}

void Application::render(View *view, SkPoint origin)
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
        view->layer->needs_repaint)
    {
        view->layer->needs_repaint = true;
        view->layer->draw();
        view->layer->needs_repaint = false;
    }

    SkCanvas *canvas = this->window->surface->getCanvas();

    if (view->drop_shadow)
    {
        SkPaint paint;
        paint.setColor(SK_ColorBLACK);
        paint.setAntiAlias(true);
        paint.setMaskFilter(SkMaskFilter::MakeBlur(kNormal_SkBlurStyle, 5.0f));
        canvas->drawRRect(SkRRect::MakeRectXY(layer_frame, view->background_radius, view->background_radius), paint);
    }

    if (view->clip_to_bounds)
    {
        SkRRect clip_rect = SkRRect::MakeRectXY(layer_frame, view->background_radius, view->background_radius);
        canvas->save();
        canvas->clipRRect(clip_rect, SkClipOp::kIntersect, true);
        view->layer->backing_surface->draw(this->window->surface->getCanvas(), new_origin.x(), new_origin.y());
        for (UI::View *view : view->children)
        {
            this->render(view, new_origin);
        }
        canvas->restore();
    }
    else
    {
        view->layer->backing_surface->draw(this->window->surface->getCanvas(), new_origin.x(), new_origin.y());
        for (UI::View *view : view->children)
        {
            this->render(view, new_origin);
        }
    }
}

static void frame_handle_done(void *data, struct wl_callback *callback,
                              uint32_t time)
{
    std::cout << "frame\n";
    UI::Application *app = static_cast<UI::Application *>(data);
    wl_callback_destroy(callback);
    app->frame_pending = false;
    app->render_window();
    wl_surface_damage_buffer(app->window->toplevel->wl_surface, 0, 0, INT32_MAX, INT32_MAX);
}

static const struct wl_callback_listener frame_listener = {
    .done = frame_handle_done,
};

void Application::render_window()
{
    UI::Animation::Transaction::begin();
    window->needsRepaint = true;

    if (this->window->toplevel->resized)
    {
        this->window->toplevel->resized = false;
        this->window->on_resize(this->window->toplevel->width, this->window->toplevel->height);
        if (this->window->root_view != nullptr)
        {
            this->window->root_view->set_frame(SkRect::MakeXYWH(0, 0, this->window->toplevel->width, this->window->toplevel->height));
            this->window->root_view->set_needs_layout();
        }
    }

    if (window->needs_layout)
    {
        window->root_view->layout_if_needed();
        window->needs_layout = false;
    }

    UI::Animation::Transaction::commit();
    UI::Animation::Transaction::flush();

    UI::Animation::AnimationCore::tick();

    if (UI::Animation::AnimationCore::animations.size() > 0)
    {
        window->needsRepaint = true;
        window->surface->getCanvas()->clear(SK_ColorWHITE);

        this->render(window->root_view, SkPoint::Make(window->root_view->frame.x(), window->root_view->frame.y()));

        window->draw();
    }
}

void Application::run(Window *window)
{
    std::cout << "run\n";
    this->window = window;

    UI::Animation::Transaction::begin();
    wl_display_dispatch(this->display);
    window->delegate->did_finish_launching(window);
    window->root_view->view_did_load();
    window->root_view->layout_if_needed();
    window->needsRepaint = true;
    window->draw();
    UI::Animation::Transaction::flush();

    this->render_window();

    while (true)
    {
        wl_display_dispatch(this->display);
        if (!this->frame_pending)
        {
            struct wl_callback *callback = wl_surface_frame(this->window->toplevel->wl_surface);
            wl_callback_add_listener(callback, &frame_listener, this);
            this->frame_pending = true;
        }
    }
}

Application::~Application()
{
    if (this->display != nullptr)
    {
        wl_display_disconnect(display);
    }
}
