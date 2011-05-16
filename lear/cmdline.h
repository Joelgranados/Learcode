// {{{ file documentation
/**
 * @file
 * @brief Cmdline parsing class.
 *
 * Cmdline is Navneet Dalal's interface to boost::program_options package.
 * It hides various program_options commands and present user a even much simpler
 * interface for parsing commands.
 *
 * @todo
 * 1. Check if file exists
 * 2. Put confilicting and depends upon checks
 */
// }}}

#ifndef _LEAR_CMDLINE_H_
#define _LEAR_CMDLINE_H_

// {{{ Headers
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

#include <lear/util/util.h>
#include <lear/option.h>
// }}}

namespace lear {

/// make parameter as namespace lear alias.
// using boost::program_options::parameter;

// make a alias "po" in lear for boost::program_options
namespace po = boost::program_options;

class Cmdline;
/**
 * Provide a wrapper and holder to command line arguments. This way, we
 * can specify command line options and positional parameters in one command
 */
class ArgumentHolder {// {{{
    public:
        ArgumentHolder(const char* title) :
            desc(title)
        {};
        ArgumentHolder& operator()( 
                const char* name, 
                const po::value_semantic* s, 
                const char* description,
                const int count=1)
        {
            desc.add_options()(name,s,description);
            position.add(name,count);
            args.push_back(name);
            return *this;
        }


    private:
    friend class Cmdline;

    /// Options_description for parameters
    po::options_description  desc;

    /// Store parameter's positions
    po::positional_options_description  position;

    std::vector< std::string> args;
};// }}}

// {{{ Cmdline class
// {{{ class documentation
/**
 * Cmdline provide user a command line parser. It supports optional, 
 * positional, required, conflicting and dependent options. Also range 
 * checking for basic types is provided.
 *
 * Internally, Cmdline uses boost::program_options library.
 */
// }}}
class Cmdline {
    public:
        /// status enum. Return various error types.
        enum status { ok=0, info=1, failed, invalid_usage, duplicate_usage, 
            conflicting_usage}; 

        Cmdline(const bool configfileOnCmdline_ = false,
                const std::string configfile_ = "",
                const bool helpOnNoArgs_ = false);

        status parse(int argc, char** argv);

        po::options_description_easy_init addOption() {
            return descConfigfile.add_options();
        }
        po::options_description_easy_init addCmdline() {
            return descCmdline.add_options();
        }
        po::options_description_easy_init addHidden() {
            return descHidden.add_options();
        }
        ArgumentHolder& addArgument() {
            return argument;
        }

        // {{{ set version, description, commandName, usageIssues
        std::string version() const {
            return version_ + "\n" + versionDetails_;
        }

        void version(
                const std::string& v, 
                const std::string& d=std::string() ) 
        {
            version_ = v;
            versionDetails_ = d;
        }

        std::string brief() const {
            return brief_ ;
        }

        void brief(const std::string& b ) {
            brief_ = b;
            warptext(brief_,WARP_WIDTH);
        }
        void appendBrief(const std::string& b ) {
            brief_ += b;
            warptext(brief_,WARP_WIDTH);
        }

        std::string description() const {
            return description_ ;
        }

        void description(const std::string& d ) {
            description_ = d;
            warptext(description_,WARP_WIDTH);
        }

        std::string usageString() const {
            return usageString_ ;
        }

        std::string commandName() const {
            return commandName_ ;
        }

        void commandName(const std::string& c ) {
            commandName_ = c;
        }

        std::string usageIssues() const {
            return usageIssues_ ;
        }

        void usageIssues(const std::string& u ) {
            usageIssues_ = u;
            warptext(usageIssues_,WARP_WIDTH);
        }
        void appendUsageIssues(const std::string& u ) {
            usageIssues_ += u;
            warptext(usageIssues_,WARP_WIDTH);
        }
        // }}}

        void printUsage(std::ostream& o = std::cout) const ;
        void printOptions(std::ostream& o = std::cout) const ;
        void printVersion(std::ostream& o = std::cout) const;
        void printError(const std::string& mesg, std::ostream& o = std::cerr) const;
        void output(std::ostream& os, const po::options_description& d) const; 

//          /// option 'a' conflicts with option 'b'
//          void conflicts(const std::string& a, const std::string& b) {
//              conflict_.push_back(make_pair(a,b));
//          }
//          /// option 'which' depens 'on' option
//          void depends(const std::string& which, const std::string& on) {
//              depend_.push_back(make_pair(which,on));
//          }
    private:
        static const unsigned WARP_WIDTH = 127;
        po::options_description desc, descCmdline, descConfigfile;
        po::options_description  descHidden, descVisual;

        ArgumentHolder argument;

        /// Variable map for options
        po::variables_map vm;

        /// Show usage message if no arguments are provided on command line
        bool helpOnNoArgs;

        std::string configfile;
        bool configfileOnCmdline;

        std::string version_, versionDetails_;
        std::string usageString_, usageIssues_;
        std::string brief_, description_;
        std::string commandName_;
        
//          std::vector<std::pair<std::string, std::string> > conflict_;
//          std::vector<std::pair<std::string, std::string> > depend_;

        void constructUsageString(const std::string& name);

        /* Auxilliary functions for checking input for validity. */

        /* Function used to check that 'opt1' and 'opt2' are not specified
        at the same time. */
        void conflicting_options(const char* opt1, const char* opt2) const;

        /* Function used to check that of 'for_what' is specified, then
        'required_option' is specified too. */
        void option_dependency(
                const char* for_what, 
                const char* required_option) const;
};
// }}}

/// << operator overload for os stream. Print complete usage information
std::ostream& operator<<(std::ostream& os, const Cmdline& desc) ;

} // namespace lear
#endif // _LEAR_CMDLINE_H_


