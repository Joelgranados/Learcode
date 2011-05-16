/*
 * =====================================================================================
 *
 *       Filename:  windetectmain.cpp
 *
 *    Description:  Provides implementation of windetectmain.h
 *
 * =====================================================================================
 */

#include <blitz/array.h>
#include <blitz/tinyvec.h> 

#include <lear/util/fileutil.h>
#include <lear/image/imageio.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "windetectmain.h"

//## common main options

typedef blitz::TinyVector<RealType,3>       PixelType;
typedef blitz::TinyVector<int,3>            RGBType;
typedef blitz::Array<PixelType,2>           ImageType;

using namespace std;
using namespace lear;

void WinDetectMain::fill(WinDetect* param) const
{
    param->size_x = size[0];
    param->size_y = size[1];

    param->winstride_x = winstride[0];
    param->winstride_y = winstride[1];

    param->topleft_x  = topleft[0];
    param->topleft_y  = topleft[1];

    param->fullsize_x  = fullsize[0];
    param->fullsize_y  = fullsize[1];

}
void WinDetectDumpMain::fill(WinDetectDump* param) const
{
    WinDetectMain::fill(param);

    // WinDetectDump parameters
    param->jitterstep_x  = jitterstep[0];
    param->jitterstep_y  = jitterstep[1];
}
void WinDetectClassifyMain::fill(WinDetectClassify* param) const
{
    WinDetectMain::fill(param);

    // WinDetectClassify parameters
    param->margin_x = margin[0];
    param->margin_y = margin[1];

    param->avsize_x = avsize[0];
    param->avsize_y = avsize[1];

    param->alignmargin_x = alignmargin[0];
    param->alignmargin_y = alignmargin[1];

    param->fullstride_x = fullstride[0];
    param->fullstride_y = fullstride[1];

    param->nonmaxsigma_x = nonmaxsigma[0];
    param->nonmaxsigma_y = nonmaxsigma[1];
    param->nonmaxsigma_scale = nonmaxsigma[2];

    param->score2prob_a = score2prob[0];
    param->score2prob_b = score2prob[1];

}

// Cmdline parsing 
void WinDetectMain::setCommonMainParam(lear::Cmdline& cmdline, WinDetect* param) 
{// {{{
    IndexOpt::seperator(",x");
    MultIndexOpt::seperator(":^");

#ifdef __RELEASE__
    std::string s=" (Release date ";
    s+=__DATE__;
    s+=")";
    cmdline.appendBrief(s);
#endif
    using namespace lear;
    std::ostringstream usageIssues ;
    usageIssues <<
"options 'stride', 'startscale', 'scaleratio' and 'endscale' "
" are valid iff 'pyramid' is true'\n"
"  'infile'         can be an image file, a directory, or a list file.\n"
"  'imageext'       empty string implies all listed files, otherwise images "
"with given extension are selected\n"
;
    cmdline.appendUsageIssues(usageIssues.str());


    cmdline.addOption()
        ("window,W",option<IndexOpt>(&(size))
            ->defaultValue(IndexOpt(64,128))->minValue(3),
            "window width,height")
        ("winstride",option<IndexOpt>(&(winstride))
            ->defaultValue(8),
            "window stride along x-y")

        ("scaleratio",option<RealType>(&(param->scaleratio))
            ->defaultValue(1.05)->minValue(1),
            "scale ratio")
        ("endscale",option<RealType>(&(param->endscale)),
            "scale-space pyramid end scale\n"
            "  (Default: till image size >= window size)")
        ("startscale",option<RealType>(&(param->startscale))
            ->defaultValue(1)->minValue(1),
            "start scale")

        ("topleft",option<IndexOpt>(&(this->topleft)),
            "top-left X,Y-coordinate of descriptor window\n"
            "  (Default window is at image center )")
#ifndef __RELEASE__
        ("fullsize",option<IndexOpt>(&(this->fullsize)),
            "full window width,height\n"
        "  Use iff top-left is valid, not used for hard examples")
#endif
        ("cachesize",option<int>(&(param->cachesize))
            ->defaultValue(128)->minValue(4),
            "descriptor cache size (im MB) to use\n"
            "  NOTE: small cache size can drastically reduce speed")
        ("verbose,v",option<int>(&(param->verbose))
            ->defaultValue(0)->minValue(0)->maxValue(9),
            "verbose level")

        ("testlocs",option<std::string>(&(testlocs))
            ->defaultValue(""),
            "filename where to write all test or feature dump locations\n"
            "  (Format: imagename X Y Width Height)")

        ("imageext,x",option<std::string>(&(imageext))
            ->defaultValue(""),
            "image extension (valid only if 'infile' = dir or list)")
        ;

        cmdline.addArgument()
            ("infile",option<std::string>(&(infile)),"input image file/dir/list")
            ("outfile",option<std::string>(&(outfile)),"out file");
}// }}}

