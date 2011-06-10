/*
 * =====================================================================================
 * 
 *        Filename:  imageutil.h
 * 
 *     Description:  Utilitiy functions for images.
 * 
 * =====================================================================================
 */

#ifndef _LEAR_IMAGE_UTIL_H
#define _LEAR_IMAGE_UTIL_H

#include <lear/util/fileutil.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

namespace lear {
    /** 
     * Same function as in imageio. Provided here with different name so as
     * to optimize compiling time. This way we do not need to include
     * blitz
     */
    std::string imagefmt(const std::string& imagename) ;

    /** 
     * Same function as in imageio. Provided here with different name so as
     * to optimize compiling time. This way we do not need to include
     * blitz
     */
    void imagesize(const std::string& imagename,int& width, int& height); 
    /**
     * Read image file names from inlist or directory and pushes them
     * to the list. It returns the number of images read from file/directory.
     * If infile is an image file, the return value is 0.
     */
    template<class PathVector>
    int imagelist(
        PathVector& inlist, const std::string file, const std::string ext="")
    {
        namespace fs=boost::filesystem;
        if (fs::is_directory(fs::path(file))) {
        //if (fs::is_directory(fs::path(file,fs::native))) {
            return lear::dirlist(inlist, file, ext);
        } else {
            std::string format = lear::imagefmt(file);
            if (format.size()){
                inlist.push_back(file);
                return 0;
            } else {
                return lear::dirlist(inlist, file, ext);
            }
        }
    }
}
#endif
