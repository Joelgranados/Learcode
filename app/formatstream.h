#ifndef _LEAR_FORMAT_STREAM_H_
#define _LEAR_FORMAT_STREAM_H_

#include <string>
#include <fstream>
#include <blitz/array.h>
#include <lear/exception.h>
#include <lear/io/biostream.h>
#include <lear/io/streamtypeid.h>

#define _FORMAT_BASIC_TYPEDEF_                                  \
typedef ElementType_                            ElementType;    \
typedef TargetType_                             TargetType;

#define _FORMAT_STREAM_TYPEDEF_ \
_FORMAT_BASIC_TYPEDEF_                                   \
typedef blitz::TinyVector<int,2>            IndexType;   \
typedef blitz::Array<ElementType,1>         Array1DType; \
typedef blitz::Array<ElementType,2>         Array2DType; \
typedef blitz::Array<TargetType,1>          Target1DArray;

#define _PARENT_STREAM_TYPEDEF_                                 \
typedef typename Parent::Array1DType        Array1DType;    \
typedef typename Parent::Array2DType        Array2DType;    \
typedef typename Parent::Target1DArray      Target1DArray;  
// {{{ FormatStream
/**
 * Supports different file format's
 */
template<class ElementType_, class TargetType_>
class FormatOutStream {
    public:
        _FORMAT_STREAM_TYPEDEF_

        FormatOutStream(){}
//          virtual void write(
//                  const Array2DType& data, const Target1DArray& target) 
//          {
//              const int ex = data.rows();

//              for (int i= 0; i< ex; ++i) {
//                  write(data(i,blitz::Range::all()), target(i));
//              }
//          }
        virtual void write(
                const Array1DType& data, const TargetType target) = 0;
        virtual void write(
            const Array1DType& data, const TargetType target,
            const ElementType, const int, const char*)
        { write(data,target); }

        virtual ~FormatOutStream(){}
};
// }}}
// {{{ AsciiStream
template<class ElementType_, class TargetType_>
class AsciiOutStream : public FormatOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        AsciiOutStream( const std::string& filename) :
            stream(filename.c_str())
        {
            if (!stream) {
                throw lear::Exception("AsciiOutStream::constructor()",
                    "Unable to open file " + filename);
            }
            using std::ios_base;
            stream.setf(ios_base::scientific,ios_base::floatfield);
            stream.precision(15);
        }
        virtual ~AsciiOutStream(){stream.close();}
    protected:
        std::ofstream stream;
};// }}}
// {{{ BinaryStream
template<class ElementType_, class TargetType_>
class BinaryOutStream : public FormatOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        BinaryOutStream( const std::string& filename) :
            stream(filename.c_str())
        {
            if (!stream) {
                throw lear::Exception("BinaryOutStream::constructor()",
                    "Unable to open file " + filename);
            }
        }
        virtual ~BinaryOutStream(){
            stream.close();
        }
    protected:
        lear::BiOStream stream;
};// }}}

// {{{ SVMLightStream
template<class ElementType_, class TargetType_>
class SVMLightOutStream : public AsciiOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        SVMLightOutStream( const std::string& filename) :
            Parent(filename)
        {}
    
        virtual void write(const Array1DType& data, const TargetType target) {
            const int ex = data.size();

            Parent::stream << target << ' ';
            for (int i= 0; i< ex; ++i) {
                Parent::stream << (i+1) << ":" << data(i) << ' ';
            }
            Parent::stream <<'\n';
        }
};// }}}
// {{{ BiSVMLightStream
template<class ElementType_, class TargetType_>
class BiSVMLightOutStream : public BinaryOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BinaryOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        BiSVMLightOutStream( const std::string& filename,
                const bool hasCost = false, const bool hasComment = false,
                const bool hasQuery=false) :
            Parent(filename), num_feature(0),feature_dim(0),
            hasCost(hasCost), hasComment(hasComment), hasQuery(hasQuery)
        {
            int version = 0x10000;
            if (hasComment)version |=0x10;
            if (hasCost)   version |=0x08;
            if (hasQuery)  version |=0x02;

//printf("Version number %x = %d , hasCost=%d,hasquery=%d, 
//hasComment=%d\n",version, version,hasCost,hasQuery,hasComment);

            Parent::stream << version;
            int type = lear::IOTypeIdentifier<ElementType>::ID;
            Parent::stream << type;
            type = lear::IOTypeIdentifier<TargetType>::ID;
            Parent::stream << type;
            initpos = Parent::stream.tellp();
            Parent::stream << num_feature << feature_dim;
        }
    
        virtual void write(const Array1DType& data, const TargetType target)
        {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("BiSVMLightOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            Parent::stream << target;
            for (int i= 0; i< feature_dim; ++i) {
                Parent::stream << data(i);
            }
            ++num_feature;
        }
        virtual void write(const Array1DType& data, const TargetType target,
            const ElementType cost, const int query, const char* comment)
        {
            write(data,target);
            if (hasQuery) Parent::stream << query;
            if (hasCost) Parent::stream << cost;
            if (hasComment) {
                if (comment && *comment){
                    Parent::stream << comment;
                } else{
                    char a[]={0}; Parent::stream << a;
                }
            }
        }
        virtual ~BiSVMLightOutStream() {
            Parent::stream.seekp(initpos);
            Parent::stream << num_feature << feature_dim;

            std::cout << "Written " << num_feature 
                << " of dimension " << feature_dim << std::endl;
        }
    private:
        int num_feature, feature_dim;
        bool hasCost, hasComment, hasQuery;

        lear::BiOStream::pos_type initpos;
};// }}}

