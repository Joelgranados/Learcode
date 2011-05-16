#ifndef _LEAR_MS_PROCESS_RESULT_H_
#define _LEAR_MS_PROCESS_RESULT_H_

#include <list>
#include <blitz/tinyvec.h>
#include <lear/classifier/processresult.h>
#include <lear/cvision/transfunc.h>

namespace lear {

    struct SS_ProcessResult : public ProcessResult {
        typedef ProcessResult                       Parent;
        typedef Parent::RealType                    RealType;
        typedef Parent::DetectionWin                DetectionWin;
        typedef DetectionWin::const_iterator        const_iterator;

        typedef blitz::TinyVector<int,2>            IndexType;
        typedef blitz::TinyVector<RealType,2>       Real2DType;
        typedef blitz::TinyVector<RealType,3>       Real3DType;
        typedef blitz::TinyVector<RealType,2>       SigmoidType;

        typedef blitz::Array<RealType,1>            Array1DType;

        typedef blitz::Array<RealType,2>            ScoreImage;

        typedef std::vector<ScoreImage>             ScaleSpaceType;

        typedef std::vector<IndexType>              ScaleSpaceIndexType;
        typedef std::vector<RealType>               ScaleSpaceRealType;


        SS_ProcessResult(
            const IndexType extent_,
            const RealType threshold_,
            const RealType finalthreshold_,
            const SigmoidType score2prob_,
            const Real3DType sigma_,
            const int transfunc,
            const IndexType stride_, 
            const int winsize_=1);

        ~SS_ProcessResult();

        void newpyramid(
                const IndexType topleft,
                const IndexType extent,
                const RealType scale) ;

        /** Do non-maximum suppression on the detection results */
        virtual void clear();
        virtual void doit();
        virtual bool operator()(const DetectInfo& r);


        virtual std::string toString() { 
            return std::string("Scale-Space NonMax Process Result\n    ")
                + sigmoid->toString();
        }

        protected:
            RealType toscore(
                    const ScaleSpaceType& pyimage,
                    const Real2DType xy, const unsigned scaleindex) const ;
            RealType toscore(
                    const Real2DType xy, const unsigned scaleindex) const 
            {
                return toscore(pyimage,xy,scaleindex);
            }

            // get image x-y coordinates
            Real2DType toxy(
                    const IndexType lb,
                    const unsigned scaleindex) const ;

            const IndexType extent_;
            const RealType threshold_, finalthreshold_;
            const Real3DType sigma_;
            const IndexType stride_;
            const int winsize_;

            const Array1DType smoothkernelX, smoothkernelY, smoothkernelS;

            IndexType topleft;
            ScoreImage* image;
            TransFunc<RealType> *sigmoid;

            ScaleSpaceType pyimage;
            ScaleSpaceRealType pyscale;
            ScaleSpaceIndexType pytopleft, pyextent;
    };
}
#endif // _LEAR_MS_PROCESS_RESULT_H_
