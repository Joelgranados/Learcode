/*
 * =====================================================================================
 * 
 *       Filename:  windetect.h
 * 
 *    Description:  Groups all common options for windows based object
 *    detection.
 *
 * =====================================================================================
 */

#ifndef  _LEAR_WIN_DETECT_H_
#define  _LEAR_WIN_DETECT_H_

#include <list>
#include <vector>
#include <string>
#include <iostream>

#define BUILD_APP

// Set required RHOG Dense parameters in an object of this class.
struct RHOGDenseParam {
    typedef float        RealType;

    enum NormalizerFlags {
        NormL2Hys=0,
        NormL1Sqrt,
        NormL2,
        NormL1,
        NoNorm
    };

    enum PreprocessorFlags {
        RGB_Grad=0, // Use gradient of RGB Image
        Lab_Grad, // Use gradient of Lab Image
        RGB_Sqrt_Grad, // Use gradient of Sqrt(RGB)
        Lab_Sqrt_Grad, // Use gradient of Sqrt(Lab)
        RGB_Log_Grad   // Use log(RGB+1)
    };


    int                 cellsize_x, cellsize_y;
    int                 numcell_x, numcell_y;
    int                 descstride_x, descstride_y;
    int                 orientbin;
    bool                semicirc; 
    RealType            wtscale;
    RealType            gscale;
    RealType            epsilon;
    RealType            maxvalue;


    /// Normalization to use
    PreprocessorFlags                   preprocessing2use;
    NormalizerFlags                     norm2use;


    RHOGDenseParam():
        cellsize_x(8), cellsize_y(8),
        numcell_x(2), numcell_y(2),
        descstride_x(8), descstride_y(8),
        orientbin(9), semicirc(true), wtscale(2), 
        gscale(0), epsilon(1), maxvalue(0.2),
        preprocessing2use(RGB_Sqrt_Grad), norm2use(NormL2Hys)
    { }

};

/**
 * This is the main class. It provides the initialization routine which given
 * an pointer to RHOGDenseParam, initializes the RHOG Dense descriptor
 * computation. The computefeature method can then be used to compute 
 * HOG descriptors at any given location.
 *
 * Use it if you are interested in computing HOG features at specified locations.
 */
struct WinDetect {
    // ===================================
    //------ Type definitions -------
    // ===================================

    // All calculations done in realtype precision.
    typedef RHOGDenseParam::RealType               RealType;

    // ===================================
    //------ Funciton declarations -------
    // ===================================

    // No need to change any variable if you intend to call default person detection.
    WinDetect() :
        // common options
        endscale(0), scaleratio(1.05), startscale(1),
        size_x(64), size_y(128), winstride_x(8), winstride_y(8),
        topleft_x(-1), topleft_y(-1), fullsize_x(-1), fullsize_y(-1),

        // common options
        label(DefaultLabel),
        verbose(0), cachesize(16)
    { } 
    
    virtual ~WinDetect() {}

    /**
     * Creates and initializes rhogdense descriptors from provided parameters
     *
     * The RHOGDenseParam* may be destroyed immediately after this call.
     */
    virtual void init(const RHOGDenseParam* param);
    /**
     * Same as above. We use std::vector as the code supports simultaneous 
     * computation of multiple RHOGDense descriptors each with different parameter
     * configurations, i.e. one can compute different descriptors on the same test 
     * location.
     *
     * The vector may be destroyed immediately after this call.
     *
     * NOTE: The initialization must be called only once. Currently there is
     * no support for dynamically changing the RHOG parameters.
     */
    virtual void init(const std::vector<const RHOGDenseParam*>& param);

    /**
     * Preprocesses an image and computes the HOG descriptor. The descriptor
     * is set in the 'result' pointer.
     *
     * Return value is 'true' if descriptor computation was successful, else
     * its false.
     *
     * NOTE 1: User is expected to pre-allocate the 'result' pointer with its 
     * length at least as long as the output of featurelength() 
     *
     * NOTE 2: This is an inefficient way to compute descriptors. If you need
     * to compute features at several location at once, use the vector version
     *
     * Image data is assumed to be in row first format via unsigned char* pointer.
     * The data must be RGB format.
     *
     * The i,j th pixel is read as
     * unsigned char *pixel = image+(j*step + i)*3*sizeof(unsigned char);
     * unsigned char R = pixel[0];
     * unsigned char G = pixel[1];
     * unsigned char B = pixel[2];
     *
     * Step of 0 implies use same value as width.
     */
    bool computefeature(float* result, const int xloc, const int yloc,
            const unsigned char* image, int width, int height, int step=0) const;

