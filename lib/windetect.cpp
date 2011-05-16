/*
 * =====================================================================================
 *
 *       Filename:  windetect.cpp
 *
 *    Description:  Provides implementation to windetect.h interface.
 *
 * =====================================================================================
 */



#include <list>
#include <fstream>
#include <iostream>

#include <boost/random.hpp>
#include <blitz/array.h>
#include <blitz/tinyvec.h>

#include <lear/blitz/ext/tvmutil.h>
#include <lear/blitz/ext/domainiter.h>

#include <lear/io/biostream.h>
#include <lear/io/fileheader.h>
#include <lear/blitz/tvmio.h>
#include <lear/blitz/blitzio.h>

#include <lear/image/rescale.h>

#include <lear/cvision/rhogdense.h>
#include <lear/cvision/iprocessor.h>
#include <lear/cvision/dnormalizer.h>

#include <lear/cvision/densegrid.h>
#include <lear/cvision/imageslider.h>
#include <lear/cvision/scalepyramid.h>
#include <lear/cvision/windescriptor.h>

#include <lear/image/imageio.h>

#include <lear/interface/windetect.h>

typedef RHOGDenseParam::RealType        RealType;
typedef blitz::Array<RealType,1>        Array1DType;
typedef blitz::Array<RealType,2>        Array2DType;
typedef blitz::Array<RealType,3>        Array3DType;

typedef blitz::TinyVector<int,2>        IndexType;

typedef lear::DenseGrid<2>::GridType    GridType;

typedef lear::WinDescriptor             WinDescType;
typedef WinDescType::DescCont           DescContainer;
typedef WinDescType::GridCont           GridContainer;

using namespace std;
using namespace lear;
using namespace blitz;

const char WinDetect::DefaultLabel='O';
/**
 * A static object of this type will be created as we dont want to export 
 * complicated RHOGDense, IProcessor, Normalizer, WinDescriptor interface.
 */
struct WinDetectDescHolder {
    bool initialized;
    DescContainer                descarray;
    GridContainer                gridarray;

    WinDescType*                 windesc;

    WinDetectDescHolder() :
        initialized(false), windesc(NULL)
    {}

    ~WinDetectDescHolder() 
    {
        delete windesc;
        for (DescContainer::iterator i=descarray.begin(); i != descarray.end(); ++i)
            delete *i; 
    }
    static RHOGDense* init(const RHOGDenseParam& param, int verobse=0);
};
static WinDetectDescHolder          descholder;

// creates gridtype and windescriptor.
void WinDetect::init(const RHOGDenseParam* param) 
{
    std::vector<const RHOGDenseParam*> vector(1);
    vector[0] = param;
    init(vector);
}

void WinDetect::init(const std::vector<const RHOGDenseParam*>& param) 
{// {{{ init
    if (descholder.initialized) {
        throw Exception("WinDetect::init", 
            "Init is supposed to be called only once. This is a repeat initialization.");
    }
    IndexType size(size_x, size_y);

    for (unsigned i= 0; i< param.size(); ++i) {
        WinDescType::DescType* desc =  WinDetectDescHolder::init(*param[i], verbose);
        descholder.descarray.push_back(desc);
        descholder.gridarray.push_back( DenseGrid<2>::get( size, desc->extent(), desc->stride()) );
    }

    descholder.windesc = new WinDescType(
            size, descholder.descarray, descholder.gridarray, cachesize);
    descholder.initialized = true;
    if (verbose > 1) 
    { cout << *descholder.windesc << endl; }
} // }}}

RHOGDense* WinDetectDescHolder::init(const RHOGDenseParam& param, int verbose) 
{ //{{{
    ImageNoRemap* mapperBefore = NULL;// remap image before computing gradient
    ImageNoRemap* mapperAfter = NULL;// remap gradient magnitude 
    switch (param.preprocessing2use) {
        // gradient operator
        case RHOGDenseParam::RGB_Log_Grad:
            mapperBefore = new ImageLogRemap();
            break;

        case RHOGDenseParam::RGB_Sqrt_Grad:
            mapperBefore = new ImageSqrtRemap();
            break;

        case RHOGDenseParam::Lab_Sqrt_Grad:
            mapperBefore = new ImageLabSqrtRemap();
            break;

        case RHOGDenseParam::Lab_Grad:
            mapperBefore = new ImageLabRemap();
            break;

        case RHOGDenseParam::RGB_Grad:
        default:
            break;
    };
    IProcessor* preprocessor=NULL;
    if (param.gscale < 1e-3) {
        // It owns and deletes all pointers. So no worries.
        preprocessor = new GradProcessor_NoSmooth(param.semicirc, NULL, mapperBefore, mapperAfter);
    } else {
        // It owns and deletes all pointers. So no worries.
        preprocessor = new GradProcessor(param.gscale, param.semicirc, NULL, mapperBefore, mapperAfter);
    }
    DescNormalizer<RealType>* normalizer=NULL;
    switch (param.norm2use) {
        case RHOGDenseParam::NoNorm:
            normalizer = new DescNormalizer<RealType>();
            break;

        case RHOGDenseParam::NormL1:
            normalizer = new L1Normalizer<RealType>(param.epsilon);
            break;

        case RHOGDenseParam::NormL2:
            normalizer = new L2Normalizer<RealType>(param.epsilon);
            break;

        case RHOGDenseParam::NormL2Hys:
            normalizer = new L2HysNormalizer<RealType>(param.epsilon, param.maxvalue);
            break;

        case RHOGDenseParam::NormL1Sqrt:
        default:
            normalizer = new L1SqrtNormalizer<RealType>(param.epsilon);
            break;
    }
    IndexType cellsize(param.cellsize_x, param.cellsize_y); 
    IndexType numcell(param.numcell_x, param.numcell_y); 
    IndexType stride(param.descstride_x, param.descstride_y); 

    WinDescType::DescType* desc = new RHOGDense(
                cellsize, numcell, stride, 
                param.orientbin, param.wtscale, param.semicirc, 
                preprocessor, normalizer);// owns preprocessor & normalizer and frees them when they not required

    if (verbose > 1) 
        std::cout << *desc << std::endl;
    return desc;
}//}}}

