#ifndef _LEAR_WIN_DESCRIPTOR_H_
#define _LEAR_WIN_DESCRIPTOR_H_

#include <list>

#include <blitz/tinyvec.h>
#include <lear/exception.h>

#include <lear/io/biostream.h>
#include <lear/cvision/densegrid.h>
#include <lear/cvision/cachedesc.h>

#include <lear/cvision/rhogdense.h>
#include <lear/cvision/iprocessor.h>
namespace lear {
/** WinDescriptor
 */
class WinDescriptor {  
    protected:
        struct DescOp;

    public:
        typedef IProcessor::RealType                RealType; 

        typedef IProcessor::GrayType                GrayType;
        typedef IProcessor::RGBType                 RGBType;

        typedef IProcessor::GrayImage               GrayImage;
        typedef IProcessor::RGBImage                RGBImage;

        typedef RGBImage                            ImageType;


        typedef RHOGDense                           DescType;
        typedef DenseGrid<2>::GridType              GridType;

        typedef std::list<DescType*>                DescCont; 
        typedef std::list<GridType >                GridCont; 


        typedef DescType::ElemType                  ElemType;
        typedef blitz::TinyVector<int,2>            IndexType;

        /// Combined big feature type for all points on the grid
        typedef blitz::Array< ElemType, 1>          FeatType;
        typedef blitz::Array< IndexType,1>          IndexArray;

        /// Singular feature type, i.e. feature type at each point
        typedef DescType::FeatType         SingFeatType;

        typedef std::list<DescType::Preprocessor>    Preprocessor;
        
    public:

        WinDescriptor(
                const IndexType t_extent_, 
                const DescCont& t_desc_,
                const GridCont& t_grid_,
                const int cachesize = 16
                );
        ~WinDescriptor() {}

        Preprocessor& preprocess( const GrayImage& image) ;
        Preprocessor& preprocess( const RGBImage& image) ;
        FeatType compute( const IndexType gridTopLeft) const ;

        IndexType extent() const { return extent_; }
        int length() const { return length_; }

        void print(std::ostream& o) const ;
        void print(lear::BiOStream& o) const ;

    protected:
        typedef CacheDesc<ElemType,2>               CacheType;
        typedef std::list< CacheType >              CacheCont;

        /// window size
        const IndexType     extent_;
        
        /// descriptor array object
        const DescCont      desc_;

        /// compute descriptor over points in this grid
        const GridCont      grid_;

        /// output description length
        int initlength_, length_, numItem_;

        IndexArray          indexrange_;

        Preprocessor preprocessor;
        
        mutable CacheCont   cache_;

        std::string title() const {
            return "Win Descriptor ::       ";
        }
        lear::FileHeader header() const ;

        struct DescOp {// {{{
            typedef DescType::Preprocessor Processor;
            const DescType* d;
            const Processor& p;

            DescOp(const DescType* d, const Processor& p)
                : d(d),p(p){}
            ElemType* operator()(const IndexType loc) const {
                return (*d)(loc,p).data();
            }
            int size() const { return d->size(); }
        };// }}}

        template <class PixelType>
        Preprocessor& template_preprocess( const blitz::Array<PixelType,2>& image) ;
};

}
std::ostream& operator<<( std::ostream& o, const lear::WinDescriptor& w) ;
lear::BiOStream& operator<<( lear::BiOStream& o, const lear::WinDescriptor& w) ;

#endif // _LEAR_WIN_DESCRIPTOR_H_

