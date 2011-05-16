#include <lear/util/util.h>
#include <cstdlib>
#include <iostream>

using namespace std;

int lear::NoCaseComparison(const string & s1, const string& s2) {// {{{
    //stop when either string's end has been reached
    for ( string::const_iterator it1=s1.begin(), it2=s2.begin();
            (it1!=s1.end()) && (it2!=s2.end()); ++it1, ++it2) 
    { 
        // return -1 to indicate smaller than, 1 otherwise
        if(toupper(*it1) != toupper(*it2)) //letters differ?
            return (toupper(*it1)  < toupper(*it2)) ? -1 : 1; 
    }
    string::size_type size1=s1.size(), size2=s2.size();// cache lengths
    //return -1,0 or 1 according to strings' lengths
    if (size1==size2) 
        return 0;
    return (size1<size2) ? -1 : 1;
}// }}}

string lear::toUpper(const string& str) {// {{{
    string ustr (str.size(),' ');
    string::iterator it2=ustr.begin();
    for ( string::const_iterator it1=str.begin();
            it1!=str.end(); ++it1, ++it2) 
        *it2 = toupper(*it1);
    //transform(str.begin(), str.end(), ustr.begin(), toupper);
    return ustr;
}// }}}

string& lear::warptext(string& str, const unsigned width) {// {{{
    if (width < 1)
        return str;

    typedef string::size_type INT;
    INT EMPTY = string::npos;
    const char space=' ';
    const char newline='\n';

    INT end = str.size();
    INT beg = 0;
    while (end > width) {
        INT c = str.substr(beg,width).find_last_of(newline);
        if (c != EMPTY) {
            beg += c+1;
            end -= c+1;
            continue;
        }
        INT pos = str.substr(beg,width).find_last_of(space);

        if (pos == EMPTY) {
            INT s = str.substr(beg).find_first_of(space);
            INT n = str.substr(beg).find_first_of(newline);

            INT t = EMPTY;
            if (s != EMPTY && n!= EMPTY)
                t = min(s,n);
            else if (s != EMPTY)
                t = s;
            else if (n != EMPTY)
                t = n;

            if (t == EMPTY)
                break;
            str[beg+t] = newline;
            beg += t+1;
            end -= t+1;
            continue;
        } else {
            str[beg+pos] = newline;
            beg += pos+1;
            end -= pos+1;
        }
    }
    return str;
}// }}}

double lear::getRunningTime() {// computes runtime in ms
   clock_t start = std::clock();       // for timing purposes
   return(start*1000/(double)CLOCKS_PER_SEC);
}
