/*
 * =====================================================================================
 *
 *       Filename:  dump_rhog.cpp
 *
 *    Description:  Provides functionality to dump dense RHOGDense
 *    descriptors.
 *
 * =====================================================================================
 */


#include "windetectmain.h"
#include <lear/image/imageutil.h>

int main(int argc, char** argv) {
    using namespace std;
    lear::Cmdline cmdline;
    bool hardtest = false;


    WinDetectDumpMain windetectmain;
    RHOGDenseMain rhogdensemain;

    WinDetectDump windetect;
    {  // cmdline
        cmdline.commandName("dump_rhog");
        cmdline.version("0.0.1", "Author: Navneet Dalal "
                "(mailto: Navneet.Dalal@inrialpes.fr)");
        cmdline.brief(
"Compute RHOGDense features over images");

        cmdline.description(
"Compute RHOG fetures using image gradient strength over image window. The whole algorithm is:\n"
"1) divide image window into uniformly spaced dense points with point spacing given by 'descstride'\n"
"2) for each point, compute 3-dimensional position-orientation gradient histogram\n"
"3) finally create a big feature vector containing descriptor vector from each point in the window\n"
);

    cmdline.addOption()
        ("dumphard",lear::bool_option(&(hardtest)),
            "infile is hard examples file")
        ;
        // set command line options. The statement below will fill up usage, and code for reading various command line options
        windetectmain.setCommonMainParam(cmdline, &windetect) ;
        windetectmain.setDumpParam(cmdline, &windetect) ;
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

        if (hardtest) {
            windetect.writeHardTest(windetectmain.infile, windetectmain.outfile);
        } else {
            WinDetectDump::PathVector inlist;
            lear::imagelist(inlist, windetectmain.infile, windetectmain.imageext);
            windetect.writeWinDesc(inlist, windetectmain.outfile, windetectmain.testlocs);
        }
    } catch(std::exception& e) {
        cerr << "Caught "<< e.what() << endl;
        return 1;
    } catch(...) {
        cerr << "Caught unknown exception" << endl;
        return 1;
    }
    return 0;
}


