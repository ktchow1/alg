#include<iostream>
#include<iomanip>
#include<cstdint>
#include<vector>
#include<iterator>
#include<algorithm>


// ****************** //
// *** Question 1 *** //
// ****************** //
// Given vector of sorted integers, find max-min diff of non-contiguous subset of size k : 
// where k is given and programmable
//
// Ideas : 
// * as numbers are sorted in set, vec[0] and vec[vec.size()-1] must be selected, only need to pick the rest k-2 numbers
// * as numbers are sorted in subset, only difference between two adjacent elements in subset are considered
// * so it is like the piecewise linear regression problem in dynprog
//
// Given N = vec.size() :
// 
// max_min_diff(vec[0,N-1],k) = std::max( std::min( max_min_diff(vec[0,k-2],k-1), max_min_diff(vec[k-2,N-1]],2) ),
//                                        std::min( max_min_diff(vec[0,k-1],k-1), max_min_diff(vec[k-1,N-1]],2) ),
//                                        std::min( max_min_diff(vec[0,k-0],k-1), max_min_diff(vec[k-0,N-1]],2) ),
//                                        std::min( max_min_diff(vec[0,k+1],k-1), max_min_diff(vec[k+1,N-1]],2) ),
//                                        ...
//                                        std::min( max_min_diff(vec[0,N-2],k-1), max_min_diff(vec[N-2,N-1]],2) ))
//
// math notation : vec[i,j]    includes both vec[i] and vec[j]
// prog notation : [begin,end) includes begin, excludes end
//
// recursive implementation is O(N!), why? Assume it is f(N), then f(N) = (N-1)*f(N-1) = N!
// iterative implementation is O(N^3)
//
namespace interview {
std::uint32_t max_min_diff_in_subset_recursive(const std::vector<std::uint32_t>::const_iterator& begin,
                                               const std::vector<std::uint32_t>::const_iterator& end,
                                               const std::uint32_t k)
{
    std::uint32_t N = std::distance(begin, end);

    // *** Boundary case *** //
    if (N==0) return 0;
    if (N==1) return 0;
    if (N>=2 && k==2)
    {
        return *(end-1) - *begin;
    }

    // *** Recursion *** //
    std::uint32_t max_min = 0;
    for(auto iter=begin+(k-2); iter!=begin+(N-1); ++iter)
    {
        auto subproblem0 = max_min_diff_in_subset_recursive(begin, iter+1, k-1);
        auto subproblem1 = max_min_diff_in_subset_recursive(iter, end, 2);
        auto subproblem  = std::min(subproblem0, subproblem1);
        max_min = std::max(max_min, subproblem);
    }
    return max_min;
}

std::uint32_t max_min_diff_in_subset_recursive(const std::vector<std::uint32_t>& vec, const std::uint32_t k)
{
    return max_min_diff_in_subset_recursive(vec.begin(), vec.end(), k);
}

template<typename T>
class matrix
{
public:
    matrix(std::uint32_t y, std::uint32_t x) : size_y(y), size_x(x), impl(y*x,0)
    {
    }

    const T& operator()(std::uint32_t y, std::uint32_t x) const noexcept
    {
        return impl[y*size_x+x];
    }

    T& operator()(std::uint32_t y, std::uint32_t x) noexcept
    {
        return impl[y*size_x+x];
    }
    
    void debug() const noexcept
    {
        std::cout << "\nmat";
        for(std::uint32_t y=0; y!=size_y; ++y)
        {
            std::cout << "\n|";
            for(std::uint32_t x=0; x!=size_x; ++x)
            {
                std::cout << std::setw(3) << impl[y*size_x+x] << " ";
            }
            std::cout << "|";
        }
    }

private:
    std::uint32_t  size_y;
    std::uint32_t  size_x;
    std::vector<T> impl;
};

std::uint32_t max_min_diff_in_subset_iterative(const std::vector<std::uint32_t>& vec, const std::uint32_t k)
{
    // n = vector size
    // m = subset size
    // mat(n,m) = subproblem with vec[0,n-1] and subset size m 
    // mat(:,0) = no use
    // mat(:,1) = no use
    // mat(0,:) = no use
    // mat(1,:) = no use
    //
    // mat looks like this, where cell A depends on all cells B :
    // ...........           ...........
    // ...........           ...........
    // ..*........           ..*........
    // ..**.......           ..**.......
    // ..***......  --->     ..***......
    // ..****.....           ..****.....
    // ..*****....           ..**B**....
    // ..******...           ..**BA**...
    // ..*******..           ..**B****..
    // ..********.           ..**B*****.
    // ..*********           ..*********
    //
    matrix<std::uint32_t> mat(vec.size()+1, k+1); // size is 1 bigger than last index

    // *** Boundary case *** //
    for(std::uint32_t n=2; n<=vec.size(); ++n)
    {
        mat(n,2) = vec[n-1]-vec[0];
    }

    // *** Iteration *** //
    for(std::uint32_t m=3; m<=k; ++m)
    {
        for(std::uint32_t n=m; n<=vec.size(); ++n)
        {
            mat(n,m) = 0;
            for(std::uint32_t n0=m-1; n0<n; ++n0)
            {
                auto subproblem0 = mat(n0, m-1);    
                auto subproblem1 = vec[n-1] - vec[n0-1];  
                auto subproblem  = std::min(subproblem0, subproblem1); 
                mat(n,m) = std::max(mat(n,m), subproblem);
            }

            // *** Debug *** //
        //  mat.debug();
        }
    }

    return mat(vec.size(), k);
}
}

