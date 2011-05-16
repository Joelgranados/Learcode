/*
 * =====================================================================================
 *
 *       Filename:  classify_rhog.cpp
 *
 *    Description:  Provides functionality to detect objects which are
 *    described by dense RHOG descriptors and learned via linear SVMs.
 *
 *    Can optionally perform non-maximum suppression 
 *
 * =====================================================================================
 */

#include "windetectmain.h"
#include <lear/image/imageutil.h>

int main(int argc, char** argv) {
    using namespace std;
    lear::Cmdline cmdline;


    WinDetectClassifyMain windetectmain;
    RHOGDenseMain rhogdensemain;

    WinDetectClassify windetect;
    {  // cmdline
        cmdline.commandName("classify_rhog");
        cmdline.version("0.0.1", "Author: Navneet Dalal "
                "(mailto: Navneet.Dalal@inrialpes.fr)");
        cmdline.brief(
"Compute HOG features and classify objects in image(s)");

        cmdline.description(
"For all windows at all scales in the image, compute HOG features and classify each window as positive or negative class");

        // set command line options. The statement below will fill up usage, and code for reading various command line options
        windetectmain.setCommonMainParam(cmdline, &windetect) ;
        windetectmain.setClassifyParam(cmdline,&windetect);
        rhogdensemain.setRHOGDenseParam(cmdline) ;
    }

    int status = cmdline.parse(argc, argv);
    if (status != cmdline.ok)
        return status;

    if (!windetectmain.imageext.empty() && windetectmain.imageext[0] != '.')
        windetectmain.imageext = '.' + windetectmain.imageext;

    // now set options read from command line 
    windetectmain.fill(&windetect);

    std::vector<const RHOGDenseParam*> desc = rhogdensemain.fill();
    windetect.init(desc); 
    for (unsigned i= 0; i< desc.size(); ++i) 
        delete desc[i];

    try {
        LinearClassify* classifier = NULL;
        if (windetectmain.modelfile == "defaultperson")
            classifier = new LinearClassify();
        else 
            classifier = new LinearClassify(windetectmain.modelfile, windetect.verbose);

        try {
            WinDetectDump::PathVector inlist;
            lear::imagelist(inlist, windetectmain.infile, windetectmain.imageext);
            windetect.runImageSlider(*classifier, inlist, windetectmain.outfile,
                    windetectmain.outimage, windetectmain.outhist,
                    windetectmain.falsetxt, windetectmain.testlocs
                    );
        }catch (std::exception& e) {
            delete classifier;
            throw e;
        }
        delete classifier;
    } catch(std::exception& e) {
        cerr << "Caught "<< e.what() << endl;
        return 1;
    } catch(...) {
        cerr << "Caught unknown exception" << endl;
        return 1;
    }
    return 0;
}


