#include <iostream> 
#include <fstream> 
#include <sstream>
#include <lear/cmdline.h>

using namespace lear;
using namespace std;
using namespace boost::program_options;
// {{{ constructor
lear::Cmdline::Cmdline(
        const bool configfileOnCmdline_,
        string configfile_,
        const bool helpOnNoArgs_):  
    desc("Allowed options"),
    descCmdline("Command line options only"),
    descConfigfile("Options common to config file and command line"),
    argument("Required Arguments"),

    helpOnNoArgs (helpOnNoArgs_),

    configfile (configfile_),
    configfileOnCmdline (configfileOnCmdline_)
{
    descCmdline.add_options()
        ("help,h", "show help screen")
        ("version", "print version number and contact details")
        ;

    if (configfileOnCmdline) {
        descCmdline.add_options()
            ("configfile",option<string>(&configfile)
                ->defaultValue(configfile.c_str()), 
                "options configuration file name")
            ;
    }
}
//}}}

// {{{ parse function
lear::Cmdline::status lear::Cmdline::parse(int argc, char** argv){
    if (commandName_.empty())
        commandName_ = argv[0];
    constructUsageString(argv[0]);

    desc.add(descConfigfile)
        .add(descCmdline)
        .add(descHidden)
        .add(argument.desc);

    descVisual
        .add(descConfigfile)
        .add(descCmdline)
        .add(argument.desc);

    if (helpOnNoArgs && argc ==1) {
        printUsage();
        return failed;
    }

    try {
        store(command_line_parser(argc, argv)
                .options(desc).positional(argument.position).run(), vm);

        string cfg = configfile;
        if (configfileOnCmdline && vm.count("configfile")) {
            cfg = vm["configfile"].as<string>();
        }
    
        if (!cfg.empty()) {
            ifstream from(cfg.c_str());
            if (from) {
                options_description tmpdesc;
                tmpdesc.add(descConfigfile).add(descHidden);
                // Parse config file and store results. Note
                // that different set of allowed options is used.
                store(parse_config_file(from, tmpdesc), vm);
            } 
//              else {
                // unable to open config file. Show user a message.
//                  cerr << "Warning: Unable to open configuration file: "
//                            << configfile << endl;
//              }
        }
        // notify results
        notify(vm);


        if (vm.count("help")) {
            printUsage();
            return info;
        }

        if (vm.count("version")) {
            printVersion();
            return info;
        }

        // flag error for missing arguments
        for (vector<string>::const_iterator ai = argument.args.begin();
                ai != argument.args.end(); ++ai) 
        {
            if (vm.count(*ai)< 1) {
                throw validation_error(string("missing required argument ") 
                        + argument.desc.find(*ai, false).format_name());
            }
        }
    } catch (validation_error& e) {
#ifdef __MY_BOOST__
        string mesg (e.what());
#else
        string mesg ("during validation");
#endif
        mesg.append("\n\tplease check values and bounds of all options");
        printError(mesg);
        printOptions(cerr);
        return failed;
    } catch (error& e) {
        printError(e.what());
        printOptions(cerr);
        return failed;
    }
    return ok;
}
// }}}

// {{{ constructUsageString
void lear::Cmdline::constructUsageString(const string& name) {
    usageString_ = "Usage: ";
    usageString_.append(name).append(" [options] ");

    for (vector<string>::const_iterator ai = argument.args.begin();
            ai != argument.args.end(); ++ai) 
    {
        usageString_.append(" ").append(*ai).append(" ");
    }
    usageString_.append("\n");
    warptext(usageString_, WARP_WIDTH);
}
// }}}

// {{{ print functions
void lear::Cmdline::printUsage(ostream& o) const {
    o << commandName_ << '\n';
    const int length = commandName_.length();

    for (int i= 0; i< length; ++i) 
        o << "=";
    o << '\n';

    if (!brief_.empty() ) 
        o << "\t" << brief_ << "\n\n";
    else 
        o << '\n';

    if (!description_.empty() ) 
        o << "Description: " << "\n" << description_ << "\n\n";

    printOptions(o);
}

void lear::Cmdline::printOptions(ostream& o) const {
    if (!usageString_.empty()) {
        o << usageString_;
    }

    output(o,descVisual);
    if (!usageIssues_.empty() ) {
        o << "\nUsage Issues:\n" << usageIssues_ << '\n';
    }
    if (!configfile.empty()) {
        string s= 
"\nThis command also accepts configuration file. Default configuration file "
"name is '" + configfile + "'.\n" + "First, all options on command line are "
"read, then all options present in configuration file are read. If there are "
"still some undefined options, the program substitutes their default values.";
        if (configfileOnCmdline) {
            s.append("You can also specify configuration\nfile name on "
                    "command line using 'configfile' option.\n");
        }
        o << warptext(s,WARP_WIDTH);
    }

}

void lear::Cmdline::printVersion(ostream& o) const {
    o << version() << '\n';
}

void lear::Cmdline::printError(const string& mesg, ostream& o) const {
    o << "Error "  << mesg << "\n\n";
}

//  namespace {// {{{
//      void format_one(
//              std::ostream& os,
//              const option_description& opt, 
//              unsigned first_column_width) 
//      {
//          std::ostringstream ss;
//          ss << "  " << opt.format_name() << ' ' << opt.format_parameter();
//      
//          os << ss.str();

