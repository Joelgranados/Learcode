#ifndef _LEAR_DESC_NORMALIZER_H_
#define _LEAR_DESC_NORMALIZER_H_

#include <cmath>

#include <blitz/array.h>
#include <lear/io/biostream.h>

namespace lear {

template<class RealType_>
class DescNormalizer {// {{{
    enum {Method = 0};
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    DescNormalizer(){}
    virtual ~DescNormalizer(){}

    template<int N>
    void operator()(blitz::Array<RealType,N>& vec) const {
        using namespace blitz;
        Array1DType t(vec.data(),shape(vec.size()),neverDeleteData);
        doit(t);
    }
    virtual const char* toString() const {
        return "'NONE'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }
    virtual void print(std::ostream& o) const {
        o <<    "Normalizer " << toString();
    }
    virtual void print(lear::BiOStream& o) const {
        o << toMethod();
    }

    protected:
    // Does nothing
    virtual void doit(Array1DType&) const {}
};// }}}

template<class RealType_>
class L1Normalizer : public DescNormalizer<RealType_> {// {{{
    enum {Method = 1};
    typedef DescNormalizer<RealType_>           Parent;
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L1Normalizer(const RealType epsilon): 
        DescNormalizer<RealType>(), epsilon(epsilon)
    {}

    virtual const char* toString() const {
        return "'L1 norm'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }

    virtual void print(std::ostream& o) const {
        using namespace std;
        o <<    "Normalizer " << toString() <<  
            ", eps " << setw(2) << setprecision(1) << epsilon;
    }
    virtual void print(lear::BiOStream& o) const {
        o << toMethod() << epsilon;
    }
    protected:
    virtual void doit(Array1DType& vec) const {
        const RealType norm = blitz::sum(vec) + epsilon*vec.size();
        vec /= norm;
    }
    /// add small epsilon to make normalization process stable
    RealType epsilon;
};// }}}

template<class RealType_>
class L2Normalizer : public L1Normalizer<RealType_>{// {{{
    enum {Method = 2};
    typedef L1Normalizer<RealType_>             Parent;
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L2Normalizer(const RealType epsilon): L1Normalizer<RealType>(epsilon) {}

    virtual const char* toString() const {
        return "'L2 norm'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }
    virtual void print(std::ostream& o) const {
        L1Normalizer<RealType>::print(o);
    }
    virtual void print(lear::BiOStream& o) const {
        L1Normalizer<RealType>::print(o);
    }
    protected:
    virtual void doit(Array1DType& vec) const {
        const RealType norm = std::sqrt(blitz::sum(vec*vec)) + 
            Parent::epsilon*vec.size();
        vec /= norm;
    }
};// }}}

template<class RealType_>
class L2HysNormalizer : public L2Normalizer<RealType_>{// {{{
    enum {Method = 3};
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L2HysNormalizer(const RealType epsilon, const RealType maxval)
        : L2Normalizer<RealType>(epsilon),maxval(maxval), epsilon2(0.01) {}

    virtual const char* toString() const {
        return "'L2 + hys'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }
    virtual void print(std::ostream& o) const {
        L2Normalizer<RealType_>::print(o);
        using namespace std;
        o << ", hysth " << setw(4) << setprecision(1) << maxval;
    }
    virtual void print(lear::BiOStream& o) const {
        L2Normalizer<RealType_>::print(o);
        o << maxval;
    }

    protected:
    virtual void doit(Array1DType& vec) const {
        L2Normalizer<RealType>::doit(vec);
        for (int i= 0; i< vec.size(); ++i) 
            if (vec(i) > maxval)
                vec(i) = maxval;
        //L2Normalizer<RealType>::doit(vec);
        RealType norm = std::sqrt(sum(vec*vec)) + epsilon2;
        vec /= norm;
    }
    RealType maxval, epsilon2;
};// }}}

template<class RealType_>
class L1SqrtNormalizer : public L1Normalizer<RealType_>{// {{{
    enum {Method = 4};
    public:
        typedef RealType_                           RealType;
        typedef blitz::Array<RealType,1>            Array1DType;

