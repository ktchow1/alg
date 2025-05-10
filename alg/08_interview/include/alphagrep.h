#include<cstdint>
#include<vector>
#include<iterator>
#include<algorithm>
#include<matrix.h>


// ***************************************************************************************************************** //
// Alphagrep questions have a lot of common features :
// * all 4 questions are about subseq
// * all 4 questions are not about dynprog
//
//
// Q | input        | looking for    | action   | objective                       | solution 
// --+--------------+----------------+----------+---------------------------------+-------------------------------
// 4 | bool         |     contiguous | counting | num of mirror pattern           | 1D scan
// 2 | unsorted int |     contiguous | counting | num of subseq with <= K odd num | 2  pointers (also by dynprog) 
// 3 | unsorted int | non contiguous | counting | num of inverse pattern          | 2D scan 
// 1 |   sorted int | non contiguous | maximize | minimum diff between num        | bisection
//
// For all counting problems above, two subseq are considered different 
// as long as their indices are different, even if content is the same.
//
// i.e. for input [1,2',2",3], then
//      subseq [1,2',3] != subseq [1,2",3]
// ***************************************************************************************************************** //



// ****************** //
// *** Question 4 *** //
// ****************** //
// Given string of 0&1, count num of contiguous subseq, so that it is a step-function with edge in middle :
// * 01, 0011, 000111 ...
// * 10, 1100, 111000 ...
//
namespace alphagrep 
{
    // No dynamic programming
    // No exhaustive search
    // Do heuristic search, count consecutive bool, then process them in linear scan
    //
    std::uint32_t count_step_function(const std::vector<bool>& vec)
    {
        // **************** //
        // *** 1st scan *** //
        // **************** //
        if (vec.size() == 0) return 0;

        std::vector<std::uint32_t> count;
        count.push_back(1);
        bool current = vec[0];

        for(std::uint32_t n=1; n!=vec.size(); n++)
        {
            if (current == vec[n])
            {
                ++count.back();
            }
            else
            {
                current = vec[n];
                count.push_back(1);
            }
        }

        // **************** //
        // *** 2nd scan *** //
        // **************** //
        if (count.size() <= 1) return 0;

        std::uint32_t ans = 0;
        for(std::uint32_t n=1; n!=count.size(); ++n)
        {
            ans += std::min(count[n], count[n-1]);
        }
        return ans;
    }

    // **************************************** //
    // Consider step function starting with n
    // 
    // n   m to stop growing LHS 
    // |   |
    // v   v 
    // 0000111111111
    //        ^
    //        |
    //        m to stop growing RHS 
    //
    // **************************************** //
    std::uint32_t count_step_function_exhaustive(const std::vector<bool>& vec)
    {
        if (vec.size() == 0) return 0;
        if (vec.size() == 1) return 0;
        std::uint32_t ans = 0;

        for(std::uint32_t n=0; n!=vec.size()-1; ++n)
        {
            bool lhs = true;
            std::uint32_t size;

            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {                
                if (lhs)
                {
                    if (vec[m]==vec[n]) {} // nothing
                    else
                    {
                        lhs  = false;
                        size = m-n;

                        if (m-n+1 == 2*size) // BUG : Dont forget step function with size 1
                        {
                            ++ans;
                            break;
                        }
                    }
                }
                else
                {
                    if (vec[m]==vec[n]) break;
                    else 
                    {
                        if (m-n+1 == 2*size)
                        {
                            ++ans;
                            break;
                        }
                    }
                }
            }
        }
        return ans;
    }
}


// ****************** //
// *** Question 2 *** //
// ****************** //
// Given vector of unsorted integers, count num of contiguous subseq, s.t. it has <= k odd num.
//
// method 1 : 2 pointers O(n)
// method 2 : dynprog    O(n)
// method 3 : exhaustive O(n^3)
// should work for k = 0
//
namespace alphagrep 
{
    std::uint32_t count_k_odd_subseq_2_ptrs(const std::vector<std::uint32_t>& vec, std::uint32_t k)    
    {
        std::uint32_t count = 0;
        std::uint32_t num_odd = 0;
  
        for(std::uint32_t n=0, m=0; m!=vec.size(); ++m)
        {
            // 1. move m and check odd
            if (vec[m]%2 == 1) ++num_odd;

            // 2. move n if exceed k
            while(num_odd > k)
            {
                if (vec[n]%2 == 1) --num_odd;
                ++n;

                // occur only when k=0, 
                // * n == m+1
                // * count += 0 
                // * goto next m
                //
                if (n>m) break; 
            }
            count += m-n+1;
        }
        return count;
    }