// Convert image to blitz array format
static WinDescType::ImageType getImage(
        const unsigned char* image, int width, int height, int step) 
{
    WinDescType::ImageType bimage(width, height) ;
    if (step==0)
        step = 3*width;

    for (int j= 0; j< height; ++j) 
    for (int i= 0; i< width; ++i) {
        const unsigned char* pixel = image+j*step + 3*i;
        bimage(i,j) = IProcessor::RGBType(pixel[0], pixel[1], pixel[2]);
    }
    //ImageIO::write("image.png", bimage);
    return bimage;
} 

bool WinDetect::computefeature(
    float* result, int xloc, int yloc, 
    const unsigned char* image, int width, int height, int step) const
{// {{{
    if (!descholder.initialized) {
        throw Exception("WinDetect::computefeature", 
            "Init is supposed to be called before calling computefeature");
    }
    WinDescType* windesc = descholder.windesc;

    IndexType descextent = windesc->extent();

    if ( xloc >=0 && yloc >=0 && 
        xloc+descextent[0] <=width && 
        yloc+descextent[1] <=height)
    {
        WinDescType::ImageType pyimage = getImage(image,width,height, step); 

        windesc->preprocess(pyimage);
        Array1DType desc = windesc->compute(IndexType(xloc,yloc)); 
        std::copy(desc.begin(), desc.end(), result);
        return true;
    }
    return false;
}// }}}

std::vector<bool> WinDetect::computefeature(
    std::vector<float*>& result, const std::vector<int>& xlocs, const std::vector<int>& ylocs, 
    const unsigned char* image, int width, int height, int step) const
{// {{{
    std::vector<bool> status(xlocs.size());
    if (!descholder.initialized) {
        throw Exception("WinDetect::computefeature", 
            "Init is supposed to be called before calling computefeature");
    }

    WinDescType* windesc = descholder.windesc;

    IndexType descextent = windesc->extent();

    WinDescType::ImageType pyimage = getImage(image,width,height, step); 
    windesc->preprocess(pyimage);

    for (unsigned i= 0; i< xlocs.size(); ++i) {
        IndexType topleft (xlocs[i], ylocs[i]);
        if ( topleft[0] >=0 && topleft[1] >=0 && 
            topleft[0]+descextent[0] <= width && 
            topleft[1]+descextent[1] <= height)
        {
            Array1DType desc = windesc->compute(topleft); 
            std::copy(desc.begin(), desc.end(), result[i]);
            status[i] = true;
        } else 
            status[i] = false;
    }
    return status;
}// }}}

    
int WinDetect::featurelength() const
{
    if (!descholder.initialized) {
        throw Exception("WinDetect::featurelength", 
            "Init is supposed to be called before calling featurelength");
    }
    return descholder.windesc->length();
}

// Used to create jitter in windows in order to learn more robust detector. 
// This is test code with not so good results from it. So you may ignore it. 
class JitterWindow {// {{{
    public:
    typedef std::vector<IndexType>::const_iterator iterator;

    JitterWindow(
        const IndexType winextent_,
        const IndexType jitter_,
        const int steps_) 
        :
        winextent_(winextent_),
        jitter_(jitter_),
        steps_(steps_),
        samples_(steps_*8+1)
        
    {
        ptlist_.reserve(samples_);
    }

    void generatepointlist(
            const IndexType imageextent,
            const IndexType startloc) 
    {
        ptlist_.clear();

        for (blitz::DomainIter<2> i(-steps_,steps_); i; ++i) {
            IndexType start = startloc + (*i)*jitter_;
            IndexType end ( start+winextent_);
            if (blitz::sum(start>=0) ==2 && blitz::sum(end<=imageextent)==2) {
                ptlist_.push_back(start);
            }
        }
    }

    iterator begin() const 
    {
        return ptlist_.begin();
    }
    iterator end() const 
    {
        return ptlist_.end();
    }
    int samples() const {
        return samples_;
    }
    private:
        IndexType winextent_;
        IndexType jitter_;
        int steps_,samples_;
        std::vector<IndexType> ptlist_;

};// }}}

#ifdef BUILD_APP
static void writeExtra(
    const WinDetectDump& param,
    lear::BiOStream& to,// output file stream
    const IndexType lbound, const IndexType extent,
    const RealType scale, const std::string& filename
    )
{
    to << lbound << extent << scale;
    if (param.dumpfulldetail>1)
        to << filename;
}

