#ifndef _LEAR_TH_PROCESS_RESULT_H_
#define _LEAR_TH_PROCESS_RESULT_H_

#include <list>
#include <blitz/tinyvec.h>
#include <lear/classifier/processresult.h>

namespace lear {

    struct Th_ProcessResult : public ProcessResult {
        typedef ProcessResult                       Parent;
        typedef Parent::RealType                    RealType;
        typedef Parent::DetectionWin                DetectionWin;
        typedef DetectionWin::const_iterator        const_iterator;

        Th_ProcessResult(
                const RealType threshold_,
                const int label_=1
                )
                :
            ProcessResult(),
            totwindow_(0),
            threshold_(threshold_),
            label_(label_)
        {}

        virtual bool operator()(const DetectInfo& r){
            ++totwindow_;
            
            if ( ( label_ && r.score > threshold_) || 
                 (!label_ && r.score < threshold_) ) 
            {
                return Parent::operator()(r);
            }
            return false;
        }
        virtual std::string toString() { 
            return "Threshold Process Result";
        }

        protected:
            int totwindow_;
            const RealType threshold_;

            const int label_;
    };
}
#endif // _LEAR_TH_PROCESS_RESULT_H_
