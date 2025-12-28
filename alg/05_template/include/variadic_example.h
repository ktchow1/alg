#pragma once
#include<string>


// ***************** //
// *** Objective *** //
// ***************** //
//
// Write a class string_selector, with ffwk-specialized requirement :
// * takes a variadic number of string   as input in constructor
// * takes a std::function<bool(string)> as input in constructor to select 1st string that returns true
// * has a reference member to point to the string and provide access function
// * has a std::string member called "description" and provide access function
// * description is initialized in member initializer, NOT in constructor body
// * description = "select_from." + s0 + "." + s1 + "." ...
// * constructor input is NOT std::vector<std::string>, as we want to constructor like method 1 (NOT method 2) :
//
//   string_selector sel( "str0", "str1", "str2" );     // method 1 : this is what we want  
//   string_selector sel({"str0", "str1", "str2"});     // method 2 : we do NOT want this
// 
// Remark :
// * This is NOT a ordinary variadic template, as all input types are known, and same as std::string.
//
//

namespace alg
{

    template<typename PREDICATE, typename...STRS>
    requires (std::same_as<STRS, std::string> && ...)
    class string_selector
    {
    public:
        static_assert((std::is_same_v<STRS, std::string> && ...), "all types in STRS should be std::string");

    public: 
        // ******************************************************************* //             
        // Techique 1 : CTAD in to deduce template parameter pack
        // Techique 2 : Fold expression with comma operator acts like for-loop 
        //              that iterates variadic template parameter pack
        // ******************************************************************* //             
        string_selector(PREDICATE& predicate, 
                        STRS& ...  strs) 
                      : m_desc0(generate_description_format0(strs...))
                      , m_desc1(generate_description_format1(strs...))
        {
            bool found = ((is_target_string(predicate, strs)) || ...);
            if (!found)
            {
                throw std::runtime_error("no string fulfill predicate");
            }
        }

    public:
        const std::string& get_description_format0() const
        {
            return m_desc0;
        }

        const std::string& get_description_format1() const
        {
            return m_desc1;
        }

        const std::string& get_string() const
        {
            return *m_selected_ptr;
        }

        std::string& get_string() 
        {
            return *m_selected_ptr;
        }

    private:
        static std::string generate_description_format0(STRS&... strs) // select_one_of.s0.s1.s2.s3
        {
            std::stringstream ss;
            ss << "select_one_of";
            ((ss << "." << strs), ...); 
            return ss.str();
        }
                                                                       //              v--- here is the difference
        static std::string generate_description_format1(STRS&... strs) // select_one_of:s0.s1.s2.s3
        {
            bool first = true;

            std::stringstream ss;
            ss << "select_one_of";
            ((ss << (first? ":":".") << strs, first = false), ...); // by including one more update inside comma operator
            return ss.str();
        }

        bool is_target_string(PREDICATE& predicate, std::string& str)
        {
            if (predicate(str))
            {
                m_selected_ptr = &str;
                return true;
            }
            return false;
        }

    private:
        std::string  m_desc0;
        std::string  m_desc1;
        std::string* m_selected_ptr = nullptr;
    };

}