void WinDetectDump::writeHardTest( 
        const std::string& hardfile,
        const std::string& outfile)
{// {{{
    if (!descholder.initialized) {
        throw Exception("WinDetect::writeHardTest", 
            "Init is supposed to be called before we can use writeHardTest");
    }
    WinDescType* windesc = descholder.windesc;
    if (verbose > 1) 
    { std::cout << *this << std::endl; }

    if (verbose > 2)
        std::cout << "Writing hard file for detection score > " 
                  << hardscore << std::endl;
    int totallines = 0;
    {
        std::ifstream from(hardfile.c_str());
        if (!from) {
            throw lear::Exception("WinDetectDump::writeHardTest()",
                    "Unable to open hard test case file " + hardfile);
        }
        char line[1024];
        while (from.getline(line,1024)) {
            totallines++;
        }
    }
    std::ifstream from(hardfile.c_str());
    if (!from) {
        throw lear::Exception("WinDetectDump::writeHardTest()",
                "Unable to open hard test case file " + hardfile);
    }

    if (from && verbose > 2 ) {
        cout << "From stream is good again and can read" << std::endl;
    }
    IndexType size(size_x, size_y);
    IndexType jitterstep(jitterstep_x, jitterstep_y);

    JitterWindow jitwin(size, jitterstep, jitterwinstep);
    if (memorylimit) {
        int descsize = windesc->length()*sizeof(RealType);// in bytes now

        if (verbose > 1) {
            cout<< "Upper memory limit is " << memorylimit 
                << ", Descriptor size (in bytes) is " << descsize << endl;
        }

        int maxdescriptor=memorylimit*1024*1024/descsize;
        if (poscases)
        maxdescriptor-=poscases;
        if (negcases)
        maxdescriptor-=negcases;

        samples = totallines*jitwin.samples()/maxdescriptor;
        if (verbose > 1) {
            cout<< "Total lines are " << totallines 
                << ", jitter windows are " << jitwin.samples()
                << ", maximum descriptors are " << maxdescriptor 
                << ", samples value " << samples << endl;
        }
    }

    lear::BiOStream to(outfile.c_str());
    if (!to) {
        throw lear::Exception("WinDetectDump::writeHardTest()",
                "Unable to open output file " + outfile);
    }
    lear::FileHeader header("RawDesc",100);
    to << header;

    to << *windesc;

    lear::BiOStream::pos_type start = to.tellp();
    int count = 0;
    // number of elements
    to << count;

    bool newimage=true;
    RealType prevscale=0;
    std::string prevfile="";

    IndexType topleft(topleft_x, topleft_y);

    bool hasTopLeft = false;
    if (blitz::sum(topleft>=0) == 2) {
        hasTopLeft = true;
        if (verbose > 2) {
            cout<< "Top-Left corner specified ( " 
                << topleft[0] << ", " << topleft[1] << " ) " << endl;
        }
    }

    bool negcase = label!='P';
    int skipped = 1;
    WinDescType::ImageType origimage;
    IndexType newsize;
    while (from ) {
        std::string file;
        from >> file;
        IndexType lbound, extent;
        RealType scale, score;
        from >> lbound[0] >> lbound[1] >> extent[0] >> extent[1] 
             >> scale >> score;

        if (!from)
            break;

        if (file.empty())
            continue;

        if ((negcase && score < hardscore) || (!negcase && score > hardscore))
            continue;

        if (prevfile != file){
            lear::ImageIO::read(file,origimage);
            newimage = true;
            prevfile =file;
        } else {
            newimage = false;
        }
        if (newimage || std::abs(scale - prevscale) > 1e-3) {
            prevscale = scale;
            newsize = ceil(origimage.extent()/scale);

            windesc->preprocess(lear::rescale(origimage,newsize));
        }
        IndexType start ( floor(lbound/scale));
        // if we are looking only on a sub window of hard example

        //if (hasTopLeft) {
        //    start += topleft;
        //} else 
        {
            // extent difference, can have one-two pixel error
            IndexType exdiff = extent/scale - size; 
            exdiff = max(exdiff,0); // in case, there is small errr due 
                                    // to rounding off while writing extent
            start += exdiff/2;
        }
        if (verbose > 3) {
            cout << "Processing file " << file << 
                " at lbound " << lbound << " extent " << extent << endl;
        }
        IndexType end ( start+size);
        if (blitz::sum(end >=newsize))
            continue;

        jitwin.generatepointlist(newsize, start);
        for (JitterWindow::iterator ji = jitwin.begin(); 
                ji!=jitwin.end();++ji) 
        {
            if (samples && skipped <= samples){
                ++skipped;
                continue;
            } else 
                skipped =1;

            if (verbose > 8)  {
                std::cout << "Computing descriptor at location " 
                    << *ji << ", " << newsize << std::endl;
            }

            if (!negcase && blitz::sum(*ji==start)==2)
                continue;
            to << windesc->compute(*ji); ++count;
        }

        if (verbose > 5) 
            cout << "Processed location " << file << std::endl;
    }
    to.seekp(start);
    to << count;
    to.close();

    if (verbose > 2)
        cout << "Written " << count << " features" << std::endl;
}// }}}

