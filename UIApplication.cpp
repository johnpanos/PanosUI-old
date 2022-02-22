#define SK_GL 1
#include "UIApplication.hpp"
#include "UIAnimation.hpp"
#include "UIView.hpp"
#include <iostream>

using namespace UI;

Application::Application()
{
    this->display = wl_display_connect(nullptr);
}

GrDirectContext *Application::getSkiaContext()
{
    if (this->interface == nullptr && this->context == nullptr)
    {
        std::cout << "making skia context\n";
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
        window->needsRepaint = true;
        std::queue<UI::View *> view_queue;
        view_queue.push(window->root_view);

        window->surface->getCanvas()->clear(SK_ColorWHITE);

        while (!view_queue.empty())
        {
            UI::View *view = view_queue.front();
            view_queue.pop();
            if (!view->loaded)
            {
                view->view_did_load();
            }

            view->needs_repaint = true;

            view->layer->draw();

            int global_x = 0;
            int global_y = 0;

            UI::View *parent = view->parent;
            while (parent != nullptr)
            {
                global_x += parent->frame.x();
                global_y += parent->frame.y();
                parent = parent->parent;
            }

            view->layer->backing_surface->draw(window->surface->getCanvas(), global_x + view->layer->x.get(), global_y + view->layer->y.get());

            for (UI::View *view : view->children)
            {
                view_queue.push(view);
            }
        }

        window->draw();
        UI::Animation::Transaction::flush();
    }
}

Application::~Application()
{
    if (this->display != nullptr)
    {
        wl_display_disconnect(display);
    }
}
