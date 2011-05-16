#include <iomanip>
#include <iostream>
#include <lear/exception.h>
#include <lear/util/fileutil.h>
#include <lear/image/painter.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs=boost::filesystem;

#include <lear/classifier/markimage.h>

lear::MarkImage::MarkImage(
    const std::string outimage_,
    const bool outfileIsDir_,
    const bool hidescore_)
            :
    PostProcessResult(),
    outimage_(outimage_),
    outfileIsDir_(outfileIsDir_),
    hidescore_(hidescore_)
{}
bool lear::MarkImage::verifyImageOut(
        const std::string outimage_,
        const bool outfileIsDir_) 
{
    bool doImageOut_ = (!outimage_.empty());
    using namespace std;
    // check if outimage is valid
    if (doImageOut_ && outfileIsDir_) {
        fs::path outdir(outimage_);
        if (fs::exists(outdir)) {
            // make sure that outfile is also a directory
            if (!fs::is_directory(outdir)) {
                cerr<< "WARNING:: 'infile' is a directory/list, "
                    " thus 'outimage'=" << outimage_ 
                    <<"' must also be a directory."
                    << "Disabling image output." << endl;
                doImageOut_ = false;
            }
        } else {
            // it does not exist, create a directory
            fs::create_directory(outdir);
            if (!fs::exists(outdir)) {
                cerr<< "WARNING:: Unable to create output image directory "
                    << outimage_ << "Disabling image output." << endl;
                doImageOut_ = false;
            } else {
                cerr<< "WARNING:: Directory " << outdir.string() << 
                    " does not exist. Creating it." << endl;
            }
        }
    }
    return doImageOut_;
}

namespace somegarbagename {
    class DrawBoundingBox {// {{{
        public:
            typedef blitz::TinyVector<int,2>        IndexType;

            DrawBoundingBox() {}
            DrawBoundingBox(
                    const std::string& infile, const std::string& outfile) :
                painter(infile), outfile_(outfile)
            {}

            void load(const std::string& infile) {
                painter.load(infile);
            }
            void saveto(const std::string& outfile) {
                outfile_ = outfile;
            }

            void draw(const IndexType& lbound, const IndexType& extent) {
                lear::Rectangle<int>  r(
                        lbound[0]+1,lbound[1]+1,extent[0]-2,extent[1]-2);

                painter.setColor(colorR, colorG, colorB);
                painter.drawRect(r);
                painter.setColor(colorR, colorB, colorG);
                painter.drawRect(r - 1);
            }
            void draw(const IndexType& xy, const std::string& txt) {
                painter.setBrush(0, 0, 0);
                painter.drawRect(xy[0],xy[1],54,15);
                painter.noBrush();
                painter.setColor(255, 255, 255);
                painter.drawText(txt,xy[0]+2,xy[1]+1);
            }
            ~DrawBoundingBox() {
                if (!outfile_.empty())
                    painter.save(outfile_);
            }

        private:
            lear::Painter painter;
            std::string outfile_;

            static const int colorR  = 255;
            static const int colorG  = 255;
            static const int colorB  = 0;
    };// }}}
}

void lear::MarkImage::write( 
    const_iterator s, const_iterator e, const std::string filename)
{// {{{
    std::string fname = outimage_;
    if (outfileIsDir_) {
        fname += "/" + lear::basename(filename) +".jpg";
    }
    somegarbagename::DrawBoundingBox marker(filename,fname);

    char scoretxt[24];
    while (s != e){
        marker.draw(s->lbound, s->extent);
        if (!hidescore_) {
            sprintf(scoretxt,"%7.4f",s->score);
            marker.draw(s->lbound, scoretxt);
        }
        ++s;
    }
}// }}}

