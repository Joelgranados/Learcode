
#include <iostream>

#include <algorithm>
#include <lear/blitz/tvmio.h>
#include <lear/io/biostream.h>
#include <lear/cvision/phistogram.h>
#include <lear/blitz/ext/domainiter.h>

#include <lear/io/fileheader.h>

#include <lear/cvision/rhogdense.h>

std::ostream& operator<<(std::ostream& o, const lear::RHOGDense& d)
{ d.print(o); return o; }

namespace lear 
{
    BiOStream& operator<<(BiOStream& o, const RHOGDense& d)
    { d.print(o); return o;}
}
using namespace lear;

// {{{ constructor
RHOGDense::RHOGDense(
        const IndexType cellsize_,
        const IndexType numcell_,
        const IndexType stride_,
        const int orientbin_,
        const RealType wtscale_,
        const bool semicirc_,
        const IProcessor* p,
        const DescNormalizer<RealType>* n
        ):
    cellsize_(cellsize_), 
    numcell_(numcell_), 
    stride_(stride_),
    orientbin_(orientbin_), 
    wtscale_(wtscale_),
    semicirc_(semicirc_), 
    processor(p),
    normalizer(n),

    descsize_(blitz::product(numcell_)*orientbin_),
    extent_(cellsize_*numcell_),
    ubound_(extent_ - 1),
    weight_(extent_), 
    h_extent( extent_[0], extent_[1], (semicirc_ ? 180.0: 360.0)),
    h_bandwidth( cellsize_[0], cellsize_[1], (semicirc_? 180.0: 360.0)/orientbin_),

    tmag_(extent_),
    hist_(0.0,h_extent,h_bandwidth,
            blitz::TinyVector<bool,3>(false,false,true))
{
    if (wtscale_ > 1e-3) {
        using namespace blitz;
        TinyVector<RealType,N> var2=cellsize_*numcell_/(2*wtscale_);
        var2 *= var2*2;

        const TinyVector<RealType,N> center = extent_/2.0;

        // initialize weight_ to a gaussian 
        for (DomainIter<N> i(0,ubound_);i;++i) 
        {
            TinyVector<RealType,N> t = *i - center;
            t *=t/var2;
            weight_(*i) = exp(-blitz::sum(t));
        }
    } else {
        weight_ = 1;
    }
}// }}}

RHOGDense::FeatType& RHOGDense::operator() (const IndexType point, const Preprocessor& p)  const
{// {{{
    using namespace blitz;
    RectDomain<N> span(point,point+ubound_);
    tmag_ = weight_*p.mag(span);
    Preprocessor::OriAType tori = p.ori(span);

    hist_.clear();

    for (int i= 0; i< extent_[0]; ++i) 
    for (int j= 0; j< extent_[1]; ++j) 
    {
        TinyVector<RealType,3> at ( i+0.5, j+0.5, tori(i,j) );
        hist_(at, tmag_(i,j));
    }
    (*normalizer)(hist_.data());
    return hist_.data();
}// }}}

void RHOGDense::print(lear::BiOStream& o) const {// {{{
    using namespace std;
    lear::FileHeader descheader("RHOGDense",96);
    o << descheader;

    int size=96;
    o << size;
    lear::BiOStream::pos_type pos = o.tellp();

    o << descsize_ << extent_ << cellsize_ << numcell_ << 
        stride_ << orientbin_<< semicirc_ << wtscale_ ;
    o << *processor << *normalizer;

    lear::BiOStream::pos_type cur = o.tellp();
    size -= cur - pos;
    if (size < 0) {
        throw lear::Exception("RHOGDense::print",
                "Overflow while writing bistream file");
    }
    char dummy=' ';
    for (int i= 0; i< size; ++i) 
        o << dummy;
}// }}}

void RHOGDense::print(std::ostream& o) const {// {{{
    using namespace std;
    ostringstream pstr;
    pstr <<  *processor;
    ostringstream nstr;
    nstr <<  *normalizer;
    o << "RHOG Dense (Boundary Clip)::\n"
        "  | Length      " << setw(7) << descsize_ << 
        "    Extent      " << setw(3) << right << extent_[0] << 
                    "x" << setw(3) << left << extent_[1] << " |\n" <<
        "  | Cell Size   " << setw(3) << right << cellsize_[0] << 
                    "x" << setw(3) << left << cellsize_[1] << 
        "    Number Cell " << setw(3) << right << numcell_[0] <<
                    "x" << setw(3) << left << numcell_[1] << " |\n" <<
        "  | Stride      " << setw(3) << right << stride_[0] << 
                    "x" << setw(3) << left << stride_[1] << right << 
        "    WeightScale " << setw(7) << setprecision(2) << wtscale_<<" |\n"
        "  | Orient bin  " << setw(7) << orientbin_ << "     over range " 
                << (semicirc_ ? "(0-180)" : "(0-360)") <<" |\n"
        "  |--------------------------------------------|\n"
        "  | " << setw(42) << left << pstr.str() << right << " |\n"
        "  | " << setw(42) << left << nstr.str() << right << " |\n"
        "  |____________________________________________|\n";
//  "  | Orient bin  " << setw(7) << d.orientbin_ << 
//          (d.semicirc_ ? " over (0-180)" : " over (0-360)") <<"     |\n"
//  "    Orient bin  " << setw(3) << d.orientbin_ << 
//          (d.semicirc_ ? " - C" : " - O") <<" |\n"
}// }}}


