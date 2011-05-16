#ifndef _LEAR_TINY_VEC_OPT_H_
#define _LEAR_TINY_VEC_OPT_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include <stdexcept>

#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>

#include <boost/any.hpp>
#include<boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

namespace lear {

template<class T, int N>
class TinyVecOpt : public blitz::TinyVector<T,N> {
    typedef blitz::TinyVector<T,N> Parent;
    public:
    TinyVecOpt(): Parent() {}
    TinyVecOpt(const T i)
        : Parent(i) {}
    TinyVecOpt(const T i, const T j)
        : Parent(i,j) {}
    TinyVecOpt(const T i, const T j, const T k)
        : Parent(i,j,k) {}

    bool operator <(const TinyVecOpt o)const  {
        return blitz::sum(
                static_cast<const Parent&>(*this) 
                    < 
                static_cast<const Parent&>(o)
                ) ;
    }
    bool operator >(const TinyVecOpt o) const {
        return blitz::sum(
                static_cast<const Parent&>(*this) 
                    > 
                static_cast<const Parent&>(o)
                );
    }

    template<class T_, int N_>
    friend std::ostream& operator << (
            std::ostream& o, const TinyVecOpt<T_,N_>& t);

    template<class T_, int N_>
    friend std::istream& operator >> (
            std::istream& o, TinyVecOpt<T_,N_>& t);

    operator Parent& () { 
        return static_cast<Parent&>(*this);
    }
    operator const Parent&() const { 
        return static_cast<const Parent&>(*this);
    }

    /**
    * Set the default element seperator. Must not be an empty string.
    */
    static void seperator(const std::string& s) { 
        if (!s.empty() && s.size() > 0) 
            token_seperator = s; 
    }
//      /**
//      * Set the default element seperator. Must not be an empty string.
//      */
//      static void seperator(const char* c) { 
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

        int index = 0;
        for (tokenizer::iterator tok_iter = tokens.begin();
            tok_iter != tokens.end(); ++tok_iter, ++index)
        {
            if (index < N)
                (*this)[index] = lexical_cast<T>(*tok_iter);
            else {
                std::ostringstream o;
                o << "TinyVecOpt[" << N << "] access out of range";
                throw std::out_of_range(o.str());
            }
        }
        if (index < N && index > 0) {
            for (int i= index; i< N; ++i) 
                (*this)[i] = (*this)[index-1];
        }
    }

    private:
    static std::string token_seperator;
};
template<class T, int N>
std::string TinyVecOpt<T,N>::token_seperator =",";

template<class T, int N>
void validate(boost::any& v, 
              const std::vector<std::string>& values,
              TinyVecOpt<T,N>*, int)
{
    using namespace boost;

    // Make sure no previous assignment to 'a' was made.
//      validators::check_first_occurence(v);

    // Extract the first string from 'values'. If there is more than
    // one string, it's an error, and exception will be thrown.
    const std::string& s=program_options::validators::get_single_string(values);
    
    try {
        // Do regex match and convert the interesting part to int.
        v = any(TinyVecOpt<T,N>(lexical_cast< TinyVecOpt<T,N> >(s)));
    } catch(const std::exception& e) {
        throw program_options::invalid_option_value(e.what() + s);
    } 
}

template<class T,int N>
std::ostream& operator << (std::ostream& o, const TinyVecOpt<T,N>& t) {
    if (N==1)
        o << t[0];
    else {
        o << "[ " ;
        for (int i= 0; i< N-1; ++i) 
            o << t[i] << t.token_seperator[0];
        o << t[N-1] << " ]";
    }
    return o;
}
template<class T,int N>
std::istream& operator >> (std::istream& o, TinyVecOpt<T,N>& t) {
    std::string val; o >> val;
    t.fromString(val);
    return o;
}

}

#endif // _LEAR_TINY_VEC_OPT_H_
