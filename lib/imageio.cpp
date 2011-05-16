#include <lear/image/imageio.h>
#include <cctype>
#include <lear/util/fileutil.h>
#include <lear/blitz/ext/tvmutil.h>

using namespace lear;

std::string ImageIO::imageFormat(const std::string& imagename) {
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


blitz::TinyVector<int,2> ImageIO::size(const std::string& imagename) {
    Imlib_Image image = imlib_load_image(imagename.c_str());;
    
    if (image)// if the load was successful
    {   // set the image we loaded as the current context image to work on
        imlib_context_set_image(image);
        int w = imlib_image_get_width(), 
            h = imlib_image_get_height();
        imlib_free_image();
        return blitz::TinyVector<int,2> (w,h);
    } else {
        throw Exception("ImageOp::read", 
                "Unable to read image: " + imagename);
    }
}

void ImageIO::save(const std::string& imagename){
    std::string ext = lear::extension(imagename);
    //int n = imagename.rfind(".");
    //std::string ext = imagename.substr(n+1,std::string::npos); 
    if (ext.length())
        imlib_image_set_format(ext.c_str());
    else
        imlib_image_set_format("png");

    /* save the image */
    Imlib_Load_Error reterr;
    imlib_save_image_with_error_return(imagename.c_str(), &reterr);
    imlib_free_image();

    if ( reterr != IMLIB_LOAD_ERROR_NONE) {
        throw Exception("ImageOp::save", 
                std::string("Unable to save file: ") + imagename);
    } 
}
