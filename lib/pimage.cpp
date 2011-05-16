/* standard headers */
#include <iostream>
#include <sstream>
#include <lear/image/pimage.h>
#include <lear/exception.h>

using namespace lear;
PImage::PImage(int w, int h, int depth){
    Imlib_Image image = imlib_create_image(w,h);
    if (!image){
        throw Exception("PImage::Constructor",
                "Unable to create image");
    }
}
void PImage::load(const std::string& imagename) {
    // an image handle 
    image = imlib_load_image(imagename.c_str());;
    // if the load was successful
    if (!image){
        throw Exception("PImage::Constructor",
                "Unable to load image " + imagename);
    }
}
