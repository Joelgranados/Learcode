/*
 * =====================================================================================
 * 
 *        Filename:  imageutil.cpp
 * 
 *     Description:  Utilitiy functions for images.
 * 
 * =====================================================================================
 */

/* standard headers */
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <lear/exception.h>
#include <lear/image/imageutil.h>

void lear::imagesize(const std::string& imagename,int& width, int& height) 
{
    Imlib_Image image = imlib_load_image(imagename.c_str());;
    
    if (image)// if the load was successful
    {   // set the image we loaded as the current context image to work on
        imlib_context_set_image(image);
        width = imlib_image_get_width(), 
        height = imlib_image_get_height();
        imlib_free_image();
    } else {
        throw Exception("ImageOp::read", 
                "Unable to read image: " + imagename);
    }
}

std::string lear::imagefmt(const std::string& imagename) {
    Imlib_Image image = imlib_load_image(imagename.c_str());
    
    if (image)// if the load was successful
    {   // set the image we loaded as the current context image to work on
        imlib_context_set_image(image);
        
        std::string format (imlib_image_format());
        imlib_free_image();
        return format;
    } else {
        return std::string();
    }
}
