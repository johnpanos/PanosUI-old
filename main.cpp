#include <iostream>

#include <queue>
#include "UIApplication.hpp"
#include "UIWindow.hpp"
#include "UIView.hpp"

// class EventResponder
// {
// public:
//     EventResponder *next;

//     virtual void respond()
//     {
//         std::cout << "default\n";
//     };
// };

// class UIApplication
// {
// private:
//     UIApplication()
//     {
//         std::cout << "we are here\n";
//         display = wl_display_connect(nullptr);
//         std::cout << "we are also here\n";
//     }

// public:
//     sk_sp<const GrGLInterface> interface;
//     GrDirectContext *context;
//     struct wl_display *display;
//     EGLProvider egl_provider;
//     static UIApplication *getSharedInstance();

//     GrDirectContext *getSkiaContext()
//     {
//         if (this->interface == nullptr && this->context == nullptr)
//         {
//             std::cout << this->interface << " " << this->context << "\n";
//             std::cout << "Allocate skia\n";
//             GrGLGetProc get_proc = [](void *context, const char name[]) -> GrGLFuncPtr
//             {
//                 return eglGetProcAddress(name);
//             };
//             this->interface = GrGLMakeAssembledGLESInterface(this, get_proc);
//             this->context = GrDirectContext::MakeGL(interface).release();
//         }

//         return context;
//     }
// };

// UIApplication *UIApplication::getSharedInstance()
// {
//     static UIApplication *sharedInstance = new UIApplication;
//     return sharedInstance;
// }

// class UILayerDelegate
// {
// public:
//     virtual void draw(SkSurface *surface)
//     {
//     }
// };

// class UILayer
// {

// public:
//     SkSurface *backingSurface;
//     UILayerDelegate *delegate;
//     SkRect frame;

//     UILayer()
//     {
//         this->backingSurface = nullptr;
//     }

//     virtual void render()
//     {
//         delegate->draw(backingSurface);
//     };
// };

// class UIView : public EventResponder, public UILayerDelegate
// {
// public:
//     UIView *parent;
//     UILayer layer;
//     std::vector<UIView *> children{};
//     SkColor backgroundColor;
//     SkRect frame;
//     SkRect bounds;

//     UIView()
//     {
//         this->layer = UILayer();
//         this->layer.delegate = this;
//     }

//     virtual void render()
//     {
//         std::cout << "render\n";
//     };

//     void addSubView(UIView *view)
//     {
//         this->children.push_back(view);
//         view->next = this;
//         view->parent = this;
//     }

//     void removeSubview(UIView *view)
//     {
//         auto it = std::find(this->children.begin(), this->children.end(), view);
//         if (it != this->children.end())
//         {
//             view->parent = nullptr;

//             if (view->next == this)
//             {
//                 view->next = nullptr;
//             }

//             this->children.erase(it);
//         }
//     }

//     void set_frame(SkRect frame)
//     {
//         this->frame = frame;
//         UIApplication *app = UIApplication::getSharedInstance();

//         if (this->layer.backingSurface != nullptr)
//         {
//             this->layer.backingSurface->unref();
//             this->layer.backingSurface = nullptr;
//         }

//         const SkImageInfo info = SkImageInfo::MakeN32(frame.width(), frame.height(), kPremul_SkAlphaType);
//         this->layer.backingSurface = SkSurface::MakeRenderTarget(app->getSkiaContext(), SkBudgeted::kYes, info).release();
//         this->layer.frame = frame;
//     }

//     virtual void draw(SkSurface *surface)
//     {
//         std::cout << "Drawing canvas\n";
//         SkCanvas *canvas = surface->getCanvas();
//         canvas->clear(SK_ColorWHITE);

//         SkPaint paint;
//         paint.setColor(SK_ColorBLUE);
//         paint.setAlpha(255 / 2);
//         canvas->drawCircle(SkPoint::Make(frame.width() / 2, frame.height() / 2), 50, paint);

//         surface->flush();
//     }
// };

class MyView : public UI::View
{
public:
    using UI::View::View;
    virtual void draw(UI::Layer *layer)
    {
        SkCanvas *canvas = layer->backing_surface->getCanvas();

        SkPaint paint;
        paint.setColor(SK_ColorRED);
        paint.setAlpha(255);
        canvas->drawCircle(SkPoint::Make(frame.width() / 2, frame.height() / 2), 50, paint);

        layer->backing_surface->flush();
    }
};

int main()
{
    UI::Application *app = UI::Application::getInstance();
    UI::Window *window = new UI::Window("Hello World", SkRect::MakeXYWH(0, 0, 500, 500));
    UI::View *view1 = new UI::View(SkRect::MakeXYWH(0, 0, 100, 100));

    while (true)
    {
        wl_display_dispatch(app->display);
        window->needsRepaint = true;
        std::queue<UI::View *> view_queue;
        view_queue.push(view1);

        window->surface->getCanvas()->clear(SK_ColorBLACK);

        while (!view_queue.empty())
        {
            UI::View *view = view_queue.front();
            view_queue.pop();

            view->layer->draw();
            view->layer->backing_surface->draw(window->surface->getCanvas(), view->frame.x(), view->frame.y());

            for (UI::View *view : view->children)
            {
                view_queue.push(view);
            }
        }

        window->draw();
    }
}