    std::uint32_t count_k_odd_subseq_dynprog(const std::vector<std::uint32_t>& vec, std::uint32_t k)    
    {
        std::vector<std::uint32_t> odd_index; // odd_index[m] = n means (m+1)th odd number is vec[n]

        std::uint32_t count = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (vec[n]%2==1) 
            {
                odd_index.push_back(n);
            }
            if (odd_index.size() <= k)
            {
                count += (n + 1); // subseq [0,n], [1,n] ... [n,n]
            }
            else if (k == 0)
            {
                count += n - odd_index[odd_index.size()-1];
            }
            else
            {
                count += n - odd_index[odd_index.size()-1-k]; 
            }
        }
        return count;
    }

    std::uint32_t count_k_odd_subseq_exhaustive(const std::vector<std::uint32_t>& vec, std::uint32_t k)    
    {
        std::uint32_t count = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                std::uint32_t num_odd = 0;
                for(std::uint32_t k=n; k<=m; ++k)
                {
                    if (vec[k]%2==1) ++num_odd;
                }
                if (num_odd <= k) ++count;
            }
        }
        return count;
    }
}


// ****************** //
// *** Question 3 *** //
// ****************** //
// Given vector of unsorted integers, count num of non-contiguous subsequence with size of 3, so that it is strictly decreasing : 
// i.e. find v[n0] > v[n1] > v[n2]
//      where  n0  <   n1  <   n2
//
namespace alphagrep 
{
    std::uint32_t count_decreasing_triplet_heuristic(const std::vector<std::uint32_t>& vec) // O(N^2)
    {
        if (vec.size() < 3) return 0;

        std::uint32_t count = 0;
        for(std::uint32_t n=1; n!=vec.size()-1; ++n)
        {
            // *********** //
            // *** LHS *** //
            // *********** //
            std::uint32_t num_larger_on_LHS = 0;
            for(std::uint32_t m=0; m!=n; ++m)
            {
                if (vec[m] > vec[n]) ++num_larger_on_LHS;
            }

            // *********** //
            // *** RHS *** //
            // *********** //
            std::uint32_t num_smaller_on_RHS = 0;
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                if (vec[m] < vec[n]) ++num_smaller_on_RHS;
            }
            count += num_larger_on_LHS * num_smaller_on_RHS;
        }
        return count;
    }

    std::uint32_t count_decreasing_triplet_exhaustive(const std::vector<std::uint32_t>& vec) // O(N^3)
    {
        if (vec.size() < 3) return 0;

        std::uint32_t count = 0;
        for(std::uint32_t n=0; n!=vec.size()-2; ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size()-1; ++m)
            {
                if (vec[n] > vec[m])
                {
                    for(std::uint32_t k=m+1; k!=vec.size(); ++k)
                    {
                        if (vec[m] > vec[k]) ++count;
                    }
                } 
            }
        }
        return count;
    }

}


// ****************** //
// *** Question 1 *** //
// ****************** //
// Given vector of sorted integers, find max-min diff of non-contiguous subseq of size K.
//
// Ideas : 
// * as num are sorted, only difference between two adjacent elements are considered
// * let f() be the function
// 
// f(vec[0,N-1],K) = max( min( f(vec[0,K-2],K-1), f(vec[K-2,N-1],2) ),
//                        min( f(vec[0,K-1],K-1), f(vec[K-1,N-1],2) ),
//                        min( f(vec[0,K-0],K-1), f(vec[K-0,N-1],2) ),
//                        ...
//                        min( f(vec[0,N-2],k-1), f(vec[N-2,N-1],2) ))
//
//
// * recursive is O(N!), why? f(N) = (N-1)*f(N-1) = N!
// * iterative is O(N^3)
// * bisection is O(NlogN)
//
namespace alphagrep
{
    std::uint32_t max_min_diff_in_subseq_recursive_impl(const std::vector<std::uint32_t>::const_iterator& begin,
                                                        const std::vector<std::uint32_t>::const_iterator& end,
                                                        const std::uint32_t K)
    {
        std::uint32_t N = std::distance(begin, end);

        // ********************* //
        // *** Boundary case *** //
        // ********************* //
        if (N==0) return 0;
        if (N==1) return 0;
        if (K==0) return 0;
        if (K==1) return 0;
        if (K==2 && N>=2)
        {
            return *(end-1) - *begin;
        }

        // ***************** //
        // *** Recursion *** //
        // ***************** //
        std::uint32_t max_min = 0;
        for(auto iter=begin+(K-2); iter!=begin+(N-1); ++iter)
        {
            auto subproblem0 = max_min_diff_in_subseq_recursive_impl(begin, iter+1, K-1);
            auto subproblem1 = max_min_diff_in_subseq_recursive_impl(iter, end, 2);
            auto subproblem  = std::min(subproblem0, subproblem1);
            max_min = std::max(max_min, subproblem);
        }
        return max_min;
    }

