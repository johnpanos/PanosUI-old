#include <iostream>
#include <thread>
#include <queue>
#include "UIApplication.hpp"
#include "UIWindow.hpp"
#include "UIView.hpp"
#include "UIAnimation.hpp"

#include <include/core/SkFont.h>
#include <include/core/SkTextBlob.h>

class Label : public UI::View
{
    using UI::View::View;

    std::string value;
    SkRect text_bounds;
    SkFont font;

public:
    virtual void view_did_load()
    {
        UI::View::view_did_load();

        font = SkFont(nullptr, 48.0f, 1.0f, 0.0f);
        (void)font.measureText(value.c_str(), strlen(value.c_str()), SkTextEncoding::kUTF8, &this->text_bounds);

        std::cout << "X: " << text_bounds.x() << " Y: " << text_bounds.y() << " W: " << text_bounds.width() << " H: " << text_bounds.height() << "\n";

        this->set_frame(SkRect::MakeXYWH(this->frame.x(), this->frame.y(), text_bounds.width(), text_bounds.height()));
    }

    void set_value(std::string str)
    {
        this->value = str;
    }

    virtual void draw(UI::Layer *layer)
    {
        UI::View::draw(layer);

        SkCanvas *canvas = layer->backing_surface->getCanvas();

        SkPaint paint;
        paint.setColor(SK_ColorBLACK);

        font.setSubpixel(true);
        canvas->drawSimpleText(this->value.c_str(), strlen(this->value.c_str()), SkTextEncoding::kUTF8, -text_bounds.x(), -text_bounds.y(), font, paint);
    }
};

class CircleView : public UI::View
{
    using UI::View::View;

    int PADDING = 2;

    int size;
    int center;

    virtual void view_did_load()
    {
        UI::View::view_did_load();

        this->PADDING = (int)((float)this->parent->frame.height() * (4.0f / 51.0f));

        this->size = (int)((float)this->parent->frame.height() * .88f);
        this->center = (this->parent->frame.height() / 2) - size / 2;

        this->background_radius.set(this->parent->frame.height());

        this->value = false;

        this->set_frame(SkRect::MakeXYWH(PADDING, center, size, size));
    }

public:
    bool value;

    void queue_animation()
    {
        auto animation_lambda = [this]()
        {
            if (this->value)
            {
                this->set_frame(SkRect::MakeXYWH(this->parent->frame.width() - size - PADDING, center, size, size));
            }
            else
            {
                this->set_frame(SkRect::MakeXYWH(PADDING, center, size, size));
            }
        };

        UI::View::animate(150, animation_lambda);
    }

    void toggle()
    {
        this->value = !this->value;
        this->queue_animation();
    }
};

class MyView : public UI::View
{
public:
    using UI::View::View;

    UI::View *green_view;
    CircleView *circle_view;

    static const int WIDTH = 51;
    static const int HEIGHT = 31;

    virtual void
    view_did_load()
    {
        UI::View::view_did_load();

        this->set_frame(SkRect::MakeXYWH(0, 0, WIDTH, HEIGHT));

        this->background_radius.set(31);

        // gray
        this->background_color = SkColorSetARGB(255, 120, 120, 128);
        this->opacity.set(40);

        green_view = new UI::View(SkRect::MakeXYWH(0, 0, WIDTH, HEIGHT));
        // green
        green_view->background_radius.set(31);
        green_view->background_color = SkColorSetARGB(255, 52, 199, 89);
        green_view->opacity.set(0);

        circle_view = new CircleView(SkRect::MakeXYWH(0, 0, 0, 0));
        this->add_subview(green_view);
        this->add_subview(circle_view);
    }

    void toggle()
    {
        this->circle_view->toggle();

        auto animation_lambda = [this]()
        {
            if (this->circle_view->value)
            {
                this->green_view->opacity.set(255);
            }
            else
            {
                this->green_view->opacity.set(0);
            }
        };

        UI::View::animate(300, animation_lambda);
    }

    virtual void on_mouse_click()
    {
        std::cout << "new mouse down\n";
        // auto animation_lambda = [this]()
        // {
        //     SkRect new_frame = this->circle_view->frame;
        //     new_frame.setXYWH(new_frame.x(), new_frame.y(), new_frame.width() * 1.1, new_frame.height());
        //     this->circle_view->set_frame(new_frame);
        // };

        // UI::View::animate(150, animation_lambda);
    }

    virtual void on_mouse_up(int x, int y)
    {
        std::cout << "Click from UISwitch!\n";
        if (this->point_inside(SkPoint::Make(x, y)))
        {
            this->toggle();
        }
        // else
        // {
        //     auto animation_lambda = [this]()
        //     {
        //         SkRect new_frame = this->circle_view->frame;
        //         new_frame.setXYWH(new_frame.x(), new_frame.y(), new_frame.height(), new_frame.height());
        //         this->circle_view->set_frame(new_frame);
        //     };

        //     UI::View::animate(150, animation_lambda);
        // }
    }
};

class MyWindowDelegate : public UI::WindowDelegate
{
    virtual void did_finish_launching(UI::Window *window)
    {
        UI::View *root_view = new UI::View(SkRect::MakeXYWH(0, 0, 500, 500));
        MyView *view = new MyView(SkRect::MakeXYWH(0, 0, 50, 50));
        root_view->add_subview(view);
        Label *label = new Label(SkRect::MakeXYWH(0, 100, 100, 100));
        label->set_value("Airplane Mode");
        root_view->add_subview(label);

        window->root_view = root_view;
    }
};

int main()
{
    UI::Application *app = UI::Application::getInstance();
    UI::Window *window = new UI::Window("Hello World", SkRect::MakeXYWH(0, 0, 500, 500));
    window->delegate = new MyWindowDelegate();

    app->run(window);
}