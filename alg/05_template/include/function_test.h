#pragma once
#include<cstdint>
#include<string>


// *************************************************** //
// *** Collection of functions for testing purpose *** //
// *************************************************** //
namespace function_test
{
    static std::uint32_t nullary_function_count     = 0;
    static std::uint32_t nullary_functor_count      = 0;
    static std::uint32_t nullary_member_count       = 0;
    static std::uint32_t nullary_lambda_count       = 0;
    static std::uint32_t nullary_std_function_count = 0;
    static std::uint32_t nullary_std_bind_count     = 0;
    static std::uint32_t nullary_std_ref_count      = 0;

    static std::uint32_t N_ary_function_count     = 0;
    static std::uint32_t N_ary_functor_count      = 0;
    static std::uint32_t N_ary_member_count       = 0;
    static std::uint32_t N_ary_lambda_count       = 0;
    static std::uint32_t N_ary_std_function_count = 0;
    static std::uint32_t N_ary_std_bind_count     = 0;
    static std::uint32_t N_ary_std_ref_count      = 0;

    static std::uint32_t N1_ary_function_count     = 0;
    static std::uint32_t N1_ary_functor_count      = 0;
    static std::uint32_t N1_ary_member_count       = 0;
    static std::uint32_t N1_ary_lambda_count       = 0;
    static std::uint32_t N1_ary_std_function_count = 0;
    static std::uint32_t N1_ary_std_bind_count     = 0;
    static std::uint32_t N1_ary_std_ref_count      = 0;

    inline void reset_counters()
    {
        nullary_function_count     = 0;
        nullary_functor_count      = 0;
        nullary_member_count       = 0;
        nullary_lambda_count       = 0;
        nullary_std_function_count = 0;
        nullary_std_bind_count     = 0;
        nullary_std_ref_count      = 0;

        N_ary_function_count     = 0;
        N_ary_functor_count      = 0;
        N_ary_member_count       = 0;
        N_ary_lambda_count       = 0;
        N_ary_std_function_count = 0;
        N_ary_std_bind_count     = 0;
        N_ary_std_ref_count      = 0;

        N1_ary_function_count     = 0;
        N1_ary_functor_count      = 0;
        N1_ary_member_count       = 0;
        N1_ary_lambda_count       = 0;
        N1_ary_std_function_count = 0;
        N1_ary_std_bind_count     = 0;
        N1_ary_std_ref_count      = 0;
    }
}


namespace function_test
{
    // *************** //
    // *** Nullary *** //
    // *************** //
    inline void nullary_function() 
    { 
        ++nullary_function_count;
    }

    struct nullary_functor
    {
        inline void operator()() const 
        {
            ++nullary_functor_count;
        }

        inline void process() const
        {
            ++nullary_member_count;
        }
    };


    // ************* //
    // *** N ary *** //
    // ************* //
    inline std::string N_ary_function(int, int) 
    {
        std::cout << "\ndebug : " << N_ary_function_count << std::flush;
        ++N_ary_function_count;
        std::cout << "\ndebug : " << N_ary_function_count << std::flush;
        return "xxx"; 
    }

    struct N_ary_functor
    {
        inline std::string operator()(int, int) const 
        { 
            ++N_ary_functor_count;
            return "yyy"; 
        }

        inline std::string process(int, int) const 
        { 
            ++N_ary_member_count;
            return "zzz"; 
        }
    };


    // ************* //
    // *** N ary *** //
    // ************* //
    inline std::string N1_ary_function(int, int, const std::string& str) 
    {
        ++N1_ary_function_count;
        return str; 
    }

    struct N1_ary_functor
    {
        inline std::string operator()(int, int, const std::string& str) const 
        { 
            ++N1_ary_functor_count;
            return str; 
        }

        inline std::string process(int, int, const std::string& str) const 
        { 
            ++N1_ary_member_count;
            return str; 
        }
    };
}
