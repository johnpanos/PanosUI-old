#include <iostream>

#include <queue>
#include "UIApplication.hpp"
#include "UIWindow.hpp"
#include "UIView.hpp"

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