void WinDetectDump::writeWinDesc( 
        const PathVector& inlist,
        const std::string& outfile,
        const std::string& testlocs)
{// {{{
    if (!descholder.initialized) {
        throw Exception("WinDetect::writeHardTest", 
            "Init is supposed to be called before we can use writeWinDesc");
    }
    WinDescType* windesc = descholder.windesc;
    if (verbose > 1) 
    { std::cout << *this << std::endl; }

    lear::BiOStream to(outfile.c_str());
    if (!to) {
        throw lear::Exception("WinDetectDump::writeWinDesc()",
                "Unable to open output file " + outfile);
    }
    lear::FileHeader header("RawDesc",100+(dumpfulldetail*10));
    to << header;

    to << *windesc;

    lear::BiOStream::pos_type start = to.tellp();
    int count = 0;
    // number of elements
    to << count;

    std::ofstream testlocsstr;
    bool dotestlocs = testlocs.size();
    if (dotestlocs) {
        testlocsstr.open(testlocs.c_str());
        if (!testlocsstr) {
            std::cerr << "Could not open file to write tested/dumped locations " << testlocsstr << ". Ignoring ..." << std::endl;
            dotestlocs = false;
        }
    }

    
    typedef boost::variate_generator<boost::mt19937&, boost::uniform_int<> > RandEng; 
    boost::mt19937 rng(randomSeed ? static_cast<unsigned long> (std::time(0)) : 0 );
    

    IndexType topleft(topleft_x, topleft_y);
    const bool hasTopLeft = (blitz::sum(topleft>=0) == 2);
    if (hasTopLeft && verbose > 2) {
            cout<< "Top-Left corner specified ( " 
                << topleft[0] << ", " << topleft[1] << " ) " << endl;
    }

    IndexType fullsize(fullsize_x, fullsize_y);
    const bool hasFullSize = (blitz::sum(fullsize>=0) == 2);
    if (hasFullSize && verbose > 2) {
            cout<< "Full-Window size specified ( " 
                << fullsize[0] << ", " << fullsize[1] << " ) " << endl;
    }

    IndexType size(size_x, size_y);
    IndexType winstride(winstride_x, winstride_y);
    IndexType winsize=(hasFullSize && hasTopLeft)? fullsize: size;

    IndexType jitterstep(jitterstep_x, jitterstep_y);
    JitterWindow jitwin(size, jitterstep, jitterwinstep);
    for (PathVector::const_iterator f=inlist.begin();
            f!= inlist.end();++f)
    {
        WinDescType::ImageType image;
        lear::ImageIO::read(*f,image);

        if (verbose > 3) {
            cout << "Processing file " << *f << endl;
        }
        if (blitz::sum(image.extent()<winsize)) {
            std::cerr <<  "Image extent "
                << "( " << std::setw(4) << image.rows() << ", " 
                << std::setw(4) << image.cols() << " )"
                << " is smaller than descripor size "
                << "( " << std::setw(4) << winsize[0] << ", " 
                << std::setw(4) << winsize[1] << " ). Ignoring ..." ;
            continue;
        }

        if (pyramid) {
            typedef lear::ImageSlider<2>                SliderType;
            typedef lear::ScalePyramid<2>               PyramidType;

            const PyramidType pyramid(image.extent(),size,
                    scaleratio, 
                    endscale, 
                    startscale);

            if (verbose > 6) {// {{{
                cout << "Scale levels" << setw(2) << pyramid.size() <<
                    ", Start scale = " << 
                    setw(3) << setprecision(1) << pyramid.startScale() <<
                    ", End scale = " <<
                    setw(3) << setprecision(1) << pyramid.endScale() <<
                    endl;
            }// }}}

            int imagewindows =0;
            for (PyramidType::iterator piter = pyramid.begin(); 
                    piter != pyramid.end(); ++piter) 
            {// {{{
                WinDescType::ImageType pyimg (lear::rescale(image, *piter));
                SliderType slider(pyimg.extent(),size,winstride);

                windesc->preprocess(pyimg);

                for (SliderType::iterator siter = slider.begin(); 
                        siter != slider.end(); ++siter) 
                {
                    to << windesc->compute(*siter); ++imagewindows;
                    if (dotestlocs){
                        testlocsstr << *f 
<< ' ' << setprecision(4) << setw(10) << (*siter)[0] 
<< ' ' << setprecision(4) << setw(10) << (*siter)[1] 
<< ' ' << setprecision(4) << setw(10) << size[0]*piter.scale() 
<< ' ' << setprecision(4) << setw(10) << size[1]*piter.scale() 
<< std::endl;
                    }
                }
            }// }}}
            if (verbose > 1) 
                cout << "Image " << *f << " | Descriptors " 
                    << setw(8) << imagewindows << endl;
            count+= imagewindows;
        } else {
            windesc->preprocess(image);
            blitz::TinyVector<unsigned long,2> ex;
            ex = image.extent() - winsize;

            RandEng randX(rng, boost::uniform_int<>(topleft[0],ex[0]-1));
            RandEng randY(rng, boost::uniform_int<>(topleft[1],ex[1]-1));
            for (int i= 0; i< samples; ++i) {
                IndexType tl;
                if (samples >1 ) {
                    tl[0] = randX(); tl[1] = randY(); 
                } else if (hasTopLeft) {
                    tl = topleft;
                    if (hasFullSize)
                        tl += (image.extent() - fullsize)/2;
                } else
                    tl = (image.extent() - size)/2;

                jitwin.generatepointlist(image.extent(), tl);
                for (JitterWindow::iterator ji = jitwin.begin(); 
                        ji!=jitwin.end();++ji) 
                {
                    to << windesc->compute(*ji); ++count;
                    if (dumpfulldetail) {
                        writeExtra(*this, to,*ji, size, 1, *f);
                    }
                    if (dotestlocs){
                        testlocsstr << *f 
                        << ' ' << setprecision(4) << setw(10) << (*ji)[0] 
                        << ' ' << setprecision(4) << setw(10) << (*ji)[1] 
                        << ' ' << setprecision(4) << setw(10) << size[0]
                        << ' ' << setprecision(4) << setw(10) << size[1]
                                  << std::endl;
                    }
                }
            }
        }

        if (verbose > 5) 
            cout << "Processed file " << *f << std::endl;
    }
    to.seekp(start);
    to << count;

    if (verbose > 2)
        cout << "Written " << count << " features" << std::endl;
}// }}}
#endif



#include <string>
#include <cstdio>
#include <fstream>

#include <lear/util/fileutil.h>
#include <lear/blitz/ext/globalfunc.h>

#include <lear/classifier/resultholder.h>
#include <lear/classifier/th_processresult.h>
#include <lear/classifier/ms_processresult.h>

/**
 * A static object. It hides the complexity of determining the best object
 * locations from the main interface.
 */
struct WinDetectClassifyHolder {
    bool initialized;
    MS_ProcessResult *             holder;

    WinDetectClassifyHolder() :
        initialized(false), holder(NULL)
    {}

    ~WinDetectClassifyHolder() 
    {
        delete holder;
    }
};
static WinDetectClassifyHolder      classifierholder;


