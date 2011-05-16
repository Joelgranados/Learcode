#include <iostream>
#include <lear/exception.h>

#include <lear/classifier/list_ppresult.h>

lear::List_PPResult::List_PPResult(
        const std::string& listfile_,
        const int marker)
            :
    Parent(),
    to(listfile_.c_str()), marker(marker)
{
    using namespace std;
    if (!to) {
        throw lear::Exception("List_PPResult::constructor()",
                    "Unable to open output file " + listfile_);
    }
}

void lear::List_PPResult::write( 
    const_iterator s, const_iterator e, const std::string)
{// {{{
     using namespace std;

    to  << setw(6) << 0 << ' ' << setw(6) << 0 << ' ' 
        << setw(6) << 0 << ' ' << setw(6) << 0 << ' ' 
        << setw(6) << 0 << ' ' << setw(6) << 0 << ' '
        << setw(6) << 0 << '\n';
    for (; s != e; ++s) 
        to  << *s << setw(4) << marker << '\n';

}// }}}

