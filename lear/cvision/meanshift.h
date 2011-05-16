// {{{ file documentation
/**
 * @file
 * @brief Compute Mean Shift's for general arrays
 */
// }}}

#ifndef _LEAR_MEANSHIFT_H_
#define _LEAR_MEANSHIFT_H_

//#define CONVERGETOMODE_DEBUG
// #define TIMER

#include <list>
#include <algorithm>
#include <functional>
#include <lear/util/functional.h>

#ifdef TIMER
#include <iostream>
#include <boost/timer.hpp>
#endif

namespace lear {

template<class RealType_>
struct Basic1DKernel {// {{{
    typedef RealType_                       RealType;
    typedef RealType                        PointType;

    Basic1DKernel(const PointType sigma): 
        sigma(sigma) {}

    template<class Pt, class Wt, class Ref>
    inline PointType nvalue(Ref rf, Pt first, Pt last, Wt wt) const {
        using namespace std;
        PointType numer(.0);
        PointType denom(.0);

        for (;first != last; ++first, ++wt) {
            PointType x = (*first - rf)/sigma;

            RealType w = (*wt)*exp(-x*x/2);

            numer += w*(*first);
            denom += w;
            //std::cout << "Loc " << *first << " | ref " << rf << " | sigma " << sigma << " | wt " << *wt << " | w " << w << std::endl;
        }
        numer /= denom;
        //std::cout << "Final value " << numer << std::endl;
        return numer;
    } 
    template<class Pt, class Wt, class Ref>
    inline RealType fvalue(Ref rf, Pt first, Pt last, Wt wt) const {
        RealType numer=0;

        for (;first != last; ++first, ++wt) {
            PointType x = (*first - rf)/sigma;
            numer+=(*wt)*std::exp(-x*x/2)/(2*3.1415926536*sigma); 
        }
        return numer;
    } 
    RealType distsq(PointType a, PointType b) const {
        b-=a;
        b/=sigma;
        return b*b;
    }
    protected:
    PointType sigma;
};// }}}

// Mean Shift class
//{{{ documentation
/**
 * Compute Mean shift vectors for an image.
 * weight vector and position vector must be of equal size
 */
//}}}

/**
 * Implements basic mean shift operations without weight function
 */
template<class Kernel_, class PtContType_>
class BasicMeanshift{// {{{
    // Meanshift typedefs
    public:
        typedef Kernel_                                 Kernel;
        typedef typename Kernel::RealType               RealType;
        typedef typename Kernel::PointType              PointType;

        typedef PtContType_                             PtContType;

        typedef BasicMeanshift<Kernel, PtContType>      ThisType;

    // variables private
    protected:
        /// reference to position vector
        const PtContType&   at;

        /// smooth kernel to use
        const Kernel&       kernel;

	const RealType      modeEpsilon;

        const int           maxIterations;

    public:
    // Constructors
    BasicMeanshift(
            const PtContType& at_,
            const Kernel&   kernel_,
            const RealType modeEpsilon_ = 1e-4,
            const int maxIterations_ = 20
            ): 
        at(at_), kernel(kernel_),
        modeEpsilon(modeEpsilon_), 
        maxIterations (maxIterations_)
    {}

    //{{{ Converge to Mode
    inline PointType shiftToMode(PointType I) {
        int count = 0;

        // store previous value 
        PointType i;
        do {
            i = I;
            I = ThisType::nvalue(i);
            ++count;
        } while ( kernel.distsq(I,i) > modeEpsilon 
                && count < maxIterations) ;
#ifdef CONVERGETOMODE_DEBUG
        std::cout << "< " <<std::setw(5) << count << " > ";
#endif
        return I;
    }
    //}}}

    /**
     * Compute the new location of the vector. 
     * To compute mean shift, subtract position vector pt.
     */
    inline PointType nvalue(PointType pt) {
        return kernel.nvalue(pt,at.begin(),at.end());
    }
    inline RealType fvalue(PointType pt) {
        return kernel.fvalue(pt,at.begin(),at.end());
    }
};// }}}

/**
 * Implements basic mean shift operations with weight function
 */
template<class Kernel_, class PtContType_, class WtContType_>
class BasicWtMeanshift {// {{{
    public:
    //{{{ Meanshift typedefs
        typedef Kernel_                                 Kernel;
        typedef typename Kernel::RealType               RealType;
        typedef typename Kernel::PointType              PointType;

