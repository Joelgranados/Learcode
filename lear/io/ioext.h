// {{{ file documentation
/**
 * @file
 * @brief Implements standard IO in text mode for some standard containers
 */
// }}}

#ifndef _LEAR_IOEXT_H_
#define _LEAR_IOEXT_H_

// {{{ headers
#include <fstream>
#include <list>
#include <vector>
// }}}

namespace lear {

    class BiIStream;
    class BiOStream;
    /*
     * WARNING: DONOT CHANGE THE VECTOR, LIST CODE BELOW TO
     * FULL TEMPLATE FUNCTIONS. OTHERWISE, COMPILER MAY COMPLAIN
     * THAT IT IS UNABLE TO RESOLVE THE AMBIBUITIES.
     */
// {{{ vector io
/// read containter from the stream in txt format
template<class Type>
std::istream& operator>>(std::istream& o, std::vector<Type>& vector) {
    Type value;
    while ( o>> value) {
        vector.push_back(value);
    }
    return o;
}
/// write containter to the stream in txt format each element seperated by a new line
template<class Type>
std::ostream& operator<<(std::ostream& o, const std::vector<Type>& vector) {
    for (typename std::vector<Type>::const_iterator iter = vector.begin();
        iter != vector.end(); ++iter) 
    {
        o << *iter << " ";
    }
    return o; 
}
// }}}
// {{{ list io
/// read containter from the stream in txt format
template<class Type>
std::istream& operator>>(std::istream& o, std::list<Type>& list) {
    Type value;
    while ( o>> value) {
        list.push_back(value);
    }
    return o;
}
/// write containter to the stream in txt format each element seperated by a new line
template<class Type>
std::ostream& operator<<(std::ostream& o, const std::list<Type>& list) {
    for (typename std::list<Type>::const_iterator iter = list.begin();
        iter != list.end(); ++iter) 
    {
        o << *iter << " ";
    }
    return o; 
}// }}}
//{{{ std::pair
/// read std::pair from the stream
template<class First, class Second>
std::istream& operator>>(std::istream& o, std::pair<First, Second>& p) {
    o >> p.first >> p.second;
    return o;
}
/// write std::pair to the stream, with space between characters
template<class First, class Second>
std::ostream& operator<<(std::ostream& o, const std::pair<First, Second>& p) {
//     o << "Pair [ " << p.first << " " << p.second << " ]";
    o <<  p.first << " " << p.second ;
    return o;
}
// }}}

//  // {{{ vector io
//  /// read containter from the stream in txt format
//  template<class Type>
//  lear::BiIStream& operator>>(lear::BiIStream& o, std::vector<Type>& v) {
//      typename std::vector<Type>::size_type size;
//      o >> size;
//      v.clear();
//      if (size > 0)
//          v.resize(size);

//      unsigned index=0;
//      while (size > 0) {
//          o >> v[index];
//          --size;
//          ++index;
//      }
//      return o;
//  }
//  /// write containter to the stream in txt format each element seperated 
//  /// by a new line
//  template<class Type>
//  lear::BiOStream& operator<<(
//          lear::BiOStream& o, const std::vector<Type>& v) 
//  {
//      o << v.size();
//      for (typename std::vector<Type>::const_iterator iter = v.begin();
//          iter != v.end(); ++iter) 
//      {
//          o << *iter;
//      }
//      return o; 
//  }
//  // }}}
template<class T>
struct CustomPrint {
    const T& obj;

    CustomPrint(const T& o): obj(o){}

};

template<class T>
inline CustomPrint<T> toCustomPrint(const T& a) {
    return CustomPrint<T>(a);
}
template<class T>
inline CustomPrint<T> makeCustomPrint(const T& a) {
    return CustomPrint<T>(a);
}

template<class T>
std::ostream& operator<<(std::ostream& o, const CustomPrint<T>& p) {
    o << p.obj;
    return o;
}


} // namespace lear

#endif // _LEAR_IOEXT_H_

