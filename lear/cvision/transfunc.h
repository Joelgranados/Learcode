#ifndef _LEAR_TRANSFUNC_H_
#define _LEAR_TRANSFUNC_H_

#include <cstdio>
namespace lear {

    template<class _RealType>
    struct TransFunc {
        typedef _RealType RealType;
        virtual RealType operator()(const RealType sc) const {
            return sc;
        }
        virtual std::string toString() const {
            return "t(w)=w";
        }
        virtual ~TransFunc() {}
    };
    template<class _RealType>
    struct Sigmoid : public TransFunc<_RealType> {
        typedef TransFunc<_RealType> Parent;
        typedef typename Parent::RealType RealType;
        Sigmoid(const RealType scale, const RealType center):
            scale(scale), center(center)
            {}

        virtual RealType operator()(const RealType sc) const {
            return 1/(1+std::exp(scale*sc+center));
        }
        virtual std::string toString() const {
            char s[1024];
            sprintf(s, "t(w)=1/(1+exp(%2.3f*w + %2.3f))",scale,center);
            return s;
        }
        protected:
        RealType scale;
        RealType center;
    };


    template<class _RealType>
    struct SoftMax : public Sigmoid<_RealType>{
        typedef Sigmoid<_RealType> Parent;
        typedef typename Parent::RealType RealType;
        SoftMax(const RealType scale, const RealType center):
            Parent(scale,center) {}

        virtual RealType operator()(const RealType sc) const {
            return std::log(1+std::exp(
                        Parent::scale*(sc-Parent::center)))/Parent::scale;
        }
        virtual std::string toString() const {
            char s[1024];
            sprintf(s, "t(w)=log(1+exp(%2.3f*(w - %2.3f))/%2.3f",
                        Parent::scale,Parent::center,Parent::scale);
            return s;
        }
    };

    template<class _RealType>
    struct HardMax : public Sigmoid<_RealType> {
        typedef Sigmoid<_RealType> Parent;
        typedef typename Parent::RealType RealType;
        HardMax(const RealType scale, const RealType center):
            Parent(scale,center) {}

        virtual RealType operator()(const RealType sc) const {
            return sc > Parent::center ? 
                Parent::scale*(sc-Parent::center) : 0;
        }
        virtual std::string toString() const {
            char s[1024];
            sprintf(s, "t(w)=max(%2.3f*(w - %2.3f),0)",Parent::scale,Parent::center);
            return s;
        }
    };
}
#endif
