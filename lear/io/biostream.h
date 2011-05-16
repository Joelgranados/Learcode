// {{{ file documentation
/**
 * @file
 * @brief Implements file IO in binary
 *
 * @todo Do rigrous checks that has been mode in binary mode
 */
// }}}

#ifndef _LEAR_BI_OSTREAM_H_
#define _LEAR_BI_OSTREAM_H_

// {{{ headers
#include <fstream>
#include <string.h>

#include <lear/io/bistreamable.h>
// }}}

namespace lear {

// {{{ class documentation
/**
 * Binary Input Output steram.
 * Ensures that the stream we are writing to is binary stream.
 *
 * Also implements overloaded functions for BiStreamable class.
 */
// }}}
class BiOStream{
    protected:

        // {{{ private members
        std::ofstream to;

        const int MAXBUFF;

        static const int DEFAULT_MAXBUFF = 1024;
        // }}}

    public:
        typedef std::ofstream::pos_type     pos_type;
        // {{{ constructors
        /// Default constructor. 
        BiOStream(const int maxbuff = DEFAULT_MAXBUFF) 
            : to(), MAXBUFF(maxbuff) {} 

        /// Open filename for output. Ensures that stream is opened in 
        /// binary mode.
        BiOStream(
                const char* filename, 
                const std::ios::openmode mode = std::ios::binary, 
                const int maxbuff = DEFAULT_MAXBUFF):
            to(filename, mode | std::ios::binary), MAXBUFF(maxbuff){}
    
        /// Open a stream for output. Ensures that stream is opened in 
        /// binary mode.
        void open(
                const char* filename, 
                const std::ios::openmode mode = std::ios::binary)
        {
            to.open(filename, mode | std::ios::binary); 
        }
        // }}}

        /// Write a BiOStreamable object to stream
        template<class T_obj>
        BiOStream& operator<<(const BiStreamable<T_obj>& obj) {
            return obj.bsave(*this);
        }      
        

        // {{{ Basic built in types
#define LSGENERAL(TYPE)  \
	BiOStream& operator<<(const TYPE buffer){ \
            to.write((char*)&buffer, sizeof(buffer)); \
            return *this; \
	} 

        // general io operation to the stream
        // basic types
	LSGENERAL(char);
	LSGENERAL(unsigned char);
	LSGENERAL(short);
	LSGENERAL(unsigned short);
	LSGENERAL(int);
	LSGENERAL(unsigned int);
	LSGENERAL(long);
	LSGENERAL(unsigned long);
	LSGENERAL(float);
	LSGENERAL(double);
	LSGENERAL(bool);

#undef LSGENERAL
        // }}}

        /** Write char array, char buffer must be null terminated.
         * Char buffer longer than MAXBUFF can be written. However,
         * it will be hard to read
         */
	BiOStream& operator<<(const char buffer[]){
            to.write(buffer, strlen(buffer)+1);
            return *this;
	}
        
        /// write the string to the stream. String is
        /// converted to c-style strings before writing.
        BiOStream& operator<<(const std::string& buffer){
            (*this) << buffer.size();
            for (std::string::size_type i= 0; i< buffer.size(); ++i) 
            to.put(buffer[i]);
            return *this;
	} 

        void close() { to.close(); }
        bool good() const { return to.good(); }
        bool eof() const { return to.eof(); }
        bool fail() const { return to.fail(); }
        bool bad() const { return to.bad(); }

        bool operator!() const { return !to; }
        operator void*() const {return static_cast<void*>(to);}

        pos_type tellp() { return to.tellp(); }
        BiOStream& seekp(pos_type p) { 
            to.seekp(p);
            return *this;
        }
        BiOStream& seekp(pos_type p,std::ios_base::seekdir dir) { 
            to.seekp(p,dir);
            return *this;
        }
        BiOStream&  put ( char ch ){ to.put(ch); return *this;}

        BiOStream& flush() { to.flush(); return *this;}
}; // end class


} // namespace lear

#endif // _LEAR_BI_OSTREAM_H_

