#include <iostream>
#include <thread>
#include <queue>
#include "UIApplication.hpp"
#include "UIWindow.hpp"
#include "UIView.hpp"
#include "UIAnimation.hpp"

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
    UI::Animation::Transaction::begin();
    UI::Application *app = UI::Application::getInstance();
    UI::Window *window = new UI::Window("Hello World", SkRect::MakeXYWH(0, 0, 500, 500));
    UI::View *view1 = new UI::View(SkRect::MakeXYWH(0, 0, 100, 100));
    MyView *view2 = new MyView(SkRect::MakeXYWH(0, 0, 100, 100));

    view2->backgroundColor = SK_ColorBLUE;

    view1->add_subview(view2);

    UI::Animation::Transaction::begin();
    UI::Animation::Transaction::set_duration(1000);
    view2->set_frame(SkRect::MakeXYWH(100, 100, 100, 100));
    UI::Animation::Transaction::commit();

    while (true)
    {
        wl_display_dispatch(app->display);
        UI::Animation::Transaction::begin();

        window->needsRepaint = true;
        std::queue<UI::View *> view_queue;
        view_queue.push(view1);

        window->surface->getCanvas()->clear(SK_ColorBLACK);

        while (!view_queue.empty())
        {
            UI::View *view = view_queue.front();
            view_queue.pop();

            view->layer->draw();
            view->layer->backing_surface->draw(window->surface->getCanvas(), view->layer->x.get(), view->layer->y.get());

            for (UI::View *view : view->children)
            {
                view_queue.push(view);
            }
        }

        UI::Animation::Transaction::flush();
        UI::Animation::AnimationCore::tick();
        window->draw();
    }
}