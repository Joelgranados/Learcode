// {{{ file documentation
/** 
 * @file
 * @brief Defines various image input/output operations.
 */
// }}}

#ifndef _LEAR_IMAGEIO_H_
#define _LEAR_IMAGEIO_H_

#include <string>

#include <blitz/array.h>
#include <blitz/tinyvec.h>

#include <lear/exception.h>

/* standard headers */
#include <X11/Xlib.h>
#include <Imlib2.h>

namespace lear {

    namespace detail {
    // {{{ format classes
    template<class T>
    struct Format {

        enum {depth = 8*4}; /// no. of pallet colors 
        typedef T Type;

        inline static void apply() {
            imlib_image_set_irrelevant_alpha(1);
            imlib_image_set_has_alpha(0);
            imlib_image_set_irrelevant_border(1);
            // do nothing
        }
    };
    template<>
    struct Format<bool> {

        enum {depth = 1};
        typedef bool Type;

        inline static void apply() {
            imlib_image_set_irrelevant_alpha(1);
            imlib_image_set_has_alpha(0);
            imlib_image_set_irrelevant_border(1);
        }
    };

    template<class T>
    struct Format<blitz::TinyVector<T,3> > {

        enum {depth = 8*3};
        typedef T Type;

        inline static void apply() {
            imlib_image_set_irrelevant_alpha(1);
            imlib_image_set_has_alpha(0);
            imlib_image_set_irrelevant_border(1);
        }
    };

    template<class T>
    struct Format<blitz::TinyVector<T,2> > {

        enum {depth = 8*2};
        typedef T Type;

        inline static void apply() {
            imlib_image_set_irrelevant_alpha(0);
            imlib_image_set_has_alpha(1);
            imlib_image_set_irrelevant_border(1);
        }
    };

    template<class T>
    struct Format<blitz::TinyVector<T,4> > {

        enum {depth = 8*4};
        typedef T Type;

        inline static void apply() {
            imlib_image_set_irrelevant_alpha(0);
            imlib_image_set_has_alpha(1);
            imlib_image_set_irrelevant_border(1);
        }
    };

    // }}}
    }

/**
 * @brief Image Input/Output struct
 *
 * ImageIO uses QT for image input output.
 */
struct ImageIO {

    static void setCache(int cacheSizeInMBs) {
        imlib_set_cache_size(cacheSizeInMBs*1024 * 1024);
    }

    //  {{{ image read functions RGB
    /**
    * @defgroup read_Functions Overloaded ImageIO::read functions
    * @brief read image from file
    * 
    * Read image from file and returns bltiz::Array. read automatically reads
    * the data that best fits in blitz::Array element. For e.g.
    *
    * if blitz::Array element (i.e. template T) is
    *  - int A grayscale image is read
    *  - double A grayscale image is read
    *  - TinyVector<double/int,3> A color rgb image is read
    *  - TinyVector<double/int,4> A color rgba image is read
    */
    // @{ 
    template<class T>
    static inline blitz::Array<T,2> read(const std::string& imagename) {
            // Read a file into image object
        // an image handle 
        Imlib_Image image = imlib_load_image(imagename.c_str());;
        // if the load was successful
        if (image)
        {// set the image we loaded as the current context image to work on
            imlib_context_set_image(image);
        } else {
            throw Exception("ImageOp::read", "Unable to read image: " + imagename);
        }
        detail::Format<T>::apply();
        return read<T>();
    }

    template<class T>
    static inline void read(const std::string& imagename, blitz::Array<T,2>& imageData) {
        imageData.reference(read<T>(imagename));
    }
    // @} 
    //  }}}

    //  {{{ image write functions
    /**
    * @brief write blitz::Array as image to file
    * 
    * Automatically writes an image in the format best described 
    * by blitz::Array element (provided the image format supports it).
    * For e.g.
    *
    * if blitz::Array element (i.e. template T) is
    *  - int A grayscale image is written
    *  - double A grayscale image is written
    *  - TinyVector<double/int,3> A color rgb image is written
    *  - TinyVector<double/int,4> A color rgba image is written
    *
    */
    template<class T> static inline
    void write(const std::string& imagename, const blitz::Array<T,2>& imageData) {

        int columns = imageData.columns();
        int rows = imageData.rows();

        Imlib_Image image = imlib_create_image(rows, columns);
        if (image) {
            // set the image we loaded as the  current context image to work on
            imlib_context_set_image(image);
        } else {
            throw Exception("ImageOp::write", "Unable to create image in memory");
        }

        DATA32* data = imlib_image_get_data();

        // copy data to array
        for (int i=0, k=imageData.lbound(0); i<rows; k++,i++)
        for (int j=0, l=imageData.lbound(1); j<columns; l++,j++) {
            data[j*rows+i] = toColor(imageData(k,l));
        }
        imlib_image_put_back_data(data);
        detail::Format<T>::apply();

        save(imagename);
    }
    //  }}}