void WinDetectDumpMain::setDumpParam( lear::Cmdline& cmdline, WinDetectDump* param) 
{ // {{{
    using namespace lear;
#ifndef __RELEASE__
    cmdline.appendUsageIssues(
"   'dumpdetail'    dump window detail in the output file"
"(currently does not work for hard examples or with pyramid)\n"
"                   0 - Write only descriptor\n"
"                   1 - Write descriptor top-left, extent, scale\n"
"                   2 - Write filename information also\n"
            );
#endif

    cmdline.addOption()
#ifndef __RELEASE__
        ("dumpdetail,f",option<int>(&(param->dumpfulldetail))
            ->minValue(0)->maxValue(3)->defaultValue(0),
            "dump extra information about the feature in the file")
#endif
        ("samples,s",option<int>(&(param->samples))
            ->defaultValue(1)->minValue(0),
            "number of random sampling windows over each image OR  "
            "if hard is true, number of samples to skip")
        ("random,r",bool_option(&(param->randomSeed)),
            "initialize random generator with current time")
        ("hardscore",option<RealType>(&(param->hardscore))
            ->defaultValue(0),
            "use only points with score greater than hardscore")

        ("poscases",option<int>(&(param->poscases)),
            "number of positive cases")
        ("negcases",option<int>(&(param->negcases)),
            "number of negative cases")
        ("memorylimit",option<int>(&(param->memorylimit)),
            "upper memory limit on training examples")
#ifndef __RELEASE__
        ("jitterwinstep",option<int>(&(param->jitterwinstep))
            ->defaultValue(0)->minValue(0),
            "jitter window steps")
        ("jitterstep",option<IndexOpt>(&(param->jitterstep))
            ->defaultValue(1)->minValue(1),
            "jitter step size")
#endif
        ("label,l",option<char>(&(param->label))
            ->defaultValue(WinDetect::DefaultLabel)->minValue('N')->maxValue('P'),
            "hard file labels (Default is ignore label)")

        ("pyramid,p",bool_option(&(param->pyramid)),
            "compute descriptors over scale-space pyramid")

        ;
}// }}}


void WinDetectClassifyMain::setClassifyParam( lear::Cmdline& cmdline, WinDetectClassify* param) {
    using namespace lear;
    cmdline.addOption()
        ("svmthreshold,m",option<RealType>(&(param->lightthreshold))
            ->defaultValue(0),
            "threshold on svm score")
        ("threshold,t",option<RealType>(&(param->threshold))
            ->defaultValue(0.1),
            "threshold to classify")
        ("no_nonmax",bool_option(&(param->no_nonmax)),
            "do not perform non maximum suppression")
        ("avsize",option<IndexOpt>(&avsize)
            ->minValue(0)->defaultValue(IndexOpt(0,96)),
            "compute image margin to add using average size. 0 imply do not use average size.")
        ("margin",option<IndexOpt>(&margin)
            ->minValue(0)->defaultValue(4),
            "pixels margin at each side in normalized image. 0 imply do not use margin size.")
#ifndef __RELEASE__
        ("fullstride",option<IndexOpt>(&fullstride),
            "full reference stride\n"
"  Use iff working with parts & top-left is valid")
#endif
        ("nopyramid",bool_option(&(param->nopyramid)),
            "do not create scale space pyramid, use center window")
        ("softmax",option<int>(&(param->softmax))
            ->defaultValue(0)->minValue(0)->maxValue(3),
            "transformation function method used  to convert scores to "
                "probabilities. "
                "Values (0=hardclip,1=sigmoid,2=softclip,3=none). "
                "NOTE 1: score2prob provides A,B parameters for sigmoid"
                " and softclip method. " 
                "NOTE 2: (score2prob[0],svmthreshold) provides A,B values"
                " for hardclip methods."
                )
        ("score2prob,p",option<SigmaOptType>(&score2prob)
            ->defaultValue(SigmaOptType(1,0)),
            "(A,B) parameters of sigmoid function (1+exp(A*score+B)^-1"
            " or soft clip log(1+exp(A*(score-B)")
        ("nonmaxsigma,z",option<NonmaxOptType>(&nonmaxsigma)
            ->defaultValue(NonmaxOptType(8,16,1.3))
            ->minValue(NonmaxOptType(0,0,0)),
            "smooth sigma for non-max suppression (x,y, scale)")

        ("outimage,i",option<std::string>(&outimage),
            "align input image to max of classifier\n"
            "  (valid iff no_nonmax=1 && alignin=1)"
            " ELSE\n"
            "  mark classification result on input image"
            )
        ("alignin",bool_option(&(param->aligninimage)),
            "align input image (outimage must be specified)")
        ("alignmargin",option<IndexOpt>(&alignmargin)
            ->minValue(0)->defaultValue(4),
            "margin to add to aligned image")
        ("outhist,c",option<std::string>(&outhist),
            "write score histogram (for roc curve)")
        ("label,l",option<char>(&(param->label))
            ->defaultValue(WinDetect::DefaultLabel)->minValue('N')->maxValue('P'),
            "show label (Default means ignore label)")
        ("falsetxt,d",option<std::string>(&falsetxt)
            ->defaultValue(""),
            "output false + descriptor windows (for hard retraining)")
        ;
    cmdline.addArgument()
        ("modelfile",option<std::string>(&modelfile),
            "learned SVM light model file. NOTE: If value is equal to 'defaultperson', it loads default person detector.")
        ;
}// }}}

