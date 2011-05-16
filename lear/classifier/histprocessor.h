#ifndef _LEAR_HISTOGRAM_PROCESSOR_H_
#define _LEAR_HISTOGRAM_PROCESSOR_H_

#include <lear/cvision/dhistogram.h>

namespace lear {
struct HistProcessor {
        typedef float                               RealType;
        HistProcessor(
                const std::string& histfile_,
                const RealType minTh_ = -20,
                const RealType extTh_ = 40, 
                const RealType bandwidth_ = 0.01);
        virtual ~HistProcessor();
        const lear::DiscreteHistogram<int,RealType>::HistType& histogram() const {
            return histall_.data();
        }
        virtual void operator()(const RealType r);
        virtual std::string toString() { 
            return "Histogram Processor";
        }

    protected:
     /// output file
     const std::string histfile_;

     const bool dohistout_;
     /// min, max threshold and bandwidth for computing score histogram
     const RealType minTh_, maxTh_;

     /// score histogram, can be used to compute ROC curves
     lear::DiscreteHistogram<int,RealType> histall_;
};
}
#endif //_LEAR_HISTOGRAM_PROCESSOR_H_
