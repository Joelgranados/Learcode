// {{{ file documentation
/** 
 * @file
 * @brief Defines various painting operations
 */
// }}}

#ifndef _LEAR_PIMAGE_H_
#define _LEAR_PIMAGE_H_

/* standard headers */
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <string>

namespace lear {

class Painter;
/**
 * @brief PImage class. Mainly for use by Painter
 */
class PImage {
    public:
    PImage():image() {}
    PImage(const std::string& imagename){
        load(imagename);
    }
    PImage(int w, int h, int depth=32);

    void load(const std::string& imagename) ;
    int width() const {
        if (image) {
            imlib_context_set_image(image);
            return imlib_image_get_width(); 
        }else 
            return -1;
    }
    int height() const {
        if (image) {
            imlib_context_set_image(image);
            return imlib_image_get_height();
        }else 
            return -1;
    }

    PImage crop( int x, int y, int width, int height){
        if (image) {
            imlib_context_set_image(image);
            return PImage( imlib_create_cropped_image(x, y, width, height));
        }else 
            return PImage();
    }
    DATA32* data(){
        if (image) {
            imlib_context_set_image(image);
            return imlib_image_get_data();
        }else 
            return 0;
    }
    // data must be the pointer returned by DATA32* data() function
    bool data(DATA32* data){
        if (image) {
            imlib_context_set_image(image);
            imlib_image_put_back_data(data);
        }
        return data;
    }
    int depth() const { return 32; }
    int rows() const { return width(); }
    int cols() const { return height(); }
    bool valid() const {return static_cast<bool>(image);}
    private:
        PImage(Imlib_Image image):image(image){}
        Imlib_Image image;
        friend class Painter;
};

} // namespace lear

#endif // #ifdef _LEAR_PIMAGE_H_

