/*
 * =====================================================================================
 *
 *       Filename:  iprocessor.cpp
 *
 *    Description:  Provides implementation to iprocessor.h class
 *
 * =====================================================================================
 */

#include <lear/cvision/iprocessor.h>
#include <lear/cvision/edge.h>
#include <lear/cvision/phistogram.h>

#include <lear/numericutil/gauss.h>
#include <lear/blitz/ext/stdfuncs.h>

namespace lear {
std::ostream& operator<<(std::ostream& o, const IProcessor& d)
{ d.print(o); return o; }

lear::BiOStream& operator<<(lear::BiOStream& o, const IProcessor& d)
{ d.print(o); return o; }

}

using namespace lear;

ImageLabSqrtRemap::GrayImage ImageLabSqrtRemap::operator() (const GrayImage& img) const 
{
    GrayImage t(blitz::sqrt(img));
    return t;// do nothing for gray scale images
}
ImageLabSqrtRemap::RGBImage ImageLabSqrtRemap::operator() (const RGBImage& img) const 
{
    RGBImage t(blitz::sqrt(ImageLabRemap::operator()(img)));
    return t;
}
ImageSqrtRemap::GrayImage ImageSqrtRemap::operator() (const GrayImage& img) const 
{
    GrayImage t(blitz::sqrt(img));
    return t;
}
ImageSqrtRemap::RGBImage ImageSqrtRemap::operator() (const RGBImage& img) const 
{
    RGBImage t(blitz::sqrt(img));
    return t;
}
ImageLogRemap::GrayImage ImageLogRemap::operator() (const GrayImage& img) const 
{
    GrayImage t(blitz::log(img+1));
    return t;
}
ImageLogRemap::RGBImage ImageLogRemap::operator() (const RGBImage& img) const 
{
    RGBImage t(blitz::log(img+1));
    return t;
}
ImageLabRemap::RGBImage ImageLabRemap::operator() (const RGBImage& img) const 
{
    // convert to Lab space
    RGB2LabFunctor toLab;
    RGBImage imageLab (img.lbound(), img.extent());
    std::transform(img.begin(), img.end(), imageLab.begin(), toLab);

    imageLab[0] *= 255.0/100;
    imageLab[1] = (imageLab[1] +  87)*(255.0/( 99+ 87));
    imageLab[2] = (imageLab[2] + 108)*(255.0/( 95+108));

    return imageLab;
}

std::pair<ChannelMax::GrayImage, ChannelMax::GrayImage> ChannelMax::operator() 
    (const GrayImage& dervX, const GrayImage& dervY) const 
{  
    GrayImage mag (sqrt(dervX*dervX+dervY*dervY));
    GrayImage ori (mag.lbound(),mag.extent());

    AnglePolicy_atan2 angle;
    angle(dervY,dervX, ori);

    return std::make_pair(mag, ori);
}
std::pair<ChannelMax::GrayImage, ChannelMax::GrayImage> ChannelMax::operator() 
    (const RGBImage& dervX, const RGBImage& dervY) const 
{  
    RGBImage mag (sqrt(dervX*dervX+dervY*dervY));
    RGBImage ori (mag.lbound(),mag.extent());

    AnglePolicy_atan2 angle;
    angle(dervY,dervX, ori);

    GrayImage maxMag(mag.lbound(),mag.extent());
    GrayImage maxOri(ori.lbound(),ori.extent());

    for (blitz::DomainIter<2> i(mag.lbound(), mag.ubound()); i; ++i){
        int maxIndex= blitz::maxIndex(mag(*i));
        maxMag(*i) = mag(*i)[maxIndex];
        maxOri(*i) = ori(*i)[maxIndex];
    }
    return std::make_pair(maxMag,maxOri);
} 

GradProcessor_NoSmooth::InfoType GradProcessor_NoSmooth::operator()( const RGBImage& image) const 
{// {{{
    using namespace blitz;

    pair<RGBImage, RGBImage> res = nosmoothgradientXY((*remapBefore)(image));

    pair<Array2DType,Array2DType> best = (*to1d)(res.first,res.second);

    OriAType_ ori (best.second*180/MathConst<RealType>::PI_Value + 180);
    if (semicirc) { ori %= 180; }
    return InfoType((*remapAfter)(best.first),ori);
}// }}}

GradProcessor_NoSmooth::InfoType GradProcessor_NoSmooth::operator()(const GrayImage& image) const 
{// {{{
    using namespace blitz;

    pair<GrayImage, GrayImage> best = 
        nosmoothgradient ((*remapBefore)(image), AnglePolicy_atan2());

    OriAType_ ori (best.second*180/MathConst<RealType>::PI_Value + 180);
    if (semicirc) { ori %= 180; }
    return InfoType((*remapAfter)(best.first),ori);
}// }}}


GradProcessor::InfoType GradProcessor::operator()( const RGBImage& image) const 
{// {{{
    using namespace blitz;

    pair<RGBImage, RGBImage> res=
        gradientXY< ConvolveExact<CPolicy_Chop> >((*remapBefore)(image),kernel);

    pair<Array2DType,Array2DType> best = (*to1d)(res.first,res.second);

    OriAType_ ori (best.second*180/MathConst<RealType>::PI_Value + 180);
    if (Parent::semicirc) { ori %= 180; }
    return InfoType((*remapAfter)(best.first),ori);
}// }}}
GradProcessor::InfoType GradProcessor::operator()(const GrayImage& image) const 
{// {{{
    using namespace blitz;

    pair<GrayImage, GrayImage> best = 
        gradient((*remapBefore)(image), kernel, AnglePolicy_atan2());

    OriAType_ ori (best.second*180/MathConst<RealType>::PI_Value + 180);
    if (Parent::semicirc) { ori %= 180; }
    return InfoType((*remapAfter)(best.first),ori);
}// }}}

GradProcessor::GradProcessor(
    const RealType dscale, const bool semicirc,
    const ChannelMax* to1d,
    const ImageNoRemap* remapBefore, 
    const ImageNoRemap* remapAfter): 
Parent(semicirc, to1d, remapBefore, remapAfter),
dscale(dscale), kernel ( Gauss::discrete(dscale,3))
{}
//////////////// other functions
std::string IPrePostProcessor::toString() const 
{
    std::ostringstream s ;
    s   << "'" << remapBefore->toString() 
        << ' ' << name()
        << ", " << to1d->toString()
        << ' ' << remapAfter->toString() << "'" ;
    return s.str();
}


std::string GradProcessor_NoSmooth::toString() const {
    std::ostringstream s ;
    s << Parent::toString() ;
    s << ", " << (semicirc?("0-180"):("0-360"));
    return s.str();
}


