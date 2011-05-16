/*
 * =====================================================================================
 * 
 *       Filename:  windetectmain.h
 * 
 *    Description:  Provides interface for windows detectors main application.
 *    No need to care about this file as it only reads command and sets
 *    various parameters appropriately.
 * 
 * =====================================================================================
 */

#ifndef  _LEAR_WIN_DETECT_MAIN_H_
#define  _LEAR_WIN_DETECT_MAIN_H_

#include <list>
#include <string>

#include <lear/cmdline.h>
#include <lear/vectoropt.h>
#include <lear/tinyvecopt.h>
#include <lear/util/customoption.h>

#include <lear/interface/windetect.h>

typedef WinDetect::RealType                         RealType;
typedef lear::TinyVecOpt<int,2>                     IndexOpt;
typedef lear::TinyVecOpt<RealType,2>                Real2DOpt;
typedef lear::VectorOpt<std::string>                MultStrOpt;

typedef lear::VectorOpt<bool>                       MultBoolOpt;
typedef lear::VectorOpt<int>                        MultIntOpt;
typedef lear::VectorOpt<RealType>                   MultRealOpt;
typedef lear::VectorOpt<IndexOpt>                   MultIndexOpt;

typedef lear::TinyVecOpt<RealType,2>                SigmaOptType;
typedef lear::TinyVecOpt<RealType,3>                NonmaxOptType;

//  Used when called with command line.
struct WinDetectMain {
    // --- Variable declarations
    std::string infile, outfile, imageext, testlocs;

    // WinDetect parameters
    IndexOpt size, winstride;

    IndexOpt topleft, fullsize;

    WinDetectMain():
        size(64,128), winstride(8),
        topleft(-1), fullsize(-1)
    { }

    virtual ~WinDetectMain() 
    {}


    // Some parameters of windetect are set basic types. We 
    // read them from the command line in IndexOpt or Real2DOpts and
    // set them to windetect 
    // Other such as scaleratio etc are already in basic types
    // and are directly set in WinDetect
    void virtual fill(WinDetect* param) const;

    void setCommonMainParam( lear::Cmdline& cmdline, WinDetect* param) ;

};
struct WinDetectDumpMain : public WinDetectMain {
    // --- Variable declarations

    // WinDetectDump parameters
    IndexOpt jitterstep;

    WinDetectDumpMain(): WinDetectMain(),
        jitterstep(1)
    { }


    // Some parameters of windetect are set basic types. We 
    // read them from the command line in IndexOpt or Real2DOpts and
    // set them to windetect 
    // Other such as scaleratio etc are already in basic types
    // and are directly set in WinDetect
    virtual void fill(WinDetectDump* param) const;

    void setDumpParam( lear::Cmdline& cmdline, WinDetectDump* param) ;
};
struct WinDetectClassifyMain: public WinDetectMain {
    // --- Variable declarations
    std::string modelfile, outimage, outhist, falsetxt;

    // WinDetectClassify parameters
    IndexOpt        margin;
    IndexOpt        avsize;
    IndexOpt        alignmargin;
    IndexOpt        fullstride;
    NonmaxOptType   nonmaxsigma; 
    SigmaOptType    score2prob;
     

    WinDetectClassifyMain(): 
        WinDetectMain(),
        margin(0), avsize(0), 
        alignmargin(0), fullstride(-1),
        nonmaxsigma(12,24,1.2), score2prob(1,0)
    { }


    // Some parameters of windetect are set basic types. We 
    // read them from the command line in IndexOpt or Real2DOpts and
    // set them to windetect 
    // Other such as scaleratio etc are already in basic types
    // and are directly set in WinDetect
    virtual void fill(WinDetectClassify* param) const;

    void setClassifyParam( lear::Cmdline& cmdline, WinDetectClassify* param) ;

};

//  Used when called with command line.
struct RHOGDenseMain {

    MultStrOpt  iprocessor, normalizer;
    MultIndexOpt cellsize, numcell, descstride;
    MultIntOpt  orientbin; 
    MultBoolOpt fullcirc;
    MultRealOpt wtscale;
    MultRealOpt gscale, epsilon, maxvalue;  

    lear::CustomOption iprocessoropt_;
    lear::CustomOption normalizeropt_;

    RHOGDenseMain():
        iprocessoropt_("image preprocessor",RHOGDenseParam::RGB_Sqrt_Grad),
        normalizeropt_("descriptor normalizer",RHOGDenseParam::NormL2Hys)
    {
        iprocessoropt_
            .add("RGB", RHOGDenseParam::RGB_Grad)
            .add("Lab", RHOGDenseParam::Lab_Grad)
            .add("RGB_Sqrt", RHOGDenseParam::RGB_Sqrt_Grad)
            .add("Lab_Sqrt", RHOGDenseParam::Lab_Sqrt_Grad)
            .add("RGB_Log", RHOGDenseParam::RGB_Log_Grad)
            ;
        normalizeropt_
            .add("NoNorm",RHOGDenseParam::NoNorm)
            .add("L1",RHOGDenseParam::NormL1)
            .add("L2",RHOGDenseParam::NormL2)
            .add("L1Sqrt",RHOGDenseParam::NormL1Sqrt)
            .add("L2Hys",RHOGDenseParam::NormL2Hys)
            ;
    }

    unsigned size() const ;

    // Some parameters of windetect are set basic types. We 
    // read them from the command line in IndexOpt or Real2DOpts and
    // set them to windetect 
    // Other such as scaleratio etc are already in basic types
    // and are directly set in WinDetect
    std::vector<const RHOGDenseParam*> fill() const;

    void setRHOGDenseParam(lear::Cmdline& cmdline) ;
};

#endif   // ----- #ifndef _LEAR_WIN_DETECT_MAIN_H_ 