//  SigmaOptType sigma;
static inline lear::DetectInfo bound(
        const RealType score,
        const RealType scale,
        const IndexType lbound, 
        const IndexType extent)
{
    typedef blitz::TinyVector<RealType,2> Real2DType;
    Real2DType s (extent*scale);
    Real2DType c (scale*(lbound+extent/2));

    return lear::DetectInfo(
            static_cast<float>(score),
            static_cast<float>(scale),
            blitz::ceil(c - s/2), blitz::floor(s), lbound);
}

void WinDetectClassify::initclassifier() 
{ // {{{ 
    using namespace lear;
    using std::setw; using std::setprecision;

    if (classifierholder.initialized) {
        throw Exception("WinDetectClassify::initclassifier", 
            "Init is supposed to be called only once. This is a repeat initialization.");
    }

    typedef blitz::TinyVector<RealType,2>                SigmaType;
    SigmaType score2prob(score2prob_a, score2prob_b);

    typedef blitz::TinyVector<RealType,3>                NonmaxType;
    NonmaxType nonmaxSigma(nonmaxsigma_x,nonmaxsigma_y, nonmaxsigma_scale); 

    IndexType size(size_x, size_y);

    MS_ProcessResult* holder = new MS_ProcessResult(
            size, lightthreshold, threshold, 
            score2prob, nonmaxSigma, softmax);

    if (verbose > 1)
        std::cout << "Processor " << holder->toString() << std::endl; 

    classifierholder.holder = holder;
    classifierholder.initialized = true;
}//}}}

void WinDetectClassify::test(
    const LinearClassify& classifier,
    std::list<DetectedRegion>& detections,
    const unsigned char* imagedata, int width, int height, int step 
    ) const
{//{{{
    if (!descholder.initialized) {
        throw Exception("WinDetectClassify::test", 
            "Init is supposed to be called before we can use test");
    }
    if (!classifierholder.initialized) {
        throw Exception("WinDetectClassify::test", 
            "Init is supposed to be called before we can use test");
    }

    WinDescType* windesc = descholder.windesc;
    MS_ProcessResult& holder = *(classifierholder.holder);
    if (verbose > 1) 
    { std::cout << *this << std::endl; }

    // create classifier
    if (classifier.length() != windesc->length()) {
        std::cerr << "Classifier length " << 
                    classifier.length() << std::endl;
        throw Exception("WinDetectClassify::test()", 
            "Dimension mismatch between window feature vector "
            "and SVM learned feature vectors");
    }


    IndexType winsize(size_x, size_y);
    IndexType winstride(winstride_x, winstride_y);

    IndexType tmargin(margin_x, margin_y);
    IndexType tavsize(avsize_x, avsize_y);
    bool addmargin ( blitz::sum((tmargin*tavsize) > 0));

    WinDescType::ImageType image;
    IndexType toadd = 0;

    WinDescType::ImageType origimage = getImage(imagedata,width,height, step);
    if (addmargin) {
        IndexType newext = origimage.extent();
        IndexType toaddX = 0, toaddY = 0;
        if (tavsize[0])
            toaddX = newext*tmargin/tavsize[0];
        if (tavsize[1])
            toaddY = newext*tmargin/tavsize[1];

        toadd = blitz::max(toaddX,toaddY);
        newext += 2*toadd;

        image.reference(extendBorder(
                origimage, newext/2-1, newext/2, origimage.extent()/2));
    } else {
        image.reference(origimage);
    }

    int imagewindows = 0; 
    holder.clear();

    typedef lear::ImageSlider<2>                SliderType;
    typedef lear::ScalePyramid<2>               PyramidType;

    const PyramidType pyramid(image.extent(),winsize,
            scaleratio, endscale, startscale);

    if (verbose > 6) {// {{{
        cout << "Scale levels" << setw(2) << pyramid.size() <<
            ", Start scale = " << 
            setw(6) << setprecision(2) << pyramid.startScale() <<
            ", End scale = " <<
            setw(6) << setprecision(2) << pyramid.endScale() <<
            endl;
    }// }}}

    for (PyramidType::iterator piter = pyramid.begin(); 
            piter != pyramid.end(); ++piter) 
    {// {{{
        WinDescType::ImageType pyimg (lear::rescale(image, *piter));
        SliderType slider(pyimg.extent(),winsize,winstride);

        windesc->preprocess(pyimg);

        for (SliderType::iterator siter = slider.begin(); 
                siter != slider.end(); ++siter) 
        {
            IndexType tl=*siter;

            Array1DType desc = windesc->compute(tl); 

            DetectInfo r = bound(
                    classifier(desc.data()),piter.scale(),
                    tl, windesc->extent());
            r.lbound -=toadd;

            holder(r);
            ++imagewindows;
        }
    }// }}}

    if (verbose > 3) {// {{{
        cout << "Processed " << setw(5) << imagewindows << " windows" <<  endl;
    }// }}}
    detections.clear();
    holder.doit();

    MS_ProcessResult::const_iterator s = holder.begin();
    MS_ProcessResult::const_iterator e = holder.end();
    for (; s!= e; ++s) {
        detections.push_back(DetectedRegion( s->score, s->scale, 
            s->lbound[0], s->lbound[1], s->extent[0], s->extent[1]));
    }
}
// }}}


#ifdef BUILD_APP
#include <lear/classifier/hist_processresult.h>
#include <lear/classifier/hard_ppresult.h>
#include <lear/classifier/list_ppresult.h>
#include <lear/classifier/markimage.h>
#include <lear/classifier/aligninimage.h>

