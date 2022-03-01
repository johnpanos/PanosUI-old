#include "Label.hpp"

using namespace UI;

Label::Label() : View(0, 0, 2, 2)
{
    this->set_font_size(12);
    this->color = SK_ColorBLACK;
    this->background_color = SK_ColorTRANSPARENT;
}

void Label::set_font_size(int size)
{
    SkFontStyle font_style(SkFontStyle::kMedium_Weight, SkFontStyle::kNormal_Width, SkFontStyle::Slant::kUpright_Slant);
    this->font = SkFont(SkTypeface::MakeFromName("Inter", font_style), size);
    this->font.setEdging(SkFont::Edging::kSubpixelAntiAlias);
    this->font.setSubpixel(true);
}

void Label::size_to_fit()
{
    (void)font.measureText(this->contents.c_str(), strlen(this->contents.c_str()), SkTextEncoding::kUTF8, &text_bounds);
    this->set_frame(UI::Shape::Rect(this->frame.x(), this->frame.y(), text_bounds.width(), text_bounds.height()));
}

void Label::set_contents(std::string contents)
{
    this->contents = contents;
}

std::string Label::get_contents()
{
    return this->contents;
}

void Label::draw(Layer *layer)
{
    View::draw(layer);

    SkCanvas *canvas = layer->backing_surface->getCanvas();
    SkPaint paint;

    paint.setColor(this->color);
    paint.setAlpha(layer->opacity.get());

    canvas->drawSimpleText(this->contents.c_str(), strlen(this->contents.c_str()), SkTextEncoding::kUTF8, -text_bounds.x(), -text_bounds.y(), font, paint);
}