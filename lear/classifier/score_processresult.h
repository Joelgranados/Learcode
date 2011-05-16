#ifndef _LEAR_SCORE_PROCESS_RESULT_H_
#define _LEAR_SCORE_PROCESS_RESULT_H_

#include <lear/classifier/processresult.h>
#include <fstream>

namespace lear {
struct Score_ProcessResult : public ProcessResult{
    public:
        typedef ProcessResult                Parent;
        typedef ProcessResult::RealType      RealType;
        Score_ProcessResult(const std::string& scorefile_): 
            Parent(), to(scorefile_.c_str())
        {
            if (!to) {
            throw lear::Exception("Score_ProcessResult::constructor()",
                "Unable to open output file " + scorefile_);
            }
        }
        virtual bool operator()(const DetectInfo& r) {
            to << r.score << std::endl;
            return true;
        }
        virtual std::string toString() { 
            return "Score Process Result";
        }
    private:
        std::ofstream to;
};
}
#endif //_LEAR_SCORE_PROCESS_RESULT_H_
