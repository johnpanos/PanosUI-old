#ifndef _UI_LABEL_H
#define _UI_LABEL_H

#include "../View.hpp"
#include <include/core/SkFont.h>
#include <include/core/SkTextBlob.h>

namespace UI
{
    class Label : public View
    {
        std::string contents;
        SkRect text_bounds;
        SkFont font;

    public:
        SkColor color;

        Label();

        void set_font_size(int size);
        void size_to_fit();
        void set_contents(std::string contents);
        std::string get_contents();

        virtual void draw(Layer *layer);
    };
};

#endif