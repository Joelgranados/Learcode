/* standard headers */
#include <iostream>
#include <iomanip>
#include <sstream>
#include <lear/image/painter.h>
#include <lear/util/fileutil.h>
#include <lear/exception.h>

using namespace lear;

Painter::Painter(const std::string& imagename, ColorMode mode) :
    fill(false)
{
    load(imagename);
    setfont();
}

void Painter::setfont(std::string defaultfont) {
    imlib_set_font_cache_size(512 * 1024);
    /* add the ./ttfonts dir to our font path - you'll want a
    * notepad.ttf in that dir for the text to display */
    imlib_add_path_to_font_path("/usr/X11R6/lib/X11/fonts/TTF");
    imlib_add_path_to_font_path(".ttf");

    std::string fontstr=defaultfont+"/10";
    font = imlib_load_font(fontstr.c_str());

    if (!font) {
        int numfonts;
        char ** fontlist = imlib_list_fonts(&numfonts);
        if (numfonts) {
            // try Aerial
            if (!font){
                fontstr="aerial/10";
                font = imlib_load_font(fontstr.c_str());
            }
            if (!font){
                fontstr="verdana/10";
                font = imlib_load_font(fontstr.c_str());
            }
            if (!font){
                fontstr="times/10";
                font = imlib_load_font(fontstr.c_str());
            }
            if (!font){
                fontstr="monotype/10";
                font = imlib_load_font(fontstr.c_str());
            }
            if (!font){
                fontstr=fontlist[0];
                font = imlib_load_font(fontstr.c_str());
            }
        }
    }
    if (!font) {
        std::cerr << "Could not find aerial, times, verdana, veramono fonts."
           " No fonts loaded" << std::endl;
    }
}

Painter::Painter ( int w, int h, int depth, Optimization optimization):
    fill(false)
{
    image = imlib_create_image(w,h);
    if (!image){
        std::ostringstream ostr("Unable to create ");
        ostr << std::setw(4) << w << " X " << std::setw(4) << h << 
            " pixmap at depth "<< std::setw(3) << depth ;
        throw Exception("Painter::Constructor", ostr.str());
    }
    setfont();
}

Painter::~Painter() {
    if (image) {
        imlib_context_set_image(image);
        imlib_free_image();
    }
    if (font) {
        imlib_context_set_font(font);
        imlib_free_font();
    }
}

bool Painter::load ( const std::string& imagename, ColorMode mode) {
    // an image handle 
    image = imlib_load_image(imagename.c_str());;
    // if the load was successful
    if (!image){
        throw Exception("Painter::Constructor",
                "Unable to load image " + imagename);
    }
    return true;
}

bool Painter::save ( const std::string& imagename) const {
    if (!image) return false;
    imlib_context_set_image(image);
    std::string ext = lear::extension(imagename);
    if (ext.length())
        imlib_image_set_format(ext.c_str());
    else
        imlib_image_set_format("png");

    Imlib_Load_Error reterr;
    imlib_save_image_with_error_return(imagename.c_str(), &reterr);

    if ( reterr != IMLIB_LOAD_ERROR_NONE) {
        throw Exception("ImageOp::save", 
                std::string("Unable to save file: ") + imagename);
    } 
    return true;
}


void Painter::drawPoint(const int xt, const int yt, const int size, 
        const PointStyle style) 
{// {{{
    if (!image)  return;
    imlib_context_set_image(image);
    
    const int x = static_cast<int> (xt);
    const int y = static_cast<int> (yt);

    switch(style) {
        case Plus:
            imlib_image_draw_line(x-size, y, x+size, y,0);
            imlib_image_draw_line(x, y-size, x, y+size,0);
            break;
        case Cross:
            imlib_image_draw_line(x-size, y -size, x+size, y+size,0);
            imlib_image_draw_line(x-size, y +size, x+size, y-size,0);
            break;
        case Dot:
            imlib_image_draw_line(x, y, x, y,0);
            break;
    }
}// }}}
void Painter::drawLine(const int x1, const int y1, const int x2, const int y2) // {{{
{
    if (!image)  return;
    imlib_context_set_image(image);
    imlib_image_draw_line(
            static_cast<int>(x1),
            static_cast<int>(y1),
            static_cast<int>(x2),
            static_cast<int>(y2),0);
}
// }}}
//void Painter::fillRect(const int x, const int y, const int w, const int h) {// {{{
//    if (!image)  return;
//    imlib_context_set_image(image);
//
//        imlib_image_fill_rectangle(
//            static_cast<int>(x), static_cast<int>(y),
//            static_cast<int>(w), static_cast<int>(h));
//}
//// }}}
void Painter::drawRect(const int x, const int y, const int w, const int h) // {{{
{
    if (!image)  return;
    imlib_context_set_image(image);

    if (fill) {
        imlib_image_fill_rectangle(
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h));
    } else{
        imlib_image_draw_rectangle(
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h));
    }
}// }}}

void Painter::drawEllipse(// {{{
const int x, const int y, const int l1, const int l2, const double angle)
{
    if (!image)  return;
    imlib_context_set_image(image);

    if (fill)
        imlib_image_fill_ellipse(
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(l1), static_cast<int>(l2));
    else 
        imlib_image_draw_ellipse(
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(l1), static_cast<int>(l2));
}// }}}

// {{{ drawImage
void Painter::drawImage(const PImage& image_, 
        int dx, int dy, int dw, int dh, 
        int sx, int sy, int sw, int sh) 
{
    if (!image)  return;
    imlib_context_set_image(image_.image);

    if (sw ==-1) sw = imlib_image_get_width(); 
    if (sh ==-1) sh = imlib_image_get_height(); 

    imlib_context_set_image(image);
    if (dw ==-1) dw = sw;
    if (dh ==-1) dh = sh;

    imlib_blend_image_onto_image(image_.image,0,
            static_cast<int>(sx),
            static_cast<int>(sy),
            static_cast<int>(sw),
            static_cast<int>(sh),
            static_cast<int>(dx),
            static_cast<int>(dy),
            static_cast<int>(dw),
            static_cast<int>(dh));
}
// }}}
bool Painter::drawText (const std::string& text, const int x, const int y)// {{{
{
    if (!image)  return false;
    imlib_context_set_image(image);

    if (font) {
        imlib_context_set_font(font);
        //int text_w, text_h;
        //imlib_get_text_size(text.c_str(), &text_w, &text_h); 
        imlib_text_draw(
            static_cast<int>(x),
            static_cast<int>(y), text.c_str());
        return true;
    }
    return false;
}
// }}}
bool Painter::getTextSize(const std::string& text, int& width, int &height)// {{{
{
    if (font) {
        imlib_context_set_font(font);
        imlib_get_text_size(text.c_str(), &width, &height); 
        return true;
    }
    return false;
}
// }}}
void Painter::setColor(const int r, const int g, const int b) {// {{{
    if (!image)  return;
    imlib_context_set_image(image);

    int a = 255;
    imlib_context_set_color(
            static_cast<int>(r), static_cast<int>(g),
            static_cast<int>(b), static_cast<int>(a));
}// }}}
void Painter::fillBackground() {// {{{
    if (!image)  return;
    imlib_context_set_image(image);

    drawRect(0,0, imlib_image_get_width(), imlib_image_get_height()); 
}// }}}