// {{{ Basic
template<class ElementType_, class TargetType_>
class BasicOutStream : public AsciiOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        BasicOutStream( const std::string& filename) :
            Parent(filename),
            num_feature(0),feature_dim(0)
        {}
    
        virtual void write(const Array1DType& data, const TargetType ) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("BasicOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< feature_dim; ++i) {
                Parent::stream << data(i) << ' ';
            }
            Parent::stream << '\n';
            ++num_feature;
        }
        virtual ~BasicOutStream() {}
    protected:
        int num_feature, feature_dim;
        std::ofstream::pos_type initpos;
};// }}}
// {{{ Matlab
template<class ElementType_, class TargetType_>
class MatlabOutStream : public BasicOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BasicOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        MatlabOutStream( const std::string& filename) :
            Parent(filename)
        {
            using std::ios_base;
            // write 1 lines empty line with 10 spaces each
            initpos = Parent::stream.tellp();
            for (int j= 0; j<80; ++j) 
                    Parent::stream << ' ';
            Parent::stream << '\n';
        }
    
        virtual void write(const Array1DType& data, const TargetType ) {
            if (Parent::feature_dim <1) {
                Parent::feature_dim = data.size();
            } else if (Parent::feature_dim != data.size()){
                throw lear::Exception("MatlabOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< Parent::feature_dim; ++i) {
                Parent::stream << data(i) << ' ';
            }
            Parent::stream << '\n';
            ++Parent::num_feature;
        }
        virtual ~MatlabOutStream() {
            Parent::stream.seekp(initpos);
            Parent::stream  << "# " << std::setw(10) << Parent::num_feature << ' '
                    << " " << std::setw( 10) << Parent::feature_dim<< std::ends;
            std::cout << "Written points " << Parent::num_feature << std::endl;
        }
    private:
        std::ofstream::pos_type initpos;
};// }}}
// {{{ BiMatlab
template<class ElementType_, class TargetType_ = bool >
class BiMatlabOutStream : public BinaryOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BinaryOutStream<ElementType,TargetType>  Parent;
        _PARENT_STREAM_TYPEDEF_

        BiMatlabOutStream( const std::string& filename) :
            Parent(filename), 
            num_feature(0),feature_dim(0)
        {
            int type = lear::IOTypeIdentifier<ElementType>::ID;
            Parent::stream << type;
            initpos = Parent::stream.tellp();
            Parent::stream << num_feature << feature_dim;
        }
    
        virtual void write(const Array1DType& data, const TargetType ) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("BiMatlabOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< feature_dim; ++i) {
                Parent::stream << data(i);
            }
            ++num_feature;
        }
        virtual ~BiMatlabOutStream() {
            Parent::stream.seekp(initpos);
            Parent::stream << num_feature << feature_dim;
        }
    protected:
        int num_feature, feature_dim;
        lear::BiOStream::pos_type initpos;
};// }}}

#undef  _FORMAT_BASIC_TYPEDEF_
#undef  _FORMAT_STREAM_TYPEDEF_
#undef  _PARENT_STREAM_TYPEDEF_

#endif // _LEAR_FORMAT_STREAM_H_