        typedef PtContType_                             PtContType;
        typedef WtContType_                             WtContType;

        typedef BasicWtMeanshift<Kernel, PtContType, WtContType> 
                                                        ThisType;
    //}}}

    // variables private
    protected:
        /// reference to weight vector 
        const WtContType&   wt;

        /// reference to position vector
        const PtContType&   at;

        /// smooth kernel to use
        const Kernel&       kernel;

	const RealType      modeEpsilon;

        const int           maxIterations;

    public:
    // Constructors
    BasicWtMeanshift(
            const WtContType& wt_,
            const PtContType& at_,
            const Kernel&   kernel_,
            const RealType modeEpsilon_ = 1e-4,
            const int maxIterations_ = 20
            ): 
        wt(wt_), at(at_), kernel(kernel_),
        modeEpsilon(modeEpsilon_), 
        maxIterations (maxIterations_)
    {}
    //{{{ Converge to Mode
    inline PointType shiftToMode(PointType I) {
        int count = 0;

        // store previous value 
        PointType i;
        do {
            i = I;
            I = ThisType::nvalue(i);
            ++count;
#ifdef CONVERGETOMODE_DEBUG
        std::cout << "< " <<std::setw(5) << count << ": from " << i << " to " << I << " > " << std::endl;
#endif
        } while ( kernel.distsq(I,i) > modeEpsilon 
                && count < maxIterations) ;
#ifdef CONVERGETOMODE_DEBUG
        std::cout << "< " <<std::setw(5) << count << " > " << std::endl;
#endif
        return I;
    }
    //}}}

