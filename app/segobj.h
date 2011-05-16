#ifndef _LEAR_SEGMENTED_DATA_H_
#define _LEAR_SEGMENTED_DATA_H_

#include <string>
#include <sstream>

#include <lear/io/bistreamable.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>
#include <lear/exception.h>

#include <lear/util/rectangle.h>
#include <lear/util/point.h>
//  #include <boost/tokenizer.hpp>

class SegmentedObj : public lear::BiStreamable<SegmentedObj> {
    public:
        typedef lear::Rectangle<int>                        Rectangle;
        typedef lear::Point2D<int>                          Point;

        SegmentedObj(const int version_ = FILE_VERSION):
            center_(-1,-1), 
            orientation_(0,0), 
            objsize_(100),
            version_(version_)
        {}
        // set functions
        void center(const int x, int y) {
            center_.x(x);
            center_.y(y); 
        }
        void center(const Point& center_) {
            this->center_ = center_; 
        }
        void orientation(const int x, int y) {
            orientation_.x(x);
            orientation_.y(y); 
        }
        void orientation(const Point& o) {
            orientation_ = o; 
        }
        void addRegion(const Rectangle& r) {
            region_.push_back(r); 
        }
        void objsize(const int& e) {
            objsize_ = e; 
        }

        // get functions
        int x() const { return center_.x(); }
        int y() const { return center_.y(); }
        Point center() const{ return center_; }
        int objsize() const { return objsize_;}
        Point orientation() const { return orientation_; }
        unsigned int regionCount() const { return region_.size(); }

        bool hasCenter() const {// {{{
            if (center_.x() < 0 || center_.y() < 0)
                return false;
            return true;
        }// }}}
        bool hasOrientation() const {// {{{
            if (orientation_.x() == 0 && orientation_.y() == 0)
                return false;
            return true;
        }// }}}
        Rectangle& operator[](int i) { return region_[i]; }
        Rectangle& operator[](unsigned int i) { return region_[i]; }
        const Rectangle& operator[](int i) const { return region_[i]; }
        const Rectangle& operator[](unsigned int i) const { return region_[i];}

        inline lear::BiIStream& bload(lear::BiIStream& in) {// {{{
            in >> center_ >> orientation_ ;
            int c;
            in >> c;
            if (version_ > FILE_VERSION_100) {
                objsize_ = c;
                in >> c;
            }
            region_.clear();
            region_.reserve(c);
            region_.resize(c);
            for (int i= 0; i< c; ++i) 
                in >> region_[i];
            return in;
        }// }}}

        inline lear::BiOStream& bsave(lear::BiOStream& out) const {// {{{
            out << center_ << orientation_ << objsize_;
            out << region_.size();
            for (unsigned int i= 0; i< region_.size(); ++i) 
                out << region_[i];
            return out;
        }// }}}

        template<class T>
    static void read(const std::string& filename, T& obj) {// {{{
        lear::BiIStream from(filename.c_str());
        if (!from)
            throw lear::Exception("read", "Unable to open file: " +filename);

        int version;
        from >> version;
        if (!from.good()) {
            std::ostringstream str;
            str << "File : "<< filename << " is not properly formatted.\n"
                << "Unable to read version number";
            throw lear::Exception("read", str.str());
        }
        if (version < FILE_VERSION_100) {
            std::ostringstream str;
            str << "File : "<< filename << " is not properly formatted.\n"
                << "Expecting version == " << FILE_VERSION << ", read " << version;
            throw lear::Exception("read", str.str());
        }

        int c;
        from >> c;
        for (int i= 0; i< c; ++i) {
            typename T::value_type o(version);
            from >> o;
            if (!from.good()) {
                std::ostringstream str;
                str << "File : "<< filename << " is not properly formatted.\n"
                    << "Unable to read data";
                throw lear::Exception("read", str.str());
            }
            obj.push_back(o);
        }
    }// }}}

