#ifndef _LEAR_FILE_UTIL_H_
#define _LEAR_FILE_UTIL_H_

#include <string>
#include <utility>
#include <iostream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <lear/io/ioext.h>
#include <lear/io/fileheader.h>

#include <lear/exception.h>
namespace lear {

template<class PathType = boost::filesystem::path>
struct BoostPath2PathType {
    PathType operator() (const boost::filesystem::path& p) const {
        return (PathType)p;
    }
};
template<>
struct BoostPath2PathType<std::string> {
    std::string operator() (const boost::filesystem::path& p) const {
        return p.native_file_string();
    }
};


/** 
 * Find basename from a full path string.
 * Basename is defined as the string left after removing
 * parent directory path and everything including last "."
 * from fullpath. 
 * If ext is a string, then if ext string occurs as suffix,
 * remove it else the string has no extension
 * If ext is empty string, then do not remove extension.
 *
 * @param   fullpath        Full path
 * @param   ext             ext string (Default is ".")
 */
std::string basename(
        const std::string& fullpath,
        const std::string ext = ".");

/** 
 * Find dirname from a full path string.
 * Dirname is defined as the string before removing
 * the file name from fullpath. 
 *
 * @param   fullpath        Full path
 */
std::string dirname(
        const std::string& fullpath);

/** 
 * Find extenstion from a full path string.
 * Extension is defined as the string after removing
 * parent directory path and everything including last "."
 * from fullpath. 
 * If there is no string after ext delimiter, filename has no extension
 * If ext is null, then do not remove extension.
 *
 * @param   fullpath        Full path
 * @param   ext             ext delimiter (Default is '.')
 */
std::string extension(
        const std::string& fullpath,
        const std::string ext = ".");

/**
 * Verify if the outfile points to a directory. If not, create it.
 * \return true if it is a directory and successfully created it
 * \return true if it isdir is false and outfile can be a file
 * else returns false
 */
bool verifydir(const std::string outfile, const bool isdir) ;

/** 
 * dirlist lists all the files in a directory with given extension.
 * Note: Only files are listed, not directories and list file may not be sorted
 *
 * \param filelist  a containter type such that file names can be 
 *                  "push_back"ed to it
 * \param indir     input directory to be listed
 * \param ext       extension of the files to be listed
 * \param infile    list only those files in indir with filenames as in infile 
 * \param convertor if not using string or boost::filesystem::path as pathname,
 *                  then specify a convertor class that can convert a 
 *                  boost::filesystem::path to required type
 *  \return         number of files read.
 *
 * If "indir" is empty, then "infile" filelist must start from root name, 
 * else "infile" filenames are relative filenames. If "indir" is not empty 
 * and is not a directory, then "indir" must be a list file and "infile" 
 * argument is ignored in this case.
 */
template<class ContainterType, class ConvertorType>
int dirlist(
        ContainterType& filelist,
        const std::string& indir, 
        const std::string& ext = "", 
        const std::string& infile = "",
        const ConvertorType convertor = ConvertorType())
{// {{{
    namespace fs=boost::filesystem;

    bool absPath = indir.empty();
    fs::path inpath(indir,fs::native);
    std::string listfile;

    // fill filelist with all the files in this directory
    if ( !absPath && !fs::is_directory( inpath ) ) {
        absPath = true; // set absPath as true -- treat indir as list file
        listfile = indir;
    } else 
        listfile = infile;

    bool hasExt = !ext.empty();
    int numelem=0;

    if (!absPath && infile.empty()) { // iterate over all files in the directory
        fs::directory_iterator end_iter;
        for ( fs::directory_iterator dir_iter( inpath ); dir_iter != end_iter; ++dir_iter ) {
            if ( !fs::is_directory( *dir_iter ) && 
                (!hasExt || (hasExt && fs::extension(*dir_iter) == ext) ) 
                ) 
            {
                ++numelem;
                filelist.push_back(convertor(*dir_iter));
            }
        }
    } else { // iterate over only file names listed in infile
        std::ifstream from(listfile.c_str());
        if (!from) {
            throw lear::Exception("dirlist", "Unable to open file: " + listfile );
        }
        char filename[1024*16];
        while (from.getline(filename,1024*16,'\n')) {
            fs::path file (filename,fs::native);
            if (!hasExt || (hasExt && fs::extension(file) == ext)) {
                fs::path t = absPath ? file : inpath/file;
                if (fs::exists(t)){
                    filelist.push_back(convertor(t));
                    ++numelem;
                } else
                    std::cerr << "WARNING:: In function dirlist(), "
                        << "looking for file with extension " << ext 
                        << ". No such file or directory: " << t.string() 
                        << std::endl;
            }
        }
    }
    return numelem;
}// }}}

template<class ContainterType >
int dirlist(// {{{
        ContainterType& filelist,
        const std::string& indir, 
        const std::string& ext = "", 
        const std::string& infile = "")
{
    return dirlist(filelist, indir, ext, infile, BoostPath2PathType< typename ContainterType::value_type>());
}// }}}

template<class ContainterType>
int getfilelist(
    ContainterType& inlist, const std::string& file, 
    const std::string ext="", const std::string fileext="")
{// {{{
    namespace fs=boost::filesystem;
    if (fs::is_directory(file)) {
        return lear::dirlist(inlist, file, ext);
    } else {
        std::string format = extension(file);
        if (format == fileext){
            return lear::dirlist(inlist, file, ext);
        } else {
            inlist.push_back(file);
            return 1;
        }
    }
}// }}}

/**
 * Read file list from a directory, list file or file in proper format
 * \param Container to hold names of files
 * \param input file directory name, list file name or file name
 * \param headertag. In case it is a directory, use headertag to verify
 * if file is a list file or single instance of a file.
 * \param ext. If directory, get only files with particular extension
 * \return 0 if one file instance, else number of files in directory or list
 * file
 * \see dirlist 
 */
template<class PathVector> static 
int filelist(
    PathVector& inlist, const std::string file, 
    const std::string& headertag,
    const std::string ext="") 
{
    namespace fs=boost::filesystem;
    if (fs::is_directory(fs::path(file,fs::native))) {
        return lear::dirlist(inlist, file, ext);
    } else {
        FileHeader header(headertag);
        if (header.verify(file)){
            inlist.push_back(file);
            return 0;
        } else {
            return lear::dirlist(inlist, file, ext);
        }
    }
}
/**
 * If infile is not a directory and if infile extension is ".lst" assumes
 * it to be list file else this file is pushed to the inlist.
 */
template<class PathVector> static 
int loosefilelist(
    PathVector& inlist, const std::string file,const std::string ext="") 
{
    namespace fs=boost::filesystem;
    if (fs::is_directory(fs::path(file,fs::native))) {
        return lear::dirlist(inlist, file, ext);
    } else {
        std::string format = lear::extension(file);
        if ((format.size() && format=="lst" )){
            return lear::dirlist(inlist, file, ext);
        } else {
            inlist.push_back(file);
            return 0;
        }
    }
}
/**
 * Assumes that inlist is either a directory or a list file.
 */
template<class PathVector> static 
int onlyfilelist(
    PathVector& inlist, const std::string file,const std::string ext="") 
{
    namespace fs=boost::filesystem;
    if (fs::is_directory(fs::path(file,fs::native))) {
        return lear::dirlist(inlist, file, ext);
    } else {
        std::string format = lear::extension(file);
        if ((format.size() && format=="lst" )){
            return lear::dirlist(inlist, file, ext);
        } else {
            inlist.push_back(file);
            return 0;
        }
    }
}
/** 
 * filelist reads infile and create a list of each word in file
 *
 * \param infile        input file to be listed
 * \param container     to "push_back" new elements
 */
/*
template<class ContainterType >
void filelist(ContainterType& container, const std::string& infile) // {{{
{
    std::ifstream from(infile.c_str());
    if (!from) {
        throw lear::Exception("filelist", "Unable to open file: " + infile );
    }
    std::string line;

    from >> container;
}
template<class ContainterType >
void filelist( const std::string& infile, ContainterType& container) 
{ // same as above
    filelist(container,infile);
}// }}}
*/
}

#endif // _LEAR_FILE_UTIL_H_
