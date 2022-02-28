#ifndef _UI_IMAGE_H
#define _UI_IMAGE_H

#include <optional>
#include "include/core/SkImage.h"
#include "../View.hpp"

namespace UI
{
    class Image : public View
    {
    public:
        SkImage *backing_image;
        Image(std::string path);

        virtual void view_did_load();
        virtual void draw(Layer *layer);
    };
};

#endif