    L1SqrtNormalizer(const RealType epsilon): 
        L1Normalizer<RealType>(epsilon) {}

    virtual const char* toString() const {
        return "'L1 + sqrt'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }

    protected:
    virtual void doit(Array1DType& vec) const {
        L1Normalizer<RealType>::doit(vec);
        vec = blitz::sqrt(vec);
    }
};// }}}

///// Second set of normalizers.. These one check if norm < epsilon, if yes normalize them to set to 1.
template<class RealType_>
class L1TradNormalizer : public L1Normalizer<RealType_> {// {{{
    enum {Method = 5};
    typedef L1Normalizer<RealType_>             Parent;
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L1TradNormalizer(const RealType epsilon): 
        Parent(epsilon)
    {}

    virtual const char* toString() const {
        return "'L1Trad norm'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }

    protected:
    virtual void doit(Array1DType& vec) const {
        RealType norm = blitz::sum(vec);
        if (norm < Parent::epsilon*vec.size()) norm = 1;
        vec /= norm;
    }
};// }}}

template<class RealType_>
class L2TradNormalizer : public L2Normalizer<RealType_>{// {{{
    enum {Method = 6};
    typedef L2Normalizer<RealType_>             Parent;
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L2TradNormalizer(const RealType epsilon): Parent(epsilon) {}

    virtual const char* toString() const {
        return "'L2Trad norm'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }
    protected:
    virtual void doit(Array1DType& vec) const {
        RealType norm = std::sqrt(blitz::sum(vec*vec));
        if (norm < Parent::epsilon*vec.size()) norm = 1;
        vec /= norm;
    }
};// }}}

template<class RealType_>
class L2TradHysNormalizer : public L2HysNormalizer<RealType_>{// {{{
    enum {Method = 7};
    typedef L2HysNormalizer<RealType_>          Parent;
    public:
    typedef RealType_                           RealType;
    typedef blitz::Array<RealType,1>            Array1DType;

    L2TradHysNormalizer(const RealType epsilon, const RealType maxval)
        : Parent(epsilon,maxval) {}

    virtual const char* toString() const {
        return "'L2Trad + hys'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }
    protected:
    virtual void doit(Array1DType& vec) const {
        RealType norm = std::sqrt(blitz::sum(vec*vec));
        if (norm < Parent::epsilon*vec.size()) norm = 1;
        vec /= norm;
        for (int i= 0; i< vec.size(); ++i) 
            if (vec(i) > Parent::maxval)
                vec(i) = Parent::maxval;
        norm = std::sqrt(sum(vec*vec));
        if (norm < Parent::epsilon*vec.size()) norm = 1;
        vec /= norm;
    }
};// }}}

template<class RealType_>
class L1TradSqrtNormalizer : public L1TradNormalizer<RealType_>{// {{{
    enum {Method = 8};
    typedef L1TradNormalizer<RealType_>          Parent;
    public:
        typedef RealType_                           RealType;
        typedef blitz::Array<RealType,1>            Array1DType;

    L1TradSqrtNormalizer(const RealType epsilon): Parent(epsilon) {}

    virtual const char* toString() const {
        return "'L1Trad+sqrt'";
    }
    virtual unsigned toMethod() const {
        return Method;
    }

    protected:
    virtual void doit(Array1DType& vec) const {
        L1TradNormalizer<RealType>::doit(vec);
        vec = blitz::sqrt(vec);
    }
};// }}}

template<class RealType_> std::ostream& 
    operator<<( std::ostream& o, const DescNormalizer<RealType_>& d)
{ d.print(o); return o; }
template<class RealType_> lear::BiOStream& 
    operator<<( lear::BiOStream& o, const DescNormalizer<RealType_>& d)
{ d.print(o); return o; }

}

#endif // _LEAR_NORMALIZER_H_