void WinDetectClassify::runImageSlider(
        const LinearClassify& classifier, const PathVector& inlist, 
        const std::string& outfile, const std::string& outimage, 
        const std::string& outhist, const std::string& falsetxt,
        const std::string& testlocs
        )  const
{ // {{{ 
    using namespace lear;
    using std::setw; using std::setprecision;
    if (!descholder.initialized) {
        throw Exception("WinDetectClassify::runImageSlider", 
            "Init is supposed to be called before we can use runImageSlider");
    }

    WinDescType* windesc = descholder.windesc;
    if (verbose > 1) 
    { std::cout << *this << std::endl; }

    // create classifier
    if (classifier.length() != windesc->length()) {
        std::cout << "Classifier length " << 
                    classifier.length() << std::endl;
        throw Exception("WinDetectClassify::runImageSlider()", 
            "Dimension mismatch between window feature vector "
            "and SVM learned feature vectors");
    }

    IndexType topleft(topleft_x, topleft_y);
    const bool hasTopLeft = (blitz::sum(topleft>=0) == 2);
    if (hasTopLeft && verbose > 2) {
            cout<< "Top-Left corner specified ( " 
                << topleft[0] << ", " << topleft[1] << " ) " << endl;
    }

    IndexType fullsize(fullsize_x, fullsize_y);
    IndexType fullstride(fullstride_x, fullstride_y);

    const bool hasFullSize = (blitz::sum(fullsize>=0) == 2);
    if (hasFullSize && verbose > 2) {
            cout<< "Full-Window size specified ( " 
                << fullsize[0] << ", " << fullsize[1] << " ) " << endl;
    }

    IndexType size(size_x, size_y);
    IndexType stride(winstride_x, winstride_y);
    IndexType winsize=(hasFullSize && hasTopLeft)? fullsize: size;

    const bool hasFullStride = (blitz::sum(fullstride>=0) == 2);
    if (hasFullStride && verbose > 2) {
            cout<< "Full-Window stride specified ( " 
                << fullstride[0] << ", " << fullstride[1] << " ) " << endl;
    }
    IndexType winstride=
            (hasFullSize && hasTopLeft && hasFullStride)? fullstride: stride;

    if (nopyramid && verbose > 2) {
        cout << "No pyramid specified. Compare center/top-left "
            "window only" << endl;
    }

    const bool doFalseOut = !falsetxt.empty() && label != DefaultLabel;
    bool infileIsDir = inlist.size() > 1;
    bool doImageOut = lear::MarkImage::verifyImageOut(outimage,infileIsDir);

    std::ofstream testlocsstr;
    bool dotestlocs = testlocs.size();
    if (dotestlocs) {
        testlocsstr.open(testlocs.c_str());
        if (!testlocsstr) {
            std::cerr << "Could not open file to write tested/dumped locations " << testlocsstr << ". Ignoring ..." << std::endl;
            dotestlocs = false;
        }
    }

    
    ResultHolder holder;

    if (outhist.empty() || (doImageOut && aligninimage)) {
        holder.push_back( new Th_ProcessResult(lightthreshold, label!='P'));
    } else { // write histogram now for efficiency reasons
        holder.push_back(
            new Hist_ProcessResult(outhist,lightthreshold, label!='P', -20, 40));
    }
    if (doFalseOut) {
        try {
            Hard_PPResult* p = new Hard_PPResult(falsetxt);
            holder.push2last(p);
            if (verbose > 2) {
                cout << "Detected false +/- will be written to "<< 
                    falsetxt << endl;
            }
        }catch (Exception& ) {
            cerr << "Unable to open file " << falsetxt <<
                " for writing false +/-. Disabling false out." << endl;
        }
    }
    typedef blitz::TinyVector<RealType,2>                SigmaType;
    SigmaType score2prob(score2prob_a, score2prob_b);

    typedef blitz::TinyVector<RealType,3>                NonmaxType;
    NonmaxType nonmaxSigma(nonmaxsigma_x,nonmaxsigma_y, nonmaxsigma_scale); 

    int list_marker=1;
    if (!no_nonmax) {
        list_marker=2;
        holder.push_back(
            new MS_ProcessResult(size, lightthreshold, threshold, 
                score2prob, nonmaxSigma, softmax));
    } 
    if (doImageOut && !aligninimage) {
        holder.push2last( new MarkImage(outimage, infileIsDir, !showscore));
    }
    holder.push2last(new List_PPResult(outfile, list_marker));

    IndexType alignmargin(alignmargin_x, alignmargin_y);
    if (doImageOut && aligninimage ) {
        holder.push2last( new AlignInImage(
            outimage, infileIsDir,alignmargin,size,outhist,-20,40));
    }
    if (verbose > 3)
        holder.print(cout);

    unsigned long totalwindows=0; 

    IndexType tmargin(margin_x, margin_y);
    IndexType tavsize(avsize_x, avsize_y);
    //margin = maximum(margin,0);
    //avsize = maximum(avsize,0);
    bool addmargin ( blitz::sum((tmargin*tavsize) > 0));
    for (PathVector::const_iterator f=inlist.begin();
            f!= inlist.end();++f)
    {
        WinDescType::ImageType image;
        IndexType toadd = 0;

        WinDescType::ImageType origimage;
        ImageIO::read(*f,origimage);
        if (addmargin) {
            IndexType newext = origimage.extent();
            IndexType toaddX = 0, toaddY = 0;
            if (tavsize[0])
                toaddX = newext*tmargin/tavsize[0];
            if (tavsize[1])
                toaddY = newext*tmargin/tavsize[1];

            toadd = blitz::max(toaddX,toaddY);
            newext += 2*toadd;

            image.reference(extendBorder(
                    origimage, newext/2-1, newext/2, origimage.extent()/2));
        } else {
            image.reference(origimage);
        }

        if (verbose > 5) cout << "Processing file " << *f << endl;

        int imagewindows = 0; 
        holder.clear();

        if (nopyramid) {
            if (blitz::sum(image.extent()<winsize)) {
                std::cerr <<  "Image extent "
                    << "( " << std::setw(4) << image.rows() << ", " 
                    << std::setw(4) << image.cols() << " )"
                    << " is smaller than descripor size "
                    << "( " << std::setw(4) << winsize[0] << ", " 
                    << std::setw(4) << winsize[1] << " ). Ignoring ..." ;
                continue;
            }
            windesc->preprocess(image);
            IndexType tl;
            if (hasTopLeft) {
                tl = topleft;
                if (hasFullSize)
                    tl += (image.extent() - fullsize)/2;
            } else
                tl = (image.extent() - size)/2;

            Array1DType desc = windesc->compute(tl); 

            DetectInfo r = bound(classifier(desc.data()),1, tl, windesc->extent());
            r.lbound -=toadd;

            holder(r);
            if (dotestlocs){
                using std::setw; using std::setprecision;
                testlocsstr << *f 
                << ' ' << setprecision(4) << setw(10) << tl[0] -toadd[0]
                << ' ' << setprecision(4) << setw(10) << tl[1] -toadd[1]
                << ' ' << setprecision(4) << setw(10) << size[0]
                << ' ' << setprecision(4) << setw(10) << size[1]
                            << std::endl;
            }
            ++imagewindows;
        } else {
            typedef lear::ImageSlider<2>                SliderType;
            typedef lear::ScalePyramid<2>               PyramidType;

            const PyramidType pyramid(image.extent(),winsize,
                    scaleratio, endscale, startscale);

            if (verbose > 6) {// {{{
                cout << "Scale levels" << setw(2) << pyramid.size() <<
                    ", Start scale = " << 
                    setw(6) << setprecision(2) << pyramid.startScale() <<
                    ", End scale = " <<
                    setw(6) << setprecision(2) << pyramid.endScale() <<
                    endl;
            }// }}}

            for (PyramidType::iterator piter = pyramid.begin(); 
                    piter != pyramid.end(); ++piter) 
            {// {{{
                WinDescType::ImageType pyimg (lear::rescale(image, *piter));
                SliderType slider(pyimg.extent(),winsize,winstride);

                windesc->preprocess(pyimg);

                for (SliderType::iterator siter = slider.begin(); 
                        siter != slider.end(); ++siter) 
                {
                    IndexType tl=*siter;
                    if (hasTopLeft && hasFullSize) {
                        tl += topleft;
                    }

                    Array1DType desc = windesc->compute(tl); 

                    DetectInfo r = bound(
                            classifier(desc.data()),piter.scale(),
                            tl, windesc->extent());
                    r.lbound -=toadd;

                    holder(r);
                    ++imagewindows;
                    if (dotestlocs){
                        using std::setw; using std::setprecision;
                        testlocsstr << *f 
<< ' ' << setprecision(4) << setw(10) << tl[0]*piter.scale() - toadd[0] 
<< ' ' << setprecision(4) << setw(10) << tl[1]*piter.scale() - toadd[1]
<< ' ' << setprecision(4) << setw(10) << size[0]*piter.scale() 
<< ' ' << setprecision(4) << setw(10) << size[1]*piter.scale() 
<< std::endl;
                    }
                }
            }// }}}
        }
        totalwindows+=imagewindows;

        holder.write(*f); //just write to files
        if (verbose > 3) {// {{{
            cout << "Processed " << setw(5) << imagewindows 
                 << " windows in file " << *f <<  endl;
        }// }}}
    }
    if (verbose > 0)  {// {{{
        cout << "Tested " << totalwindows << " windows" << endl;
    }// }}}
}
// }}}
#endif


