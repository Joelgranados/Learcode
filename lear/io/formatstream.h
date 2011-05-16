#ifndef _LEAR_FORMAT_STREAM_H_
#define _LEAR_FORMAT_STREAM_H_

#include <string>
#include <fstream>
#include <blitz/array.h>
#include <lear/exception.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>

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
typedef typename ParentType::Array1DType        Array1DType;    \
typedef typename ParentType::Array2DType        Array2DType;    \
typedef typename ParentType::Target1DArray      Target1DArray;  
// {{{ Type class
template<class T>
struct Type {
    enum { ID = 0};
};
template<>
struct Type<int> {
    enum { ID = 1};
};
template<>
struct Type<float> {
    enum { ID = 2};
};
template<>
struct Type<double> {
    enum { ID = 3};
};
// }}}
// {{{ FormatStream
/**
 * Supports different file format's
 */
template<class ElementType_, class TargetType_>
class FormatInStream {
    public:
        _FORMAT_STREAM_TYPEDEF_

        virtual void header(std::string& ) = 0;

        virtual void read(
                Array2DType& data, Target1DArray& target) {}
        virtual void read(
                Array1DType& data, TargetType& target) = 0;

        virtual ~FormatInStream(){}
};
template<class ElementType_, class TargetType_>
class FormatOutStream {
    public:
        _FORMAT_STREAM_TYPEDEF_

        virtual void header(const std::string& ) = 0;

        virtual void weight(const ElementType_){}
        virtual void write(
                const Array2DType& data, const Target1DArray& target) 
        {
            const int ex = data.rows();

            for (int i= 0; i< ex; ++i) {
                write(data(i,blitz::Range::all()), target(i));
            }
        }
        virtual void write(
                const Array1DType& data, const TargetType target) = 0;

        virtual ~FormatOutStream(){}
};
// }}}
// {{{ AsciiStream
template<class ElementType_, class TargetType_>
class AsciiInStream : public FormatInStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatInStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        AsciiInStream(
                const std::string& filename) 
            : ParentType(),
            stream(filename.c_str())
        {
            if (!stream) {
                throw lear::Exception("AsciiInStream::constructor()",
                    "Unable to open file " + filename);
            }
        }
    protected:
        std::ifstream stream;
};
template<class ElementType_, class TargetType_>
class AsciiOutStream : public FormatOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatOutStream<ElementType,TargetType>  ParentType;
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
    protected:
        std::ofstream stream;
};// }}}
// {{{ BinaryStream
template<class ElementType_, class TargetType_>
class BinaryInStream : public FormatInStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatInStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        BinaryInStream(
                const std::string& filename) 
            : ParentType(),
            stream(filename.c_str())
        {
            if (!stream) {
                throw lear::Exception("BinaryInStream::constructor()",
                    "Unable to open file " + filename);
            }
        }
    protected:
        lear::BiIStream stream;
};
template<class ElementType_, class TargetType_>
class BinaryOutStream : public FormatOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef FormatOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        BinaryOutStream( const std::string& filename) :
            stream(filename.c_str())
        {
            if (!stream) {
                throw lear::Exception("BinaryOutStream::constructor()",
                    "Unable to open file " + filename);
            }
        }
    protected:
        lear::BiOStream stream;
};// }}}

// {{{ SVMLightStream
template<class ElementType_, class TargetType_>
class SVMLightInStream : public AsciiInStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiInStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        SVMLightInStream( const std::string& filename) :
            ParentType(filename)
        {}
    
        void header(std::string& ) {}

        void read(
                Array1DType& data, TargetType& target) {}
};
template<class ElementType_, class TargetType_>
class SVMLightOutStream : public AsciiOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        SVMLightOutStream( const std::string& filename) :
            ParentType(filename)
        {}
    
        void header(const std::string&) {}
        void write(const Array1DType& data, const TargetType target) {
            const int ex = data.size();

            stream << target << ' ';
            for (int i= 0; i< ex; ++i) {
                stream << (i+1) << ":" << data(i) << ' ';
            }
            stream <<'\n';
        }
};// }}}
// {{{ BiSVMLightStream
template<class ElementType_, class TargetType_>
class BiSVMLightInStream : public BinaryInStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BinaryInStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        BiSVMLightInStream( const std::string& filename) :
            ParentType(filename)
        {}
    
        void header(std::string& ) {}

        void read(Array1DType& data, TargetType& target) {}
};
template<class ElementType_, class TargetType_>
class BiSVMLightOutStream : public BinaryOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BinaryOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        BiSVMLightOutStream( const std::string& filename) :
            ParentType(filename), num_feature(0),feature_dim(0)
        {
            int version = 100;
            stream << version;
            int type = Type<TargetType>::ID;
            stream << type;
            initpos = stream.tellp();
            stream << num_feature << feature_dim;
        }
    
        void header(const std::string& ) {}
        void write(const Array1DType& data, const TargetType target) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("BiSVMLightOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            stream << target;
            for (int i= 0; i< feature_dim; ++i) {
                stream << data(i);
            }
            ++num_feature;
        }
        ~BiSVMLightOutStream() {
            stream.seekp(initpos);
            stream << num_feature << feature_dim;
        }
    private:
        int num_feature, feature_dim;
        lear::BiOStream::pos_type initpos;
};// }}}

