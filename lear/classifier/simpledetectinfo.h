#ifndef _LEAR_DETECT_OBJ_H_
#define _LEAR_DETECT_OBJ_H_

#include <list>
struct LocType {
    int x, y;
};
struct DetectionResult {
    float score, scale;
    LocType lbound, extent;
};

void detect_person(const int* infile, const int width, const int height,
        std::list<DetectionResult>& r) ;
#endif // _LEAR_DETECT_OBJ_H_