LinearClassify::LinearClassify(std::string& modelfile, const int verbose) 
    :
    length_ (0), linearwt_(0),  linearbias_(0)
{// {{{
    if (verbose > 2) 
        std::cout << "Reading model file: " << modelfile;

    FILE *modelfl;
    if ((modelfl = fopen (modelfile.c_str(), "rb")) == NULL)
    { throw Exception("LinearClassify::LinearClassify", "Unable to open the modelfile"); }

    char version_buffer[10];
    if (!fread (&version_buffer,sizeof(char),10,modelfl))
    { throw Exception("LinearClassify::LinearClassify", "Unable to read version"); }

    if(strcmp(version_buffer,"V6.01")) {
        throw Exception("LinearClassify::LinearClassify", "Version of model-file does not match version of svm_classify!"); 
    }
    /* read version number */
    int version = 0;
    if (!fread (&version,sizeof(int),1,modelfl))
    { throw Exception("LinearClassify::LinearClassify", "Unable to read version number"); }
    if (version < 200)
    { throw Exception("LinearClassify::LinearClassify", "Does not support model file compiled for light version"); }

    long kernel_type;
    fread(&(kernel_type),sizeof(long),1,modelfl);

    {// ignore these
        long poly_degree;
        fread(&(poly_degree),sizeof(long),1,modelfl);

        double rbf_gamma;
        fread(&(rbf_gamma),sizeof(double),1,modelfl);

        double  coef_lin;
        fread(&(coef_lin),sizeof(double),1,modelfl); 
        double coef_const;
        fread(&(coef_const),sizeof(double),1,modelfl);

        long l;
        fread(&l,sizeof(long),1,modelfl);
        char* custom = new char[l];
        fread(custom,sizeof(char),l,modelfl);
        delete[] custom;
    }

    long totwords;
    fread(&(totwords),sizeof(long),1,modelfl);

    {// ignore these
        long totdoc;
        fread(&(totdoc),sizeof(long),1,modelfl);

        long sv_num;
        fread(&(sv_num), sizeof(long),1,modelfl);
    }

    fread(&linearbias_, sizeof(double),1,modelfl);

    if(kernel_type == 0) { /* linear kernel */
        /* save linear wts also */
        linearwt_ = new double[totwords+1];
        length_ = totwords;
        fread(linearwt_, sizeof(double),totwords+1,modelfl);
    } else {
        throw Exception("LinearClassify::LinearClassify", "Only supports linear SVM model files");
    }
    fclose(modelfl);

    if (verbose > 2) 
        std::cout << " Done" << std::endl;
}// }}}

