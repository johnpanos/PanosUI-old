#include "Image.hpp"

using namespace UI;

Image::Image(std::string path) : View(0, 0, 10, 10)
{
    sk_sp<SkData> image_data = SkData::MakeFromFileName(path.c_str());
    this->backing_image = SkImage::MakeFromEncoded(image_data, std::nullopt).release();
    this->background_color = SK_ColorTRANSPARENT;
}

void Image::view_did_load()
{
    View::view_did_load();
    this->set_frame(UI::Shape::Rect(0, 0, this->backing_image->dimensions().width(), this->backing_image->dimensions().height()));
}

void Image::draw(Layer *layer)
{
    View::draw(layer);

    SkCanvas *canvas = layer->backing_surface->getCanvas();
    SkPaint paint;

    paint.setAlpha(layer->opacity.get());
    canvas->drawImage(this->backing_image, 0, 0);
}