#ifndef _UILayer_H
#define _UILayer_H

#include <include/core/SkSurface.h>

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
        SkRect frame;
        SkRect bounds;

        bool needs_recreate;
        bool needs_repaint;

        LayerDelegate *delegate;

        Layer();
        ~Layer();

        void destroy();

        void set_frame(SkRect frame);

        void draw();
        void ensure_layer();
    };

};

#endif