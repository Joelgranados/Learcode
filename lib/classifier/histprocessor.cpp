#include <cmath>
#include <lear/exception.h>
#include <lear/blitz/blitzio.h>

#include <lear/classifier/histprocessor.h>

using namespace std;

lear::HistProcessor::HistProcessor( 
        const std::string& histfile_,
        const RealType minTh_,
        const RealType extTh_, 
        const RealType bandwidth_)
            :
    histfile_(histfile_),
    dohistout_(!histfile_.empty()),
    minTh_(minTh_), maxTh_(minTh_+extTh_-bandwidth_),
    histall_(minTh_,extTh_,bandwidth_)
{}
lear::HistProcessor::~HistProcessor()
{ if (dohistout_) lear::writeArray(histfile_,histall_.data()); }


void lear::HistProcessor::operator()(const RealType score)
{
    if (dohistout_) {
        RealType d = std::max(static_cast<RealType>(score),minTh_); 
        d = std::min(d,maxTh_);
        histall_(d);
    }
}