        template<class T, class Cont>
        static std::string readPascalAnnotations(
                const std::string& filename, T& obj, const Cont& objectclass, int verbose=0) 
        {// {{{
            using namespace std;

            ifstream from(filename.c_str());
            if (!from)
                throw lear::Exception("read", 
                        "Unable to open file: " +filename);

            const int LEN=1024;
            char line[LEN];
            string imagefile, errmesg;
            int objectnum = 0;
            
            char * filenameHeader= "Image filename : "; 
            size_t filenameLen = strlen(filenameHeader);
            char * boundboxHeader = "Bounding box for object ";
            size_t boundboxLen = strlen(boundboxHeader);
            char * imagesizeHeader = "Image size (X x Y x C) : ";
            size_t imagesizeLen = strlen(imagesizeHeader);

            int width=0, height=0;
            while (from.getline(line,LEN)) {
                if (line[0] == '#' || !line[0]) {
                    continue;
                }
                char * start=0, *end = 0;
                if (start = strstr(line,filenameHeader)) {
                    start +=filenameLen;
                    imagefile = start;
                } else if (start = strstr(line,imagesizeHeader)) {
                    start +=imagesizeLen;
                    char c;
                    istringstream istr(start);
                    istr >> width >> c >> height;
                } else if (start = strstr(line,boundboxHeader)) {
                    start += boundboxLen;
                    if (sscanf(start,"%d",&objectnum) !=1) {
                        errmesg = "Could not read object number. "
                            "Skipping object..."; break;
                    } 
                    while (*start != '"') ++start;
                    ++start; // go past "
                    end = start+1;
                    while (*end != '"') ++end; // find ending "
                    *end=0;

                    char * t = start;
                    while (*t) { *t=(char)tolower(*t); ++t; }

                    bool found=false;
                    for (typename Cont::const_iterator oc=objectclass.begin();
                            oc!=objectclass.end();++oc) {

                        if (strcmp(start, oc->c_str())==0) {
                            found=true;
                            if (verbose > 3) cout << "Found token " << *oc << " in label " << start << endl;
                            break;
                        }
                    }
                    if (!found){
                        if (verbose > 3) cout << "Token not found. Ignoring..." << endl;
                        continue;
                    }
                    while (*start != ':') ++start;
                    ++start;
                    char c;
                    int xmin, ymin, xmax, ymax;
                    int txmin, tymin, txmax, tymax;
                    istringstream istr(start);
                    istr >> c >> txmin >> c >> tymin >> c >> 
                        c >> c >> txmax >> c >> tymax;

                    xmin= min(txmin,txmax); xmax= max(txmin,txmax);
                    ymin= min(tymin,tymax); ymax= max(tymin,tymax);

                    --xmin;--ymin;--xmax;--ymax;

                    if (!width || !height) {
                        errmesg = "Invalid image width or height. Skipping...";
                        break;
                    }


                    if (xmin < 0) xmin=0;
                    if (xmax >= width) xmax=width-1;
                    if (ymin < 0) ymin=0;
                    if (ymax >= height) ymax=height-1;

                    SegmentedObj o;
                    o.center((xmin+xmax)/2, (ymin+ymax)/2);
                    Rectangle r(xmin, ymin, xmax-xmin+1, ymax-ymin+1);
                    o.addRegion(r);
                    obj.push_back(o);
                }
            }
            if (errmesg.size()){
                cerr << "Error reading annotation file: " << filename
                    << "\n    Error: "<< errmesg << endl;
            }
            return imagefile;
        }// }}}

        template<class T>
        static void readText(
            const std::string& filename, T& obj, bool startFromOne=true) 
        {// {{{
            // every row indicates new annotation and positions start from 1
            using namespace std;

            ifstream from(filename.c_str());
            if (!from)
                throw lear::Exception("read", 
                        "Unable to open file: " +filename);

            const int LEN=1024;
            char line[LEN];
            string imagefile;
            int objectnum = 0;
            
            int width=0, height=0;
            while (from.getline(line,LEN)) {
                if (line[0] == '#' || !line[0] || line[0] == '%' ) {
                    continue;
                }
                istringstream istr(line);
                if (istr.fail()) continue;

                float xmin, ymin, xmax, ymax;
                float txmin, tymin, txmax, tymax;
                istr >> txmin >> tymin; if (istr.bad()) continue;
                istr >> txmax >> tymax; if (istr.bad()) continue;

                xmin= min(txmin,txmax); xmax= max(txmin,txmax);
                ymin= min(tymin,tymax); ymax= max(tymin,tymax);

                if (startFromOne) {
                    --xmin;--ymin;--xmax;--ymax;
                }

                SegmentedObj o;
                o.center(static_cast<int>((xmin+xmax)/2), 
                         static_cast<int>((ymin+ymax)/2));
                Rectangle r(xmin, ymin, xmax-xmin+1, ymax-ymin+1);
                o.addRegion(r);
                obj.push_back(o);
            }
        }// }}}

