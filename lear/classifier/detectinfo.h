#ifndef _LEAR_DETECT_INFO_H_
#define _LEAR_DETECT_INFO_H_

#include <iostream>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

namespace lear {
    struct DetectInfo {
        typedef blitz::TinyVector<int,2>            IndexType;

        float score, scale;
        IndexType lbound, extent, orig_lbound;

        DetectInfo() : score(0), scale(0), lbound(0), extent(0) {}
        DetectInfo(
                const float score, const float scale,
                const IndexType l, const IndexType e, const IndexType olb=0):
            score(score), scale(scale), lbound(l), extent(e), orig_lbound(olb)
        {}

        DetectInfo(const float p, const float s, 
                const int x, const int y, const int w, const int h)
                : 
            score(p), scale(s), lbound(x,y), extent(w,h) {}
        void print(std::ostream& o) const;

    };
std::ostream& operator<< (std::ostream& o, const DetectInfo& r);
}

#endif // _LEAR_DETECT_INFO_H_
