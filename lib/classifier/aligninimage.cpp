#include <iomanip>
#include <iostream>
#include <lear/exception.h>
#include <lear/util/fileutil.h>

#include <blitz/array.h>
#include <lear/image/imageio.h>
#include <lear/classifier/aligninimage.h>

lear::AlignInImage::AlignInImage(
    const std::string outimage_,
    const bool outfileIsDir_,
    const IndexType margin_,
    const IndexType normalsize_,
    const std::string& histfile_,
    const RealType minTh_,
    const RealType extTh_, 
    const RealType bandwidth_
    ) :
    PostProcessResult(),
    HistProcessor(histfile_,minTh_,extTh_,bandwidth_),
    outimage_(outimage_),
    outfileIsDir_(outfileIsDir_),
    margin_(margin_),normalsize_(normalsize_)
{}

void lear::AlignInImage::write( 
    const_iterator s, const_iterator e, const std::string filename)
{// {{{
    std::string fname = outimage_;
    if (outfileIsDir_) {
        fname += "/" + lear::basename(filename) +".png";
    }

    typedef blitz::TinyVector<int,3>    PixelType;
    typedef blitz::Array<PixelType,2>   ImageType;

    ImageType inimage;
    ImageIO::read(filename,inimage);

    DetectInfo best;
    bool bestvalid=false;
    for (const_iterator j=s;j!=e;++j){
        if (bestvalid) {
            if (best.score < j->score ) 
                best = *j;
        } else {
            best = *j;
            bestvalid=true;
        }
    }
    if (bestvalid) {
        HistProcessor::operator()(best.score);
    }
    if (!bestvalid && blitz::sum(inimage.extent()-normalsize_>=0)==2) {
        best.lbound=(inimage.extent()-normalsize_)/2;
        best.extent=normalsize_;
        best.scale=1;
        bestvalid=true;
    }
    if (bestvalid) {
        IndexType rlb (best.lbound-margin_);

        // refer to top-left in inimage
        IndexType dlb = blitz::max(rlb,inimage.lbound()); 

        IndexType rub (best.lbound+best.extent+margin_-1);

        // refer to bottom-right in inimage
        IndexType dub = blitz::min(rub,inimage.ubound());

        // refer to timage range
        blitz::RectDomain<2> domain(dlb, dub);
        
        // refer to top-left in outimage
        rlb = dlb - rlb;
        // refer to bottom-right in outimage
        rub = dub - dlb + rlb;

        // refer to centered image range
        blitz::RectDomain<2> range(rlb, rub);

        IndexType oex =best.extent+2*margin_;
        ImageType outimage(oex);
        //outimage=255;

        // refer to centered image range
        outimage(range) = inimage(domain);

        blitz::Range all = blitz::Range::all();

        for (int i= 0; i< rlb[0]; ++i)
            outimage(i,all) = outimage(rlb[0],all);
        for (int i= 0; i< rlb[1]; ++i)
            outimage(all,i) = outimage(all,rlb[1]);

        for (int i= rub[0]+1; i< oex[0]; ++i)
            outimage(i,all) = outimage(rub[0],all);
        for (int i= rub[1]+1; i< oex[1]; ++i)
            outimage(all,i) = outimage(all,rub[1]);
        
        ImageIO::write(fname,outimage);

        std::cout << filename  << best << std::ends;
        std::cout << " Domain " << dlb << ", " << dub << std::ends;
        //std::cout << " Range  " << rlb << ", " << rub << std::ends;
        std::cout << std::endl;
    }
}// }}}