unsigned RHOGDenseMain::size() const
{
    std::vector<unsigned int> svec;

    svec.push_back(iprocessor.size());
    svec.push_back(normalizer.size());
    svec.push_back(cellsize.size());
    svec.push_back(numcell.size());
    svec.push_back(descstride.size());
    svec.push_back(orientbin.size());
    svec.push_back(wtscale.size());
    svec.push_back(fullcirc.size());
    svec.push_back(gscale.size());
    svec.push_back(epsilon.size());
    svec.push_back(maxvalue.size());

    return *std::max_element(svec.begin(),svec.end());
}
// descriptor specific parameters
void RHOGDenseMain::setRHOGDenseParam(lear::Cmdline& cmdline) 
{
    using namespace lear;

    cmdline.appendUsageIssues( iprocessoropt_.usage());
    cmdline.appendUsageIssues( normalizeropt_.usage());

    cmdline.addOption()
        ("proc",option<MultStrOpt>(&(iprocessor))
            ->defaultValue(iprocessoropt_.defaultOption()),
            "Image preprocessor and gradient computation method to use")
        ("norm",option<MultStrOpt>(&(normalizer))
            ->defaultValue(normalizeropt_.defaultOption()),
            "Descriptor normalizing method to use")

        ("cellsize,C",option<MultIndexOpt>(&(cellsize))
            ->defaultValue(8)->minValue(1),
            "cell size, position invariance tolerance in each cell")
        ("numcell,N",option<MultIndexOpt>(&(numcell))
            ->defaultValue(2)->minValue(1),
            "number of cells in each block")
        ("descstride,G",option<MultIndexOpt>(&(descstride))
            ->defaultValue(8)->minValue(1),
            "descriptor stride in window")
        ("orientbin,B",option<MultIntOpt>(&(orientbin))
            ->defaultValue(9)->minValue(1),
            "number of orientation bin")
        ("wtscale",option<MultRealOpt>(&(wtscale))
            ->defaultValue(2)->minValue(0),
            "magnitude weighting scale")
        ("fullcirc,O",option<MultBoolOpt>(&(fullcirc)),
            "take orientations in range (0-360)")
        ("gscale,S",option<MultRealOpt>(&(gscale))
            ->defaultValue(0)->minValue(0),
            "gradient computation scale")
        ("epsilon",option<MultRealOpt>(&(epsilon))
            ->defaultValue(1)->minValue(0),
            "epsilon to add while normalizing")
        ("maxvalue",option<MultRealOpt>(&(maxvalue))
            ->defaultValue(0.2)->minValue(0)->maxValue(1),
            "chop feature vector values > maxvalue after normalizing\n"
    "  Used iff normalizing method == NormL2Hys")
        ;
}

std::vector<const RHOGDenseParam*> RHOGDenseMain::fill() const
{
    unsigned numdesc = size();

    std::vector<const RHOGDenseParam*>    desclist(numdesc);
    for (unsigned i= 0; i< numdesc; ++i) {
        RHOGDenseParam* desc = new RHOGDenseParam();

        if (iprocessor.size())
            desc->preprocessing2use = (RHOGDenseParam::PreprocessorFlags)iprocessoropt_.check(iprocessor.at(i));

        if (normalizer.size())
            desc->norm2use = (RHOGDenseParam::NormalizerFlags)normalizeropt_.check(normalizer.at(i));

        if (cellsize.size()) {
            desc->cellsize_x = cellsize.at(i)[0];
            desc->cellsize_y = cellsize.at(i)[1];
        }

        if (numcell.size()) {
            desc->numcell_x = numcell.at(i)[0];
            desc->numcell_y = numcell.at(i)[1];
        }

        if (descstride.size()) {
            desc->descstride_x = descstride.at(i)[0];
            desc->descstride_y = descstride.at(i)[1];
        }

        if (orientbin.size()) 
            desc->orientbin = orientbin.at(i);

        if (fullcirc.size()) 
            desc->semicirc = !fullcirc.at(i);

        if (wtscale.size()) 
        desc->wtscale = wtscale.at(i);

        if (gscale.size()) 
            desc->gscale = gscale.at(i);

        if (epsilon.size()) 
            desc->epsilon = epsilon.at(i);
        if (maxvalue.size()) 
            desc->maxvalue = maxvalue.at(i);

        desclist[i] = desc;
    }
    return desclist;
}

