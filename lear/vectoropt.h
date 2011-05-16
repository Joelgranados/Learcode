#ifndef _LEAR_VECTOR_OPT_H_
#define _LEAR_VECTOR_OPT_H_

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include <boost/any.hpp>
#include<boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

namespace lear {

template<class T, class Alloc = std::allocator<T> >
class VectorOpt : public std::vector<T, Alloc> {
    typedef std::vector<T, Alloc> Parent;
    public:
    typedef typename Parent::allocator_type     allocator_type;
    typedef typename Parent::pointer            pointer;
    typedef typename Parent::const_pointer      const_pointer;
    typedef typename Parent::reference          reference;
    typedef typename Parent::const_reference    const_reference;
    typedef typename Parent::value_type         value_type;
    typedef typename Parent::iterator           iterator;
    typedef typename Parent::const_iterator     const_iterator;
    typedef typename Parent::size_type          size_type;
    typedef typename Parent::difference_type    difference_type;
    typedef typename Parent::reverse_iterator   reverse_iterator;
    typedef typename Parent::const_reverse_iterator const_reverse_iterator;

    VectorOpt(): Parent() {}

    VectorOpt(const T& i, size_type count=1)
        : Parent(count,i) {}

    VectorOpt(const T& i, size_type count, const Alloc& al)
        : Parent(count,i,al) {}

    /** Safe 'at', returns the last element if offset is out of bound.
     * throws exception if and only if there are no elements in the container.
     */
    reference at(size_type off) {
        if (Parent::size() > 0) {
            if (off >=Parent::size()) {
                return Parent::operator[](Parent::size()-1);
            } else 
                return Parent::operator[](off);
        } else {
            throw std::out_of_range("VectorOpt[] has zero elements");
        }
    }
    /** Safe 'at', returns the last element if offset is out of bound.
     * throws exception if and only if there are no elements in the container.
     */
    const_reference at(size_type off) const{
        if (Parent::size() > 0) {
            if (off >=Parent::size()) {
                return Parent::operator[](Parent::size()-1);
            } else 
                return Parent::operator[](off);
        } else {
            throw std::out_of_range("VectorOpt[] has zero elements");
        }
    }

    template<class T_, class A_>
    friend std::ostream& operator << (
            std::ostream& o, const VectorOpt<T_,A_>& t);

    template<class T_, class A_>
    friend std::istream& operator >> (
            std::istream& o, VectorOpt<T_,A_>& t);

    operator Parent& () { return static_cast<Parent&>(*this); }
    operator const Parent&() const { return static_cast<const Parent&>(*this);}

    /**
     * Set the default element seperator. Must not be an empty string.
     */
    static void seperator(const std::string& s) { 
        if (!s.empty() && s.size() > 0) 
            token_seperator = s; 
    }
//      /**
//       * Set the default element seperator. Must not be an empty string.
//       */
//      static void seperator(const char*c) { 
//          seperator(std::string(c));
//      }
    /**
     * Get the default seperator.
     */
    static std::string seperator() { return token_seperator; }

    /**
     * Push back tokens or elements from the string. 
     * The string tokens may be seperated by default seperator. 
     */
    void fromString(const std::string& val) {
        using namespace boost;
        typedef tokenizer<char_separator<char> > tokenizer;

        char_separator<std::string::value_type> sep(token_seperator.c_str());
        tokenizer tokens(val, sep);

        for (tokenizer::iterator tok_iter = tokens.begin();
            tok_iter != tokens.end(); ++tok_iter)
        {
            push_back( lexical_cast<T>(*tok_iter));
        }
    }

    private:
    static std::string token_seperator;
};
template<class T, class Alloc>
std::string VectorOpt<T, Alloc>::token_seperator = ",:^";

/** 
 * Validate VectorOpt command line values
 */
template<class T, class charT>
void validate(boost::any& v, const std::vector<std::basic_string<charT> >& s,
            VectorOpt<T>*, int)
{
    using namespace boost::program_options;

    if (v.empty()) { 
        v = boost::any(VectorOpt<T>());
    }

    VectorOpt<T>* tv = boost::any_cast< VectorOpt<T> >(&v);
    assert(tv);
    for (unsigned i = 0; i < s.size(); ++i) {
        try {
            tv->fromString(s[i]);
        } catch(const boost::bad_lexical_cast& /*e*/) {
            throw invalid_option_value(s[i]);
        }
    }
}

template<class T, class A>
std::ostream& operator << (std::ostream& o, const VectorOpt<T,A>& t) {
    if (t.size()>0) {
        for (unsigned i= 0; i< t.size()-1; ++i) 
            o << t[i] << t.token_seperator[0];
        o << t[t.size()-1] ;
    }
    return o;
}
template<class T, class A>
std::istream& operator >> (std::istream& o, VectorOpt<T,A>& t) {
    std::string val; o >> val;
    t.fromString(val);
    return o;
}

}

#endif // _LEAR_VECTOR_OPT_H_
