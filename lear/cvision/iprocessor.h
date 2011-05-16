#ifndef _LEAR_IMAGE_PROCESSOR_H_
#define _LEAR_IMAGE_PROCESSOR_H_

#include <cmath>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <lear/io/biostream.h>

namespace lear {

    struct ImageNoRemap {// {{{
        typedef float                           RealType;

        typedef RealType                        GrayType; 
        typedef blitz::TinyVector<RealType,3>   RGBType; 

        typedef blitz::Array<RealType,2>        GrayImage;
        typedef blitz::Array<RGBType,2>         RGBImage;

        enum {Method = 0};

        virtual GrayImage operator() (const GrayImage& img) const 
        { return img; }

        virtual RGBImage operator() (const RGBImage& img) const 
        { return img; }

        virtual const char * toString() const 
        { return "NoMap"; }

        virtual unsigned toMethod() const { return Method;}

        virtual ~ImageNoRemap() {}
    };// }}}
    struct ImageSqrtRemap : public ImageNoRemap {// {{{
        enum {Method = 1};

        virtual GrayImage operator() (const GrayImage& img) const ;
        virtual RGBImage operator() (const RGBImage& img) const ;
        virtual const char * toString() const 
        { return "Sqrt"; }

        virtual unsigned toMethod() const { return Method;}
    };// }}}
    struct ImageLogRemap : public ImageNoRemap {// {{{
        enum {Method = 2};

        virtual GrayImage operator() (const GrayImage& img) const ;
        virtual RGBImage operator() (const RGBImage& img) const ;

        virtual const char * toString() const 
        { return "Log"; }

        virtual unsigned toMethod() const { return Method;}
    };// }}}
    struct ImageLabRemap : public ImageNoRemap{// {{{
        enum {Method = 3};

        // do nothing for gray scale images
        virtual GrayImage operator() (const GrayImage& img) const 
        { return img; }

        virtual RGBImage operator() (const RGBImage& img) const ;

        virtual const char * toString() const 
        { return "Lab"; }

        virtual unsigned toMethod() const { return Method;}
    };// }}}
    struct ImageLabSqrtRemap : public ImageLabRemap{// {{{
        enum {Method = 4};

        virtual GrayImage operator() (const GrayImage& img) const ;
        virtual RGBImage operator() (const RGBImage& img) const ;

        virtual const char * toString() const 
        { return "Lab Sqrt"; }

        virtual unsigned toMethod() const { return Method;}
    };// }}}

    struct ChannelMax {// {{{
        typedef float                           RealType;

        typedef RealType                        GrayType; 
        typedef blitz::TinyVector<RealType,3>   RGBType; 

        typedef blitz::Array<RealType,2>        GrayImage;
        typedef blitz::Array<RGBType,2>         RGBImage;

        enum {Method = 0};

        virtual std::pair<GrayImage, GrayImage> operator() 
            (const GrayImage& dervX, const GrayImage& dervY) const ;

        virtual std::pair<GrayImage, GrayImage> operator() 
            (const RGBImage& dervX, const RGBImage& dervY) const ;

        virtual const char * toString() const { return "ChMax"; }

        virtual unsigned toMethod() const { return Method;}

        virtual ~ChannelMax() {}
    };// }}}

class IProcessor {// {{{
    protected:
        typedef blitz::Array<int,2>             OriAType_;
    public:
        typedef float                           RealType;
        typedef blitz::TinyVector<int,2>        IndexType;

        typedef blitz::TinyVector<RealType,3>   RGBType;
        typedef RealType                        GrayType;

        typedef blitz::Array<RealType,2>        Array2DType;
        typedef blitz::Array<RealType,2>        GrayImage;
        typedef blitz::Array<RGBType,2>         RGBImage;


        enum {Method=0};

        struct InfoType {
            typedef OriAType_           OriAType;
            typedef Array2DType         MagAType;
            typedef RealType            MagElemType;
            typedef int                 OriElemType;

            MagAType                    mag;
            OriAType                    ori;
            IndexType                   extent;

            InfoType(MagAType tmag, OriAType tori) 
                : mag(tmag), ori(tori), extent(tmag.extent()) {}
        };

