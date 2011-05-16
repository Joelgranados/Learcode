/*
 * =====================================================================================
 *
 *       Filename:  test_library.cpp
 *
 *    Description:  Code to test library routine.
 *
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <iterator>

#include <X11/Xlib.h>
#include <Imlib2.h>

#include <lear/interface/windetect.h>// change this path as appropriate.

int main(int argc, char** argv) {
    using namespace std;

    if (argc < 2) {
        std::cout << "Usage: test_library <input image>" << std::endl;
        std::cout << "It dumps detections on the command line" << std::endl;
        exit(1);
    }


    // initialize the person detector. All default parameters are set for person detector.
    WinDetectClassify windetect;// use default person detector.
    RHOGDenseParam desc;
    LinearClassify classifier;// initialize it to 64x128 person detector.

    windetect.init(&desc); // initialize the descriptor computation

    // read image
    Imlib_Image image = imlib_load_image(argv[1]);
    // if the load was successful
    if (image)
    {// set the image we loaded as the current context image to work on
        imlib_context_set_image(image);
    } else {
        std::cerr << "Unable to read image: " << argv[1] << std::endl; 
        exit(1);
    }
    int width  = imlib_image_get_width(), 
        height = imlib_image_get_height();

    typedef unsigned char uchar;
    DATA32* data = imlib_image_get_data_for_reading_only();
    uchar* imagedata = new uchar[3*width*height*sizeof(uchar)];
    for (int j= 0; j< height; ++j) 
    for (int i= 0; i< width; ++i) {
        uchar* pixel = imagedata+(i+j*width)*3; 
        int argb = data[i+j*width];
        pixel[0] = static_cast<uchar>((argb & 0x00FF0000)>>16);
        pixel[1] = static_cast<uchar>((argb & 0x0000FF00)>> 8);
        pixel[2] = static_cast<uchar>((argb & 0x000000FF)    );
    }
    imlib_free_image();


    // now get detections
    std::list<DetectedRegion> detections;
    windetect.test(classifier, detections, imagedata, width, height);

    delete[] imagedata;

    //print detections
    std::copy(detections.begin(), detections.end(), std::ostream_iterator<DetectedRegion>(std::cout, "\n"));

    // dump detections to screen.
    return 0;
}


