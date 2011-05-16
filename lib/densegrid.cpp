#include <cmath>
#include <lear/cvision/densegrid.h>

lear::DenseGrid<1>::GridType lear::DenseGrid<1>::get(
        const int size, const int bound, const int stride)
{
    const int extent ((size-bound)/stride+1);

    using namespace blitz;

    GridType grid(extent);

    grid = tensor::i*stride;

    const int leftover = (size - bound - grid(extent-1))/2;
    grid += leftover;

//      std::cout << " ex = " << extent << " lo = " << leftover << std::endl;
//      std::cout << grid << std::endl;
    return grid;
}

