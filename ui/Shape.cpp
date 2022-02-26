#include "Shape.hpp"

using namespace UI::Shape;

Rect::Rect(int x, int y, int width, int height)
{
    this->_x = x;
    this->_y = y;
    this->_width = width;
    this->_height = height;
}

Rect::Rect(SkRect initial_rect)
{
    this->_x = initial_rect.x();
    this->_y = initial_rect.y();
    this->_width = initial_rect.width();
    this->_height = initial_rect.height();
}

int Rect::x()
{
    return this->_x;
}

void Rect::set_x(int x)
{
    this->_x = x;
}

int Rect::y()
{
    return this->_y;
}

void Rect::set_y(int y)
{
    this->_y = y;
}

int Rect::width()
{
    return this->_width;
}

void Rect::set_width(int width)
{
    this->_width = width;
}

int Rect::height()
{
    return this->_height;
}

void Rect::set_height(int height)
{
    this->_height = height;
}

AnimatableRect::AnimatableRect(int x, int y, int width, int height)
{
    this->_x.value = x;
    this->_y.value = y;
    this->_width.value = width;
    this->_height.value = height;
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