        void print(std::ostream& o ) const {
            o  <<"  Object | Center ("
                << center_.x() << ',' << center_.y() 
                << ") | Orientation (" 
                << orientation_.x() << ',' 
                << orientation_.y() << ") | "
                << "Size " << objsize_ << '\n';

            unsigned N = region_.size();
            o  << "    Regions [" << N << " ]\n";
            for (unsigned ri= 0; ri < N; ++ri) {
                o  << "\t" << region_[ri].l() << ' ' << region_[ri].t() << ' '
                    << region_[ri].r() << ' ' << region_[ri].b() <<'\n';
            }
        }

        template<class T>
    static void write(const std::string& filename, const T& obj) {// {{{
        lear::BiOStream to(filename.c_str());
        if (!to)
            throw lear::Exception("write", "Unable to open file: " +filename);

        int size = 0;
        to << FILE_VERSION;
        lear::BiOStream::pos_type spos = to.tellp();
        to << size;

        for (typename T::const_iterator i=obj.begin();i!=obj.end();++i,++size)
            to << *i;

        lear::BiOStream::pos_type cpos = to.tellp();
        to.seekp(spos);
        to << size;
        to.seekp(cpos);
    }// }}}
    static void writeObjectfile(const std::string& filename, const std::vector<SegmentedObj>& obj) {// {{{
        std::ofstream to(filename.c_str());
        if (!to)
            throw lear::Exception("writeObjectfile", "Unable to open file: " +filename);

        for (unsigned int i= 0; i< obj.size(); ++i) 
        for (unsigned int j= 0; j< obj[i].region_.size(); ++j) {
            const Rectangle& r =obj[i].region_[j]; 
            to << "Object: " << 
                    r.x()  << '\t'<<r.y() << '\t' << 
                    r.w() << '\t'<<r.h() << '\n';
        }
    }// }}}
    protected:
        Point center_;
        Point orientation_;
        std::vector<Rectangle> region_;
        int objsize_;

    private:
        int version_;

        static const int FILE_VERSION = 101;
        static const int FILE_VERSION_100 = 100;

};

/*std::ostream& operator<<( std::ostream& o, const SegmentedObj& obj) {
    obj.print(o); return o;
}*/

#endif // _LEAR_SEGMENTED_DATA_H_
//              char * numObjectHeader= "Objects with ground truth : "; 
//              size_t numObjectLen = strlen(numObjectHeader);
//                  } else if (start = strstr(line,numObjectHeader)) {
//                      start += numObjectLen;
//                      if (sscanf(start,"%d",&totalobjectnum) !=1) {
//                          errmesg = "Could not read number of objects. Skipping file..."; break;
//                      } 
//                      else {
//                          while (*start != '{') ++start;
//                          if (start - line > LEN) {
//                              errmesg = "Could not find { in objects. Ill-formated file?";break;
//                          }
//                          ++start;// go past the 
//                          end = start;
//                          while (*end != '}') ++end;
//                          if (end - line > LEN) {
//                              errmesg = "Could not find } in objects. Ill-formated file?";break;
//                          }
//                          *end = 0;
//                          for (int i= 0; i<totalobjectnum; ++i) {
//                              Seperator sep("", " ", "\"");
//                              Tokenizer tok(start,sep);
//                              for(Tokenizer::iterator beg=tok.begin(); beg!=tok.end();++beg){
//                                          cout << *beg << "\n";
//                                          }
//                          }
//                      } 
