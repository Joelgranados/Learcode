// {{{ file documentation
/** 
 * @file
 * @brief Defines various painting operations
 */
// }}}

#ifndef _LEAR_RECTANGLE_H_
#define _LEAR_RECTANGLE_H_

#include <lear/io/bistreamable.h>

namespace lear {

    /**
     * Rectangle is a simple class for encapsulating rectangle concept.
     * It is mainly provided to check if one rectangle bounds other one or not
     */
template<class T_element=int>
class Rectangle : public BiStreamable< Rectangle<T_element> >{
    public:
        typedef T_element ElemType;

        Rectangle(const ElemType x_=0, const ElemType y_=0, const ElemType w_=-1, const ElemType h_=-1):
            x_(x_), y_(y_), w_(w_), h_(h_) {}

        ElemType x() const {
            return x_;
        }
        ElemType y() const {
            return y_;
        }
        /// top
        ElemType t() const {
            return y_;
        }
        /// bottom
        ElemType b() const {
            return y_ + h_-1;
        }
        /// left
        ElemType l() const {
            return x_;
        }
        /// right
        ElemType r() const {
            return x_ + w_ -1;
        }
        ElemType w() const {
            return w_;
        }
        ElemType h() const {
            return h_;
        }
        ElemType width() const {
            return w();
        }
        ElemType height() const {
            return h();
        }

        /** Return true if argument rectangle is within this rectangle*/
        template<class T_other>
        bool bounds(const Rectangle<T_other>& r) const {
            if (x_ <= r.x_ && y_ <= r.y_ && (x_+w_ >= r.x_+r.w_) && (y_+h_ >= r.y_+r.h_))
                return true;
            return false;
        }
        /// check if point x,y is inside rectangle
        template<class T>
        bool inside(const T x, const T y) const {
            return x>=x_ && y >= y_ && x<(x_+w_) && y<(y_+h_);
        }
        /// Increase the rectangle size by e from all sides
        Rectangle<ElemType> operator + (const ElemType e) const {
            return Rectangle<ElemType>(x_-e, y_-e, w_+2*e, h_+2*e);
        }
        /// Increase the rectangle size by e from all sides
        Rectangle<ElemType>& operator +=(const ElemType e) {
            x_-=e; y_-=e; w_+=2*e; h_+=2*e;
            return *this;
        }
        /// Decrease the rectangle size by e from all sides
        Rectangle<ElemType> operator - (const ElemType e) const {
            return *this + (-e);
        }
        /// Decrease the rectangle size by e from all sides
        Rectangle<ElemType>& operator -=(const ElemType e) {
            return (*this)+=-e;
        }

        bool operator < (const Rectangle<ElemType>& r) const {
            return w_*h_ < r.w_*r.h_;
        }
        inline BiIStream& bload(BiIStream& in) {
            in >> x_ >> y_ >> w_ >> h_;
            return in;
        }

        inline BiOStream& bsave(BiOStream& out) const {
            out << x_ << y_ << w_ << h_;
            return out;
        }
    private:
        ElemType    x_, y_, w_, h_;
};

} // namespace lear

#endif // #ifdef _LEAR_RECTANGLE_H_

