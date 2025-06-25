#pragma once
#include<cstdint>
#include<string>
#include<functional>


// *************************************************** //
// *** Collection of functions for testing purpose *** //
// *************************************************** //
namespace function_test
{
    // Implemented as singleton
    class count
    { 
    public:
        std::uint32_t nullary_function     = 0;
        std::uint32_t nullary_functor      = 0;
        std::uint32_t nullary_member       = 0;
        std::uint32_t nullary_lambda       = 0;
        std::uint32_t nullary_std_function = 0;
        std::uint32_t nullary_std_bind     = 0;
        std::uint32_t nullary_std_ref      = 0;

        std::uint32_t N_ary_function     = 0;
        std::uint32_t N_ary_functor      = 0;
        std::uint32_t N_ary_member       = 0;
        std::uint32_t N_ary_lambda       = 0;
        std::uint32_t N_ary_std_function = 0;
        std::uint32_t N_ary_std_bind     = 0;
        std::uint32_t N_ary_std_ref      = 0;

        std::uint32_t N1_ary_function     = 0;
        std::uint32_t N1_ary_functor      = 0;
        std::uint32_t N1_ary_member       = 0;
        std::uint32_t N1_ary_lambda       = 0;
        std::uint32_t N1_ary_std_function = 0;
        std::uint32_t N1_ary_std_bind     = 0;
        std::uint32_t N1_ary_std_ref      = 0;

    public:
        inline static count& instance()
        {
            static count x;
            return x;
        }

        inline void reset()
        {
            nullary_function     = 0;
            nullary_functor      = 0;
            nullary_member       = 0;
            nullary_lambda       = 0;
            nullary_std_function = 0;
            nullary_std_bind     = 0;
            nullary_std_ref      = 0;

            N_ary_function     = 0;
            N_ary_functor      = 0;
            N_ary_member       = 0;
            N_ary_lambda       = 0;
            N_ary_std_function = 0;
            N_ary_std_bind     = 0;
            N_ary_std_ref      = 0;

            N1_ary_function     = 0;
            N1_ary_functor      = 0;
            N1_ary_member       = 0;
            N1_ary_lambda       = 0;
            N1_ary_std_function = 0;
            N1_ary_std_bind     = 0;
            N1_ary_std_ref      = 0;
        }

    private:        
        count() = default;
       ~count() = default;
        count(const count&) = delete;
        count& operator=(const count&) = delete;
    };
}


namespace function_test
{
    // *************** //
    // *** Nullary *** //
    // *************** //
    inline void nullary_function() 
    { 
        ++count::instance().nullary_function;
    }

    struct nullary_functor
    {
        inline void operator()() const 
        {
            ++count::instance().nullary_functor;
        }

        inline void process() const
        {
            ++count::instance().nullary_member;
        }
    };

    inline std::function<void()> nullary_std_function{[]() // <--- see Remark A : inline variable vs static variable
    {
        ++count::instance().nullary_std_function;
    }}; 


    // ************* //
    // *** N ary *** //
    // ************* //
    inline std::string N_ary_function(int, int) 
    {
        ++count::instance().N_ary_function;
        return "xxx"; 
    }

    struct N_ary_functor
    {
        inline std::string operator()(int, int) const 
        { 
            ++count::instance().N_ary_functor;
            return "yyy"; 
        }

        inline std::string process(int, int) const 
        { 
            ++count::instance().N_ary_member;
            return "zzz"; 
        }
    };

    inline std::function<std::string(int,int)> N_ary_std_function{[](int, int) 
    {
        ++count::instance().N_ary_std_function;
        return "www";
    }}; 


    // ************** //
    // *** N1 ary *** //
    // ************** //
    inline std::string N1_ary_function(int, int, const std::string& str) 
    {
        ++count::instance().N1_ary_function;
        return str; 
    }

    struct N1_ary_functor
    {
        inline std::string operator()(int, int, const std::string& str) const 
        { 
            ++count::instance().N1_ary_functor;
            return str; 
        }

        inline std::string process(int, int, const std::string& str) const 
        { 
            ++count::instance().N1_ary_member;
            return str; 
        }
    };

    inline std::function<std::string(int,int,const std::string&)> N1_ary_std_function{[](int, int, const std::string& str) 
    {
        ++count::instance().N1_ary_std_function;
        return str;
    }}; 
} 


// Remark A : inline variable vs static variable
//
// If we declare variable in header, that is included in multiple cpp, 
// * as "inline", then each translation unit share the same instance    
// * as "static", then each translation unit has its own instance
//
// It is counter-intuitive, as we are 
// * talking about inline variable / static variable here
// * different from inline function / static function.
