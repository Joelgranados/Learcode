#ifndef _LEAR_LOOKUP_H_
#define _LEAR_LOOKUP_H_

#include <blitz/tinyvec.h>
namespace lear {
// {{{ Constants
template<class T>
struct MathConst {
    typedef T           RealType;

    static const RealType PI_Value; // value of PI
    static const RealType Sqrt_Value;// square root of 2
};
// }}}
// {{{ Trignometry Table lookups
template<class RealType_>
class Trignometry {
    public:
        typedef RealType_           RealType;
        /**
         * Returns the cosine of angle. Angle must be in degrees 
         * and within the range given by maxAngle and minAngle
         */
        inline static RealType cos(int angle) {
#ifdef BZ_DEBUG
            if (!isAngleValid(angle)) {
                std::cerr << "Trignometry::cos(angle) is out of bounds. Current Angle " << angle << std::endl;
                std::abort();
            }
#endif
            return cos_[angle - minAngle];
        }
        inline static RealType sin(int angle) {
#ifdef BZ_DEBUG
            if (!isAngleValid(angle)) {
                std::cerr << "Trignometry::sin(angle) is out of bounds. Current Angle " << angle << std::endl;
                std::abort();
            }
#endif
            return sin_[angle - minAngle];
        }
        inline static RealType tan(int angle) {
#ifdef BZ_DEBUG
            if (!isAngleValid(angle)) {
                std::cerr << "Trignometry::tan(angle) is out of bounds. Current Angle " << angle << std::endl;
                std::abort();
            }
#endif
            return tan_[angle - minAngle];
        }
        inline static RealType cot(int angle) {
#ifdef BZ_DEBUG
            if (!isAngleValid(angle)) {
                std::cerr << "Trignometry::tan(angle) is out of bounds. Current Angle " << angle << std::endl;
                std::abort();
            }
#endif
            return cot_[angle - minAngle];
        }
        inline static bool isAngleValid(int angle) {
            return angle >= minAngle && angle < maxAngle;
        }

    private:
    static const int minAngle = -360;
    static const int maxAngle =  360;
    static const RealType cos_[];
    static const RealType sin_[];
    static const RealType tan_[];
    static const RealType cot_[];
};
// }}}
// {{{ Gauss Table lookups
template<class RealType>
class GaussLookup {
    public:
        /**
         * Returns the unit Gauss value 
         */
        inline static RealType value(const RealType at) {
            return (at < Range && at > -Range) ? 
                value_[ static_cast<int>(at*SampleRate) + Bound]: 0;

//              int x = static_cast<int>(at*SampleRate);
//              return (x < Bound && x > -Bound) ? value_[x + Bound]: 0;
        }

        inline static RealType value(const RealType at, const RealType Range) {
            return (at < Range && at > -Range) ? 
                value_[ static_cast<int>(at*SampleRate) + Bound]: 0;
        }


    private:
    static const int SampleRate = 1000;
    static const int Range = 5;
    static const int Bound = SampleRate*Range;

    static const RealType value_[];
};
// }}}

// {{{ Direction Table lookups
template<class RealType>
class Direction {
    public:
        typedef blitz::TinyVector<RealType,2> Real2DType;
        /**
         * Returns the direction vector for angle. Angle must be in degrees 
         * and within the range given by maxAngle and minAngle
         */
        inline static Real2DType vector(int angle) {
#ifdef BZ_DEBUG
            if (!isAngleValid(angle)) {
                std::cerr << "Direction::vector(angle) is out of bounds. Current Angle " << angle << std::endl;
                std::abort();
            }
#endif
            return vector_[angle - minAngle];
        }
        inline static bool isAngleValid(int angle) {
            return angle >= minAngle && angle < maxAngle;
        }

    private:
    static const int minAngle = -360;
    static const int maxAngle =  360;
    static const Real2DType vector_[];
};
// }}}

}
#include <lear/util/lookup.tcc>
#endif // _LEAR_LOOKUP_H_
