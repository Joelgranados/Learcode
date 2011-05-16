/*
 * =====================================================================================
 *
 *       Filename:  dump4svmlearn.cpp
 *
 *    Description:  Convert dump_rhog output to binary svm_learn format.
 *
 * =====================================================================================
 */

#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <lear/io/ioext.h>
#include <lear/blitz/blitzio.h>
#include <lear/io/fileheader.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>
#include <lear/util/customoption.h>
#include <lear/util/util.h>

#include <lear/cmdline.h>

#include "rawdescio.h"
#include "formatstream.h"

using std::cout; using std::cerr; using std::endl;
// ==========================================================================
// -------- Command typedefs        -----------------------------------------
// ==========================================================================
typedef std::vector<std::string>    FileListVector;

typedef int                         TargetType;

typedef blitz::Array<int,1>         Array1DInt;


enum FileFormat { BiSVMLight, SVMLight, BiMatlab, Matlab};

// ==========================================================================
// -------- Command line parameters -----------------------------------------
// ==========================================================================
static FileListVector posfile, negfile;
static std::string outfile;

static int maxposvec, maxnegvec;
static int verbose;
static FileFormat formatType;

/// length of feature vector
static int featureLength;

/// local feature vector cache
static Array1DType feature;

static void writeData(
        FormatOutStream<RealType,TargetType>* to, 
        const std::string& filename,  
        int& maxvec, 
        const TargetType target)
{// {{{
    RawDescIn desc(filename, verbose);

    if (featureLength <= 0) 
    {
        featureLength = desc.featureLength();
        // allocate space for computing norm and std
        feature.resize(featureLength);

    } else if (desc.featureLength() != featureLength) 
    {
        std::ostringstream mesg;
        mesg << "Unequal feature length in file " << filename
            <<". Expected feature length " << featureLength
            <<", found feature length " << desc.featureLength();
        throw lear::Exception("writeData()",mesg.str());
    }
    int size = desc.featureCount();
    if (maxvec > 0){
        size = std::min(maxvec,desc.featureCount());
        maxvec -=size;
    }

    for (int m= 0; m< size && desc; ++m) {
        desc.next(feature); 
        to->write(feature, target);
        if (verbose > 5){
            cout << "Read descriptor " << std::setw(5) << m << endl;
        }
    }
    if (verbose > 2) {
        cout << "Processed descriptors " << std::setw(7) << size ;
        if (verbose > 3) 
            cout << " from file " << filename;
        cout << endl;
    }
}// }}}

// {{{ compute
static void compute() {
    FormatOutStream<RealType,TargetType>* to = 0;
    switch (formatType) {
        case BiSVMLight:
            to= new BiSVMLightOutStream<RealType, TargetType> (outfile);
            break;
        case SVMLight:
            to= new SVMLightOutStream<RealType, TargetType> (outfile);
            break;
        case Matlab:
            to= new MatlabOutStream<RealType, TargetType>    (outfile);
            break;
        case BiMatlab:
            to= new BiMatlabOutStream<RealType, TargetType>  (outfile);
            break;
        default :
            throw lear::Exception("compute()", "Unspecified format specified");
    }


    if (verbose > 5) {
        cout << "Created formated stream" << endl;
    }

    TargetType target = 1;
    for (FileListVector::const_iterator f=posfile.begin();
            f!= posfile.end();++f)
    {
        writeData(to,*f,maxposvec,target);
        if (maxposvec==0) break;
    }
    target = -1;
    for (FileListVector::const_iterator f=negfile.begin();
            f!= negfile.end();++f)
    {
        writeData(to,*f,maxposvec,target);
        if (maxnegvec==0) break;
    }
    delete to;
    if (verbose > 5) {
        cout << "All done " << endl;
    }
}
// }}}

// {{{ main
int main(int argc, char** argv) {
    lear::Cmdline cmdline;
    using namespace lear;

    lear::CustomOption fileformat("out file format options ",BiSVMLight);
    fileformat
        .add("BiSVMLight", BiSVMLight, "Binary custom svmlight format")
        .add("SVMLight",SVMLight, "Default svmlight format")
        .add("BiMatlab", BiMatlab, "Format: <int: ignore this> <int:#features> <int: feat_dim> <float: #features*feat_dim>")
        .add("Matlab", Matlab, "Format: <First line: #features, feat_dim> Next #features line with feat_dim numbers in each line")
        ;

    { // {{{ cmdline
        cmdline.commandName("dump2svmlearn");
        cmdline.version("0.0.1", "Author: Navneet Dalal "
                "(mailto: Navneet.Dalal@inrialpes.fr)");
        cmdline.brief( "Dump descriptor to svm_learn format");

        cmdline.description( "Dump descriptor to svm_learn format");

        using namespace lear;
        cmdline.usageIssues(
    "  'posfile'        positive descriptor file, directory, or a list file.\n"
    "  'negfile'        negative descriptor file, directory, or a list file.\n"
                    );
        cmdline.appendUsageIssues(fileformat.usage());

        cmdline.addOption()
            ("verbose,v",option<int>(&verbose)
                ->defaultValue(0)->minValue(0)->maxValue(9),
                "verbose level")

            ("maxpos,P",option<int>(&maxposvec)
                ->defaultValue(-1)->minValue(-1),
                "maximum positive vectors, -1 implies no limit")
            ("maxneg,N",option<int>(&maxnegvec)
                ->defaultValue(-1)->minValue(-1),
                "maximum negative vectors, -1 implies no limit")

            ("posfile,p",option< FileListVector >(&posfile),
                    "positive descriptor input file")
            ("negfile,n",option< FileListVector >(&negfile),
                    "negative descriptor input file")

            // select data format
            ("format,f",option<std::string>(&(fileformat.option))
                ->defaultValue(fileformat.defaultOption()),
                "specify out file format")
            ;

            cmdline.addArgument()
                ("outfile",option<std::string>(&outfile),"out file")
                ;
    } // }}}

    int status = cmdline.parse(argc, argv);
    if (status != cmdline.ok) 
        return status;

    formatType = static_cast<FileFormat> (fileformat.check());
    try {
        compute();
    } catch(std::exception& e) {
        cerr << "Caught "<< e.what() << endl;
        return 1;
    } catch(...) {
        cerr << "Caught unknown exception" << endl;
        return 1;
    }
    return 0;
}
// }}}

