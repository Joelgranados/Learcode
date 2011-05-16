#ifndef _LEAR_CACHE_DESC_H_
#define _LEAR_CACHE_DESC_H_

#include <sstream>
#include <algorithm>

#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <lear/exception.h>

namespace lear {

template<class RealType_,int N_>
class CacheDesc {  
    public:
        enum {N = N_, Rank =  N_};
        typedef RealType_                           RealType;
        typedef blitz::TinyVector<int,N>            IndexType;
        typedef blitz::Array<RealType_,2>           CacheType;

        CacheDesc(
                const int t_featsize_,
                const int t_cachesize_ = 16 // 16 MB
                )
            :
            cachesize_(t_cachesize_*1024*1024/(sizeof(RealType)*t_featsize_)), 
            featsize_(t_featsize_),
            toindex_(cachesize_),
            cache_(cachesize_,featsize_),
            fifoindex_(0)
#ifdef CACHE_BRIEF_DEBUG
                ,hitcount(0), totalcount(0)
#endif
        { toindex_ = -1; }
        
        template<class Op>
        blitz::Array<RealType,1> operator()( IndexType loc, Op op ) {
            using namespace blitz;

            if (tocache_(loc) < 0) { // not in cache
#ifdef CACHE_DEBUG
                std::cout << "Filling cache for point " << loc << std::endl;
#endif
#ifdef BZDEBUG
                if (op.size() !=  featsize_) {
                    std::ostringstream mesg;
                    mesg << "Cache and Operator dimension mismatch."
                        << "  Cache size = " << featsize_ 
                        << ", Operator size = " << op.size();
                    throw lear::Exception("CacheDesc", mesg.str());
                }
#endif
                if (toindex_(fifoindex_)[0] >= 0) 
                    tocache_(toindex_(fifoindex_)) = -1;

                RealType* s = op(loc);
                RealType* d = cache_.data()+fifoindex_*featsize_;
//                  cache_(fifoindex_,Range::all());
                std::copy(s,s+featsize_,d);

                tocache_(loc) = fifoindex_;
                fifoindex_ = (fifoindex_+1)%cachesize_;

#ifdef CACHE_BRIEF_DEBUG
                if (!fifoindex_) {
                    std::cout << "Relooping cache at location " << loc << std::endl;
                }
#endif
            }
#ifdef CACHE_BRIEF_DEBUG
            else {
                ++hitcount;
            }
#endif
#ifdef CACHE_BRIEF_DEBUG
            ++totalcount;
#endif
#ifdef CACHE_DEBUG
            std::cout << "Using cache for point " << loc << std::endl;
//            std::cout << "Old location " << 
//                (cache_.data()+tocache_(loc)*featsize_) << std::endl;
#endif
            return cache_(tocache_(loc), Range::all());
        }

        void clear(const IndexType extent) {
            if (blitz::sum(tocache_.extent() == extent) != 2) 
                tocache_.resize(extent);

            tocache_ = -1;
            toindex_ = -1;
            fifoindex_ = 0;
#ifdef CACHE_BRIEF_DEBUG
            std::cout << "Cache brief summary:: "
                "total count " << totalcount  << ", "
                "hit count " << 
                (totalcount==0?0:(static_cast<float>(hitcount)/totalcount))
                << std::endl;
            totalcount =0;
            hitcount = 0;
#endif
        }

    protected:
        int                                 cachesize_, featsize_; 

        blitz::Array<IndexType,1>           toindex_;
        blitz::Array<int,N>                 tocache_;

        CacheType                           cache_;
        int                                 fifoindex_;
#ifdef CACHE_BRIEF_DEBUG
        int                                 hitcount, totalcount;
#endif
};

}

#endif // _LEAR_CACHE_DESC_H_
