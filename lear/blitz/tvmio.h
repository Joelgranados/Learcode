// {{{ file documentation
/**
 * @file 
 * @brief Input/Output utilities for TinyVector and TinyMatrix
 */
// }}}

#ifndef _LEAR_TVMIO_H_
#define _LEAR_TVMIO_H_

// {{{ headers
#include <iostream>
#include <fstream>
#include <string>

#include <blitz/tinyvec.h>
#include <blitz/tinymat.h>

#include <lear/exception.h>
#include <lear/io/ioext.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>
// }}}

namespace lear {

// {{{ writeMatrix
/**
 * @brief Write TinyMatrix to file.
 *
 * @param filename File name to write TinyMatrix
 * @param a TinyMatrix to write
 * @exception lear::util::Exception If unable to open file
 */
template<class T, int N, int M> inline 
void writeMatrix(const std::string& filename, const blitz::TinyMatrix<T,N,M>& a) {
    std::ofstream to(filename.c_str());

    if (!to)
        throw lear::Exception("writeMatrix<T,N,M>", "Unable to open file: " + filename);

    for (int i=0; i< N; ++i) {
        for (int j=0; j< M-1; ++j) 
            to << a(i,j) << " ";
        to << a(i,M-1) << std::endl;
    }

    to.close();
}
// }}}

// {{{ readMatrix
/**
 * @brief Read a TinyMatrix from file. 
 *
 * Read a matrix from a file. All elements to be read must be seperated by space, tabs or newlines.
 *
 * @param filename Input file to read TinyMatrix from
 * @param a TinyMatrix to put read data
 * @exception lear::util::Exception If unable to open file
 */
template<class T, int N, int M> inline 
int readMatrix(
        const std::string& filename,
        blitz::TinyMatrix<T,N,M>& a,
        const bool flagErrorOnUnreadElements = true) 
{
    std::ifstream from(filename.c_str());
    if (!from)
            throw lear::Exception("readMatrix<T,N,M>","Unable to open file");

    T element;
    for (int i=0; i< N; ++i) 
    for (int j=0; j< M; ++j){
        if (from.good()) 
            from >> element;
        if (!from.eof() && !from.fail())  {
            a(i,j) = element;
        } else if (flagErrorOnUnreadElements) {
            std::ostringstream mesg;
            mesg << "Unable to read TinyVector fully from file: " << filename << 
                " ( read " << (i*M+j) << " element(s). Expected " << N*M << " )";
            throw lear::Exception("readVector",mesg.str());
        } else
            return (i*M+j) ;
    }
    from.close();
    return N*M;
}
//}}}

// {{{ writeVector
/**
 * @brief Write TinyVector to file.
 *
 * @param filename File name to write TinyVector
 * @param a TinyVector to write
 * @exception lear::util::Exception If unable to open file
 */
template<class T, int N> inline 
void writeVector(const std::string& filename, const blitz::TinyVector<T,N>& a) {
    std::ofstream to(filename.c_str());

    if (!to)
        throw lear::Exception("writeVector<T,N>", "Unable to open file: " + filename);

    for (int i=0; i< N-1; ++i) 
        to << a(i) << " ";
    to << a(N-1) << std::endl;

    to.close();
}
// }}}

// {{{ readVector
/**
 * @brief Read a TinyVector from file. 
 *
 * Read a TinyVector from a file. All elements to be read must be seperated by space, tabs or newlines.
 *
 * @param filename Input file to read TinyVector from
 * @param a TinyVector to put read data
 * @exception lear::util::Exception If unable to open file
 *
 * @warning The code assumes that your file has atleast N distinct elements and no
 * extra elements like comma(,).
 */
template<class T, int N> inline 
int readVector(
        const std::string& filename,
        blitz::TinyVector<T,N>& a,
        const bool flagErrorOnUnreadElements = true) {
    std::ifstream from(filename.c_str());
    if (!from)
        throw lear::Exception("readVector<T,N>","Unable to open file: " + filename);

    T element;
    for (int i=0; i< N; ++i) {
        if (from.good())
            from >> element;
        if (!from.eof() && !from.fail())  {
            a(i) = element;
        } else if (flagErrorOnUnreadElements) {
            std::ostringstream mesg;
            mesg << "Unable to read TinyVector fully from file: " << filename << 
                " ( read " << i << " element(s). Expected " << N << " )";
            throw lear::Exception("readVector",mesg.str());
        } else 
            return i;
    }
    from.close();
    return N;
}
//}}}

// {{{ binary io
/**
 * @brief Write TinyVector in binary format
 *
 * @param BiOStream object
 * @param a TinyVector to write
 */
template<class T, int N> inline 
BiOStream& operator << (BiOStream& to, const blitz::TinyVector<T,N>& a) {
    for (int i=0; i< N; ++i) 
        to << a(i);
    return to;
}
/**
 * @brief Read a TinyVector from binary format. 
 *
 * @param BiIStream object
 * @param a TinyVector to put read data
 *
 * @warning The code assumes that you already know the type 
 * and size of data
 */
template<class T, int N> inline 
BiIStream& operator >>(
        BiIStream& from,
        blitz::TinyVector<T,N>& a)
{
    for (int i=0; i< N; ++i) {
        from >> a(i);
    }
    return from;
}
// }}}

// {{{ CustomPrint operator << for TinyVector and TinyMatrix
/** write tinyvector to the stream in txt format each element seperated by a space
  */
template<class T, int N>
std::ostream& operator<<(std::ostream& o, const CustomPrint<blitz::TinyVector<T,N> >& p) {
    for (int i= 0; i< N-1; ++i) 
        o << std::setw(7) << std::setprecision(4) << p.obj(i) << " "; 
    o << std::setw(7) << std::setprecision(4) << p.obj(N-1);
    return o;
}

/// write tinymatrix to the stream in txt format each element seperated by a space
template<class T, int N, int M>
std::ostream& operator<<(std::ostream& o, const CustomPrint<blitz::TinyMatrix<T,N,M> >& p) {
    for (int i= 0; i< N ; ++i) {
        for (int j= 0; j< M-1 ; ++j) {
            o << std::setw(7) << std::setprecision(4) << p.obj(i,j) << " "; 
        }
        o << std::setw(7) << std::setprecision(4) << p.obj(i,M-1) << '\n';
    }
    return o;
}
// }}}

//     //{{{ TxtStream >> operator for TinyVector and TinyMatrix
//     /// read tinyvector from the stream in txt format
//     template<class T, int N>
//     TxtStream& operator>>(TxtStream& stream, blitz::TinyVector<T,N>& vector) {
//         for (int i=0; i< N; ++i)
//             stream >> vector[i];
//         return stream;
//     }
// 
//     /// read tinymatrix from the stream in txt format
//     template<class T, int N, int M>
//     TxtStream& operator>>(TxtStream& stream, blitz::TinyMatrix<T,N, M>& matrix) {
//         for (int i=0; i< N; ++i)
//         for (int j=0; j< M; ++j)
//             stream >> matrix(i,j);
//         return stream;
//     }
//     // }}}
} // lear

#endif // #ifndef _LEAR_TVMIO_H_
