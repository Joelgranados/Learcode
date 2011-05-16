#ifndef _LEAR_HISTOGRAM_PROCESS_RESULT_H_
#define _LEAR_HISTOGRAM_PROCESS_RESULT_H_

#include <lear/cvision/dhistogram.h>
#include <lear/classifier/histprocessor.h>
#include <lear/classifier/th_processresult.h>

namespace lear {
struct Hist_ProcessResult : public Th_ProcessResult, public HistProcessor {
    public:
        typedef Th_ProcessResult                Parent;
        typedef Th_ProcessResult::RealType      RealType;
        Hist_ProcessResult(
                const std::string& histfile_,
                const RealType threshold_,
                const int label_,
                const RealType minTh_ = -20,
                const RealType extTh_ = 40, 
                const RealType bandwidth_ = 0.01):
            Parent(threshold_,label_),
            HistProcessor(histfile_,minTh_,extTh_,bandwidth_)
        {}
        virtual bool operator()(const DetectInfo& r) {
            HistProcessor::operator()(r.score);
            return Parent::operator()(r);
        }
        virtual std::string toString() { 
            return "Histogram Process Result";
        }
};
}
#endif //_LEAR_HISTOGRAM_PROCESS_RESULT_H_
