// {{{ copyright notice
// This notice below is not valid any more. The code has been changed to
// a recent version and does not match with the version in vigra library.
/************************************************************************/
/*                                                                      */
/*               Copyright 1998-2002 by Ullrich Koethe                  */
/*       Cognitive Systems Group, University of Hamburg, Germany        */
/*                                                                      */
/*    This file is part of the lear computer vision library.           */
/*    ( Version 1.2.0, Aug 07 2003 )                                    */
/*    You may use, modify, and distribute this software according       */
/*    to the terms stated in the LICENSE file included in               */
/*    the lear distribution.                                           */
/*                                                                      */
/*    The lear Website is                                              */
/*        http://kogs-www.informatik.uni-hamburg.de/~koethe/lear/      */
/*    Please direct questions, bug reports, and contributions to        */
/*        koethe@informatik.uni-hamburg.de                              */
/*                                                                      */
/*  THIS SOFTWARE IS PROVIDED AS IS AND WITHOUT ANY EXPRESS OR          */
/*  IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED      */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. */
/*                                                                      */
/************************************************************************/

// The code is adopted from vigra computer vision library.
 // }}}
// {{{ file documentation
/**
 * @file
 * @brief color conversion code. 
 */
// }}}

#ifndef _LEAR_COLORCONVERSIONS_H_
#define _LEAR_COLORCONVERSIONS_H_
// {{{ headers
#include <cmath>
#include <blitz/tinyvec.h>

// }}}
namespace lear {
// {{{ detail
namespace detail {

    inline double gammaCorrection(double value, double gamma) {
        return (value < 0.0) ? std::pow(-value, gamma) : std::pow(value, gamma);
    }

    inline double gammaCorrection(double value, double gamma, double norm) {
        return (value < 0.0) ?  -norm*std::pow(-value/norm, gamma) : norm*std::pow(value/norm, gamma);
    }


    inline double newGammaCorrection(double R) {
        return (R < 0.018)? 4.5138*R : 1.099*std::pow(R,0.45) - 0.099;
    }
    inline double newInvGammaCorrection(double Rp) {
        double R = std::pow((Rp + 0.099)/1.099,1/0.45);
        return (R < 0.018) ? Rp/4.5138: R;
    }


    inline double newfY(double Y) {
        return (Y < 0.008856) ? Y*841.0/108 + 4.0/29: std::pow(Y,1.0/3);
    }

    inline double newInvfY(double fY) {
        double Y = std::pow(fY,3.0);
        return (Y < 0.008856)? (fY - 4.0/29)*108.0/841 : Y ;
    }

} // namespace detail
// }}}

class ConvertColor
{
  public:
    typedef double                               component_type;

    ///the functor's argument type
    typedef blitz::TinyVector<component_type, 3> argument_type;
  
    /// the functor's result type 
    typedef blitz::TinyVector<component_type, 3> result_type;
  
    /** apply the transformation
    */
    virtual result_type operator()(const argument_type& ) const =0;

    virtual ~ConvertColor() {}
};
// {{{ RGB2XYZFunctor
class RGB2XYZFunctor: public ConvertColor
{
  public:
  
    /// default constructor. The maximum value for each RGB component defaults to 255.
    RGB2XYZFunctor()
    : max_(255.0)
    {}
    
    /// constructor \arg max - the maximum value for each RGB component
    RGB2XYZFunctor(component_type max)
    : max_(max)
    {}
    
    /// apply the transformation 
    result_type operator()(const argument_type & rgb) const;

  protected:
    component_type max_;
};
// }}}

// {{{ XYZ2LuvFunctor
class XYZ2LuvFunctor: public ConvertColor
{
  public:
  
    XYZ2LuvFunctor() :
        WhitePoint(0.950456,1,1.088754), WhitePointDenom(WhitePoint(0) + 15*WhitePoint(1) + 3*WhitePoint(2)),
        WhitePointU (4*WhitePoint(0)/WhitePointDenom), WhitePointV (9*WhitePoint(1)/WhitePointDenom)
    {}
    
    result_type operator()(const argument_type & xyz) const;
  protected:
    const result_type WhitePoint; 
    const component_type WhitePointDenom;
    const component_type WhitePointU, WhitePointV;
};
// }}}

// {{{ XYZ2LabFunctor
class XYZ2LabFunctor: public ConvertColor
{
  public:
    XYZ2LabFunctor()
        : gamma_(1.0/3.0)
    {}
    
    /// apply the transformation
    result_type operator()(const argument_type & xyz) const;

  protected:
    component_type gamma_;
};
// }}}

// {{{ XYZ2LchFunctor
class XYZ2LchFunctor : public ConvertColor
{
  public:
    XYZ2LchFunctor() {}
    
