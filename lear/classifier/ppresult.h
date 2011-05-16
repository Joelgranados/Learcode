#ifndef _LEAR_POST_PROCESS_RESULT_H_
#define _LEAR_POST_PROCESS_RESULT_H_

#include <string>

#include <lear/classifier/processresult.h>
namespace lear {
    class PostProcessResult{
        public:
            typedef ProcessResult::DetectionWin         DetectionWin;
            typedef DetectionWin::const_iterator        const_iterator;

            PostProcessResult(){}
            virtual ~PostProcessResult() {}

            /** Write detection results, must be invoked after invoking
            * operator() on all tested window to write corresponding detection 
            * results. 
            */
            virtual void write(
                const_iterator s, const_iterator e, 
                const std::string filename="")=0;

            virtual std::string toString() { return "Post Process Result";}
    };

}
#endif // _LEAR_POST_PROCESS_RESULT_H_
