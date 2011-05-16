/*
 * =====================================================================================
 *
 *       Filename:  windescriptor.cpp
 *
 *    Description:  Provides implementation to windescriptor.h
 *
 * =====================================================================================
 */

#include <sstream>
#include <algorithm>
#include <functional>

#include <lear/blitz/ext/globalfunc.h>
#include <lear/blitz/ext/tvmutil.h>

#include <lear/blitz/tvmio.h>
#include <lear/io/ioext.h>
#include <lear/io/fileheader.h>

#include <lear/cvision/windescriptor.h>

using namespace lear;
typedef WinDescriptor::DescCont::const_iterator   DescIter;
typedef WinDescriptor::GridCont::const_iterator   GridIter;

WinDescriptor::WinDescriptor(
        const IndexType t_extent_, 
        const DescCont& t_desc_,
        const GridCont& t_grid_,
        const int cachesize
        )
    :
    extent_(t_extent_), 
    desc_(t_desc_),
    grid_(t_grid_),
    initlength_(0),
    length_(0),
    numItem_(desc_.size()),
    indexrange_(numItem_)
{
    if (static_cast<int>(grid_.size()) != numItem_) 
    {
        std::ostringstream mesg;
        mesg << "Number of position grid objects " 
            << " is not equal to descriptor grid objects";

        throw lear::Exception("WinDescriptor::Constructor()",
                mesg.str());
    }
    using namespace std;

    // compute length_ using formula
    DescIter d = desc_.begin();
    GridIter g = grid_.begin();

    int gridlen = 0;
    for (int j= 0; j< numItem_; ++j, ++d, ++g) {
        indexrange_(j)=IndexType(length_, (*d)->size()*g->size());

        length_ += (*d)->size()*g->size();
        gridlen +=g->size();

        // fill cache also
        cache_.push_back(CacheType((*d)->size(), cachesize));
    }
    initlength_ = length_;

}
WinDescriptor::Preprocessor& WinDescriptor::preprocess( const IProcessor::GrayImage& image) 
{
    return template_preprocess(image);
}
WinDescriptor::Preprocessor& WinDescriptor::preprocess( const IProcessor::RGBImage& image) 
{
    return template_preprocess(image);
}
template <class PixelType>
WinDescriptor::Preprocessor& WinDescriptor::template_preprocess( const blitz::Array<PixelType,2>& image) 
{// {{{
    using namespace std;
    preprocessor.clear();

    IndexType image_extent = image.extent();
    if (blitz::isLess(image.extent(),extent_)) {
//                  ostringstream mesg;
//                  mesg << "Descriptor extent "
//                      << "( " << setw(4) << extent_[0] << ", " 
//                      << setw(4) << extent_[1] << " )" 
//                      << " is bigger than image size "
//                      << "( " << setw(4) << image.rows() << ", " 
//                      << setw(4) << image.cols() << " )";

//                  throw lear::Exception("WinDescriptor::preprocess()", mesg.str());

        blitz::Array<PixelType,2> nimage = extendBorder(
                image, extent_/2-1, extent_/2, image_extent/2);
        for (DescIter i = desc_.begin(); i != desc_.end(); ++i) {
            preprocessor.push_back((*i)->preprocess(nimage));
        }
        image_extent = nimage.extent();
    } else {
        for (DescIter i = desc_.begin(); i != desc_.end(); ++i) {
            preprocessor.push_back((*i)->preprocess(image));
        }
    }
    //for_each(cache_.begin(), cache_.end(),
    //        bind2nd(mem_fun_ref(&CacheType::clear),image_extent));
    for(WinDescriptor::CacheCont::iterator iter = cache_.begin(); iter!= cache_.end(); iter++){
        iter->clear(image_extent);
    }
    return preprocessor;
}// }}}

WinDescriptor::FeatType WinDescriptor::compute( const IndexType gridTopLeft) const {// {{{
    typedef WinDescriptor::CacheCont::iterator        CacheIter;

    FeatType vec(initlength_);
    FeatType::iterator dest = vec.begin();
        
    DescIter d=desc_.begin(); 
    GridIter g=grid_.begin(); 
    CacheIter c =  cache_.begin();
    Preprocessor::const_iterator p = preprocessor.begin();
    for (; d!=desc_.end(); ++d, ++p, ++c, ++g)
    {
        DescOp op(*d,*p);
        for (GridType::const_iterator i=g->begin(); 
                i != g->end(); ++i) 
        {
            FeatType f = (*c)(*i+gridTopLeft,op);
            dest = std::copy(f.begin(),f.end(),dest);
        }
    }
    return vec;
}// }}}

void WinDescriptor::print(std::ostream& o) const {// {{{
    using namespace std;
    o << setw(14) << title() << 
        "   Grids Count " << setw(7) << numItem_ << " |\n" <<
        "  | Length    " << setw(9) << length_ << 
        "    Extent    " << setw(4) << right << extent_[0] << 
                "x" << setw(4) << left << extent_[1] << right << " |\n";

    int count = 1;
    for (GridIter i = grid_.begin(); i != grid_.end(); ++i,++count) {
        o << 
        "  | Grid Num  " << setw(9) << count <<
        "    Size      " << setw(9) << i->size() <<" |\n";
    }
    o << "  |____________________________________________|\n";
}// }}}

lear::FileHeader WinDescriptor::header() const {
    return lear::FileHeader("WinDesc",100);
}
void WinDescriptor::print(lear::BiOStream& o) const {// {{{
    using namespace std;

    o << header();
    o << numItem_;
    for (DescIter i = desc_.begin(); i != desc_.end(); ++i) 
        o << *(*i);

    o << numItem_;
    for (GridIter i = grid_.begin(); i != grid_.end(); ++i) 
        o << *i;

    o << extent_ << length_;
}// }}}

std::ostream& operator<<( std::ostream& o, const lear::WinDescriptor& w) 
{ w.print(o); return o;}

lear::BiOStream& operator<<( lear::BiOStream& o, const lear::WinDescriptor& w) 
{ w.print(o); return o;}
