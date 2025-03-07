#pragma once
#include<cstdint>
#include<vector>

namespace alg 
{
    template<std::uint32_t N, std::uint32_t M>
    consteval bool less_than() 
    {
        return N<M;
    }

    template<std::uint32_t N, std::uint32_t M>
    consteval std::uint32_t abs_diff() 
    {
        if (less_than<N,M>()) return M-N; // if consteval is optional here ...
        else return N-M;
    }

    template<std::uint32_t N, std::uint32_t M, typename std::enable_if<less_than<N,M>(), int>::type dummy = 0>
    consteval std::uint32_t abs_diff2()
    {
        return M-N;
    }

    template<std::uint32_t N, std::uint32_t M, typename std::enable_if<!less_than<N,M>(), int>::type dummy = 0>
    consteval std::uint32_t abs_diff2()
    {
        return N-M;
    };
}


// ***************************** //
// *** Factorial & Fibonacci *** //
// ***************************** //
namespace alg
{
    template<int N> struct factorial    : std::integral_constant<int, N * factorial<N - 1>::value> {};
    template<>      struct factorial<1> : std::integral_constant<int, 1> {};

    template<int N> struct fibonacci    : std::integral_constant<int, fibonacci<N - 1>::value + fibonacci<N - 2>::value> {};
    template<>      struct fibonacci<1> : std::integral_constant<int, 1> {};
    template<>      struct fibonacci<2> : std::integral_constant<int, 1> {};
}


// ************************ //
// *** Min/Max function *** //
// ************************ //
// BUG : using signed integer so that it supports negative profit in max-profit-problem
//
namespace alg 
{
    template<std::int32_t N, std::int32_t M>
    consteval std::int32_t min() 
    {
        if (N<M) return N;
        else return M;
    }

    template<std::int32_t N, std::int32_t M>
    consteval std::int32_t max() 
    {
        if (N>=M) return N;
        else return M;
    }
}


// ******************************* //
// *** Min/Max variadic traits *** //
// ******************************* //
// BUG : using signed integer so that it supports negative profit in max-profit-problem
//
namespace alg 
{
    template<std::int32_t N, std::int32_t...Ns>
    struct min_item                                            // <--- This is recursion.
    {
        static const std::int32_t value = min_item<N, min_item<Ns...>::value>::value;
    };

    template<std::int32_t N, std::int32_t M> 
    struct min_item<N,M>                                       // <--- This is boundary case, delegates to a consteval function.
    {
        static const std::int32_t value = min<N, M>();
    };

    template<std::int32_t N> 
    struct min_item<N>                                         // <--- This is boundary case.
    {
        static const std::int32_t value = N;
    };

    template<std::int32_t N, std::int32_t...Ns>
    struct max_item                                            // <--- This is recursion.
    {
        static const std::int32_t value = max_item<N, max_item<Ns...>::value>::value;
    };

    template<std::int32_t N, std::int32_t M>
    struct max_item<N,M>                                       // <--- This is boundary case, delegates to a consteval function.
    {
        static const std::int32_t value = max<N, M>();
    };

    template<std::int32_t N>
    struct max_item<N>                                         // <--- This is boundary case.
    {
        static const std::int32_t value = N;
    };
}


// ************************** //
// *** Max profit problem *** //
// ************************** //
// Remark 1 : With template metaprogramming, we have to use recursive approach (no iterative), hence duplicated calculation.
// Remark 2 : With parameter pack unpacked at the beginning, subproblem is defined as "buy at first timestep".
//
// Computation of iteration is O(N)
// Computation of recursion is O(N^2)
//
// problem(N) -> problem(N-1) -> ...
//     |             |
//     v             v
// buy@begin(N)  buy@begin(N-1)
//
namespace alg 
{
    template<std::int32_t N0, std::int32_t N1, std::int32_t...Ns>
    struct max_profit_buy_at_begin                             // <--- This is recursion of modified problem.
    {
        static const std::int32_t value = max_item
        <
            -N0+N1,                                            // case 1 : sequence ends here 
            -N0+N1 + max_profit_buy_at_begin<N1,Ns...>::value  // case 2 : sequence continues
        >
        ::value;
    };

    template<std::int32_t N0, std::int32_t N1>
    struct max_profit_buy_at_begin<N0,N1>                      // <--- This is boundary case of modified problem.
    {
        static const std::int32_t value = -N0+N1;
    };

    template<std::int32_t N, std::int32_t...Ns>
    struct max_profit                                          // <--- This is recursion.
    {
        static const std::int32_t value = max_item
        <
            max_profit<Ns...>::value, 
            max_profit_buy_at_begin<N,Ns...>::value
        >
        ::value;
    };

    template<std::int32_t N0, std::int32_t N1>
    struct max_profit<N0,N1>                                   // <--- This is boundary case.
    {
        static const std::int32_t value = -N0+N1;
    };

    // Runtime iterative forward 
    // subproblem defined as : sell at last timestep
    double max_profit_runtime_iterative_forward(const std::vector<double>& xs)
    {
        if (xs.size()<=1) return 0;

        double sub = xs[1]-xs[0]; 
        double ans = sub;
        for(int n=2; n!=xs.size(); ++n)
        {
            sub = std::max(sub-xs[n-1]+xs[n], -xs[n-1]+xs[n]);
            ans = std::max(ans,sub);    
        }
        return ans;
    }

    // Runtime iterative backward
    // subproblem defined as : buy at first timestep
    double max_profit_runtime_iterative_backward(const std::vector<double>& xs)
    {
        if (xs.size()<=1) return 0;

        double sub = xs[xs.size()-1]-xs[xs.size()-2]; 
        double ans = sub;
        for(int n=(int)(xs.size())-2; n!=-1; --n)
        {
            sub = std::max(-xs[n]+xs[n+1]+sub, -xs[n]+xs[n+1]);
            ans = std::max(ans,sub);    
        }
        return ans;
    }
}


// ********************** //
// *** Max subseq sum *** //
// ********************** //
namespace alg 
{
    template<std::int32_t N, std::int32_t...Ns>
    struct max_subseq_sum_modified                             // <--- This is recursion of modified problem.
    {
        static const std::int32_t value = max_item
        <
            N,                                                 // case 1 : sequence ends here
            N + max_subseq_sum_modified<Ns...>::value          // case 2 : sequence continues
        >
        ::value;
    };

    template<std::int32_t N>
    struct max_subseq_sum_modified<N>                          // <--- This is boundary case of modified problem.
    {
        static const std::int32_t value = N;
    };

    template<std::int32_t N, std::int32_t...Ns>
    struct max_subseq_sum                                      // <--- This is recursion.
    {
        static const std::int32_t value = max_item
        <
            max_subseq_sum<Ns...>::value, 
            max_subseq_sum_modified<N,Ns...>::value
        >
        ::value;
    };

    template<std::int32_t N>
    struct max_subseq_sum<N>                                   // <--- This is boundary case.
    {
        static const std::int32_t value = N;
    };
}


// ********************** //
// *** Reverse binary *** //
// ********************** //
namespace alg 
{
    template<bool...Bs>
    struct reverse_boolean                                     // <--- This is interface.
    {
    };

    template<bool B, bool...Bs>
    struct reverse_boolean<B, Bs...>                           // <--- This is recursion.
    {
        static const std::uint32_t value = B + 2 * reverse_boolean<Bs...>::value;
    };

    template<bool B>
    struct reverse_boolean<B>                                  // <--- This is boundary case.
    {
        static const std::uint32_t value = B;
    };
}
