#include <lear/classifier/resultholder.h>
#include <iostream>


void lear::ResultHolder::clear(){
    for (ProcessCont::iterator i=processcont_.begin();
            i != processcont_.end(); ++i) 
    {
        (**i).clear();
    }
}
bool lear::ResultHolder::operator()(const DetectInfo& r){
    for (ProcessCont::iterator i=processcont_.begin();
            i != processcont_.end(); ++i) 
    {
        (**i)(r);
    }
    return true;
}

void lear::ResultHolder::print(std::ostream& o) const {
    o << "Operations on results :: " << std::endl;
    ContCont::const_iterator pp=postcont_.begin();
    for (ProcessCont::const_iterator i=processcont_.begin();
            i != processcont_.end(); ++i,++pp) 
    {
        o << "Processor " << (*i)->toString() << std::endl;
        for (PostProcessCont::const_iterator j = pp->begin(); 
                j != pp->end(); ++j) 
        {
            o << "  PostProcessor: " << (*j)->toString() << std::endl;
        }
    }
}
void lear::ResultHolder::write(const std::string filename)
{

    ContCont::iterator pp=postcont_.begin();
    for (ProcessCont::iterator i=processcont_.begin();
            i != processcont_.end(); ++i,++pp) 
    {
        (**i).doit();
//std::cout << "processing " << (*i)->toString() << std::endl;
        for (PostProcessCont::iterator j = pp->begin(); 
                j != pp->end(); ++j) 
        {
//std::cout << "post processing " << (*j)->toString() << std::endl;
            (**j).write((**i).begin(),(**i).end(),filename);
        }
    }
}

lear::ResultHolder::~ResultHolder(){
    ContCont::iterator pp=postcont_.begin();
    for (ProcessCont::iterator i=processcont_.begin();
            i != processcont_.end(); ++i,++pp) 
    {
        for (PostProcessCont::iterator j = pp->begin(); 
                j != pp->end(); ++j) 
            delete *j;
        delete *i;
    }
}

