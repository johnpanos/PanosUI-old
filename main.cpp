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

public:
    SkFont font;
    SkColor color;

    void size_to_fit()
    {
        (void)font.measureText(value.c_str(), strlen(value.c_str()), SkTextEncoding::kUTF8, &this->text_bounds);
        this->set_frame(SkRect::MakeXYWH(this->frame.x(), this->frame.y(), text_bounds.width(), text_bounds.height()));
    }

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->background_color = SK_ColorTRANSPARENT;
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

        paint.setColor(this->color);
        paint.setAlpha(layer->opacity.get());

        font.setSubpixel(true);
        canvas->drawSimpleText(this->value.c_str(), strlen(this->value.c_str()), SkTextEncoding::kUTF8, -text_bounds.x(), -text_bounds.y(), font, paint);
    }
};

class Button : public UI::View
{
    using UI::View::View;

public:
    Label *label;
    std::string text;
    int padding_horizontal = 0;
    int padding_vertical = 0;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        label = new Label(SkRect::MakeEmpty());
        label->set_value(this->text);
        label->font = SkFont(nullptr, 18);

        this->add_subview(label);

        label->size_to_fit();
        this->set_frame(SkRect::MakeXYWH(this->frame.x(), this->frame.y(), label->frame.width() + padding_horizontal * 2, label->frame.height() + padding_vertical * 2));
        label->set_frame(label->frame.makeOffset(padding_horizontal, padding_vertical));
    }

    virtual void on_mouse_click()
    {
        UI::View::animate(50, [this]()
                          { this->label->opacity = 150; this->label->layer->opacity.set(150); });
    }

    virtual void on_mouse_up(int x, int y)
    {
        UI::View::animate(100, [this]()
                          { this->label->opacity = 255; this->label->layer->opacity.set(255); });
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

        this->layer->background_radius.set(this->parent->frame.height());

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

        this->layer->background_radius.set(31);

        // gray
        this->background_color = SkColorSetARGB(255, 120, 120, 128);
        this->opacity = 40;
        this->layer->opacity.set(40);

        green_view = new UI::View(SkRect::MakeXYWH(0, 0, WIDTH, HEIGHT));
        this->add_subview(green_view);
        // green
        green_view->layer->background_radius.set(31);
        green_view->background_color = SkColorSetARGB(255, 52, 199, 89);
        green_view->opacity = 0;
        green_view->layer->opacity.set(0);

        circle_view = new CircleView(SkRect::MakeXYWH(0, 0, 0, 0));

        this->add_subview(circle_view);
    }

    void toggle()
    {
        this->circle_view->toggle();

        auto animation_lambda = [this]()
        {
            if (this->circle_view->value)
            {
                this->opacity = 255;
                this->green_view->layer->opacity.set(255);
            }
            else
            {
                this->opacity = 0;
                this->green_view->layer->opacity.set(0);
            }
        };

        UI::View::animate(300, animation_lambda);
    }

    virtual void on_mouse_click()
    {
        // std::cout << "new mouse down\n";
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
        // std::cout << "Click from UISwitch!\n";
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

class HoverView : public UI::View
{
    using UI::View::View;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->opacity = 150;
        this->layer->opacity.set(150);
    }

    virtual void on_mouse_enter()
    {
        UI::View::animate(250, [this]()
                          { this->opacity = 255; this->layer->opacity.set(255); });
    }

    virtual void on_mouse_exit()
    {
        UI::View::animate(500, [this]()
                          { this->opacity = 150; this->layer->opacity.set(150); });
    }
};

class ShellView : public UI::View
{
    using UI::View::View;

    Label *time_label;
    HoverView *menu_button;
    HoverView *program_view;

    virtual void view_did_load()
    {
        UI::View::view_did_load();
        this->background_color = SkColorSetARGB(230, 47, 53, 69);

        menu_button = new HoverView(SkRect::MakeXYWH(8, 4, 24, 24));
        this->add_subview(menu_button);
        menu_button->background_color = SkColorSetARGB(255, 150, 150, 150);
        menu_button->background_radius = 8;
        menu_button->layer->background_radius.set(8);

        program_view = new HoverView(SkRect::MakeXYWH(this->menu_button->frame.width() + this->menu_button->frame.x() + 12, 4, 130, 24));
        this->add_subview(program_view);
        program_view->background_color = SkColorSetARGB(255, 150, 150, 150);
        program_view->background_radius = 8;
        program_view->layer->background_radius.set(8);

        time_label = new Label(SkRect::MakeEmpty());
        time_label->set_value("5:00 AM");
        time_label->font = SkFont(nullptr, 11);
        this->add_subview(time_label);
        time_label->size_to_fit();
        time_label->color = SK_ColorWHITE;

        this->time_label->set_frame(this->time_label->frame.makeOffset(this->frame.width() - this->time_label->frame.width() - 12, this->frame.height() / 2 - this->time_label->frame.height() / 2));
    }
};

class MyWindowDelegate : public UI::WindowDelegate
{
    virtual void did_finish_launching(UI::Window *window)
    {
        UI::View *root_view = new UI::View(SkRect::MakeXYWH(0, 0, 1916, 506));
        MyView *view = new MyView(SkRect::MakeXYWH(0, 0, 50, 50));
        root_view->add_subview(view);

        Button *button = new Button(SkRect::MakeXYWH(0, view->frame.height(), 0, 0));
        button->text = "Click Me";
        button->padding_horizontal = 16;
        button->padding_vertical = 8;
        root_view->add_subview(button);

        button->label->color = SkColorSetRGB(10, 132, 255);

        ShellView *shell_view = new ShellView(SkRect::MakeXYWH(0, root_view->frame.height() - 32, root_view->frame.width(), 32));
        shell_view->background_color = SK_ColorBLACK;
        root_view->add_subview(shell_view);

        // button->background_color = SK_ColorLTGRAY;
        // button->layer->background_radius.set(8);

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