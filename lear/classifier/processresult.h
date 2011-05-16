#ifndef _LEAR_PROCESS_RESULT_H_
#define _LEAR_PROCESS_RESULT_H_

#include <list>
#include <lear/classifier/detectinfo.h>

namespace lear {

    struct ProcessResult {
        typedef float                               RealType;
        typedef std::list<DetectInfo>               DetectionWin;
        typedef DetectionWin::const_iterator        const_iterator;

        ProcessResult(): numdetection_(0){}
        virtual ~ProcessResult() {}

        virtual void doit(){}
        virtual void clear(){ numdetection_ = 0; detect_.clear(); }
        virtual bool operator()(const DetectInfo& r){
            ++numdetection_; detect_.push_back(r);
            return true;
        }

        DetectionWin::const_iterator begin() const {
            return detect_.begin();
        }
        DetectionWin::const_iterator end() const {
            return detect_.end();
        }

        int numdetection() const {return numdetection_;}

        virtual std::string toString() { return "Process Result";}
        protected:
            /// detection result windows
            DetectionWin detect_;
            int numdetection_;
    };
}
#endif // _LEAR_PROCESS_RESULT_H_
