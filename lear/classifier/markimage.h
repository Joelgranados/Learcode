#ifndef _LEAR_MARK_IMAGE_H_
#define _LEAR_MARK_IMAGE_H_

#include <string>
#include <lear/classifier/ppresult.h>

namespace lear {
    struct MarkImage : public PostProcessResult {
        typedef PostProcessResult                    Parent;
        MarkImage(
            const std::string outimage_,
            const bool outfileIsDir_,
            const bool hidescore_);

            virtual void write(
                const_iterator s, const_iterator e, 
                const std::string filename);

            bool static verifyImageOut(
                const std::string outimage_,
                const bool outfileIsDir_);

            virtual std::string toString() { 
                return "Mark Image Post Process Result";
            }
        protected:
            const std::string outimage_;
            const bool outfileIsDir_;
            const bool hidescore_;
    };
}
#endif // _LEAR_MARK_IMAGE_H_
