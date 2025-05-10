#include<iostream>
#include<cstdint>
#include<cassert>
#include<array>
#include<utility.h>


// ********************************************* //
// *** Compile time processing of std::array *** //
// ********************************************* //
namespace alg
{
    template<typename T, std::uint64_t N>
    constexpr std::array<T,N> get_max(const std::array<T,N>& array0,
                                      const std::array<T,N>& array1)
    {
        std::array<T,N> ans{};
        for(std::uint64_t n=0; n!=N; ++n)
        {
            ans[n] = (array0[n] > array1[n]? array0[n] : array1[n]);
        }
        return ans;
    }

    template<typename T, std::uint64_t N>
    constexpr std::array<T,N> get_max2(const std::array<T,N>& array0, 
                                       const std::array<T,N>& array1) // This implementation also works.
    {
        std::array<T,N> ans{};
        for(std::uint64_t n=0; n!=N; ++n)
        {
            if (array0[n] > array1[n]) // <--- no need to use "if constexpr", this is for template specialization 
            {
                ans[n] = array0[n];
            }
            else
            {
                ans[n] = array1[n];
            }
        }
        return ans;
    }
}

namespace alg
{
    template<typename T, std::uint64_t N0, std::uint64_t N1>
    constexpr std::array<T,N0+N1> merge(const std::array<T,N0>& array0,
                                        const std::array<T,N1>& array1)
    {
        std::array<T,N0+N1> ans{};
        for(std::uint64_t n=0; n!=N0; ++n)
        {
            ans[n] = array0[n];
        }
        for(std::uint64_t n=0; n!=N1; ++n)
        {
            ans[N0+n] = array1[n];
        }
        return ans;
    }
}


void test_constexpr_array() 
{
    constexpr std::array<std::uint32_t,10> a0{10,21,22,13,14,15,26,27,18,29}; //  inputs are declared as constexpr to trigger compile-time calculation
    constexpr std::array<std::uint32_t,10> a1{20,11,12,23,24,25,16,17,28,19}; //  inputs are declared as constexpr to trigger compile-time calculation

    constexpr auto a2 = alg::get_max(a0,a1);                                  // outputs are declared as constexpr to trigger compile-time calculation
    static_assert(a2[0] == 20, "incorrect a2[0]");
    static_assert(a2[1] == 21, "incorrect a2[1]");
    static_assert(a2[2] == 22, "incorrect a2[2]");
    static_assert(a2[3] == 23, "incorrect a2[3]");
    static_assert(a2[4] == 24, "incorrect a2[4]");
    static_assert(a2[5] == 25, "incorrect a2[5]");
    static_assert(a2[6] == 26, "incorrect a2[6]");
    static_assert(a2[7] == 27, "incorrect a2[7]");
    static_assert(a2[8] == 28, "incorrect a2[8]");
    static_assert(a2[9] == 29, "incorrect a2[9]");

    constexpr auto a3 = alg::get_max2(a0,a1); 
    static_assert(a3[0] == 20, "incorrect a3[0]");
    static_assert(a3[1] == 21, "incorrect a3[1]");
    static_assert(a3[2] == 22, "incorrect a3[2]");
    static_assert(a3[3] == 23, "incorrect a3[3]");
    static_assert(a3[4] == 24, "incorrect a3[4]");
    static_assert(a3[5] == 25, "incorrect a3[5]");
    static_assert(a3[6] == 26, "incorrect a3[6]");
    static_assert(a3[7] == 27, "incorrect a3[7]");
    static_assert(a3[8] == 28, "incorrect a3[8]");
    static_assert(a3[9] == 29, "incorrect a3[9]");

    constexpr std::array<std::uint32_t,5> a4{10,11,12,13,14}; 
    constexpr std::array<std::uint32_t,3> a5{20,21,22};

    constexpr auto a6 = alg::merge(a4,a5);       
    static_assert(a6[0] == 10, "incorrect a6[0]");
    static_assert(a6[1] == 11, "incorrect a6[0]");
    static_assert(a6[2] == 12, "incorrect a6[0]");
    static_assert(a6[3] == 13, "incorrect a6[0]");
    static_assert(a6[4] == 14, "incorrect a6[0]");
    static_assert(a6[5] == 20, "incorrect a6[0]");
    static_assert(a6[6] == 21, "incorrect a6[0]");
    static_assert(a6[7] == 22, "incorrect a6[0]");

    print_summary("constexpr std::array", "succeeded");
}


