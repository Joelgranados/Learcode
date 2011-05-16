#ifndef _LEAR_FUNCTIONAL_H_
#define _LEAR_FUNCTIONAL_H_

#include <functional>
namespace lear {

    template<class T, class A, class R> 
    class op_mem_fun_t : public std::binary_function<T*,A,R> {
        T* obj;
        R (T::*pmf)(A);
        public:
        explicit op_mem_fun_t(T* o, R (T::*p)(A)): obj(o), pmf(p){}

        R operator()(A a) { return (obj->*pmf)(a); }
    };

    template<class T, class A, class R> 
    op_mem_fun_t<T,A,R> op_mem_fun( T* o, R(T::*p)(A)) {
        return op_mem_fun_t<T,A,R>(o,p);
    }

}
#endif // _LEAR_FUNCTIONAL_H_