LinearClassify::LinearClassify(const LinearClassify& o) :
    length_(o.length_), linearbias_(o.linearbias_)
{
    linearwt_ = new double[length_];
    std::copy(o.linearwt_, o.linearwt_+o.length_, linearwt_);
}

LinearClassify& LinearClassify::operator=(const LinearClassify& o) 
{
    if (&o != this) {
        if (linearwt_) 
            delete[] linearwt_;

        length_=o.length_; 
        linearbias_=o.linearbias_;

        linearwt_ = new double[length_];
        std::copy(o.linearwt_, o.linearwt_+o.length_, linearwt_);
    } 
    return *this;
}

double LinearClassify::operator()(const double* desc) const 
{
    double sum = 0;
    for (int i= 0; i< length_; ++i) 
        sum += linearwt_[i]*desc[i]; 
    return sum - linearbias_;
}

float LinearClassify::operator()(const float* desc) const 
{
    double sum = 0;
    for (int i= 0; i< length_; ++i) 
        sum += linearwt_[i]*desc[i]; 
    return sum - linearbias_;
}

void DetectedRegion::print(std::ostream& o) const
{
    using namespace std;
    o   << setw(6) << x << ' ' << setw(6) << y << ' '
        << setw(6) << width << ' ' << setw(6) << height << ' '
        << setw(9) << setprecision(5) << scale << ' ' 
        << setw(13) << setprecision(8) << scientific << score<<fixed;
}
std::ostream& operator<<(std::ostream& o, const DetectedRegion& region){
    region.print(o);
    return o;
}

extern const double         PERSON_WEIGHT_VEC[];
extern const int            PERSON_WEIGHT_VEC_LENGTH;
LinearClassify::LinearClassify() 
{// {{{
    linearbias_ = 6.6657914910925990525925044494215;
    length_ = PERSON_WEIGHT_VEC_LENGTH;
    linearwt_ = new double[length_];
    std::copy(PERSON_WEIGHT_VEC, PERSON_WEIGHT_VEC+PERSON_WEIGHT_VEC_LENGTH, linearwt_);
}// }}}
#include "persondetectorwt.tcc"

void WinDetect::print(std::ostream& o) const 
{
    o << "WinDetect ::\n"
        "  | Size    " << setw(3) << right << size_x   << "x" << setw(3) << left <<   size_y << right << 
        "          Stride  " << setw(3) << right << winstride_x << "x" << setw(3) << left << winstride_y << right << "   |\n" <<
        "  | TopLeft " << setw(3) << right << topleft_x<< "x" << setw(3) << left << topleft_y<< right << 
        "          FullSize" << setw(3) << right <<fullsize_x<< "x" << setw(3) << left <<fullsize_y<< right << "   |\n" <<
        "  | ScaleRatio " << setw(4) << left << scaleratio << " StartScale " << setw(3) << left << startscale << " EndScale " << setw(3) << left << endscale  << "|\n"
        "  | Label " << setw(4) << left << label  << "  CacheSize  " << setw(4) << left << cachesize  << "  Verbose  " << setw(4) << left << verbose << " |\n"
        "  |--------------------------------------------|\n";
}
#ifdef BUILD_APP
void WinDetectDump::print(std::ostream& o) const
{
    o << "WinDetectDump :: \n"
        "  | Pyramid " << pyramid << "  RandSeed " << randomSeed << "  Samples " << setw(2) << left << samples  << "  DumpDet " << dumpfulldetail << " |\n"
        "  | MemLimit " << setw(6) << left << memorylimit << "          HardScore " << setw(6) << left << hardscore << "    |\n" 
        "  | PosCases " << setw(6) << left << poscases    << "          NegCases  " << setw(6) << left << negcases << "    |\n"
        "  | JitterStep X:" << setw(4) << left << jitterstep_x << "  Y:" << setw(4) << left << jitterstep_y<< "   WinStep:" << setw(3) << jitterwinstep <<  "      |\n"
        "  |----------------------------------------------|\n";
    WinDetect::print(o);
}
#endif
void WinDetectClassify::print(std::ostream& o) const 
{
    o << "WinDetectClassify ::\n"
        "  | Pyramid " << !nopyramid << "   NonMax " << !no_nonmax << "   AlignImg " << aligninimage  << "   ShowSc " << showscore << " |\n"
        "  | Thres   " << setw(4) << left << threshold << "  LtThres   " << setw(4) << left << lightthreshold   << "  SoftMax   " << setw(4) << left << softmax << " |\n"
        "  | Sc2Prob   A:" << setw(4) << left << score2prob_a << "  B:" << setw(4) << left << score2prob_b<< "                     |\n"
        "  | NonMaxSig X:" << setw(4) <<  nonmaxsigma_x<< "  Y:" << setw(4) << nonmaxsigma_y << "  Y: " << setw(4) << nonmaxsigma_scale  << "            |\n"
        "  | AvgSize " << setw(3) << right << avsize_x << "x" << setw(3) << left << avsize_y << right << 
        "          Margin  " << setw(3) << right << margin_x << "x" << setw(3) << left << margin_y << right << "     |\n" <<
        "  | AlMargin" << setw(3) << right << alignmargin_x<< "x" << setw(3) << left << alignmargin_y<< right << 
        "          FStride " << setw(3) << right <<fullstride_x<< "x" << setw(3) << left <<fullstride_y<< right << "     |\n" <<
        "  |----------------------------------------------|\n";
    WinDetect::print(o);
}