    /// apply the transformation
    result_type operator()(const argument_type& xyz) const;

  protected: 
    XYZ2LabFunctor xyz2lab;
};
// }}}

// {{{ RGB2LuvFunctor
class RGB2LuvFunctor : public ConvertColor
{
    /*
    L in [0, 100]
    u in [-83.077, 175.015]
    v in [-134.101, 107.393]
    maximum saturation: 179.04 
    red = [53.2406, 175.015, 37.7522]
    */
  public:
    /// default constructor.  The maximum value for each RGB component defaults to 255.
    RGB2LuvFunctor()
    : rgb2xyz(255.0)
    {}
    
    /// constructor \arg max - the maximum value for each RGB component
    RGB2LuvFunctor(component_type max)
    : rgb2xyz(max)
    {}
    
    /// apply the transformation
    result_type operator()(const argument_type& rgb) const
    { return xyz2luv(rgb2xyz(rgb)); }

  protected:
    RGB2XYZFunctor rgb2xyz;
    XYZ2LuvFunctor xyz2luv;
};
// }}}

// {{{ RGB2LabFunctor
class RGB2LabFunctor: public ConvertColor
{
    /*
    L in [0, 100]
    a in [-86.1813, 98.2352]
    b in [-107.862, 94.4758] 
    maximum saturation: 133.809
    red = [53.2406, 80.0942, 67.2015]
    */
  public:
    /// default constructor.  The maximum value for each RGB component defaults to 255.
    RGB2LabFunctor()
    : rgb2xyz(255.0)
    {}
    
    /// constructor \arg max - the maximum value for each RGB component
    RGB2LabFunctor(component_type max)
    : rgb2xyz(max)
    {}
    
    /// apply the transformation
    result_type operator()(const argument_type& rgb) const
    { return xyz2lab(rgb2xyz(rgb)); }

  protected:
    RGB2XYZFunctor rgb2xyz;
    XYZ2LabFunctor xyz2lab;
};
// }}}

// {{{ RGB2LchFunctor
class RGB2LchFunctor: public ConvertColor
{
    /*
    L in [0, 100]
    h in [0, 360] 
    */
  public:
    /// default constructor.  The maximum value for each RGB component defaults to 255.
    RGB2LchFunctor()
    : rgb2xyz(255.0)
    {}
    
    /// constructor \arg max - the maximum value for each RGB component
    RGB2LchFunctor(component_type max)
    : rgb2xyz(max)
    {}
    
    /// apply the transformation
    result_type operator()(const argument_type& rgb) const
    { return xyz2lch(rgb2xyz(rgb)); }

  protected:
    RGB2XYZFunctor rgb2xyz;
    XYZ2LchFunctor xyz2lch;
};
// }}}

// {{{ RGB2XYZOldFunctor
class RGB2XYZOldFunctor: public ConvertColor
{
  public:
        /** default constructor.
            The maximum value for each RGB component defaults to 255.
        */
    RGB2XYZOldFunctor()
    : max_(255.0)
    {}
    
        /** constructor
            \arg max - the maximum value for each RGB component
        */
    RGB2XYZOldFunctor(component_type max)
    : max_(max)
    {}
    
        /** apply the transformation
        */
    result_type operator()(const argument_type & rgb) const;

  protected:
    component_type max_;
};
// }}}
// {{{ XYZ2LuvOldFunctor
class XYZOld2LuvOldFunctor: public ConvertColor
{
  public:
  
    XYZOld2LuvOldFunctor()
    : gamma_(1.0/3.0)
    {}
    
    result_type operator()(const argument_type& xyz) const;

  protected:
    component_type gamma_;
};
// }}}
// {{{ RGB2LuvOldFunctor
class RGB2LuvOldFunctor: public ConvertColor
{
    /*
    L in [0, 100]
    u in [-83.077, 175.015]
    v in [-134.101, 107.393]
    maximum saturation: 179.04 
    red = [53.2406, 175.015, 37.7522]
    */
  public:
        /** default constructor.
            The maximum value for each RGB component defaults to 255.
        */
    RGB2LuvOldFunctor()
    : rgb2xyz(255.0)
    {}
    
        /** constructor
            \arg max - the maximum value for each RGB component
        */
    RGB2LuvOldFunctor(component_type max)
    : rgb2xyz(max)
    {}
    
        /** apply the transformation
        */
    result_type operator()(const argument_type& rgb) const
    {
        return xyz2luv(rgb2xyz(rgb));
    }

  protected:
    RGB2XYZOldFunctor rgb2xyz;
    XYZOld2LuvOldFunctor xyz2luv;
};
// }}}
} // namespace lear 

#endif // _LEAR_COLORCONVERSIONS_H_


