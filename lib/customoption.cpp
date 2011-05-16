#include <sstream>
#include <lear/exception.h>
#include <lear/util/util.h>
#include <lear/util/customoption.h>
using namespace lear;
bool CustomOption::validate(const std::string& which)  const
{
    // check if format is rightly specified or not?
    for (unsigned i= 0; i< optlst_.size(); ++i) {
        if (NoCaseComparison(which, optlst_[i]) == 0) 
            return true;
    }
    throw Exception("CustomOption::check()",
        "Error in " + mesg_ + " option argument. Method '" +
        which + "' is unsupported");
}

int CustomOption::check(const std::string& which) const
{
    for (unsigned i= 0; i< optlst_.size(); ++i) {
        if (NoCaseComparison(which, optlst_[i]) == 0) 
            return optnum_[i];
    }
    throw Exception("CustomOption::check()",
        "Option argument error in " + mesg_ + ". Method '" +
        which + "' is unsupported");
}
std::string CustomOption::usage() const { 
    std::ostringstream mesg;
    int padsize=0;
    for (unsigned i= 0; i< optlst_.size(); ++i) 
        if (padsize < (int)optlst_[i].size())
            padsize = (int)optlst_[i].size(); 

    mesg << "Supported options in " << mesg_ << " are:\n";
    for (unsigned i= 0; i< optlst_.size(); ++i) 
    {
        mesg << "       " <<  optlst_[i] ;
        for (int j= 0; j< (padsize - (int)optlst_[i].size()); ++j) 
                mesg << ' ';
        if (optdetail_[i].size())
            mesg << ' ' << optdetail_[i];
        mesg << '\n';
    }
    return mesg.str();
}
std::string CustomOption::defaultOption() const {
    for (unsigned i= 0; i< optnum_.size(); ++i) 
        if (optnum_[i] == default_)
            return optlst_[i];
    return (optlst_.size()>0)?optlst_[0]:"";
}
