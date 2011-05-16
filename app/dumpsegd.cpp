#if HAVE_CONFIG_H
#include "config.h"
#endif
// {{{ headers
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include <blitz/tinyvec.h>
#include <lear/cmdline.h>
#include <lear/util/fileutil.h>
#include <lear/image/imageio.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs=boost::filesystem;

#include "segobj.h"
// }}}

// {{{  command line parameters
using namespace std;
using namespace lear;

static string datafile, dataext, outfile, imagefile;
static int verbose;

static string outformat;
static enum {Default, Matlab, Pascal} format = Default;

typedef list<std::string> PathVector;
// }}}

// {{{ compute
static void compute() {
    // {{{ read test files list and verify output path
    PathVector datalist;
    bool isdir = fs::is_directory(datafile);

    if (isdir) {
        // read all files to test 
        dirlist(datalist, datafile, dataext);
    } else {
        string ext = toUpper(extension(datafile));
        if (ext.find("LST") != string::npos) {
            dirlist(datalist, datafile, dataext);
            isdir = true;
        } else {
            // one single file on command line, 
            datalist.push_back(datafile);
        }
    }// }}}

    if (isdir) fs::create_directory(outfile);// create directory

    bool hasimage = false;
    PathVector imagelist;
    if (outformat == "matlab") {
        format = Matlab;
    } else if (outformat== "pascal") {
        format = Pascal;
        if (!imagefile.empty()) {// read image file names
            if (isdir) {
                // read all image files 
                dirlist(imagelist, imagefile);
            } else {
                // one single file on command line, 
                imagelist.push_back(imagefile);
            }
            if (imagelist.size())
                hasimage = true;
        }
        if (verbose > 5)
        std::cout << "Read images " << imagelist.size() << " data files " << datalist.size() << std::endl;

        if (!hasimage)
            throw Exception("compute",
                    "Must specify 'imagefile' option if outformat=Pascal");
        PathVector datalistb, imagelistb;

        for (PathVector::const_iterator i=imagelist.begin(), f=datalist.begin(); 
                i!= imagelist.end(); ++i) 
        {
            if (verbose > 5)
                std::cout << "Processing " << *i << std::endl;
            string ifile= basename(*i);
            bool looped=false;
            string dfile;
            while (true) {
                if (f!=datalist.end()){
                    dfile= basename(*f);
                    if (dfile==ifile) {
                        imagelistb.push_back(*i);
                        datalistb.push_back(*f);
                        if (verbose > 6)
                        std::cout << "Found " << dfile << std::endl;
                        ++f;
                        break;
                    }
                    ++f;
                }
                if (f==datalist.end()) {
                    f=datalist.begin();
                    if (looped) {
                        if (verbose > 7)
                        std::cout << "Not Found " << dfile << std::endl;
                        break;
                    }
                    looped=true;
                }
            }
        }
        if (verbose > 7)
        std::cout << "Feed all 'b' lists" << std::endl;
        datalist.clear();
        imagelist.clear();
        datalist=datalistb;
        imagelist=imagelistb;
    } else {
        format = Default;
    }

    const int ObjMarker = -100, CenMarker = -200, OriMarker = -300;

    // keep incrementing imagelist even if it is empty
    for (PathVector::const_iterator f=datalist.begin(), i=imagelist.begin();
            f!= datalist.end(); ++f, ++i)
    {
        if (verbose > 3)
            std::cout << "Dumping file " << *f << std::endl;

        blitz::TinyVector<int,2> s=0;
        if (format==Pascal){
            if (fs::exists(*i))
                s = lear::ImageIO::size(*i);
            else {
                std::cerr << "Could not find image " << *i << ". Ignoring annotation file " << *f << std::endl;
                continue;
            }
        }

        std::string write2file = outfile;
        if (isdir) write2file+='/' + basename(*f)+".txt";
        ofstream to (write2file.c_str());
        if (!to) 
            throw Exception("compute","Unable to open output file "+write2file);
        to.width(4); to.precision(2); to.fill(' ');
        to.setf(ios_base::fixed,ios_base::floatfield); 

        // read data file
        std::vector<SegmentedObj> data;
        SegmentedObj::read(*f, data);

        switch(format) {// write header
            case Matlab:
                to  << ObjMarker << ' ' << data.size() << ' ' 
                    << 0 << ' ' << 0 <<'\n';
                break;
            case Pascal:
to << "# PASCAL Annotation Version 1.00\n\n";
to << "Image filename : \"" << *i << "\"\n";
to << "Image size (X x Y x C) : " << s[0] << " x " << s[1] << " x 3\n";
to << "Database : \"The INRIA Rhône-Alpes Annotated Person Database\"\n";
to << "Objects with ground truth : " << data.size() << " { ";
for (unsigned j= 0; j< data.size(); ++j) 
    to << "\"PASperson\" ";
to << "}\n\n";
to << "# Note that there might be other objects in the image\n" ;
to << "# for which ground truth data has not been provided.\n\n";
to << "# Top left pixel co-ordinates : (0, 0)\n\n";
                break;
            default:
                to  << "FILE :: " << *f <<'\n'
                    << "\tObject count " << data.size() 
                    << ", Image size (" << 0 << ", " << 0 << ")\n";
        };

        // write objects
        for (unsigned oi = 0; oi < data.size(); ++oi) {// {{{ object index
            const SegmentedObj& obj = data[oi];

            unsigned N = obj.regionCount();
            switch(format) {
                case Matlab:
                    to  << CenMarker << ' ' << obj.objsize() << ' ' 
                        << obj.center().x() << ' ' << obj.center().y() <<'\n';

                    to  << OriMarker << ' ' << N << ' ' 
                        << obj.orientation().x() << ' ' 
                        << obj.orientation().y() <<'\n';

                    for (unsigned ri= 0; ri < N; ++ri) {
                        to  << obj[ri].l() << ' ' << obj[ri].t() << ' ' 
                            << obj[ri].r() << ' ' << obj[ri].b() <<'\n';
                    }
                    break;
                case Pascal:
to << "# Details for object " << oi+1 << " (\"PASperson\")\n";
to << "# Center point -- not available in other PASCAL databases -- refers\n";
to << "# to person head center\n";
to << "Original label for object " << oi+1 << " \"PASperson\" : \"UprightPerson\"\n";
to << "Center point on object " << oi+1 << " \"PASperson\" (X, Y) : (" 
    << obj.center().x() << ", " << obj.center().y() << ")\n";
for (unsigned ri= 0; ri < N; ++ri) {
    to << "Bounding box for object " << oi+1 << " \"PASperson\" (Xmin, Ymin) - (Xmax, Ymax) : (" 
        << obj[ri].l() << ", " << obj[ri].t() << ") - (" 
        << obj[ri].r() << ", " << obj[ri].b() << ")\n\n";
}
                    break;
                default:
                    to  <<"  Object " << (oi+1) << " | Center ("
                        << obj.center().x() << ',' << obj.center().y() 
                        << ") | Orientation (" 
                        << obj.orientation().x() << ',' 
                        << obj.orientation().y() << ") | "
                        << "Size " << obj.objsize() << '\n';

                    to  << "    Regions [" << N << " ]\n";
                    for (unsigned ri= 0; ri < N; ++ri) {
                        to  << "\t" << obj[ri].l() << ' ' << obj[ri].t() << ' '
                            << obj[ri].r() << ' ' << obj[ri].b() <<'\n';
                    }
            };
        }// }}}
    }

}
// }}}

