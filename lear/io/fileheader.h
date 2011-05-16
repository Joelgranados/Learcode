// {{{ file documentation
/**
 * @file
 * @brief Implements file get/set/check header functionality
 */
// }}}

#ifndef _LEAR_FILE_HEADER_H_
#define _LEAR_FILE_HEADER_H_

// {{{ headers
#include <string>
#include <iostream>

#include <lear/exception.h>
#include <lear/io/biistream.h>
#include <lear/io/biostream.h>
#include <lear/io/bistreamable.h>
// }}}

namespace lear {

class FileHeader : public BiStreamable<FileHeader> {
    public:
        typedef std::string::size_type LenType;

        FileHeader(const std::string& tag__ ="", const int version__ =0);

        /// return last set or read file version
        int version() const { return version_; }

        /// return last set or read file identity tag
        std::string tag() const { return tag_; }

        /// load FileHeader from an input stream file
        void bload(BiIStream& stream);

        /// save FileHeader to an output stream in binary format. 
        void bsave(BiOStream& stream) const ;

        /** Check if file's identity tag is same as current tag. Only
         * tag information is read. Return false if file is empty.
         *
         * An example: To check if file's identity tag is
         * "PointDes",
         *
         * \code
         * FileHeader("PointDes").verify(filename);
         * \endcode
         */
        bool verify(const std::string& file) const ;
        /** Check if passed tag matches current file tag. 
         *
         * Note: identify should be called only after reading file header (i.e.
         * identity tag and version number).  An example: To check if stream
         * identity tag is "PointDes",
         *
         * \code
         * FileHeader h;
         * stream >> h;
         * if (!h.identify("PointDes"))
         *      // do something, throw an error
         * \endcode
         */
        bool identify(const std::string& tagp) const ;

        static int length() { return HEADER_LEN + sizeof(int); }


    protected:
        /** Check if stream's header is same as current file header
         */
        bool verify(BiIStream& stream) const ;
        std::string readtag(BiIStream& stream) const ;

        std::string tag_;
        int version_;
        static const LenType HEADER_LEN      = 8;
};

std::ostream& operator<<(std::ostream& o, const FileHeader& f);
} // namespace lear

#endif // _LEAR_FILE_HEADER_H_

