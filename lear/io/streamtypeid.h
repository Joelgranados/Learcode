#ifndef _LEAR_STREAM_TYPE_ID_H_
#define _LEAR_STREAM_TYPE_ID_H_

namespace lear {

    template<class T>
    struct IOTypeIdentifier {
        typedef T   Type;
        enum { ID = 0};
    };
#define DEF_TYPE_ID(T, Value)       \
template<>                          \
struct IOTypeIdentifier<T> {        \
    typedef T    Type;              \
    enum { ID = Value};             \
};

DEF_TYPE_ID(bool,           1);
DEF_TYPE_ID(char,           2);
DEF_TYPE_ID(int,            3);
DEF_TYPE_ID(float,          4);
DEF_TYPE_ID(double,         5);
DEF_TYPE_ID(unsigned char,  6);
DEF_TYPE_ID(short,          7);
DEF_TYPE_ID(unsigned short, 8);
DEF_TYPE_ID(unsigned int,   9);
DEF_TYPE_ID(long,          10);
DEF_TYPE_ID(unsigned long, 11);

#undef DEF_TYPE_ID
        
}

#endif // _LEAR_STREAM_TYPE_ID_H_
