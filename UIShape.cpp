#include "UIShape.hpp"

using namespace UI::Shape;

AnimatableRect::AnimatableRect(SkRect initial_rect)
{
    this->_x.value = initial_rect.x();
    this->_y.value = initial_rect.y();
    this->_width.value = initial_rect.width();
    this->_height.value = initial_rect.height();
}

int AnimatableRect::x()
{
    return this->_x.get();
}

void AnimatableRect::set_x(int x)
{
    this->_x.set(x);
}

int AnimatableRect::y()
{
    return this->_y.get();
}

void AnimatableRect::set_y(int y)
{
    this->_y.set(y);
}

int AnimatableRect::width()
{
    return this->_width.get();
}

void AnimatableRect::set_width(int width)
{
    this->_width.set(width);
}

int AnimatableRect::height()
{
    return this->_height.get();
}

void AnimatableRect::set_height(int height)
{
    this->_height.set(height);
}