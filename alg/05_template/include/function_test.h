#pragma once
#include<cstdint>
#include<string>


// *************************************************** //
// *** Collection of functions for testing purpose *** //
// *************************************************** //
namespace function_test
{
    // Implemented as singleton
    class count
    { 
    public:
        std::uint32_t nullary_function_count     = 0;
        std::uint32_t nullary_functor_count      = 0;
        std::uint32_t nullary_member_count       = 0;
        std::uint32_t nullary_lambda_count       = 0;
        std::uint32_t nullary_std_function_count = 0;
        std::uint32_t nullary_std_bind_count     = 0;
        std::uint32_t nullary_std_ref_count      = 0;

        std::uint32_t N_ary_function_count     = 0;
        std::uint32_t N_ary_functor_count      = 0;
        std::uint32_t N_ary_member_count       = 0;
        std::uint32_t N_ary_lambda_count       = 0;
        std::uint32_t N_ary_std_function_count = 0;
        std::uint32_t N_ary_std_bind_count     = 0;
        std::uint32_t N_ary_std_ref_count      = 0;

        std::uint32_t N1_ary_function_count     = 0;
        std::uint32_t N1_ary_functor_count      = 0;
        std::uint32_t N1_ary_member_count       = 0;
        std::uint32_t N1_ary_lambda_count       = 0;
        std::uint32_t N1_ary_std_function_count = 0;
        std::uint32_t N1_ary_std_bind_count     = 0;
        std::uint32_t N1_ary_std_ref_count      = 0;

    public:
        inline static count& instance()
        {
            static count x;
            return x;
        }

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
        ++count::instance().nullary_function_count;
    }

    struct nullary_functor
    {
        inline void operator()() const 
        {
            ++count::instance().nullary_functor_count;
        }

        inline void process() const
        {
            ++count::instance().nullary_member_count;
        }
    };


    // ************* //
    // *** N ary *** //
    // ************* //
    inline std::string N_ary_function(int, int) 
    {
        ++count::instance().N_ary_function_count;
        return "xxx"; 
    }

    struct N_ary_functor
    {
        inline std::string operator()(int, int) const 
        { 
            ++count::instance().N_ary_functor_count;
            return "yyy"; 
        }

        inline std::string process(int, int) const 
        { 
            ++count::instance().N_ary_member_count;
            return "zzz"; 
        }
    };


    // ************* //
    // *** N ary *** //
    // ************* //
    inline std::string N1_ary_function(int, int, const std::string& str) 
    {
        ++count::instance().N1_ary_function_count;
        return str; 
    }

    struct N1_ary_functor
    {
        inline std::string operator()(int, int, const std::string& str) const 
        { 
            ++count::instance().N1_ary_functor_count;
            return str; 
        }

        inline std::string process(int, int, const std::string& str) const 
        { 
            ++count::instance().N1_ary_member_count;
            return str; 
        }
    };
}
