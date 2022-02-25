#include "UILayer.hpp"
#include "UIApplication.hpp"
#include <iostream>

using namespace UI;

Layer::Layer()
{
    this->backing_surface = nullptr;
    this->needs_recreate = true;
    this->needs_repaint = true;

    this->background_radius.value = 0;
    this->opacity.value = 255;

    this->frame = Shape::AnimatableRect(SkRect::MakeEmpty());
}

void Layer::set_frame(Shape::Rect frame)
{
    std::cout << "Setting layer frame " << frame.x() << " " << frame.y() << " " << frame.width() << " " << frame.height() << "\n";
    this->needs_recreate = true;
    this->needs_repaint = true;
    this->frame.set(frame);
}

void Layer::set_bounds(Shape::Rect bounds)
{
    std::cout << "Setting bounds " << frame.x() << " " << frame.y() << " " << frame.width() << " " << frame.height() << "\n";
    this->needs_recreate = true;
    this->needs_repaint = true;
    this->bounds.set(bounds);
}

void Layer::draw()
{
    if (delegate != nullptr && this->needs_repaint)
    {
        this->ensure_layer();
        if (this->frame.height() > 1 && this->frame.width() > 1)
        {
            this->delegate->draw(this);
        }
    }
}

void Layer::ensure_layer()
{
    if (this->backing_surface == nullptr ||
        this->needs_recreate ||
        this->backing_surface->width() != this->frame.width() ||
        this->backing_surface->height() != this->frame.height())
    {
        this->destroy();

        UI::Application *app = Application::getInstance();
        const SkImageInfo info = SkImageInfo::MakeN32(frame.width(), frame.height(), kPremul_SkAlphaType);
        this->backing_surface = SkSurface::MakeRenderTarget(app->getSkiaContext(), SkBudgeted::kYes, info).release();

        std::cout << "Create frame " << frame.x() << " " << frame.y() << " " << frame.width() << " " << frame.height() << "\n";

        this->needs_recreate = false;
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