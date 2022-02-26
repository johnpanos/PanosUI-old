#include "View.hpp"
#include "Application.hpp"

using namespace UI;

void View::animate(int64_t duration, std::function<void()> context)
{
    UI::Animation::Transaction::begin();
    UI::Animation::Transaction::set_duration(duration);

    context();

    UI::Animation::Transaction::commit();
}

View::View(int x, int y, int width, int height)
{
    std::cout << "Creating view\n";
    UI::Shape::Rect rect(x, y, width, height);
    this->children = std::vector<View *>();

    this->loaded = false;
    this->frame = rect;
    this->bounds = Shape::Rect(SkRect::MakeXYWH(0, 0, frame.width(), frame.height()));

    this->background_color = SK_ColorWHITE;
    this->opacity = 255;
    this->background_radius = 0;

    this->needs_repaint = true;
    this->clip_to_bounds = false;
    this->drop_shadow = false;
}

View::View(Shape::Rect frame)
{
    this->loaded = false;
    this->frame = frame;
    this->bounds = Shape::Rect(SkRect::MakeXYWH(0, 0, frame.width(), frame.height()));

    this->background_color = SK_ColorWHITE;
    this->opacity = 255;
    this->background_radius = 0;

    this->needs_repaint = true;
    this->clip_to_bounds = false;
    this->drop_shadow = false;
}

void View::set_frame(Shape::Rect frame)
{
    std::cout << "Setting frame " << frame.x() << " " << frame.y() << " " << frame.width() << " " << frame.height() << "\n";
    this->needs_repaint = true;
    this->frame = frame;
    this->bounds = UI::Shape::Rect(this->bounds.x(), this->bounds.y(), frame.width(), frame.height());
    this->layer->set_frame(frame);

    this->set_needs_layout();
}

void View::set_bounds(Shape::Rect bounds)
{
    this->bounds = bounds;
    this->layer->set_bounds(bounds);
}

void View::add_subview(View *view)
{
    this->children.push_back(view);
    view->next = this;
    view->parent = this;
    view->window = this->window;
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

        SkPoint translated_point = this->convert(point, view);

        if (view->point_inside(translated_point))
        {
            lowest_child = view;
            if (!view->children.empty())
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
    return point.x() >= 0 && point.x() <= this->bounds.width() &&
           point.y() >= 0 && point.y() <= this->bounds.height();
}

SkPoint View::convert(SkPoint point, View *to)
{
    SkPoint translated_point = point;

    View *current_view = to;
    while (current_view != this && current_view != nullptr)
    {
        // std::cout << translated_point.x() << " - " << current_view->frame.x() << "\n";
        // std::cout << translated_point.y() << " - " << current_view->frame.y() << "\n\n";
        translated_point.set(translated_point.x() - current_view->frame.x() - current_view->parent->bounds.x(),
                             translated_point.y() - current_view->frame.y() - current_view->parent->bounds.y());
        current_view = current_view->parent;
    }

    return translated_point;
}

void View::set_background_radius(int radius)
{
    this->background_radius = radius;
    this->layer->background_radius.set(radius);
}

void View::set_opacity(int opacity)
{
    this->opacity = opacity;
    this->layer->opacity.set(opacity);
}

void View::layout_subviews()
{
    // Implement
}

void View::layout_if_needed()
{
    this->layout_subviews();
    for (View *view : this->children)
    {
        view->layout_if_needed();
    }
}

void View::set_needs_layout()
{
}

void View::view_did_load()
{
    assert(this->window != nullptr);
    // Create backing layer
    this->layer = new Layer(this->frame.x(), this->frame.y(), this->frame.width(), this->frame.height());
    this->layer->needs_recreate = true;
    this->layer->set_bounds(this->bounds);
    this->layer->delegate = this;
    this->layer->context = this->window->skia.get_context();
    this->layer->ensure_layer();

    this->loaded = true;
}

void View::draw(Layer *layer)
{
    std::cout << "Drawing\n";
    SkCanvas *canvas = layer->backing_surface->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);

    if (layer->opacity.get() != 0 && this->background_color != SK_ColorTRANSPARENT)
    {
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setColor(this->background_color);
        paint.setAlpha(layer->opacity.value);
        canvas->drawRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(0, 0, layer->frame.width(), layer->frame.height()), layer->background_radius.get(), layer->background_radius.get()), paint);
    }
}