    /**
     * Return value is a std::vector of same size as xlocs and ylocs, with value set
     * to true if descriptor computation succeeded else it is set to false.
     *
     * NOTE 1: xloc size and yloc size must be same.
     *
     * NOTE 2: result vector must be preallocated with its size same as xloc.
     * Each element in the vector must also be preallocated with at least as much
     * memory as returned by the featurelength
     *
     * xloc and yloc specify the top-left corner. If location is found to be
     * out of bounds, then return value is false.
     *
     * Step of 0 implies use same value as width.
     */
    std::vector<bool> computefeature(std::vector<float*>& result, 
            const std::vector<int>& xloc, const std::vector<int>& yloc,
            const unsigned char* image, int width, int height, int step=0) const;

    
    int featurelength() const;

    virtual void print(std::ostream& o) const ;
    // ===================================
    //------ Variable declarations. Common to both feature dump and classify -------
    // ===================================

    // All variables are public on purpose. Feel free to change as you feel like.
    // However refrain from changing variables after the init calls.

    /// Scale space parameters
    RealType endscale, scaleratio, startscale;

    // window size. for e.g. in person detection I use 64x128 pixels
    int size_x, size_y;

    // HOG blocks are stride width apart.
    int winstride_x, winstride_y;

    // Ignore these. For internal use
    int topleft_x, topleft_y;

    // Ignore these. For internal use
    int fullsize_x, fullsize_y; 

    // Ignore these. For internal use
    char label; 

    /// verbose level. Between [0-9]. 9 is most verbose.
    int verbose;

    /// Cache size. Reducing cache size can significantly hurt the run-time. Keep it to at least
    /// its default value. Size in MBs
    int cachesize;

    static const char DefaultLabel;
};

#ifdef BUILD_APP
/**
 * This class is for internal use. It is used to dump HOG blocks to a binary
 * file which can then be converted via dump4svmlearn to binary svm_learn
 * format.
 *
 * Need not bother about this class if in the library version of classify_rhog
 */
struct WinDetectDump : public WinDetect{

    typedef std::list<std::string>              PathVector;

    // ===================================
    //------ Funciton declarations -------
    // ===================================

    // No need to change any variable if you intend to call default person detection.
    WinDetectDump() :
        WinDetect(), 

        // dump feature options
        pyramid(false), 
        samples(1), 
        randomSeed(false),
        dumpfulldetail(0),

        hardscore(0),
        memorylimit(0),
        poscases(0),
        negcases(0),
        jitterstep_x(1), jitterstep_y(1), 
        jitterwinstep(0)

    {
    }

    void writeWinDesc( 
        const PathVector& inlist,// filenames to compute descriptors on in each line
        const std::string& outfile,// Output file to dump
        const std::string& testlocs// if specified, dump test locations to this file. 
        );

    void writeHardTest( 
        const std::string& hardfile,// hard examples file
        const std::string& outfile // output file to dump
        );

    virtual void print(std::ostream& o) const ;
    // ===================================
    //------ Variable declarations. Used during feature dump -------
    // ===================================
    bool pyramid;

    int samples;

    bool randomSeed;

    int dumpfulldetail;

    RealType hardscore;
    
    int memorylimit;

    int poscases;

    int negcases;

    int jitterstep_x, jitterstep_y;

    int jitterwinstep;
};
inline std::ostream& operator<<(std::ostream& o, const WinDetectDump& windet) 
{ windet.print(o); return o; }

#endif
/**
 * Read output model file after svm learning and fills in bias and weight
 * vector. Only linear SVM output model file are supported.
 */
class LinearClassify {
    public:

    // The default person detection parameters are hard coded, i.e. 
    // 8x8 cell, 2x2 no. of cells in each block, 9 orientation bin etc.
    // This default constructor simply loads the learned classify model. The model is hard coded. 
    LinearClassify() ;

    // Loads the linear SVM model from file 
    LinearClassify(std::string& filename, const int verbose = 0) ;

    LinearClassify(const LinearClassify& ) ;

    LinearClassify& operator=(const LinearClassify& ) ;

    int length() const 
    { return length_; }