//          if (!opt.description().empty()) {
//              for(int pad = first_column_width - ss.str().size(); pad > 0; --pad) {
//                  os.put(' ');
//              }
//              os << " : " << opt.description();
//          }
//  //          boost::shared_ptr<TypedOption> sem = 
//  //              dynamic_pointer_cast<const TypedOption> (opt.semantic());

//  //          if (!sem.defaultValue().empty()) 
//  //              os << " ( Default: " << opt.default_value() << " )";
//  //          

//  //          if (optdesc) {
//  //              if (!optdesc->minValue().empty() && !optdesc->maxValue().empty()) 
//  //                  os << " ( Min: " << optdesc->minValue() 
//  //                     << ", Max: " << optdesc->maxValue() << " )";
//  //              else if (!optdesc->minValue().empty()) 
//  //                  os << " ( Min: " << optdesc->minValue() << " )";
//  //              else if (!optdesc->maxValue().empty()) 
//  //                  os << " ( Max: " << optdesc->maxValue() << " )";

//  //              if (optdesc->isRequired() && optdesc->isInternal() ) {
//  //                  os << " ( Required, Internal ) " ;
//  //              } else if (optdesc->isRequired() ) {
//  //                  os << " ( Required ) " ;
//  //              } else if (optdesc->isInternal() ) {
//  //                  os << " ( Internal ) " ;
//  //              }
//  //          }
//      }
//  }// }}}

/*
    void format_one(std::ostream& os, const option_description& opt, 
                    unsigned first_column_width, unsigned line_length)
    {
        stringstream ss;
        ss << "  " << opt.format_name();
        
        // Don't use ss.rdbuf() since g++ 2.96 is buggy on it.
        os << ss.str();

        if (!opt.description().empty()) {
            for(unsigned pad = first_column_width - ss.str().size(); 
                pad > 0; 
                --pad)
            {
                os.put(' ');
            }
        
            format_description(os, opt.description(),
                                first_column_width, line_length);
        } 
        string format_parameter = opt.format_parameter();
        if (format_parameter()) {
            os << "\n";
            for(unsigned pad = first_column_width; pad > 0; --pad) {
                os.put(' ');
            }
            os << ' ' << format_parameter;
        }
    }

    void options_description::print(std::ostream& os) const
    {
        if (!m_caption.empty())
            os << m_caption << ":\n";

        // Find the maximum width of the option column //
        unsigned width(16);
        unsigned i; // vc6 has broken for loop scoping
        for (i = 0; i < m_options.size(); ++i) {
            const option_description& opt = *m_options[i];
            stringstream ss;
            ss << "  " << opt.format_name() ;//<< ' ' << opt.format_parameter();
            width = (max)(width, static_cast<unsigned>(ss.str().size()));            
        }
        
        // add an additional space to improve readability //
        ++width;
            
        // The options formatting style is stolen from Subversion. //
        for (i = 0; i < m_options.size(); ++i) {
            if (belong_to_group[i])
                continue;

            const option_description& opt = *m_options[i];

            format_one(os, opt, width, m_line_length);

            os << "\n";
        }

        for (unsigned j = 0; j < groups.size(); ++j) {            
            os << "\n" << *groups[j];
        }
    }

 * */


void lear::Cmdline::output(ostream& os, const options_description& d) const {
    // for time being, call desc output.
    d.print(os);
//      if (!d.caption().empty())
//          os << d.caption() << ":\n";

//      set<string> key = d.primary_keys();
//      unsigned width(24);

//      /* Find the maximum width of the option column */
//      for (set<string>::const_iterator ki=key.begin(); ki != key.end(); ++ki) {
//          const option_description& opt = d.find(*ki);

//          stringstream ss;
//          ss << "  " << opt.format_name() << ' ' << opt.format_parameter();
//          width = max(width, static_cast<unsigned>(ss.str().size()));            
//      }

//      for (set<string>::const_iterator ki=key.begin(); ki != key.end(); ++ki) {
//          const option_description& opt = d.find(*ki);

//          format_one(os, opt, width);

//          os << "\n";
//      }
}

// }}}

/* Function used to check that 'opt1' and 'opt2' are not specified
at the same time. */
void Cmdline::conflicting_options(const char* opt1, const char* opt2) const {
    if (vm.count(opt1) && !vm[opt1].defaulted() && 
        vm.count(opt2) && !vm[opt2].defaulted())
            throw logic_error(string("conflicting options: '") 
                        + opt1 + "' and '" + opt2 + "'.");
}

/* Function used to check that of 'for_what' is specified, then
'required_option' is specified too. */
void Cmdline::option_dependency(
        const char* for_what, 
        const char* required_option) const
{
    if (vm.count(for_what) && !vm[for_what].defaulted())
        if (vm.count(required_option) == 0 || 
            vm[required_option].defaulted())
                throw logic_error(string("Option '") + for_what 
                        + "' requires option '" + required_option + "'.");
}

ostream& lear::operator<<(ostream& os, const lear::Cmdline& desc) {
    desc.printUsage(os);
    return os;
}

lear::TypedOption<bool>* lear::bool_option() {
    return bool_option(false);
}
lear::TypedOption<bool>* lear::bool_option(bool* v) {
    TypedOption<bool>* r = new TypedOption<bool>(v);
    r->defaultValue(false);
    //r->implicit();

    return r;
}