    /**
     * Compute the new location of the vector. 
     * To compute mean shift, subtract position vector pt.
     */
    inline PointType nvalue(PointType pt) {
        return kernel.nvalue(pt,
               at.begin(),at.end(),wt.begin());
    }
    inline RealType fvalue(PointType pt) {
        return kernel.fvalue(pt,
                at.begin(),at.end(),wt.begin());
    }
};// }}}

//template<class Kernel_, class PtContType_, class WtContType_>
//class Meanshift:public BasicWtMeanshift<Kernel_,PtContType_,WtContType_>{// {{{
//    //{{{ Meanshift typedefs
//    public:
//    //{{{ Meanshift typedefs
//        typedef Kernel_                                 Kernel;
//        typedef typename Kernel::RealType               RealType;
//        typedef typename Kernel::PointType              PointType;
//
//        typedef PtContType_                             PtContType;
//        typedef WtContType_                             WtContType;
//
//        typedef BasicWtMeanshift<Kernel_,PtContType_,WtContType_>     
//                                                        Parent;
//
//        typedef Meanshift<Kernel, PtContType, WtContType> ThisType;
//
//        // Mode type
//        typedef std::list<PointType >                   Cluster;
//        typedef std::list<Cluster >                     ClusterList;
//    //}}}
//
//    //{{{ variables private
//    private:
//        /// number of elements in at vector
//        int numelem;
//
//        /// stores the meanshift vector;
//        PtContType          ms; 
//        /// stores the distance vector to mode for each point
//        PtContType          tomode;
//    //}}}
//
//    public:
//    //{{{ Constructors
//    Meanshift(
//            const WtContType& wt_,
//            const PtContType& at_,
//            const Kernel&   kernel_,
//
//            const RealType modeEpsilon_ = 1e-4,
//            const int maxIterations_ = 20
//            ): 
//        Parent(wt_,at_,kernel_,modeEpsilon_,maxIterations_),
//        numelem(at_.size()),
//        ms(numelem), tomode(numelem)
//    {
//#ifdef TIMER
//        boost::timer stopwatch;
//#endif
//
//        computeNewValue();
//
//#ifdef TIMER
//        std::cout << "Meanshift computed in " << 
//            std::setw(7) << std::fixed << std::setprecision(3) <<
//            stopwatch.elapsed() << "s " << std::flush;
//        stopwatch.restart();
//#endif
//    
//        convergeToMode();
//
//#ifdef TIMER
//        std::cout << "| Points converged in " << 
//            std::setw(7) << std::fixed << std::setprecision(3) << 
//            stopwatch.elapsed() << "s " << std::endl << std::flush;
//#endif
//        std::transform(ms.begin(),ms.end(),Parent::at.begin(),
//                ms.begin(),std::minus<PointType>());
//    }
//    //}}} constructors
//
//    //{{{ get Modes 
//    template<class ModeCont>
//    inline void getModes(ModeCont& mode, const RealType epsilon) {
//#ifdef TIMER
//        boost::timer stopwatch;
//#endif
//        // fill up clusters
//        for (typename PtContType::const_iterator pt = tomode.begin();
//                pt != tomode.end(); ++pt) 
//        {
//            if (std::find_if(mode.begin(),mode.end(), 
//                FindMode(Parent::kernel, *pt, epsilon)) == mode.end()) 
//            {
//                mode.push_back(*pt);
//            }
//        }
//#ifdef TIMER
//        std::cout << "Modes found in " << 
//            std::setw(7) << std::fixed << std::setprecision(3) <<
//            stopwatch.elapsed() << "s " << std::endl << std::flush;
//#endif
//    }
//    // }}}
//    //{{{ get Modes 
//    template<class ModeCont, class ValueCont>
//    void getModes(ModeCont& mode, ValueCont& value, const RealType epsilon) {
//        getModes(mode,epsilon);
//
//        value.resize(mode.size());
//
//        std::transform(mode.begin(),mode.end(),value.begin(), 
//                op_mem_fun(this,(&Parent::fvalue)));
//    }
//    // }}}
//
////      //{{{ util functions
////      inline const PtContType& meanshift() const {
////          return ms;
////      }
//
////      inline const PtContType& convergedpoints() const {
////          return tomode;
////      }
////      //}}}
//
//    private:
//    struct FindMode {
//        FindMode(
//                const Kernel& kernel, 
//                const PointType p, 
//                const RealType epsilon) :
//            kernel(kernel), p(p), epsilon(epsilon*epsilon)
//            {}
//
//        bool operator()(const PointType m) const {
//            return kernel.distsq(p,m) < epsilon;
//        }
//        const Kernel& kernel;
//        const PointType p;
//        const RealType epsilon;
//    };
//    // compute meansift at all position vectors
//    inline void computeNewValue() {
//        std::transform(Parent::at.begin(),Parent::at.end(),ms.begin(), 
//                op_mem_fun(this,(&Parent::nvalue)));
//    }
//
//    // shift to Mode i.e. distance vector to mode
//    inline void convergeToMode() {
//        std::transform(ms.begin(),ms.end(),tomode.begin(),
//                op_mem_fun(this,(&Parent::shiftToMode)));
//    }
//};// }}}

template<class Kernel_, class PtContType_, class WtContType_>
class Meanshift{// {{{
    //{{{ Meanshift typedefs
    public:
        typedef Kernel_                                 Kernel;
        typedef typename Kernel::RealType               RealType;
        typedef typename Kernel::PointType              PointType;

        typedef PtContType_                             PtContType;
        typedef WtContType_                             WtContType;

        typedef Meanshift<Kernel, PtContType, WtContType> ThisType;
        // Mode type
        typedef std::list<PointType >                   Cluster;
        typedef std::list<Cluster >                     ClusterList;
    //}}}

    //{{{ variables private
    private:
        /// reference to weight vector 
        const WtContType&   wt;
        /// reference to position vector
        const PtContType&   at;

        /// smooth kernel to use
        const Kernel&       kernel;

        /// number of elements in at vector
        int numelem;

        /// stores the meanshift vector;
        PtContType          ms; 
        /// stores the distance vector to mode for each point
        PtContType          tomode;

	const RealType      modeEpsilon;

        const int           maxIterations;
    //}}}

