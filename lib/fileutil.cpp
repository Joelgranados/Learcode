#include <lear/util/fileutil.h>
using namespace std;

/**
 * basename_loc returns the basename start position and extension position.
 * It is advisable not to use it directly to get basename and extension.
 * Use the provided functions for this purpose.
 *
 * @return pair<startpos,extpos>
 */
std::pair<std::string::size_type, std::string::size_type>
    basename_loc(
        const std::string& fullpath,
        const std::string ext = ".");

pair<string::size_type, string::size_type>
    basename_loc(
        const string& fullpath,
        const string ext) 
{// {{{
    // set not to remove extension
    string::size_type dotpos = string::npos;
    if (ext==".")
        dotpos = fullpath.find_last_of(ext);
    else if (!ext.empty()) {
        dotpos = fullpath.rfind(ext);
        if (dotpos+ext.length()!=fullpath.length())
            dotpos = string::npos;
    }

    // if given extension string (for e.g. ".png") precisely
    // operate like `basename` command on unix
    // i.e. string must end with ext
    if (ext.length() >1 && 
        dotpos != string::npos && 
        (fullpath.length() - ext.length()) != dotpos) 
    {
        dotpos = string::npos;
    }

    string::size_type startpos = fullpath.find_last_of('/');
    
    // if no forward slash, search for backward slash
    if (startpos == string::npos) 
        startpos = fullpath.find_last_of('\\');

    if (startpos == string::npos)
        startpos = 0;
    else {
        // check if start pos is latter than dotpos
        // if yes, this implies that dot pos is not valid

        // size_type is probably unsigned, difference operator
        // for unsigned keeps string unsigned. this is bad
        // so cast to signed long int for conversion
        if (dotpos != string::npos && 
            static_cast<long >(startpos) - static_cast<long>(dotpos) > 0)
            dotpos = string::npos;
        ++startpos;
    }
    return make_pair(startpos,dotpos);
}// }}}

string lear::basename(
        const string& fullpath,
        const string ext) 
{
    pair< 
        string::size_type,
        string::size_type> loc = basename_loc(fullpath,ext);

    return fullpath.substr(loc.first, loc.second - loc.first);
}
string lear::dirname(
        const string& fullpath) 
{
    pair< 
        string::size_type,
        string::size_type> loc = basename_loc(fullpath);

    if (loc.first>0)
        return fullpath.substr(0,loc.first-1);
    else
        return string();
}
string lear::extension( const string& fullpath, const string ext) 
{
    pair< 
        string::size_type,
        string::size_type> loc = basename_loc(fullpath,ext);

    if (loc.second != string::npos){
        if (loc.second +1 == fullpath.length() && ext!=".")
        return fullpath.substr(loc.second);
        else
        return fullpath.substr(loc.second+1);
    }
    return string();
}
bool lear::verifydir(const string outfile, const bool isdir) 
{
    // check if outimage is valid
    namespace fs=boost::filesystem;
    fs::path outdir(outfile,fs::native);
    if (!isdir) 
        return ((fs::exists(outdir) && fs::is_directory(outdir))? false:true);
    return ((fs::exists(outdir) && fs::is_directory(outdir)) 
            || fs::create_directory(outdir));
    /*try {
    }catch (exception e) {
        return false;
    }*/
}
