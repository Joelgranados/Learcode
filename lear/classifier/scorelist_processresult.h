#ifndef _LEAR_SCORE_LIST_PROCESS_RESULT_H_
#define _LEAR_SCORE_LIST_PROCESS_RESULT_H_

#include <lear/cvision/dhistogram.h>
#include <lear/classifier/th_processresult.h>

namespace lear {
struct ScoreList_ProcessResult : public Th_ProcessResult {
    public:
        typedef Th_ProcessResult                    Parent;

        ScoreList_ProcessResult(
                const std::string& listfile_, const RealType threshold_)
            :
            Parent(threshold_),
            to(listfile_.c_str())
        {
            if (!to) 
                throw lear::Exception("ScoreList_ProcessResult::constructor()",
                    "Unable to open output file " + listfile_);
        }
        virtual bool operator()(const DetectInfo& r){
            to << r.score << std::endl;
            return Parent::operator()(r);
        }
        virtual std::string toString() { 
            return "Score List Process Result";
        }

    protected:
     /// output stream
     std::ofstream to;
};
}
#endif //_LEAR_SCORE_LIST_PROCESS_RESULT_H_
