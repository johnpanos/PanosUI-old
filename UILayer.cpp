#include "UILayer.hpp"
#include "UIApplication.hpp"
#include <iostream>

using namespace UI;

Layer::Layer()
{
    this->backing_surface = nullptr;
    this->needs_recreate = true;
    this->needs_repaint = true;

    this->opacity.value = 255;
    this->background_radius.value = 0;

    this->frame = Shape::AnimatableRect(SkRect::MakeEmpty());
}

void Layer::set_frame(Shape::Rect frame)
{
    this->frame.set(frame);
}

void Layer::set_bounds(Shape::Rect bounds)
{
    this->bounds.set(bounds);
}

void Layer::draw()
{
    if (delegate != nullptr)
    {
        this->ensure_layer();
        if (this->frame.height() > 1 && this->frame.width() > 1 && this->needs_repaint)
        {
            std::cout << "I am drawing\n";
            this->delegate->draw(this);
            this->needs_repaint = false;
        }
    }
}

void Layer::ensure_layer()
{
    std::cout << (this->backing_surface == nullptr) << "\n";
    if (this->backing_surface == nullptr ||
        this->backing_surface->width() != this->frame.width() ||
        this->backing_surface->height() != this->frame.height() ||
        this->needs_recreate)
    {
        this->destroy();

        UI::Application *app = Application::getInstance();
        const SkImageInfo info = SkImageInfo::MakeN32(frame.width(), frame.height(), kPremul_SkAlphaType);
        this->backing_surface = SkSurface::MakeRenderTarget(app->getSkiaContext(), SkBudgeted::kYes, info).release();

        this->needs_recreate = false;
        this->needs_repaint = true;
    }
}

void Layer::destroy()
{
    if (this->backing_surface != nullptr)
    {
        this->backing_surface->unref();
        this->backing_surface = nullptr;
    }
}

Layer::~Layer()
{
    this->destroy();
}