// {{{ file documentation
/**
 * @file
 * @brief Command line option
 *
 * Defines TypedOption class. TypedOption supports min/max/default value.
 */
// }}}

#ifndef _LEAR_OPTION_H_
#define _LEAR_OPTION_H_

// {{{ Headers
#include <string>
#include <vector>
#include <algorithm>

#include <boost/any.hpp>
#include <boost/program_options.hpp>

#include <lear/vectoropt.h>
// }}}

namespace lear {

/// make parameter as namespace lear alias.
// using boost::program_options::parameter;

// make a alias "po" in lear for boost::program_options
namespace po = boost::program_options;

// {{{ TypedOption class
/**
 * Overload typed_value to provide some additional functionalities for options.
 */
template<class EleT, class CharT = char>
class TypedOption : public po::typed_value<EleT,CharT> {
    public:
        TypedOption(EleT* store_to) 
            :
            po::typed_value<EleT,CharT>(store_to)
        {} 
 
        void notify(const boost::any& value_store) const {// {{{
            po::typed_value<EleT,CharT>::notify(value_store);

            // now check if all values are in range
            // check min value
            if (!minValue_.empty()) {
                const EleT* value = boost::any_cast<const EleT>(&value_store);
                const EleT min = minValue();

                if (*value < min) {
                    std::ostringstream ost;
                    ost << "value " << *value 
                        << " less than min value " << min;
                    throw po::validation_error(ost.str());
                }
            }

            // check max value
            if (!maxValue_.empty()) {
                const EleT* value = boost::any_cast<const EleT>(&value_store);
                const EleT max = maxValue();

                if (*value > max) {
                    std::ostringstream ost;
                    ost << "value " << *value 
                        << " greater than max value " << max;
                    throw po::validation_error(ost.str());
                }
            }
        }// }}}

        /// Get the minimum value that this parameter can hold
        EleT minValue() const {
            const EleT* value = boost::any_cast<const EleT>(&minValue_);
            return *value;
        }
        /// Get the maximum value that this parameter can hold
        EleT maxValue() const {
            const EleT* value = boost::any_cast<const EleT>(&maxValue_);
            return *value;
        }

        EleT defaultValue() const {
            const EleT* value = boost::any_cast<const EleT>(&defaultValue_);
            return *value;
        }

        /// Set the minimum value that this parameter can hold
        TypedOption* minValue(const EleT& min) {
            minValue_ = boost::any(min);
            minValue_as_text = boost::lexical_cast<std::string>(min);
            return this;
        }
        TypedOption* minValue(const EleT& min, const std::string& as_text) {
            minValue_ = boost::any(min);
            minValue_as_text = as_text;
            return this;
        }
        /// Set the maximum value that this parameter can hold
        TypedOption* maxValue(const EleT& max) {
            maxValue_ = boost::any(max);
            maxValue_as_text = boost::lexical_cast<std::string>(max);
            return this;
        }
        TypedOption* maxValue(const EleT& max, const std::string& as_text) {
            maxValue_ = boost::any(max);
            maxValue_as_text = as_text;
            return this;
        }
        /// Provided here just for sake of uniform names of functions
        /// Essentially call program_options default_value function
        TypedOption* defaultValue(const EleT& val) {
            defaultValue_ = boost::any(val);
            defaultValue_as_text = boost::lexical_cast<std::string>(val);
            po::typed_value<EleT,CharT>::default_value(val);
            return this;
        }
        TypedOption* defaultValue(const EleT& val, const std::string& as_text) {
            defaultValue_ = boost::any(val);
            defaultValue_as_text = as_text;
            po::typed_value<EleT,CharT>::default_value(val, as_text);
            return this;
        }

        TypedOption* dependsOn(const std::string& option) {
            dependsOn_.push_back(option);
            return this;
        }

        TypedOption* dependsOn(const std::vector<std::string>& option) {
           dependsOn_.insert(dependsOn_.end(), 
                   option.begin(),option.end());
            return this;
        }

        TypedOption* conflictsWith(const std::string& option) {
            conflictsWith_.push_back(option);
            return this;
        }

        TypedOption* conflictsWith(
                const std::vector<std::string>& option) 
        {
            conflictsWith_.insert(conflictsWith_.end(),
                    option.begin(),option.end());
            return this;
        }

        std::string name() const {
            std::ostringstream os;
            if (!defaultValue_.empty() && !defaultValue_as_text.empty()) {
                os << " (Default: " << defaultValue_as_text << ")";
            } 
            if (!minValue_.empty() && !maxValue_.empty()) {
                os << " (Min: " << minValue_as_text 
                   << ", Max: " << maxValue_as_text << ")";
            } else if (!minValue_.empty()) {
                os << " (Min: " << minValue_as_text << ")";
            } else if (!maxValue_.empty()) {
                os << " (Max: " << maxValue_as_text << ")";
            }
            return os.str();
        }

    private:
        boost::any minValue_, maxValue_, defaultValue_;
        std::string minValue_as_text, maxValue_as_text, defaultValue_as_text;