// {{{ GMean
template<class ElementType_, class TargetType_>
class GMeanOutStream : public AsciiOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        GMeanOutStream( const std::string& filename) :
            ParentType(filename),
            num_feature(0),feature_dim(0)
        {
            using std::ios_base;
            // write 3 lines empty line with 10 spaces each
            initpos = stream.tellp();
            for (int j= 0; j<20; ++j) 
                    stream << ' ';
            stream << '\n';
        }
    
        void header(const std::string&) {}
        void write(const Array1DType& data, const TargetType ) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("GMeanOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< feature_dim; ++i) {
                stream << data(i) << ' ';
            }
            stream << '\n';
            ++num_feature;
        }
        ~GMeanOutStream() {
            stream.seekp(initpos);
            stream  << setw(10) << num_feature << ' '
                    << setw( 9) << feature_dim<< '\n';
        }
    private:
        int num_feature, feature_dim;
        std::ofstream::pos_type initpos;
};// }}}
// {{{ BiGMean
template<class ElementType_, class TargetType_>
class BiGMeanOutStream : public BinaryOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef BinaryOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        BiGMeanOutStream( const std::string& filename) :
            ParentType(filename), 
            num_feature(0),feature_dim(0)
        {
            initpos = stream.tellp();
            stream << num_feature << feature_dim;
        }
    
        void header(const std::string& ) {}
        void write(const Array1DType& data, const TargetType ) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("BiGMeanOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< feature_dim; ++i) {
                stream << data(i);
            }
            ++num_feature;
        }
        ~BiGMeanOutStream() {
            stream.seekp(initpos);
            stream << num_feature << feature_dim;
        }
    private:
        int num_feature, feature_dim;
        lear::BiOStream::pos_type initpos;
};// }}}

// {{{ Matlab
template<class ElementType_, class TargetType_>
class MatlabInStream : public AsciiInStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiInStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        MatlabInStream( const std::string& filename) :
            ParentType(filename)
        {}
    
        void header(std::string& ) {}

        void read(
                Array1DType& data, TargetType& target) {}
};
template<class ElementType_, class TargetType_>
class MatlabOutStream : public AsciiOutStream<ElementType_,TargetType_> {
    public:
        _FORMAT_BASIC_TYPEDEF_
        typedef AsciiOutStream<ElementType,TargetType>  ParentType;
        _PARENT_STREAM_TYPEDEF_

        MatlabOutStream( const std::string& filename) :
            ParentType(filename), feature_dim(0)
        {} 
        void header(const std::string&) {}
        void write(const Array1DType& data, const TargetType target) {
            if (feature_dim <1) {
                feature_dim = data.size();
            } else if (feature_dim != data.size()){
                throw lear::Exception("MatlabOutStream::write()",
                        "Attempt to write unequal feature vector");
            }

            for (int i= 0; i< feature_dim; ++i) {
                stream << data(i) << ' ';
            }
            stream << target << '\n';
        }
        ~MatlabOutStream() {
        }
    private:
        int feature_dim;
};// }}}

#undef  _FORMAT_BASIC_TYPEDEF_
#undef  _FORMAT_STREAM_TYPEDEF_
#undef  _PARENT_STREAM_TYPEDEF_

#endif // _LEAR_FORMAT_STREAM_H_
//  enum TypeID {
//      DEFAULT, 
//      BOOL, 
//      CHAR, UNSIGNED_CHAR, 
//      SHORT, UNSIGNED_SHORT, 
//      INT, UNSIGNED_INT,
//      FLOAT, DOUBLE, LONG_DOUBLE;
//  };
