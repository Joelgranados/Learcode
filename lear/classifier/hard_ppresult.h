#ifndef _LEAR_HARD_POST_PROCESS_RESULT_H_
#define _LEAR_HARD_POST_PROCESS_RESULT_H_

#include <fstream>
#include <lear/classifier/ppresult.h>
namespace lear {
    struct Hard_PPResult : PostProcessResult {
        typedef PostProcessResult                       Parent;

        Hard_PPResult(const std::string& hardfile_);

        /** Write detection results, must be invoked after invoking
        * operator() on all tested window to write corresponding detection 
        * results. 
        */
        virtual void write(
            const_iterator s, const_iterator e, 
            const std::string filename);

        virtual std::string toString() { 
            return "Neg Hard List Post Process Result";
        }
        protected:
            std::ofstream to;
    };

}
#endif // _LEAR_HARD_POST_PROCESS_RESULT_H_