        std::vector<std::string> dependsOn_;
        std::vector<std::string> conflictsWith_;
}; 
// }}}

// {{{ TypedOption<VectorOpt> class
/**
 * Overload typed_value to provide functionalities for vector options.
 */
template<class EleT, class CharT>
class TypedOption< VectorOpt<EleT>, CharT > : 
            public po::typed_value< VectorOpt<EleT>,CharT> 
{
    public:
        typedef VectorOpt<EleT>       ConT;

        TypedOption(ConT* store_to) 
            :
            po::typed_value<ConT,CharT>(store_to)
        {} 
 
        void notify(const boost::any& value_store) const {// {{{
            po::typed_value<ConT,CharT>::notify(value_store);

            const ConT* cont = boost::any_cast<const ConT>(&value_store);

            // now check if all values are in range

            // check min value
            if (!minValue_.empty()) {
                const EleT min = minValue();

                const EleT value=*std::min_element(cont->begin(),cont->end());
                if (value < min) {
                    std::ostringstream ost;
                    ost << "VectorOpt<value> " << value
                        << " less than min value " << min;
                    throw po::validation_error(ost.str());
                }
            }

            // check max value
            if (!maxValue_.empty()) {
                const EleT max = maxValue();

                const EleT value=*std::max_element(cont->begin(),cont->end());

                if (value > max) {
                    std::ostringstream ost;
                    ost << "VectorOpt<value> " << value 
                        << " greater than max value " << max;
                    throw po::validation_error(ost.str());
                }
            }
        }// }}}

        EleT defaultValue() const {
            const EleT* value = boost::any_cast<const EleT>(&defaultValue_);
            return *value;
        }

        /// Get the minimum value that this parameter can hold
        EleT minValue() const {
            const EleT* value = boost::any_cast<const EleT>(&minValue_);
            return *value;
        }
        /// Get the maximum value that this parameter can hold
        EleT maxValue() const {
            const EleT* value = boost::any_cast<const EleT>(&maxValue_);
            return *value;
        }

        /// Provided here just for sake of uniform names of functions
        /// Essentially call program_options default_value function
        TypedOption* defaultValue(const EleT& val) {
            defaultValue_ = boost::any(val);
            defaultValue_as_text = boost::lexical_cast<std::string>(val);
            po::typed_value<ConT,CharT>::default_value(val);
            return this;
        }
        TypedOption* defaultValue(const EleT& val, const std::string& as_text) {
            defaultValue_ = boost::any(val);
            defaultValue_as_text = as_text;
            po::typed_value<ConT,CharT>::default_value(val, as_text);
            return this;
        }

        /// Set the minimum value that this parameter can hold
        TypedOption* minValue(const EleT& min) {
            minValue_ = boost::any(min);
            minValue_as_text = boost::lexical_cast<std::string>(min);
            return this;
        }
        TypedOption* minValue(const EleT& min, const std::string& as_text) {
            minValue_ = boost::any(min);
            minValue_as_text = as_text;
            return this;
        }
        /// Set the maximum value that this parameter can hold
        TypedOption* maxValue(const EleT& max) {
            maxValue_ = boost::any(max);
            maxValue_as_text = boost::lexical_cast<std::string>(max);
            return this;
        }
        TypedOption* maxValue(const EleT& max,const std::string& as_text) {
            maxValue_ = boost::any(max);
            maxValue_as_text = as_text;
            return this;
        }

        TypedOption* dependsOn(const std::string& option) {
            dependsOn_.push_back(option);
            return this;
        }

        TypedOption* dependsOn(const std::vector<std::string>& option) {
           dependsOn_.insert(dependsOn_.end(), 
                   option.begin(),option.end());
            return this;
        }

        TypedOption* conflictsWith(const std::string& option) {
            conflictsWith_.push_back(option);
            return this;
        }

        TypedOption* conflictsWith(
                const std::vector<std::string>& option) 
        {
            conflictsWith_.insert(conflictsWith_.end(),
                    option.begin(),option.end());
            return this;
        }

        std::string name() const {
            std::ostringstream os;
            if (!defaultValue_.empty() && !defaultValue_as_text.empty()) {
                os << " (Default: " << defaultValue_as_text << ")";
            } 
            if (!minValue_.empty() && !maxValue_.empty()) {
                os << " (Min: " << minValue_as_text 
                   << ", Max: " << maxValue_as_text << ")";
            } else if (!minValue_.empty()) {
                os << " (Min: " << minValue_as_text << ")";
            } else if (!maxValue_.empty()) {
                os << " (Max: " << maxValue_as_text << ")";
            }
            return os.str();
        }

    private:
        boost::any minValue_, maxValue_, defaultValue_;
        std::string minValue_as_text, maxValue_as_text, defaultValue_as_text;

        std::vector<std::string> dependsOn_;
        std::vector<std::string> conflictsWith_;
}; 
// }}}

// {{{ Option function
/** Creates a typed_value<T> instance. This function is the primary
    method to create value_semantic instance for a specific type, which
    can later be passed to 'option_description' constructor.
    The second overload is used when it's additionally desired to store the 
    value of option into program variable.
*/
template<class T> TypedOption<T>* option() {
    // Explicit qualification is vc6 workaround.
    return option<T>(0);
}

/** @overload 
*/
template<class T> TypedOption<T>* option(T* v) {
    TypedOption<T>* r = new TypedOption<T>(v);
    return r;        
}

template<class T> TypedOption<T, wchar_t>* woption() {
    return woption<T>(0);
}

template<class T> TypedOption<T, wchar_t>* woption(T* v) {
    TypedOption<T, wchar_t>* r = new TypedOption<T, wchar_t>(v);
    return r;        
}
TypedOption<bool>* bool_option();

TypedOption<bool>* bool_option(bool* v);
// }}}

} // namespace lear
#endif // _LEAR_OPTION_H_
