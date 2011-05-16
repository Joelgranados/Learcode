// {{{ file documentation
/** 
 * @file
 * @brief Defines various painting operations
 */
// }}}

#ifndef _LEAR_PAINTER_H_
#define _LEAR_PAINTER_H_

//  {{{ Headers
#include <string>
#include <lear/util/rectangle.h>
//  }}}

/* standard headers */
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <lear/image/pimage.h>

namespace lear {

/**
 * @brief Painter class for drawing points, ellipses, images etc.
 */
class Painter {
    public:
    // {{{ enums
        enum ColorMode {
            Auto , 
            Color, 
            Mono 
        };

        enum PointStyle {
            Dot, Cross, Plus
        };

        enum Optimization {
            DefaultOptim, 
            NoOptim     , 
            MemoryOptim , 
            NormalOptim , 
            BestOptim   
        };
        enum TextDirection {
            DefaultTextDir = IMLIB_TEXT_TO_RIGHT,
            RTLTextDir     = IMLIB_TEXT_TO_RIGHT,// - right to left 
            LTRTextDir     = IMLIB_TEXT_TO_LEFT//  - left to right 
        };
    // }}}
    
        Painter();
        Painter(const std::string& imagename, ColorMode mode = Auto) ;
        void setfont(std::string defaultfont="VeraMono");
        Painter ( int w, int h, int depth = -1, 
                Optimization optimization = DefaultOptim );
        ~Painter();
        bool load ( const std::string& imagename, ColorMode mode = Auto ) ;
        bool save ( const std::string& imagename) const ;

        void drawPoint(const int xt, const int yt, const int size = 2, const PointStyle style = Plus) ;

        void drawLine(const int x1, const int y1, const int x2, const int y2) ;

        //void fillRect(const int x, const int y, const int w, const int h) ;
        void drawRect(const int x, const int y, const int w, const int h) ;

        template<class T>
        inline void drawRect(const Rectangle<T>& r) {// {{{
            if (!image)  return;
            imlib_context_set_image(image);

            if (fill) {
                imlib_image_fill_rectangle(
                    static_cast<int>(r.x()),
                    static_cast<int>(r.y()),
                    static_cast<int>(r.w()),
                    static_cast<int>(r.h()));
            } else {
                imlib_image_draw_rectangle(
                    static_cast<int>(r.x()),
                    static_cast<int>(r.y()),
                    static_cast<int>(r.w()),
                    static_cast<int>(r.h()));
            }
        }// }}}
        
        /**
         * Draw an ellipse.
         *
         * @param x center of ellipse
         * @param y center of ellipse
         * @param l1 major axis
         * @param l1 minor axis
         * @param angle angle of ellipse (in degrees) from x-axis in coutner clockwise direction
         */
        void drawEllipse(
                const int x, const int y, 
                const int l1, const int l2, 
                const double angle=0) ;
        
        /**
         * Draw a Circle.
         *
         * @param x center of ellipse
         * @param y center of ellipse
         * @param r radius
         */
        inline void drawCircle( const int x, const int y, const int r) 
        { drawEllipse(x,y,r,r); }
        
        /**
         * Draw an image.
         *
         * @param image image to draw
         * @param dx destination start position x
         * @param dy destination start position y
         * @param dw destination width
         * @param dh destination height
         * @param sx source start position x
         * @param sy source start position y
         * @param sw source width
         * @param sh source height
         */
        void drawImage(const PImage& image_, 
                int dx=0, int dy=0,
                int dw=-1, int dh=-1,
                int sx = 0, int sy = 0, 
                int sw = -1, int sh = -1) ;
        /**
         * Draw a Text on image.
         *
         * @param x start position
         * @param y end position
         * @param text text to draw
         */
        bool drawText (const std::string& text, const int x, const int y);
        bool getTextSize(const std::string& text, int& width, int &height);
        
        void setColor(const int r, const int g, const int b) ;

        void fillBackground() ;

        inline void setBrush(const int r, const int g, const int b) 
        { setColor(r,g,b); fill=true; }

        inline void noBrush() 
        {fill=false;}

    
private:
        bool fill;
        Imlib_Image image;
        Imlib_Font font;
};

} // namespace lear

#endif // #ifdef _LEAR_PAINTER_H_
