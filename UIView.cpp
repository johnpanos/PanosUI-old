#include "UIView.hpp"

using namespace UI;

View::View(SkRect frame)
{
    this->frame = frame;
    this->bounds = frame;
    this->backgroundColor = SK_ColorWHITE;
    this->needs_repaint = true;
    this->layer = new Layer();
    this->layer->set_frame(frame);
    this->layer->delegate = this;
}

void View::set_frame(SkRect frame)
{
    this->needs_repaint = true;
    this->frame = frame;
    this->layer->set_frame(frame);
}

void View::set_bounds(SkRect bounds)
{
    this->bounds = bounds;
}

void View::add_subview(View *view)
{
    this->children.push_back(view);
    // view->next = this;
    view->parent = this;
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

        // if (view->next == this)
        // {
        //     view->next = nullptr;
        // }

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

void View::draw(Layer *layer)
{
    SkCanvas *canvas = layer->backing_surface->getCanvas();
    canvas->clear(this->backgroundColor);
}