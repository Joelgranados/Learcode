/*
 * =====================================================================================
 *
 *       Filename:  rawdescio.cpp
 *
 *    Description:  Provides implementation to rawdescio.h
 *
 * =====================================================================================
 */

#include <iostream>
#include <iomanip>
#include <lear/io/ioext.h>
#include <lear/blitz/tvmio.h>
#include <lear/blitz/blitzio.h>

#include "rawdescio.h"

const lear::FileHeader  RawDescIn::header("RawDesc",100);
using namespace std;
using namespace lear;

RawDescIn::RawDescIn(const string& filename_, const int verbose):
        filename_(filename_),from(filename_.c_str()),
        descNum_(0), current(0)
{// {{{
    if (!from) {
        throw Exception("RawDescIn::constructor()",
                "Unable to open input file "+filename_);
    }
    from >> fheader;
    if (verbose > 7) 
        cout<< "Tag " << fheader.tag() 
            << " Version " << fheader.version() << endl;
    if (!fheader.identify(header.tag())) {
        throw Exception("RawDescIn::constructor()",
            "File " + filename_+" does not look like a valid "
            "descriptor file");
    }
    if (fheader.version() < 100) { // check for version number
        throw Exception("RawDescIn::constructor()",
            "File " + filename_+" has unsupported version number " );
    }

    if (verbose > 5) {
        cout << "Processing file " << filename_ << endl;
    }
    unsigned char dummy;
    FileHeader win_header;
    from >> win_header;// read window header

    from >> descNum_;

    descLength_.resize(descNum_);
    descExtent_.resize(descNum_);
    
    for (int i= 0; i< descNum_; ++i) {
        FileHeader desc_header;
        from >> desc_header;// read descriptor header
        int len;
        from >> len;

        BiIStream::pos_type descLen = from.tellg();
        from >> descLength_(i);
        from >> descExtent_(i);

        len -= from.tellg() - descLen;
        for (int j= 0; j< len ; ++j) 
            from >> dummy;// read descriptor
    }

    int gridNum_;
    
    from >> gridNum_;// read number of Grid Vector 
    if (gridNum_ != descNum_) { // must be same as descriptor numbers
        throw Exception("RawDescIn::constructor()",
            "Descriptor length is not equal to grid length in "+filename_);
    }
    for (int i= 0; i< gridNum_; ++i) {
        GridType g;
        from >> g;
        grid_.push_back(g);
    }

    from >> extent_;
    if (!from) {
        throw Exception("RawDescIn::constructor()",
                "Unable to read window size from file "+filename_);
    }

    from >> length_;
    if (!from) {
        throw Exception("RawDescIn::constructor()",
                "Unable to read feature length from file "+filename_);
    }

    from >> size_;
    if (!from) {
        throw Exception("RawDescIn::constructor()",
            "Unable to read descriptor number from file "+filename_);
    }
    if (verbose > 2) {
        cout 
            << "Feature number    " << setw(5) << size_ <<'\n'
            << "\tFeature dimension " << setw(5) << length_ << '\n'
            << "\tDescriptor extent " << setw(3) << extent_[0] << 
            'x' << setw(3) << extent_[1] << endl;
    }

    startpos = from.tellg();
}// }}}

Array1DType& RawDescIn::next(Array1DType& f) {// {{{
    from >> f;
    if (length_ != f.size()) {
        throw Exception("RawDescIn::next()",
                "Feature length is not equal to expected length");
    }
    if (!from) {
        throw Exception("RawDescIn::next()",
                "Premature I/O error in file " + filename_);
    }
    ++current;
    readExtra();
    return f;
}
//}}}

void RawDescIn::readExtra() {
    validxyinfo_=false;
    if (fheader.version() >= 110) {
        from >> fwinlbound_ >> fwinextent_ >> fwinscale_;
        validxyinfo_=true;
    } 
    validfilename_=false;
    if (fheader.version() >= 120) {
        from >> fwinfilename_;
        validfilename_=true;
    } 
}
void RawDescIn::next_split(
        Array2DType& rf, const Array1DType& f, const int index) const
{// {{{
    using namespace blitz;
    int s = 0;
    for (int i= 0; i< index; ++i) 
        s += descLength_(i);

    Array1DType tf ( f(Range(s,s+grid(index).size()*descLength(index)-1)) );

    // create a new storage array 
    std::copy(tf.begin(),tf.end(),rf.begin());
}
//}}}

Array2DType RawDescIn::next_split( const Array1DType& f, const int index) const
{// {{{
    // create a new storage array 
    Array2DType sf(grid(index).size(), descLength(index));
    next_split(sf,f,index);
    return sf;
}
//}}}
