// {{{ file documentation
/**
 * @file
 * @brief Implements file IO in binary
 *
 * @todo Do rigrous checks that has been mode in binary mode
 */
// }}}

#ifndef _LEAR_BI_ISTREAM_H_
#define _LEAR_BI_ISTREAM_H_

// {{{ headers
#include <fstream>
#include <string>

#include <lear/io/bistreamable.h>
// }}}

namespace lear {

// {{{ class documentation
/**
 * Binary Input steram.
 * Ensures that the stream we are reading from a binary stream.
 *
 * Also implements overloaded functions for BiStreamable class.
 */
// }}}
class BiIStream {
    protected:

        // {{{ private members
        std::ifstream from;

        const int MAXBUFF;

        static const int DEFAULT_MAXBUFF = 1024;
        // }}}

    public:
        typedef std::ifstream::pos_type     pos_type;
        // {{{ constructors
        /// Default constructor. 
        BiIStream(const int maxbuff = DEFAULT_MAXBUFF) 
            : from(), MAXBUFF(maxbuff) {} 

        /// Open filename for input. Ensures that stream is opened in binary 
        /// mode.
        BiIStream(
                const char* filename, 
                const std::ios::openmode mode = std::ios::binary, 
                const int maxbuff = DEFAULT_MAXBUFF):
            from(filename, mode | std::ios::binary), MAXBUFF(maxbuff){}
    
        /// Open a stream for input. Ensures that stream is opened in 
        /// binary mode.
        void open(
                const char* filename, 
                const std::ios::openmode mode = std::ios::binary)
        {
            from.open(filename, mode | std::ios::binary); 
        }
        // }}}

        // User defined types
        /// Read a BiIStreamable object from stream
        template<class T_obj>
        BiIStream& operator>>(BiStreamable<T_obj>& obj) {
            return obj.bload(*this);
        }
        
        // {{{ Basic built in types
#define LSGENERAL(TYPE)  \
	BiIStream& operator>>(TYPE& buffer){ \
            from.read((char*)(&buffer), sizeof(buffer)); \
            return *this; \
	} \

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

        /** Read char array till either a null char is found on the stream 
         * or maximum MAXBUFF characters have been read.
         * REMEMBER: If MAXBUFF characters are read, and a null char is still 
         * missing, stream state is set to bad.
         *
         * @warning: buffer may overflow. No such check
         * possible in binary file. Use cautiously.
         */
	BiIStream& operator>>(char buffer[]){ 
            from.getline(buffer,MAXBUFF,'\0');
            return *this; 
	}
        
        /// read a string from the stream till a null character
        /// is found
	BiIStream& operator>>(std::string& buffer){ 
            std::string::size_type l;
            (*this) >> l;
            buffer.resize(l);
            for (std::string::size_type i= 0; i< l; ++i) 
                from.get(buffer[i]);
            return *this;
	}
        
        void close() { from.close(); }
        bool good() const { return from.good(); }
        bool eof() const { return from.eof(); }
        bool fail() const { return from.fail(); }
        bool bad() const { return from.bad(); }

        bool operator!() const { return !from; }
        operator void*() const {return static_cast<void*>(from);}

        pos_type tellg() { return from.tellg(); }
        BiIStream& seekg(pos_type p) { 
            from.seekg(p);
            return *this;
        }
        int get() { return from.get(); }
        BiIStream& get (char& c ){ from.get(c); return *this;}
}; // end class


} // namespace lear

#endif // _LEAR_BI_ISTREAM_H_

