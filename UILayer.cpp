#include "UILayer.hpp"
#include "UIApplication.hpp"
#include <iostream>

using namespace UI;

Layer::Layer()
{
    this->backing_surface = nullptr;
    this->needs_recreate = true;
    this->needs_repaint = true;
}

void Layer::set_frame(SkRect frame)
{
    this->needs_recreate = true;
    this->needs_repaint = true;
    this->frame = frame;
    this->x.set(frame.x());
    this->y.set(frame.y());
}

void Layer::draw()
{
    if (delegate != nullptr && this->needs_repaint)
    {
        this->ensure_layer();
        this->delegate->draw(this);
    }
}

void Layer::ensure_layer()
{
    if (this->needs_recreate)
    {
        this->destroy();

        UI::Application *app = Application::getInstance();
        const SkImageInfo info = SkImageInfo::MakeN32(frame.width(), frame.height(), kPremul_SkAlphaType);
        this->backing_surface = SkSurface::MakeRenderTarget(app->getSkiaContext(), SkBudgeted::kYes, info).release();

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