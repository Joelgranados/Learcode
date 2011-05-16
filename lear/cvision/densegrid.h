#ifndef _LEAR_DENSE_GRID_H_
#define _LEAR_DENSE_GRID_H_

#include <vector>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <lear/blitz/ext/domainiter.h>

namespace lear {
template<int N> struct DenseGrid;

template<>
struct DenseGrid<1> {
    typedef int IndexType;
    typedef blitz::Array<IndexType,1> GridType;

    static GridType get(IndexType size, IndexType bound, IndexType stride);
};

template<int N>
struct DenseGrid {
    typedef blitz::TinyVector<int,N> IndexType;
    typedef blitz::Array<IndexType,N> GridType;

    static GridType get(IndexType size, IndexType bound, IndexType stride) {
        typedef DenseGrid<1>::GridType Type;
        std::vector<Type> allgrid;

        IndexType numelem = 0;
        for (int i= 0; i< N; ++i) {
            allgrid.push_back(DenseGrid<1>::get(size[i],bound[i],stride[i]));
            numelem[i] = allgrid[i].size();
        }

        GridType grid(numelem);

        IndexType tmp;
        for (blitz::DomainIter<N> i(0,numelem-1); i; ++i) {
            for (int j= 0; j< N ; ++j) 
                tmp[j] = allgrid[j]((*i)[j]);
            grid(*i) = tmp;
        }
        return grid;
    }
};

}

#endif // _LEAR_DENSE_GRID_H_

/* old grid code */
//  template<>
//  struct DenseGrid<2> {
//      typedef blitz::TinyVector<int,2> IndexType;
//      typedef blitz::Array<IndexType,1> GridType;

//      static GridType get(IndexType size, IndexType bound, IndexType stride) {
//          IndexType center ( blitz::ceil(size/2.0) );
//          IndexType halfbound ( blitz::ceil(bound/2.0));

//          blitz::TinyVector<float,2> riter (size - halfbound - center);
//          riter/=stride;


//          // iter now gives no. of elements around each side of center
//          IndexType iter (riter);
//          // total no. of elements
//          IndexType extent (2*iter+1);

//          GridType grid(blitz::product(extent));
//          for (int i= 0,index=0; i< extent[0]; ++i) 
//          for (int j= 0; j< extent[1]; ++j,++index) {
//              grid(index) = (IndexType(i,j) - iter)*stride + center - halfbound;
//  //              std::cout << grid(index) << std::endl;
//          }
//          return grid;
//      }
//  };
