#ifndef _UILayer_H
#define _UILayer_H

#include <include/core/SkSurface.h>
#include "Animation.hpp"
#include "Shape.hpp"

namespace UI
{
    class Layer;
    class LayerDelegate
    {
    public:
        LayerDelegate(){};
        virtual ~LayerDelegate(){};
        virtual void draw(Layer *layer){};
    };

    class Layer
    {
    public:
        SkSurface *backing_surface;
        Shape::AnimatableRect frame;
        Shape::AnimatableRect bounds;

        Animation::AnimatableProperty background_radius;
        Animation::AnimatableProperty opacity;

        bool needs_recreate;
        bool needs_repaint;

        LayerDelegate *delegate;
        GrDirectContext *context;

        Layer();
        ~Layer();

        void destroy();

        void set_frame(Shape::Rect frame);
        void set_bounds(Shape::Rect bounds);

        void draw();
        void ensure_layer();
    };

};

#endif