#ifndef _LEAR_MS_PROCESS_RESULT_H_
#define _LEAR_MS_PROCESS_RESULT_H_

#include <list>
#include <string>
#include <blitz/tinyvec.h>
#include <lear/cvision/transfunc.h>
#include <lear/classifier/th_processresult.h>

namespace lear {

struct MS_ProcessResult : public Th_ProcessResult {
    typedef Th_ProcessResult                    Parent;

    typedef Parent::RealType                    RealType;
    typedef Parent::DetectionWin                DetectionWin;
    typedef DetectionWin::const_iterator        const_iterator;
    typedef blitz::TinyVector<RealType,3>       PointType;
    typedef blitz::TinyVector<RealType,2>       SigmoidType;
    typedef blitz::TinyVector<int,2>            IndexType;

    MS_ProcessResult(
        const IndexType extent_,
        const RealType threshold_,
        const RealType finalthreshold_,
        const SigmoidType score2prob_,
        const PointType sigma_,
        const int transfunc);

    ~MS_ProcessResult() { delete sigmoid; }

    /** Do non-maximum suppression on the detection results */
    virtual void doit();
    virtual std::string toString() { 
        return std::string("Mean Shift NonMax Process Result\n    ")
            + sigmoid->toString();
    }

    protected:
        const IndexType extent_;
        const RealType finalthreshold_;
        const PointType sigma_;
        TransFunc<RealType> *sigmoid;
};
}
#endif // _LEAR_MS_PROCESS_RESULT_H_
