#include <cmath>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <lear/exception.h>
#include <lear/cvision/meanshift.h>

using namespace std;

#include <lear/classifier/ms_processresult.h>

template<class RealType_,int N>
struct DensityKernel {// {{{
    typedef RealType_                       RealType;
    typedef blitz::TinyVector<RealType,N>   PointType;

    DensityKernel(const PointType sigma): 
        sigma(sigma) 
//         norm (
//             std::pow(2*lear::MathConst<RealType>::PI,
//             static_cast<RealType>(N/2.0)))
    {}

    template<class Pt, class Wt, class Ref>
    inline PointType nvalue(Ref rf, Pt first, Pt last, Wt wt) const {
        using namespace std;
        PointType numer(.0);
        PointType denom(.0);

        for (;first != last; ++first, ++wt) {
            PointType x = *first;
            PointType r = rf;
            PointType ns = sigma;
            ns[0] *= exp(x[2]); ns[1] *= exp(x[2]);

            x/=ns;
            r/=ns;
            // These are right equations. But I realised this mistake quite late.
            // Keep the old code as parameters have been tuned w.r.t. that.
            //RealType w = (*wt)*exp(-dot(x-r,x-r)/2)/product(ns);

            //numer += w*x/ns;
            //denom += w/(ns*ns);

            RealType w = (*wt)*exp(-dot(x-r,x-r)/2)/sqrt(product(ns));

            numer += w*x;
            denom += w/ns;
        }
        numer /= denom;
        return numer;
    } 
    template<class Pt, class Wt, class Ref>
    inline RealType fvalue(Ref rf, Pt first, Pt last, Wt wt) const {
        RealType numer=0;

        for (;first != last; ++first, ++wt) {
            PointType x = *first;
            PointType ns = sigma;
            ns[0] *= exp(x[2]); ns[1] *= exp(x[2]);

            x-= rf;
            x/=ns;

            //numer+=(*wt)*std::exp(-dot(x,x)/2)/product(ns);
            numer+=(*wt)*std::exp(-dot(x,x)/2)/std::sqrt(product(ns));
        }
        return numer;
    } 
    RealType distsq(PointType a, PointType b) const {
        PointType ns = sigma;
        ns[0] *= exp(b[2]);
        ns[1] *= exp(b[2]);
        b-=a;
        b/=ns;
        return blitz::dot(b,b);
//          return blitz::dot(a-b,a-b);
    }
    protected:
    PointType sigma;
//     RealType  norm;
};// }}}

lear::MS_ProcessResult::MS_ProcessResult(
    const IndexType extent_,
    const RealType threshold_,
    const RealType finalthreshold_,
    const SigmoidType score2prob_,
    const PointType sigma_,
    const int transfunc)
        :
    Parent(threshold_),
    extent_(extent_),
    finalthreshold_(finalthreshold_),
    sigma_(sigma_)
{
    switch (transfunc) {
        case 1:
        sigmoid=new Sigmoid<RealType>(score2prob_[0],score2prob_[1]);
        break;
        case 2:
        sigmoid=new SoftMax<RealType>(score2prob_[0],score2prob_[1]);
        break;
        case 0:
        default:
        sigmoid=new HardMax<RealType>(score2prob_[0],threshold_);
        break;
    };
}
void lear::MS_ProcessResult::doit()
{// {{{
    using namespace lear;
    using namespace std;

    if (!detect_.empty()) {

        vector<PointType> at(numdetection_);
        vector<RealType > wt(numdetection_);
        {
            unsigned index = 0;
            for (const_iterator i=detect_.begin(); 
                    i != detect_.end(); ++i,++index) 
            {
                wt[index] = (*sigmoid)(i->score);
                blitz::TinyVector<RealType,2> cen = i->lbound+i->extent/2.0;
                at[index] = PointType(cen[0],cen[1],std::log(i->scale));
            }
        }
        PointType nsigma = sigma_;
        nsigma[2] = log(sigma_[2]);

        typedef DensityKernel<RealType,3>           KernelType;
        typedef lear::Meanshift< KernelType,
                vector<PointType>, vector<RealType> >  MeanShiftType;

        KernelType kernel(nsigma);
        MeanShiftType ms(wt,at, kernel,1e-5,100);
            
        vector<PointType> mode; vector<RealType> value;
        ms.getModes(mode, value, 1);

        detect_.clear();

        for (unsigned i= 0; i< mode.size(); ++i) {
            typedef blitz::TinyVector<RealType,2> Real2DType;

            RealType scale = exp(mode[i][2]);
            Real2DType c (mode[i][0],mode[i][1]);
            Real2DType s (extent_*scale);

            DetectInfo d(
                    static_cast<float>(value[i]),
                    static_cast<float>(scale),
                    blitz::ceil(c - s/2), blitz::floor(s));

#ifdef CONVERGETOMODE_DEBUG
            cout << "Mode " << d << endl;
#endif
            if (value[i] > finalthreshold_) {
                detect_.push_back(d);
            }
        } 
    }
}// }}}


