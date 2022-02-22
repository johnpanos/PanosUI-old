#include "UIView.hpp"

using namespace UI;

void View::animate(int64_t duration, std::function<void()> context)
{
    UI::Animation::Transaction::begin();
    UI::Animation::Transaction::set_duration(duration);

    context();

    UI::Animation::Transaction::commit();
}

View::View(SkRect frame)
{
    this->loaded = false;
    this->frame = frame;
    this->bounds = frame;

    this->background_color = SK_ColorWHITE;
    this->opacity.value = 255;
    this->background_radius.value = 0;

    this->needs_repaint = true;
}

void View::set_frame(SkRect frame)
{
    this->needs_repaint = true;
    this->frame = frame;
    this->bounds = frame;
    this->layer->set_frame(frame);
}

void View::set_bounds(SkRect bounds)
{
    this->bounds = bounds;
}

void View::add_subview(View *view)
{
    this->children.push_back(view);
    view->next = this;
    view->parent = this;
    view->view_did_load();
}

void View::remove_subview(View *view)
{
    auto it = std::find(this->children.begin(), this->children.end(), view);
    if (it != this->children.end())
    {
        if (view->parent == this)
        {
            view->parent = nullptr;
        }

        if (view->next == this)
        {
            view->next = nullptr;
        }

        this->children.erase(it);
    }
}

SkPoint View::get_position()
{
    return SkPoint::Make(this->frame.x(), this->frame.y());
}

void View::set_position(SkPoint point)
{
    this->frame = SkRect::MakeXYWH(point.x(), point.y(), this->frame.width(), this->frame.height());
}

View *View::hit_test(SkPoint point)
{
    View *lowest_child = this;
    std::queue<UI::View *> view_queue;
    view_queue.push(this);

    while (!view_queue.empty())
    {
        UI::View *view = view_queue.front();
        view_queue.pop();

        std::cout << "Running hit test on " << view << "\n";

        SkPoint translated_point = this->convert(point, view);

        if (view->point_inside(translated_point))
        {
            if (view->children.empty())
            {
                lowest_child = view;
            }
            else
            {
                for (UI::View *view : view->children)
                {
                    view_queue.push(view);
                }
            }
        }
    }

    return lowest_child;
}

bool View::point_inside(SkPoint point)
{
    std::cout << "Checking if point is inside: " << point.x() << " " << point.y() << "\n";
    std::cout << "Bounds: " << this->bounds.width() << " " << this->bounds.height() << "\n";
    return point.x() >= 0 && point.x() <= this->bounds.width() &&
           point.y() >= 0 && point.y() <= this->bounds.height();
}

SkPoint View::convert(SkPoint point, View *to)
{
    SkPoint translated_point = point;

    View *current_view = to;
    while (current_view != this && current_view != nullptr)
    {
        std::cout << translated_point.x() << " - " << current_view->frame.x() << "\n";
        std::cout << translated_point.y() << " - " << current_view->frame.y() << "\n\n";
        translated_point.set(translated_point.x() - current_view->frame.x(),
                             translated_point.y() - current_view->frame.y());
        current_view = current_view->parent;
    }

    return translated_point;
}

void View::draw(Layer *layer)
{
    SkCanvas *canvas = layer->backing_surface->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);

    if (this->opacity.get() != 0)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(this->background_color);
        paint.setAlpha(this->opacity.value);
        canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(0, 0, layer->width.get(), layer->height.get()), this->background_radius.get(), this->background_radius.get()), paint);
    }
}

void View::view_did_load()
{
    // Create backing layer
    this->layer = new Layer();
    this->layer->set_frame(frame);
    this->layer->delegate = this;

    this->loaded = true;
}