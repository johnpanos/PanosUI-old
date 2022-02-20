#ifndef _UIVIEW_H
#define _UIVIEW_H

#include <vector>
#include <include/core/SkPaint.h>
#include <include/core/SkCanvas.h>
#include "UILayer.hpp"
#include "UIEventResponder.hpp"

namespace UI
{
    class View : public LayerDelegate, public EventResponder
    {
    public:
        View(SkRect frame);
        virtual ~View(){};

        View *parent;
        std::vector<View *> children;

        SkColor backgroundColor;
        SkRect frame;
        SkRect bounds;

        Layer *layer;

        bool needs_repaint;

        void set_frame(SkRect rect);
        void set_bounds(SkRect rect);

        void add_subview(View *view);
        void remove_subview(View *view);

        SkPoint get_position();
        void set_position(SkPoint point);

        virtual void draw(Layer *layer);
    };
};

#endif