        IProcessor(){}
        virtual ~IProcessor() {}

        virtual InfoType operator()( const RGBImage& image) const =0;
        virtual InfoType operator()(const GrayImage& image)const =0;

        virtual std::string toString() const = 0;
        virtual unsigned toMethod() const = 0;

        virtual void print(std::ostream& o) const {
            o << toString();
        }
        virtual void print(BiOStream& o)const{
            o << toMethod();
        }
};// }}}

class IPrePostProcessor : public IProcessor{// {{{
    public:
        typedef IProcessor    Parent;

        /// Owns to1d, remapAfter, and remapBefore
        IPrePostProcessor(
            const ChannelMax* to1d = NULL,
            const ImageNoRemap* remapBefore=NULL,
            const ImageNoRemap* remapAfter=NULL )
            : 
        Parent(), to1d(to1d), remapBefore(remapBefore), remapAfter(remapAfter)
        {
            if (!(this->to1d)) this->to1d = new ChannelMax();
            if (!(this->remapBefore)) this->remapBefore = new ImageNoRemap();
            if (!(this->remapAfter)) this->remapAfter = new ImageNoRemap();
        }
        virtual ~IPrePostProcessor() {
            delete to1d;
            delete remapAfter;
            delete remapBefore;
        }

        virtual std::string toString() const ;

        virtual unsigned toMethod() const {
            unsigned t= (to1d->toMethod()<<24)| 
                        (remapBefore->toMethod()<<16)| 
                        (remapAfter->toMethod()<< 8)| 
                        Parent::Method;
            return t;
        }

    protected:
        virtual std::string name() const = 0;

        // convert color channel to 1d signal
        const ChannelMax*     to1d;

        // remap image before computing gradient
        const ImageNoRemap*   remapBefore; 

        // remap image after computing gradient
        const ImageNoRemap*   remapAfter;
};// }}}

class GradProcessor_NoSmooth : public IPrePostProcessor
{// {{{
    public:
        typedef IPrePostProcessor                       Parent;
        typedef Parent::InfoType                        InfoType;

        enum {Method=Parent::Method | 1};

        GradProcessor_NoSmooth (const bool semicirc, 
                const ChannelMax* to1d=NULL,
                const ImageNoRemap* remapBefore=NULL, 
                const ImageNoRemap* remapAfter=NULL ): 
            Parent(to1d, remapBefore, remapAfter), semicirc(semicirc)
        {}

        virtual InfoType operator()(const RGBImage& image) const;
        virtual InfoType operator()(const GrayImage& image) const ;

        virtual unsigned toMethod() const { return Method; }
        virtual std::string toString() const ;

    protected:
        virtual std::string name() const { 
            return "Grad (no smooth) Order 2";
        }
        /// if true, angle range = 0--180
        bool semicirc;
};// }}}

class GradProcessor : public GradProcessor_NoSmooth
{// {{{
    public:
        typedef GradProcessor_NoSmooth              Parent;

        enum {Method=Parent::Method | 2};

        GradProcessor(
            const RealType dscale, const bool semicirc,
            const ChannelMax* to1d=NULL,
            const ImageNoRemap* remapBefore=NULL, 
            const ImageNoRemap* remapAfter=NULL ); 

        virtual InfoType operator()( const RGBImage& image) const ;
        virtual InfoType operator()(const GrayImage& image) const ;

        virtual unsigned toMethod() const {
            return Method;
        }
        virtual std::string toString() const {
            std::ostringstream s ;
            s   << Parent::toString() 
                << ", scale " << std::setw(3) << std::setprecision(1) << dscale;
            return s.str();
        } 
        virtual void print(BiOStream& o)const{
            Parent::print(o);
            o << dscale;
        }
    protected:
        virtual std::string name() const {
            return "Grad";
        }
        /// smooth scale
        RealType dscale;
        /// 1D Gaussian kernel of specified scale
        blitz::Array<RealType,1> kernel;
};// }}}

std::ostream& operator<<(std::ostream& o, const IProcessor& d);
lear::BiOStream& operator<<(lear::BiOStream& o, const IProcessor& d);

}

#endif // _LEAR_IMAGE_PROCESSOR_H_

