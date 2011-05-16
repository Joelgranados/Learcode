#ifndef _LEAR_ALIGN_IN_IMAGE_H_
#define _LEAR_ALIGN_IN_IMAGE_H_

#include <string>
#include <lear/classifier/ppresult.h>
#include <lear/classifier/histprocessor.h>
#include <blitz/tinyvec.h>

namespace lear {
    struct AlignInImage : public PostProcessResult, public HistProcessor{
        typedef PostProcessResult           Parent;
        typedef blitz::TinyVector<int,2>    IndexType;
        AlignInImage(
            const std::string outimage_,
            const bool outfileIsDir_,
            const IndexType margin_,
            const IndexType normalsize_,
            const std::string& histfile_="",
            const RealType minTh_ = -20,
            const RealType extTh_ = 40, 
            const RealType bandwidth_ = 0.01);

            virtual void write(
                const_iterator s, const_iterator e, 
                const std::string filename);

            virtual std::string toString() { 
                return "Align Input Image Post Process Result";
            }
        protected:
            const std::string outimage_;
            const bool outfileIsDir_;
            const IndexType margin_,normalsize_;
    };
}
#endif // _LEAR_ALIGN_IN_IMAGE_H_
