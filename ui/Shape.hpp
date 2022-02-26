#ifndef _UI_SHAPE_H
#define _UI_SHAPE_H

#include "Animation.hpp"
#include <include/core/SkRect.h>

namespace UI::Shape
{
    class IRect
    {
    public:
        virtual int x(){};
        virtual void set_x(int x){};

        virtual int y(){};
        virtual void set_y(int y){};

        virtual int width(){};
        virtual void set_width(int width){};

        virtual int height(){};
        virtual void set_height(int height){};

        virtual void set(IRect &rect)
        {
            this->set_x(rect.x());
            this->set_y(rect.y());
            this->set_width(rect.width());
            this->set_height(rect.height());
        };

        virtual bool equals(IRect &rect)
        {
            return this->x() == rect.x() &&
                   this->y() == rect.y() &&
                   this->width() == rect.width() &&
                   this->height() == rect.height();
        }
    };

    class Rect : public IRect
    {

    public:
        Rect(){};
        Rect(int x, int y, int width, int height);
        Rect(SkRect initial_rect);

        int _x = 0;
        int _y = 0;
        int _width = 0;
        int _height = 0;

        virtual int x();
        virtual void set_x(int x);

        virtual int y();
        virtual void set_y(int y);

        virtual int width();
        virtual void set_width(int width);

        virtual int height();
        virtual void set_height(int height);
    };

    class AnimatableRect : public IRect
    {
    private:
        UI::Animation::AnimatableProperty _x;
        UI::Animation::AnimatableProperty _y;
        UI::Animation::AnimatableProperty _width;
        UI::Animation::AnimatableProperty _height;

    public:
        AnimatableRect(){};
        AnimatableRect(int x, int y, int width, int height);

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