/**
 * @file
 * @brief Defines Exception class.
 */
#ifndef _LEAR_EXCEPTION_H_
#define _LEAR_EXCEPTION_H_

#include <string>
#include <exception>

namespace lear {

//   Exception class
/**
 * @brief Exception class. Used for reporting error messages very briefly.
 *
 * Currently, user has to specify a where string(for e.g. function name) and mesg string(error mesg)
 * to create Exception class.
 *
 * \todo Automatic printing of Line Number and File Name where exception is thrown in code.
 */
class Exception : public std::exception {
    protected:
	const std::string where;
	const std::string mesg;
    public:
        inline Exception(
                const std::string where_, const std::string mesg_) throw():
            where(where_) , mesg(mesg_) {}

        inline const char* what() const throw() {
            return std::string(
                    "Exception: " + mesg + "\t[ in "+where +" ]").c_str();
        }

        inline ~Exception() throw() {}
};

} // lear
#endif // #ifndef _LEAR_EXCEPTION_H_