    /// Return the format of this image, currently uses image extension
    static std::string imageFormat(const std::string& imagename) ;

    /// Return the size of the image width,height
    static blitz::TinyVector<int,2> size(const std::string& imagename) ;
protected:

    static inline DATA8 red(DATA32 c) {
        return static_cast<DATA8>((c & 0x00FF0000)>>16);
    }
    static inline DATA8 green(DATA32 c) {
        return static_cast<DATA8>((c & 0x0000FF00)>> 8);
    }
    static inline DATA8 blue(DATA32 c) {
        return static_cast<DATA8>((c & 0x000000FF));
    }
    static inline DATA8 gray(DATA32 c) {
        return static_cast<DATA8>((0.3*red(c) + 0.59*green(c) + 0.11*blue(c)));
    }
    static inline DATA8 alpha(DATA32 c) {
        return static_cast<DATA8>((c & 0xFF000000)>>24);
    }
    // {{{ fromColor functions. All overloaded versions
    template<class T>
    static inline void fromColor(const DATA32 c, bool& val) {
        val = c ? true : false;
    }

    template<class T>
    static inline void fromColor(const DATA32 c, T& val) {
        val = gray(c);
    }

    // Gray and alpha channel
    template<class T>
    static inline void fromColor(const DATA32 c, blitz::TinyVector<T,2>& val) {
        val = gray(c), alpha(c);
    }

    template<class T>
    static inline void fromColor(const DATA32 c, blitz::TinyVector<T,3>& val) {
        val = red(c), green(c), blue(c);
    }

    template<class T>
    static inline void fromColor(const DATA32 c, blitz::TinyVector<T,4>& val) {
        val = red(c), green(c), blue(c), alpha(c);
    }
    // }}}

    // {{{ toColor functions. All overloaded versions
    static inline DATA32 toColor(const bool color) {
        return color ? 0xFFFFFF: 0;
    }

    template<class T>
    static inline DATA32 toColor(const T color) {
        const DATA8 c = static_cast<DATA8> (color); 
        DATA32 cc = 0;
        cc = 0xFF000000 | (c <<16) | (c << 8) | c;
        return cc;
    }

    // Gray and alpha channel
    template<class T>
    static inline DATA32 toColor(const blitz::TinyVector<T,2> color) {
        const DATA8 c = static_cast<DATA8> (color[0]); 
        const DATA8 a = static_cast<DATA8> (color[1]); 
        DATA32 cc = (a<<24) |  (c <<16) | (c << 8) | c;
        return cc;
    }

    template<class T>
    static inline DATA32 toColor(const blitz::TinyVector<T,3> color) {
        DATA32 cc = 0;
        cc =(0xFF000000) |
            (static_cast<DATA8> (color[0]) <<16) | 
            (static_cast<DATA8> (color[1]) << 8) | 
            (static_cast<DATA8> (color[2]));
        return cc;
    }

    template<class T>
    static inline DATA32 toColor(const blitz::TinyVector<T,4> color) {
        DATA32 cc = 0;
        cc =(static_cast<DATA8> (color[3]) <<24) | 
            (static_cast<DATA8> (color[0]) <<16) | 
            (static_cast<DATA8> (color[1]) << 8) | 
            (static_cast<DATA8> (color[2]));
        return cc;
    }
    // }}}

    // {{{ read data from imlib2
    /*! \brief read imlib2 image and return blitz array.
    */
    template<class T>
    static inline blitz::Array<T,2> read() {
        DATA32* data = imlib_image_get_data_for_reading_only();

        int rows = imlib_image_get_width(), 
            columns = imlib_image_get_height();

        // resize data
        blitz::Array<T,2> imageData(rows,columns);

            // copy data to array
        for (int i=0; i<rows; i++)
        for (int j=0; j<columns; j++) {
            fromColor(data[j*rows+i], imageData(i,j));
        }
        imlib_free_image();
            return imageData;
    }

    template<class T>
    static inline void read(blitz::Array<T,2>& imageData) {
        imageData.reference(read<T>());
    }
    // }}}


    static void save(const std::string& imagename);
};

} // namespace lear

#ifdef None // defined in X11/X.h
#undef None
#endif
#ifdef GrayScale // defined in X11/X.h
#undef GrayScale
#endif

#endif // #ifdef _LEAR_IMAGEIO_H_