// ****************** //
// *** Question 2 *** //
// ****************** //
// Given vector of unsorted integers, count num of contiguous subsequence, so that it has exactly k odd integers : 
// where k is given and programmable
// 
// * integers are unsorted
// * integers may duplicated
// * identical subsequences (with different composite indices) are counted as two distinct subsequence, 
//   i.e. given 1,2',2",3 then subsequence [1,2',3] != [1,2",3], this relaxation makes thing easier
//
// Ideas :
// * no dynamic programming, subproblem does not help
// * no exhaustive search, it is O(N^3) 
// * do heuristic search
//
namespace interview {
std::uint32_t count_k_odd_subseq(const std::vector<std::uint32_t>& vec, std::uint32_t k)    
{
    std::vector<std::uint32_t> odd_index; // odd_index[m] = n means (m+1)th odd number is vec[n]

    std::uint32_t count=0;
    for(std::uint32_t n=0; n!=vec.size(); ++n)
    {
        if (vec[n]%2==1) 
        {
            odd_index.push_back(n);
        }

        // Add number of valid subseq ending with vec[n]
        if (odd_index.size() == k)
        {
            count += odd_index[0]+1; // subseq [0,n], [1,n] ... [odd_index[0],n]
        }
        else if (odd_index.size() > k)
        {
            auto i0 = odd_index[odd_index.size()-k-1]+1;
            auto i1 = odd_index[odd_index.size()-k];
            count += i1-i0+1; // subseq [i0,n] [i0+1,n] ... [i1,n]
        }
    }
    return count;
}

std::uint32_t count_k_odd_subseq_exhaustive(const std::vector<std::uint32_t>& vec, std::uint32_t num_of_odd)    
{
    std::uint32_t count=0;
    for(std::uint32_t n=0; n!=vec.size(); ++n)
    {
        for(std::uint32_t m=n; m!=vec.size(); ++m) 
        {
            // Consider subseq = vec[n:m]
            std::uint32_t temp=0;
            for(std::uint32_t k=n; k!=m+1; ++k)
            {
                if (vec[k]%2==1) ++temp;
            }
            if (temp==num_of_odd) ++count;
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
// * integers are unsorted
// * integers may duplicated (however the version that I get on web is non-duplicated)
// * identical subsequences are counted as one
//
// Ideas :
// * no dynamic programming, subproblem does not help
// * no exhaustive search, it is O(N^3) 
// * do heuristic search
// * it is limited to subsequence with size of 3 (cannot be generalized to subsequences with any size)
// * make use of the info "3", i.e. finding n1 :
//   then look for bigger v[n0] on LHS
//   then look for bigger v[n2] on RHS
//
namespace interview {

// This approach does not support duplicated int.
std::uint32_t count_decreasing_triplet(const std::vector<std::uint32_t>& vec) 
{
    std::uint32_t count=0;
    for(std::uint32_t n=1; n!=vec.size()-1; ++n)
    {
        std::uint32_t num_larger_on_LHS=0;
        for(std::uint32_t m=0; m!=n; ++m)
        {
            if (vec[m] > vec[n]) ++num_larger_on_LHS;
        }

        std::uint32_t num_smaller_on_RHS=0;
        for(std::uint32_t m=n+1; m!=vec.size(); ++m)
        {
            if (vec[m] < vec[n]) ++num_smaller_on_RHS;
        }

        count += num_larger_on_LHS * num_smaller_on_RHS;
    }
    return count;
}
}

// ****************** //
// *** Question 4 *** //
// ****************** //
// Given string of 0&1, count num of non-contiguous substring, so that it is a step-function with edge in middle :
// * 01
// * 10
// * 0011
// * 1100
// * 000111
// * 111000
// * ... etc, identical substrings are counted as one
//
// Ideas :
// * no dynamic programming, subproblem does not help
// * no exhaustive search, it is more complicated
// * do heuristic search
// * if 000111 exists, then 0011 exists
// * if 111000 exists, then 1100 exists
// * do scan twice, once for counting longest 000...111...
//                  once for counting longest 111...000...
// * given a string, for element n
//   if num of 0 on LHS (including n) = x, then
//      num of 1 on LHS (including n) = n+1-x
//      num of 0 on RHS (excluding n) = total num of 0s - x
//      nun of 1 on RHS (excluding n) = total num of 1s - (n+1-x) 
//
namespace interview {
std::uint32_t count_step_funcion(std::string& str)
{
    // ************************************************* //
    // step 1 : find total num of 0s
    // step 2 : find num of 0s on LHS (including str[n])
    // ************************************************* //
    std::uint32_t total_num_of_0s = 0;
    std::uint32_t total_num_of_1s = 0;
    std::vector<std::uint32_t> vec_num_of_0s_LHS;
    for(const auto& c:str)
    {
        if (c=='0') ++total_num_of_0s;
        vec_num_of_0s_LHS.push_back(total_num_of_0s);
    }
    total_num_of_1s = str.size() - total_num_of_0s;

    // ********************************************** //
    // step 3 : find num of 0 in longest 000...111...
    //          find num of 0 in longest 111...000...
    // ********************************************** //
    std::uint32_t longest_01 = 0; 
    std::uint32_t longest_10 = 0; 
    for(std::uint32_t n=0; n!=str.size(); ++n)
    {
        std::uint32_t num_of_0s_LHS =     vec_num_of_0s_LHS[n];
        std::uint32_t num_of_1s_LHS = n+1-vec_num_of_0s_LHS[n];
        std::uint32_t num_of_0s_RHS = total_num_of_0s - num_of_0s_LHS;
        std::uint32_t num_of_1s_RHS = total_num_of_1s - num_of_1s_LHS;

        std::uint32_t temp_01 = std::min(num_of_0s_LHS, num_of_1s_RHS);
        std::uint32_t temp_10 = std::min(num_of_1s_LHS, num_of_0s_RHS);
        longest_01 = std::max(longest_01, temp_01);
        longest_10 = std::max(longest_10, temp_10);
    }
    return longest_01 + longest_10;    
}
}


// *************** //
// *** Testing *** //
// *************** //
namespace interview {
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
    os << "[";
    for(const auto& x:vec) os << x << ", ";
    os << "]";
    return os;
}

std::vector<std::uint32_t> random_sorted_int_vector(std::uint32_t size)
{
    std::vector<std::uint32_t> output;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        output.push_back(rand()%200); 
    }
    std::sort(output.begin(), output.end());
    return output;
}

void test_alphagrep_max_min_diff_in_subset()
{
    std::cout << "\n\nAlphagrep : max min diff in subset";
    std::uint32_t num_error = 0;
    for(std::uint32_t n=0; n!=500; ++n) 
    {
        std::uint32_t subset_size = 3 + rand()%6;
        auto vec = random_sorted_int_vector(20+rand()%20);
        std::uint32_t count0 = max_min_diff_in_subset_recursive(vec, subset_size); 
        std::uint32_t count1 = max_min_diff_in_subset_iterative(vec, subset_size); 
        if (count0 != count1) ++num_error;

        std::cout << "\ntest " << n << " " << vec
                  << ", count0 = " << count0 
                  << ", count1 = " << count1 
                  << ", error = " << num_error << "/" << n+1;
    }
}

std::vector<std::uint32_t> random_int_vector(std::uint32_t size)
{
    std::vector<std::uint32_t> output;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        output.push_back(rand()%100);
    }
    return output;
}

void test_alphagrep_count_decreasing_triplet() 
{
    std::cout << "\n\nAlphagrep : count decreasing triplet";
    std::uint32_t num_error = 0;
    for(std::uint32_t n=0; n!=500; ++n)
    {
        auto vec = random_int_vector(40+rand()%40);
        std::uint32_t count0 = count_decreasing_triplet(vec);
        std::uint32_t count1 = count0; 
        if (count0 != count1) ++num_error;

        std::cout << "\ntest " << n 
                  << ", count0 = " << count0 
                  << ", count1 = " << count1 
                  << ", error = " << num_error << "/" << n+1;
    }
}

void test_alphagrep_count_k_odd_subseq()    
{
    std::cout << "\n\nAlphagrep : count k odd subseq";
    std::uint32_t num_error = 0;
    for(std::uint32_t n=0; n!=500; ++n)
    {
        std::uint32_t num_of_odd = 1 + rand()%10;
        auto vec = random_int_vector(40+rand()%40);
        std::uint32_t count0 = count_k_odd_subseq(vec, num_of_odd); 
        std::uint32_t count1 = count_k_odd_subseq_exhaustive(vec, num_of_odd); 
        if (count0 != count1) ++num_error;

        std::cout << "\ntest " << n 
                  << ", count0 = " << count0 
                  << ", count1 = " << count1 
                  << ", error = " << num_error << "/" << n+1;
    }
}

std::string random_binary_string(std::uint32_t size)
{
    std::string output;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        if (rand()%2==0) output.push_back('0');
        else             output.push_back('1');
    }
    return output;
}

void test_alphagrep_count_step_function()
{
    std::cout << "\n\nAlphagrep : count step function";
    for(std::uint32_t n=0; n!=10; ++n)
    {
        auto str = random_binary_string(10+rand()%10);
        std::uint32_t count = count_step_funcion(str); 

        std::cout << "\ntest " << n 
                  << ", str = " << str 
                  << ", count = " << count;
    }
}
}
