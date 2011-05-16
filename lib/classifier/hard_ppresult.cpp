#include <iostream>
#include <lear/exception.h>

#include <lear/classifier/hard_ppresult.h>

lear::Hard_PPResult::Hard_PPResult(const std::string& hardfile_) :
    Parent(), to(hardfile_.c_str())
{
    using namespace std;
    if (!to) {
        throw lear::Exception("Hard_PPResult::constructor()",
                    "Unable to open output file " + hardfile_);
    }
}

void lear::Hard_PPResult::write( 
    const_iterator s, const_iterator e, const std::string filename)
{// {{{
    for (; s != e; ++s) 
        to  << filename << '\t' << *s << std::endl;

}// }}}