// {{{ main
int main(int argc, char** argv) {
    lear::Cmdline cmdline;
    { // {{{ cmdline
        using namespace lear;
        cmdline.commandName("dumpsegd");
        cmdline.version("0.0.1", "Author: Navneet Dalal "
                "(mailto: Navneet.Dalal@inrialpes.fr)");
        cmdline.brief(
"Convert segd file format to text/matlab array format");

cmdline.description(
"Input file can be a directory, segmented file or list file (must end with .lst"
        );

        ostringstream usageIssues ;
        cmdline.usageIssues(usageIssues.str());

        cmdline.addOption()
            ("verbose,v",option<int>(&verbose)
                ->defaultValue(0)->minValue(0)->maxValue(9),
                "verbose level")
            ("format,f",option<string>(&outformat)
                ->defaultValue("matlab"),
                "output files format (text/matlab/pascal)")
            ("segdext",option<string>(&dataext)
                ->defaultValue(".segd"),
                "segmented data extension")
            ("imagefile,i",option<string>(&imagefile),
                "corresponding input image file")
            ;

        cmdline.addArgument()
            ("segdfile",option<string>(&datafile),"segmented data file")
            ("outfile",option<string>(&outfile),"output file")
            ;
    } // }}}

    int status = cmdline.parse(argc, argv);
    if (status != cmdline.ok) 
        return status;

    if (dataext[0] != '.')
       dataext = '.' + dataext;

    try {
        compute();
    } catch(exception& e) {
        cerr << "Caught "<< e.what() << endl;
        return 1;
    } catch(...) {
        cerr << "Caught unknown exception" << endl;
        return 1;
    }
    return 0;
}
// }}}