    double operator()(const double* desc) const ;

    float operator()(const float* desc) const ;

    ~LinearClassify() {
        delete[] linearwt_;
    }

    private:
        int length_;
        double* linearwt_;
        double linearbias_;
};

struct DetectedRegion {
    float score, scale;
    int x, y, width, height;

    DetectedRegion() : 
        score(0), scale(0), x(0), y(0), width(0), height(0)
    {}

    DetectedRegion(
            const float score, const float scale,
            const int x, const int y, const int w, const int h)
            : 
        score(score), scale(scale), x(x), y(y), width(w), height(h) 
    {}

    void print(std::ostream& o) const;
};
std::ostream& operator<<(std::ostream& o, const DetectedRegion& region);
/**
 * Detect objects in test images.
 */
struct WinDetectClassify : public WinDetect{
    typedef std::list<std::string>              PathVector;

    WinDetectClassify() :
        WinDetect(), 
        avsize_x(0), avsize_y(96),
        margin_x(4), margin_y(4),
        alignmargin_x(4), alignmargin_y(4),
        fullstride_x(-1), fullstride_y(-1),
        nopyramid(false), no_nonmax(false),
        aligninimage(false), showscore(true),
        softmax(0), threshold(0.1), lightthreshold(0),
        nonmaxsigma_x(8), nonmaxsigma_y(16), nonmaxsigma_scale(1.3),
        score2prob_a(1), score2prob_b(0)
    {}

    inline virtual void init(const RHOGDenseParam* param) 
    {
        WinDetect::init(param);// this one calls below function. which in turns calls initclassifier.
    }
    inline virtual void init(const std::vector<const RHOGDenseParam*>& param)
    {
        WinDetect::init(param);
        initclassifier();
    }

    /**
     * Run any test only after calling init.
     *
     * NOTE 1: 'detections' is cleared in each call. So it contains only fresh
     * detections on this image.
     *
     * NOTE 2: The detected region bounds may go out of image bounds.
     *
     * NOTE 3: nopyramid, no_nonmax, showscore suppression options have no effect. This
     * code always run non-maximum suppression on scale-space.
     *
     * Step of 0 implies use same value as width.
     */
    void  test(const LinearClassify& classifier, std::list<DetectedRegion>& detections,
            const unsigned char* imagedata, int width, int height, int step=0) const;

#ifdef BUILD_APP
    /** 
     * This is for internal use. Binary application functionality is coded in this.
     */
    void runImageSlider(
        const LinearClassify& classifier, const PathVector& inlist, 
        const std::string& outfile, const std::string& outimage, 
        const std::string& outhist, const std::string& falsetxt,
        const std::string& testlocs// if specified, dump test locations to this file. 
        )  const;
#endif

    virtual void print(std::ostream& o) const ;

    // ===================================
    // ------- Other options used during classification or testing
    // ===================================


    // Set the height/width of object on average.
    // For e.g. in person detection even though the window length is 128, average person is 
    // about 96 pixel tall. So its default value is 96.
    // It is used only to take care of boundary cases as so often objects are close to boundaries.
    // Average size and margin are used to expand input image and then we run classifier on it.
    int avsize_x, avsize_y;

    // Specify the margin size w.r.t. average size.
    // Formula of extension is max(margin_x/avsize_x*width, margin_y/avsize_y*height) on each side.
    int margin_x, margin_y;

    // Use to align input w.r.t. classifier max. Test code. Ignore it.
    int alignmargin_x, alignmargin_y;

    // Test code. Ignore it.
    int fullstride_x, fullstride_y;

    // If true, do not run scale space pyramid
    bool nopyramid; 

    // if true, do not perform non-maximum suppression
    bool no_nonmax; 

    // test code. ignore it.
    bool aligninimage; 

    bool showscore;

    int softmax;

    // Final threshold after non-maximum threshold.
    RealType threshold;

    // Only those locations which are above this value, are passed to non-maximum suppression stage.
    RealType lightthreshold; 

    // sigma for non-maximum suppression. 
    float nonmaxsigma_x, nonmaxsigma_y, nonmaxsigma_scale;

    float score2prob_a, score2prob_b;

    protected:
        void initclassifier() ;
};
inline std::ostream& operator<<(std::ostream& o, const WinDetectClassify& windet) 
{ windet.print(o); return o; }

#endif // 

