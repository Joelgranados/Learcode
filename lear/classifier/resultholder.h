#ifndef _LEAR_RESULT_HOLDER_H_
#define _LEAR_RESULT_HOLDER_H_

#include <list>
#include <lear/classifier/detectinfo.h>
#include <lear/classifier/processresult.h>
#include <lear/classifier/ppresult.h>

namespace lear {
class ResultHolder {
    public:
        typedef std::list<ProcessResult*>           ProcessCont;
        typedef std::list<PostProcessResult*>       PostProcessCont;
        typedef std::list<PostProcessCont>          ContCont;

        ResultHolder(){}
        ~ResultHolder();

        void push_back(ProcessResult* toadd) {
            processcont_.push_back(toadd);
            postcont_.push_back(PostProcessCont());
        }
        void push2last(PostProcessResult* pp) {
            ContCont::iterator i = postcont_.end();
            --i;
            i->push_back(pp);
        }
        void push_back(ProcessResult* toadd, PostProcessResult* pp) {
            push_back(toadd) ;
            push2last(pp);
        }
        void print(std::ostream& o) const ;

        /** Prepare contained ProcessResult's for processing a new image.
         * User must invoke this function before processing a new image.
         * Also, user must invoke write or suppress_nonmax to actuall
         * write detection results.
         */
        void clear();
        bool operator()(const DetectInfo& r);
        void write(const std::string filename="");

    protected:
        ProcessCont     processcont_;
        ContCont        postcont_;
};
}
#endif // _LEAR_RESULT_HOLDER_H_
