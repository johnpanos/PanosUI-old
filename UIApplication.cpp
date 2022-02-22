#define SK_GL 1
#include "UIApplication.hpp"
#include "UIAnimation.hpp"
#include "UIView.hpp"
#include <iostream>
#include <unistd.h>

using namespace UI;

Application::Application()
{
    this->display = wl_display_connect(nullptr);
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
    new_origin.offset(view->layer->x.get(), view->layer->y.get());

    SkRect layer_frame = SkRect::MakeXYWH(view->layer->x.get(), view->layer->y.get(), view->layer->width.get(), view->layer->height.get());

    if (layer_frame != view->frame || view->opacity != view->layer->opacity.get() || view->background_radius != view->layer->background_radius.get() || view->layer->needs_repaint)
    {
        view->layer->needs_repaint = true;
        view->layer->draw();
        view->layer->needs_repaint = false;
    }

    view->layer->backing_surface->draw(this->window->surface->getCanvas(), new_origin.x(), new_origin.y());

    for (UI::View *view : view->children)
    {
        this->render(view, new_origin);
    }
}

void Application::run(Window *window)
{
    this->window = window;
    UI::Animation::Transaction::begin();
    window->delegate->did_finish_launching(window);
    UI::Animation::Transaction::flush();
    while (true)
    {
        wl_display_dispatch(this->display);

        UI::Animation::Transaction::begin();
        if (UI::Animation::AnimationCore::animations.size() > 0)
        {
            window->needsRepaint = true;
            window->surface->getCanvas()->clear(SK_ColorWHITE);

            this->render(window->root_view, SkPoint::Make(window->root_view->frame.x(), window->root_view->frame.y()));

            window->draw();
        }

        UI::Animation::Transaction::flush();

        sleep(.16);
    }
}

Application::~Application()
{
    if (this->display != nullptr)
    {
        wl_display_disconnect(display);
    }
}
