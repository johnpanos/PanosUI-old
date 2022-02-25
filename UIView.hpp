#ifndef _UIVIEW_H
#define _UIVIEW_H

#include <vector>
#include <functional>
#include <include/core/SkPaint.h>
#include <include/core/SkCanvas.h>
#include <include/core/SkRect.h>
#include <include/core/SkRRect.h>
#include "UILayer.hpp"
#include "UIEventResponder.hpp"
#include "UIShape.hpp"

namespace UI
{
    class View : public LayerDelegate, public EventResponder
    {
    public:
        static void animate(int64_t duration, std::function<void()> context);

        View(int x, int y, int width, int height);
        View(Shape::Rect frame);
        virtual ~View(){};

        View *parent;
        std::vector<View *> children;

        SkColor background_color;

        Shape::Rect frame;
        Shape::Rect bounds;

        int background_radius;
        int opacity;

        Layer *layer;

        bool loaded;
        bool needs_repaint;
        bool clip_to_bounds;
        bool drop_shadow;

        void set_frame(Shape::Rect frame);
        void set_bounds(Shape::Rect bounds);

        void add_subview(View *view);
        void remove_subview(View *view);

        SkPoint get_position();
        void set_position(SkPoint point);

        View *hit_test(SkPoint point);
        bool point_inside(SkPoint point);

        SkPoint convert(SkPoint point, View *to);

        void set_background_radius(int radius);
        void set_opacity(int opacity);

        void layout_if_needed();
        void set_needs_layout();

        virtual void layout_subviews();
        virtual void view_did_load();
        virtual void draw(Layer *layer);
    };
};

#endif