    public:
    //{{{ Constructors
    Meanshift(
            const WtContType& wt_,
            const PtContType& at_,
            const Kernel&   kernel_,

            const RealType modeEpsilon_ = 1e-4,
            const int maxIterations_ = 20
            ): 
        wt(wt_),at(at_), kernel(kernel_), numelem(at_.size()),
        ms(numelem), tomode(numelem),

        modeEpsilon(modeEpsilon_), 
        maxIterations (maxIterations_)
    {
#ifdef TIMER
        boost::timer stopwatch;
#endif

        computeNewValue();

#ifdef TIMER
        std::cout << "Meanshift computed in " << 
            std::setw(7) << std::fixed << std::setprecision(3) <<
            stopwatch.elapsed() << "s " << std::flush;
        stopwatch.restart();
#endif
    
        convergeToMode();

#ifdef TIMER
        std::cout << "| Points converged in " << 
            std::setw(7) << std::fixed << std::setprecision(3) << 
            stopwatch.elapsed() << "s " << std::endl << std::flush;
#endif
        std::transform(ms.begin(),ms.end(),at.begin(),
                ms.begin(),std::minus<PointType>());
    }
    //}}} constructors

    //{{{ get Modes 
    template<class ModeCont>
    inline void getModes(ModeCont& mode, const RealType epsilon) {
#ifdef TIMER
        boost::timer stopwatch;
#endif
        // fill up clusters
        for (typename PtContType::const_iterator pt = tomode.begin();
                pt != tomode.end(); ++pt) 
        {
            if (std::find_if(mode.begin(),mode.end(), 
                        FindMode(kernel, *pt, epsilon)) == mode.end()) 
            {
                mode.push_back(*pt);
            }
        }
#ifdef TIMER
        std::cout << "Modes found in " << 
            std::setw(7) << std::fixed << std::setprecision(3) <<
            stopwatch.elapsed() << "s " << std::endl << std::flush;
#endif
    }
    // }}}
    //{{{ get Modes 
    template<class ModeCont, class ValueCont>
    void getModes(ModeCont& mode, ValueCont& value, const RealType epsilon) {
        getModes(mode,epsilon);

        value.resize(mode.size());

        std::transform(mode.begin(),mode.end(),value.begin(), 
                op_mem_fun(this,(&ThisType::fvalue)));
    }
    // }}}

//      //{{{ util functions
//      inline const PtContType& meanshift() const {
//          return ms;
//      }

//      inline const PtContType& convergedpoints() const {
//          return tomode;
//      }
//      //}}}

    //{{{ Converge to Mode
    inline PointType shiftToMode(PointType I) {
        int count = 0;

        // store previous value 
        PointType i;
        do {
            i = I;
            I = nvalue(i);
            ++count;
        } while ( kernel.distsq(I,i) > modeEpsilon && count < maxIterations) ;
#ifdef CONVERGETOMODE_DEBUG
        std::cout << "< " <<std::setw(5) << count << " > ";
#endif
        return I;
    }
    //}}}

    private:
    struct FindMode {
        FindMode(
                const Kernel& kernel, 
                const PointType p, 
                const RealType epsilon) :
            kernel(kernel), p(p), epsilon(epsilon*epsilon)
            {}

        bool operator()(const PointType m) const {
            return kernel.distsq(p,m) < epsilon;
        }
        const Kernel& kernel;
        const PointType p;
        const RealType epsilon;
    };
    /**
     * Compute the new location of the vector. 
     * To compute mean shift, subtract position vector pt.
     */
    inline PointType nvalue(PointType pt) {
        return kernel.nvalue(pt,at.begin(),at.end(),wt.begin());
    }
    inline RealType fvalue(PointType pt) {
        return kernel.fvalue(pt,at.begin(),at.end(),wt.begin());
    }

    // compute meansift at all position vectors
    inline void computeNewValue() {
        std::transform(at.begin(),at.end(),ms.begin(), 
                op_mem_fun(this,(&ThisType::nvalue)));
    }

    // shift to Mode i.e. distance vector to mode
    inline void convergeToMode() {
        std::transform(ms.begin(),ms.end(),tomode.begin(),
                op_mem_fun(this,(&ThisType::shiftToMode)));
    }
};// }}}

}

#endif // #ifndef _LEAR_MEANSHIFT_H_


