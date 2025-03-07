#include<iostream>
#include<iomanip>
#include<cstdint>
#include<vector>
#include<iterator>
#include<algorithm>

namespace interview {
// If there is no missing number in vec, return vec.size()+1.
//
// *********************** //
// Method 1 : With sorting
// time  = O(NlogN) 
// space = O(1)     
// *********************** //
std::int32_t min_missing_pos_num_sort(std::vector<std::int32_t>& vec)
{
    std::sort(vec.begin(), vec.end());

    std::int32_t ans = 1;
    for(std::uint32_t n=0; n!=vec.size(); ++n)
    {
        if (vec[n] <= 0)     continue;
        if (vec[n] >  ans)   return ans;
        if (vec[n] == ans)   ++ans;
    //  if (vec[n] == ans-1) duplicated number, we should proceed
    }
    return vec.size()+1; 
}

// **************************** //
// Method 2 : With visited flag
// time  = O(N) 
// space = O(N)     
// **************************** //
std::int32_t min_missing_pos_num_flag(const std::vector<std::int32_t>& vec)
{
    std::vector<bool> flags(vec.size(), false);

    for(const auto& x:vec)
    {
        if (x <= 0)         continue;
        if (x > vec.size()) continue;
        flags[x-1] = true;
    }
    for(std::uint32_t n=0; n!=flags.size(); ++n)
    {
        if (!flags[n]) return n+1;
    }
    return vec.size()+1; 
}

// ****************************************** //
// Method 3 : Reuse input vec as visited flag
// time   = O(N) 
// space  = O(1)     
// assume = all numbers within +/-BOUND 
// ****************************************** //
static const std::int32_t BOUND = 1024;
void mark_visited(std::int32_t& value)
{    
    if (value <= -BOUND) return; // it is marked as visited before
    if (value >  +BOUND) return; // it is marked as visited before
    if (value <= 0)      value -= BOUND;
    else                 value += BOUND;
}

auto get_value_and_flag(const std::int32_t value)
{
    if      (value <= -BOUND) return std::make_pair(value+BOUND, true); 
    else if (value >  +BOUND) return std::make_pair(value-BOUND, true);
    else                      return std::make_pair(value,      false);
}

std::int32_t min_missing_pos_num_bounded(std::vector<std::int32_t>& vec)
{
    // step 1 : mark visited
    for(const auto& x:vec)
    {
        auto [value,flag] = get_value_and_flag(x); // flag is not used in this case
        if (value > 0 && value <= vec.size())
        {
            mark_visited(vec[value-1]);
        }
    }

    // step 2 : check if visited
    for(std::int32_t n=0; n!=vec.size(); ++n)
    {
        auto [value,flag] = get_value_and_flag(vec[n]); // value is not used in this case
        if (!flag) return n+1;
    }
    return vec.size()+1; 
}

// *************************************************************************** //
// Method 4 : Perform pos int sorting
// time   = O(N)
// space  = O(1)
// assume = no BOUND assumption
//
// Given contiguous positive integer starting from 1 with missing numbers,
// special sorting is defined, such that 
// vec[0] = 1
// vec[1] = 2
// ...
// vec[n] = n+1
//
// with some missing numbers in between
// vec[m] <= 0 or
// vec[m] >  vec.size()
//
// This sorting can be done in O(N), simply by placing vec[n] in vec[vec[n]-1].
// For example : 1,2,3,-2,5,6,0,8,2,10,11,99,13,14,15 where ...
//  4 is missing and replaced by -2
//  7 is missing and replaced by  0
//  9 is missing and replaced by  2 (i.e. 2 is duplicated)
// 12 is missing and replaced by 99
// *************************************************************************** //
void special_sort(std::vector<std::int32_t>& vec)
{
    for(std::uint32_t n=0; n!=vec.size(); ++n)
    {
        bool duplicated = false;

        // for valid numbers, put them in correct place, using swap (since there is no extra space)
        while(vec[n]>=1 && vec[n]<=vec.size() && !duplicated)
        {
            if (vec[n] != vec[vec[n]-1])
            {
                std::swap(vec[n], vec[vec[n]-1]);
            }
            else 
            {
                duplicated = true;
            }   
        }
    }
}

std::int32_t min_missing_pos_num_special_sort(std::vector<std::int32_t>& vec)
{
    special_sort(vec);                         // O(N)
    for(std::uint32_t n=0; n!=vec.size(); ++n) // O(N)
    {
        if (vec[n] != n+1) return n+1;
    }
    return vec.size()+1; 
}

// **************** //
// *** Testing **** //
// **************** //
auto random_int_vector_with_missing(std::uint32_t size)
{
    std::vector<std::int32_t> output;
    std::int32_t answer = 1 + rand() % (size+1);  

    for(std::uint32_t n=0; n!=size; ++n)
    {
        output.push_back(n+1);
    }
    output[answer-1] = size+1; // min missing number
    if (answer < size)
    {
        for(std::uint32_t n=answer; n!=size; ++n)
        {
            std::uint32_t temp = rand()%6;
            if      (temp == 0) output[n]  = -(rand() % size);  // negative numbers
            else if (temp == 1) output[n] +=   rand() % size;   // numbers bigger than answer
            else if (temp == 2) output[n]  =   rand() % answer; // numbers smaller than answer, which are duplicated 
        }
    }

    // *** Shuffle *** //
    for(std::uint32_t n=0; n!=size; ++n)
    {
        std::uint32_t n0 = rand() % size;
        std::uint32_t n1 = rand() % size;
        std::swap(output[n0], output[n1]);
    }
    return std::make_pair(output, answer);
}

void test_archax_min_missing_pos_num()
{
    std::cout << "\n\nArchax : min missing positive number";
    std::uint32_t num_error = 0;
    for(std::uint32_t n=0; n!=5000; ++n)
    {
        std::uint32_t num_of_odd = 1 + rand()%10;
        auto [vec,ans] = random_int_vector_with_missing(10 + rand()%80);

        std::vector<std::int32_t> vec0(vec);
        std::vector<std::int32_t> vec1(vec);
        std::vector<std::int32_t> vec2(vec);
        std::vector<std::int32_t> vec3(vec);

        std::int32_t ans0 = min_missing_pos_num_sort(vec0); 
        std::int32_t ans1 = min_missing_pos_num_flag(vec1); 
        std::int32_t ans2 = min_missing_pos_num_bounded(vec2); 
        std::int32_t ans3 = min_missing_pos_num_special_sort(vec3); 

        if (ans != ans0 ||
            ans != ans1 || 
            ans != ans2 ||
            ans != ans3) 
            ++num_error;

        std::cout << "\ntest " << n 
                  << ", ans = " << ans 
                  << ", ans0 = " << ans0 
                  << ", ans1 = " << ans1 
                  << ", ans2 = " << ans2 
                  << ", ans3 = " << ans3 
                  << ", error = " << num_error << "/" << n+1;


        std::cout << " input ";
        for(const auto& x:vec) std::cout << x << ",";
    }

}
}
