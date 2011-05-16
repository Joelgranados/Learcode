#ifndef _LEAR_SORT_UTILS_H_
#define _LEAR_SORT_UTILS_H_

#include <utility>
#include <lear/triplet.h>
namespace lear {

struct CompareOnFirst {
    template<class TA, class TB>
    bool operator()(const std::pair<TA,TB>& a, const std::pair<TA,TB>& b) {
        return a.first < b.first;
    }
    template<class TA, class TB, class TC>
    bool operator()(const triplet<TA,TB,TC>& a, const triplet<TA,TB,TC>& b) {
        return a.first < b.first;
    }
};
struct CompareOnSecond {
    template<class TA, class TB>
    bool operator()(const std::pair<TA,TB>& a, const std::pair<TA,TB>& b) {
        return a.second < b.second;
    }
    template<class TA, class TB, class TC>
    bool operator()(const triplet<TA,TB,TC>& a, const triplet<TA,TB,TC>& b) {
        return a.second < b.second;
    }
};

struct CompareOnFirst_GrtrThn {
    template<class TA, class TB>
    bool operator()(const std::pair<TA,TB>& a, const std::pair<TA,TB>& b) {
        return a.first > b.first;
    }
    template<class TA, class TB, class TC>
    bool operator()(const triplet<TA,TB,TC>& a, const triplet<TA,TB,TC>& b) {
        return a.first > b.first;
    }
};
struct CompareOnSecond_GrtrThn {
    template<class TA, class TB>
    bool operator()(const std::pair<TA,TB>& a, const std::pair<TA,TB>& b) {
        return a.second > b.second;
    }
    template<class TA, class TB, class TC>
    bool operator()(const triplet<TA,TB,TC>& a, const triplet<TA,TB,TC>& b) {
        return a.second > b.second;
    }
};
}

#endif //_LEAR_SORT_UTILS_H_
//  // {{{ Correspondence Struct
//  /**
//   * Store the correspondence indices and match score.
//   */
//  template <class score_type, class index_type = int>
//  struct Correspondence {
//      typedef score_type ScoreType;
//      typedef index_type IndexType;
//      typedef Correspondence<ScoreType,IndexType> CorrespondenceType;

//      ScoreType score;
//      IndexType index;
//      
//      Correspondence() {}

//      Correspondence(const score_type score_, const index_type index_) :
//           score(score_), index(index_)
//      { }
//      inline bool operator<(const CorrespondenceType& o) const
//      {
//          return score < o.score;
//      }
//  };
//  //}}}

//  //{{{ correspondence comparison functor
//  /**
//   * Correspondence comparsion functor object.
//   * Implements () operator and operator return true if score of a <= score of b
//   */
//  struct Compare {
//      template<class ScoreA, class IndexA, class ScoreB, class IndexB>
//      inline bool operator()(
//              const Correspondence<ScoreA, IndexA>& a, 
//              const Correspondence<ScoreB, IndexB>& b) 
//      {
//          return a.score < b.score;
//      }
//  };
//  /**
//   * Correspondence comparsion functor object. Does strict comparison i.e. if a==b => false
//   * Implements () operator and operator return true if score of a < score of b
//   */
//  struct CompareStrict {
//      template<class ScoreA, class IndexA, class ScoreB, class IndexB>
//      inline bool operator()(
//              const Correspondence<ScoreA, IndexA>& a, 
//              const Correspondence<ScoreB, IndexB>& b) 
//      {
//          return a.score < b.score;
//      }

//  };
//  // }}}
// // {{{
// /**
//  * Correspondence index comparsion functor
//  */
// struct IndexEquals {
//     template<class ScoreA, class IndexA, class ScoreB, class IndexB>
//     inline bool operator()(
//             const Correspondence<ScoreA, IndexA>& a, 
//             const Correspondence<ScoreB, IndexB>& b) 
//     {
//         return a.index == b.index;
//     }
// };
// // }}}
