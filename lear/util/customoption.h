#ifndef _LEAR_CUSTOM_OPTION_H_
#define _LEAR_CUSTOM_OPTION_H_

#include <vector>
#include <string>

namespace lear {
class CustomOption {
    protected:
        typedef std::vector<unsigned>       OptionNumber;
        typedef std::vector<std::string>    OptionList;

        std::string         mesg_;
        unsigned            default_;
        OptionList          optlst_;
        OptionNumber        optnum_;
        OptionList          optdetail_;
    public:
        std::string option;
        CustomOption(const std::string& mesg, unsigned default_=0) :
            mesg_(mesg), default_(default_)
        {}
        /*
        template<class optiter>
        CustomOption(const std::string& mesg, 
                optiter start, optiter end) :
            mesg_(mesg) 
        {
            for (int i=0; start!=end ; ++start,++i){
                optlst_.push_back(*start);
                optnum_.push_back(i);
            }
        }
        */
        CustomOption& add(const std::string& str, unsigned ref, const std::string& detail=""){
            optlst_.push_back(str); optnum_.push_back(ref); optdetail_.push_back(detail);
            return *this;
        }

        bool validate(const std::string& which) const ;
        int check(const std::string& which) const ;
        int check() const { return check(option); }

        void clear() { optlst_.clear(); optnum_.clear(); optdetail_.clear();}

        std::string usage() const ;
        std::string defaultOption() const ;
};

}

#endif // _LEAR_COMMON_MAIN_H_