    std::uint32_t max_min_diff_in_subseq_recursive(const std::vector<std::uint32_t>& vec, const std::uint32_t K)
    {
        return max_min_diff_in_subseq_recursive_impl(vec.begin(), vec.end(), K);
    }

    // ********************************************************** //
    // Indices are defined in this way to make code looks nice.
    // k = subseq size
    // n = input  size
    //
    // mat(k,n) = subproblem with size k and vec with size n
    // mat(:,0) = no use
    // mat(:,1) = no use
    // mat(0,:) = no use
    // mat(1,:) = no use <--- same as boundary case in recursion
    //
    // mat looks like this, where A depends on all B :
    // ...............          ...............
    // ...............          ...............
    // ..*************          ..*************
    // ...************          ...***BBBBBB***
    // ....*********** ----->   ....********A**
    // .....**********          .....**********
    // ......*********          ......********$        $ = answer
    //
    // ********************************************************** //
    std::uint32_t max_min_diff_in_subseq_iterative(const std::vector<std::uint32_t>& vec, const std::uint32_t K)
    {
        alg::matrix<std::uint32_t> mat(K+1, vec.size()+1); // size is 1 bigger than last index

        // ********************* //
        // *** Boundary case *** //
        // ********************* //
        for(std::uint32_t n=2; n<=vec.size(); ++n)
        {
            mat(2,n) = vec[n-1] - vec[0];
        }

        // ***************** //
        // *** Iteration *** //
        // ***************** //
        for(std::uint32_t k=3; k<=K; ++k)
        {
            for(std::uint32_t n=k; n<=vec.size(); ++n)
            {
                mat(k,n) = 0; 
                for(std::uint32_t n0=k-1; n0<n; ++n0) 
                {
                    auto subproblem0 = mat(k-1, n0);             // <--- this part includes the value vec[n0-1]
                    auto subproblem1 = vec[n-1] - vec[n0-1];     // <--- this part includes the value vec[n0-1]  
                    auto subproblem  = std::min(subproblem0, subproblem1); 
                    mat(k,n) = std::max(mat(k,n), subproblem);
                }
            }
        }
        return mat(K, vec.size());
    }

    // ************************************************************************************* //
    // *** Check if we can find subseq with size K, having adjacent-difference >= delta. *** //
    // ************************************************************************************* //
    bool does_subseq_with_adj_diff_exist(const std::vector<std::uint32_t>& vec, std::uint32_t K, std::uint32_t delta)
    {
        std::uint32_t x = vec[0];
        std::uint32_t n = 0;

        for(std::uint32_t k=1; k!=K; ++k)
        {
            x += delta;
            while(vec[n] < x) 
            {
                ++n;
                if (n == vec.size()) return false;
            }
            x = vec[n];
        }
        return true;
    }
    
    std::uint32_t max_min_diff_in_subseq_bisection(const std::vector<std::uint32_t>& vec, const std::uint32_t K)
    {
        // ********************* //
        // *** Boundary case *** //
        // ********************* //
        std::uint32_t N = vec.size();
        if (N==0) return 0;
        if (N==1) return 0;
        if (K==0) return 0;
        if (K==1) return 0;

        // ***************** //
        // *** Bisection *** //
        // ***************** //
        std::uint32_t upper = vec.back()-vec.front();
        std::uint32_t lower = 0;
        while(upper - lower > 1)
        {
            std::uint32_t mid = (upper+lower) / 2;
            if (does_subseq_with_adj_diff_exist(vec, K, mid))
            {
                lower = mid;
            }
            else
            {
                upper = mid;
            }
        } 
        if (does_subseq_with_adj_diff_exist(vec, K, upper)) return upper;
        return lower;
    }
}


