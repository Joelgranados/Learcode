// {{{ copyright notice
// }}}
// {{{ file documentation
/**
 * @file
 * @brief Defines input/output routines for blitz arrays
 */
// }}}

#ifndef _LEAR_BLITZIO_H_
#define _LEAR_BLITZIO_H_

// {{{ headers
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <blitz/array.h>
#include <blitz/tinyvec.h>

#include <lear/exception.h>
#include <lear/io/ioext.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>
#include <lear/io/streamtypeid.h>
// }}}

namespace lear {
// {{{ writeArray
/**
 * @brief Write blitz::Array to file.
 *
 * @param filename File name to write blitz::Array
 * @param a blitz::Array to write
 * @exception lear::util::Exception If unable to open file
 */
template<class T, int N> inline 
void writeArray(const std::string& filename, const blitz::Array<T,N>& a) {
    std::ofstream to(filename.c_str());

    if (!to)
        throw lear::Exception("writeArray<T,N>", "Unable to open file: " + filename);

    for (typename blitz::Array<T,N>::const_iterator iter = a.begin(); iter != a.end(); ++iter) {
        to << *iter << '\n';
    }
}
template<class T> inline 
void writeArray2D(const std::string& filename, const blitz::Array<T,2>& a) {
    std::ofstream to(filename.c_str());

    if (!to)
        throw lear::Exception("writeArray2D<T,2>", "Unable to open file: " + filename);

    for (int i= a.lbound(0); i<= a.ubound(0) ; ++i)  {
        for (int j= a.lbound(1); j<= a.ubound(1) ; ++j) 
            to << a(i,j) << ' ';
        to << '\n';
    }
}
// }}}

// {{{ readAllocatedArray
/** 
 * @brief Read blitz::Array from file.
 *
 * @param filename File name to read blitz::Array
 * @param a blitz::Array to be read
 * @return number of elements read
 * @exception lear::util::Exception If unable to open file
 */
template<class T, int N> inline 
int readAllocatedArray(
        const std::string& filename, 
        blitz::Array<T,N>& a, 
        const bool flagErrorOnUnreadElements = true) 
{
    std::ifstream from(filename.c_str());

    if (!from)
        throw lear::Exception("readAllocatedArray","Unable to open file: " + filename);

    T element; int count =0;
    for (typename blitz::Array<T,N>::iterator iter = a.begin(); iter != a.end(); ++iter) {
        if (from.good())
            from >> element;
        if (!from.eof() && !from.fail())  {
            *iter = element;
            ++count;
        }else 
            break;
    }
    if (count != a.size() && flagErrorOnUnreadElements ) {
        std::ostringstream mesg;
        mesg << "Unable to read array fully from file: " << filename << 
            " ( read " << count << " element(s). Expected " << a.size() << " )";
        throw lear::Exception("readAllocatedArray",mesg.str());
    }
    return count;
}
// }}}

// {{{ readArray
/** 
 * @brief Read blitz::Array from file.
 *
 * @param filename File name to read blitz::Array
 * @param a blitz::Array to be read
 * @exception lear::util::Exception If unable to open file
 */
template<class T> 
inline int readArray(
        const std::string& filename,
        blitz::Array<T,2>& array,
        const bool flagErrorOnUnreadElements = true) 
{
    int rows = 0, cols =0;
    { // count the number of elements
        std::ifstream from(filename.c_str());

        if (!from)
            throw lear::Exception("readArray","Unable to open file: " + filename);
        { // get cols now
            // get first line
            std::stringstream linestream;
            char c;
            while ( from.good() && (c = from.get()) != '\n')
                if(!from.eof() && !from.fail())
                    linestream << c;
            linestream << '\0';

            // read number of columns
            T x;
            while (linestream >> x) {
                ++cols;
            }
            ++rows;
        }
        {// get rows now
            char c;
            while (from.good()) {
                c = from.get();
                if (c == '\n' && !from.eof() && !from.fail() )
                    ++rows;
            }
        }
    }
    if (rows >0 && cols >0) {
        array.resize(rows,cols);
        return readAllocatedArray(filename,array,flagErrorOnUnreadElements);
    } else {
        return 0;
    }
}
// read 1D array
template<class T> 
inline int readArray(
        const std::string& filename,
        blitz::Array<T,1>& array,
        const bool allocateAnyway = false,
        const bool flagErrorOnUnreadElements = true) 
{
    if (array.size() ==0 || allocateAnyway) { // allocate space and read array
        std::ifstream from (filename.c_str());
        if (!from)
            throw lear::Exception("readArray","Unable to open file: " + filename);

        std::list<T> l;
        T element;
        int count =0;

        while (from.good()) {
            from >> element;
            if (!from.eof() && !from.fail()) {
                l.push_back(element);
                ++count;
            }
        }
        if (count >0) {
            array.resize(count);
            std::copy(l.begin(),l.end(),array.begin());
        } 
        return count;
    } else { // space already allocated. just read array
        return readAllocatedArray(filename,array,flagErrorOnUnreadElements);
    }
}
// }}}

// {{{ CustomPrint output
template<class T, int N>
std::ostream& operator<<(std::ostream& o, const CustomPrint<blitz::Array<T,N> >& p) {

    for ( typename blitz::Array<T,N>::const_iterator iter = p.obj.begin(); 
          iter != p.obj.end(); ++iter)
        o << *iter << " "; 
    return o;
}

template<class T>
std::ostream& operator<<(std::ostream& o, const CustomPrint<blitz::Array<T,1> >& p) {

    const int lbound = p.obj.lbound(0);
    const int ubound = p.obj.ubound(0);
    for (int i= lbound; i<= ubound-1; ++i) 
        o << p.obj(i) << " "; 
    o << p.obj(ubound);
    return o;
}

template<class T>
std::ostream& operator<<(std::ostream& o, const CustomPrint<blitz::Array<T,2> >& p) {
    blitz::TinyVector<int,2> lbound = p.obj.lbound();
    blitz::TinyVector<int,2> ubound = p.obj.ubound();

    for (int i= lbound(0); i<= ubound(0) ; ++i) {
        for (int j= lbound(1); j<= ubound(1)-1 ; ++j) {
            o << p.obj(i,j) << " "; 
        }
        o << p.obj(i,ubound(1)) << '\n';
    }
    return o;
}
// }}}

template<class T, int N>
BiOStream& operator<<(BiOStream& o, const blitz::Array<T,N>& p) {// {{{
    o << static_cast<unsigned char>(IOTypeIdentifier<T>::ID);
    o << N;
    for (int i= 0; i<N; ++i) 
        o << p.lbound(i);// write size
    for (int i= 0; i<N; ++i) 
        o << p.extent(i);// write size
    for ( typename blitz::Array<T,N>::const_iterator iter = p.begin(); 
          iter != p.end(); ++iter)
        o << *iter; 
    return o;
}// }}}
template<class T, int N>
BiIStream& operator>>(BiIStream& o, blitz::Array<T,N>& p) {// {{{
    unsigned char id;
    o >> id;
    if (IOTypeIdentifier<T>::ID != id) {
        std::ostringstream mesg;
        mesg << "BiIStream is not properly formatted, ID mismatch. Expected "
           << static_cast<int>(IOTypeIdentifier<T>::ID) 
           << ", found " << static_cast<int>(id) << ".";
        throw lear::Exception("operator>>(BiIStream,blitz::Array)", 
                mesg.str());
    }
    int n;
    o >> n;
    if (n != N)
        throw lear::Exception("operator>>(BiIStream,blitz::Array)", 
            "BiIStream is not properly formatted, dimension mismatch");

    blitz::TinyVector<int,N> lbound;
    for (int i= 0; i<n; ++i) 
        o >> lbound[i];// read lbound

    blitz::TinyVector<int,N> extent;
    for (int i= 0; i<n; ++i) 
        o >> extent[i];// read size

    if (product(extent) != p.size() )
        p.resize(extent);
    p.reindexSelf(lbound);
    for ( typename blitz::Array<T,N>::iterator iter = p.begin(); 
          iter != p.end(); ++iter) {
        o >> *iter; 
    }
    return o;
}// }}}

} // lear

#endif // _LEAR_BLITZIO_H_
