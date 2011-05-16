/*
 * =====================================================================================
 *
 *       Filename:  colorconversion.cpp
 *
 *    Description:  Provides implementation to color conversion routines
 *
 * =====================================================================================
 */

#include <lear/image/colorconversion.h>
#include <lear/util/lookup.h>

using namespace lear;
ConvertColor::result_type RGB2XYZFunctor::operator()(const argument_type & rgb) const
{ 
    // Convert RGB to XYZ
    result_type result;
                            
    // Undo gamma correction
    component_type R = detail::newInvGammaCorrection(rgb[0]/max_);
    component_type G = detail::newInvGammaCorrection(rgb[1]/max_);
    component_type B = detail::newInvGammaCorrection(rgb[2]/max_);

    result[0] = 
(0.412453220144161469828958388461614958941936492919921875 *R + 0.357579581293500037286747783582541160285472869873046875 *G + 0.1804225899705355351532176655382500030100345611572265625 *B);
    result[1] = 
(0.212671121341218294364949770169914700090885162353515625 *R + 0.7151592053107826973956662186537869274616241455078125   *G + 0.0721687767761324538984268883723416365683078765869140625 *B);
    result[2] = 
(0.0193338164619907548757993254184839315712451934814453125*R + 0.1191935402066254534592104619150632061064243316650390625*G + 0.9502269222897068434718903517932631075382232666015625 *B);

    /*
    result[0] = (0.412453*rgb[0] + 0.357580*rgb[1] + 0.180423*rgb[2])/max_;
    result[1] = (0.212671*rgb[0] + 0.715160*rgb[1] + 0.072169*rgb[2])/max_;
    result[2] = (0.019334*rgb[0] + 0.119193*rgb[1] + 0.950227*rgb[2])/max_;
    */

    return result;
}

ConvertColor::result_type XYZ2LuvFunctor::operator()(const argument_type & xyz) const
{
    result_type result;
    //if(xyz[1] == 0 ) return result = 0;
    
    // Convert to CIE L*u*v* (CIELUV)

    component_type denom = xyz(0) + 15*xyz(1) + 3*xyz(2)+1e-28;// add small number to avoid division by zero
    component_type L = 116*detail::newfY(xyz(1)/WhitePoint(1)) - 16;

    result[0] = L;                                    // L*
    result[1] = 13*L*(4*xyz(0)/denom - WhitePointU);  // u*
    result[2] = 13*L*(9*xyz(1)/denom - WhitePointV);  // v*

    return result;
}
ConvertColor::result_type XYZ2LabFunctor::operator()(const argument_type & xyz) const
{
    // Convert XYZ to CIE L*a*b*
    component_type fX = detail::newfY(xyz[0]/0.950456);
    component_type fY = detail::newfY(xyz[1]);
    component_type fZ = detail::newfY(xyz[2]/1.088754);

    result_type result;
    result[0] = 116*fY - 16;// L*
    result[1] = 500.0*(fX - fY);// a*
    result[2] = 200.0*(fY - fZ);// b*
    return result;
}
ConvertColor::result_type XYZ2LchFunctor::operator()(const argument_type& xyz) const
{
    result_type Lab = xyz2lab(xyz);

    component_type angle = std::atan2(Lab[2],Lab[1]);
    angle = angle*180/MathConst<component_type>::PI_Value + 360*(angle < 0);
    result_type result;
    result[0] = Lab[0]; 
    result[1] = std::sqrt(Lab[1]*Lab[1] + Lab[2]*Lab[2]);
    result[2] = angle;
    return result;
}
ConvertColor::result_type RGB2XYZOldFunctor::operator()(const argument_type & rgb) const
{ 
    result_type result;
    result[0] = (0.412453*rgb[0] + 0.357580*rgb[1] + 0.180423*rgb[2])/max_;
    result[1] = (0.212671*rgb[0] + 0.715160*rgb[1] + 0.072169*rgb[2])/max_;
    result[2] = (0.019334*rgb[0] + 0.119193*rgb[1] + 0.950227*rgb[2])/max_;

    return result;
}
ConvertColor::result_type XYZOld2LuvOldFunctor::operator()(const argument_type& xyz) const
{
    result_type result;
    if(xyz[1] == 0 )
    {
        result = 0;
    }
    else
    {
        const component_type L = xyz[1] < 0.008856 ?
                                903.3 * xyz[1] :
                                116.0 * std::pow(xyz[1], gamma_) - 16.0;
        const component_type denom = xyz[0] + 15.0*xyz[1] + 3.0*xyz[2];
        const component_type uprime = 4.0 * xyz[0] / denom;
        const component_type vprime = 9.0 * xyz[1] / denom;
        result[0] = L;
        result[1] = 13.0*L*(uprime - 0.197839);
        result[2] = 13.0*L*(vprime - 0.468342);
    }
    return result;
}
