// {{{ file documentation
/** 
 * @file
 * @brief Defines various painting operations
 */
// }}}

#ifndef _LEAR_POINT2D_H_
#define _LEAR_POINT2D_H_

#include <lear/io/bistreamable.h>

namespace lear {

    /**
     * Point2D is a simple class for encapsulating point concept.
     */
template<class T_element=int>
class Point2D : public BiStreamable<Point2D<T_element> >{
    public:
        typedef T_element ElemType;
        typedef Point2D<T_element>  PointType;

        Point2D(const ElemType x_=0, const ElemType y_=0):
            x_(x_), y_(y_)
        {}

        void x(const int x_) {
            this->x_ = x_;
        }
        void y(const int y_) {
            this->y_ = y_;
        }
        ElemType x() const {
            return x_;
        }
        ElemType y() const {
            return y_;
        }

        /// Translate the point size by e
        Point2D<ElemType> operator + (const PointType e) const {
            return Point2D<ElemType>(x_+e.x_, y_+e.y_);
        }
        /// Translate the point size by e
        Point2D<ElemType>& operator +=(const PointType e) {
            x_+=e.x_; y_+=e.y_;
            return *this;
        }
        /// Decrease the point size by e
        Point2D<ElemType> operator - (const PointType e) const {
            return Point2D<ElemType>(x_-e.x_, y_-e.y_);
        }
        /// Decrease the point size by e
        Point2D<ElemType>& operator -=(const PointType e) {
            x_-=e.x_; y_-=e.y_;
            return *this;
        }

        inline BiIStream& bload(BiIStream& in) {
            in >> x_ >> y_;
            return in;
        }

        inline BiOStream& bsave(BiOStream& out) const {
            out << x_ << y_;
            return out;
        }
    private:
        ElemType    x_, y_;
};

} // namespace lear

#endif // #ifdef _LEAR_POINT2D_H_

