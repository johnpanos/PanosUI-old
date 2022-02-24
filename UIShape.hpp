#ifndef _UISHAPE_H
#define _UISHAPE_H

#include "UIAnimation.hpp"
#include <include/core/SkRect.h>

namespace UI::Shape
{
    class IRect
    {
    public:
        virtual int x() = 0;
        virtual void set_x(int x) = 0;

        virtual int y() = 0;
        virtual void set_y(int y) = 0;

        virtual int width() = 0;
        virtual void set_width(int width) = 0;

        virtual int height() = 0;
        virtual void set_height(int height) = 0;
    };

    class AnimatableRect : public IRect
    {
    private:
        UI::Animation::AnimatableProperty _x;
        UI::Animation::AnimatableProperty _y;
        UI::Animation::AnimatableProperty _width;
        UI::Animation::AnimatableProperty _height;

    public:
        AnimatableRect(SkRect initial_rect);

        virtual int x();
        virtual void set_x(int x);

        virtual int y();
        virtual void set_y(int y);

        virtual int width();
        virtual void set_width(int width);

        virtual int height();
        virtual void set_height(int height);
    };
};

#endif