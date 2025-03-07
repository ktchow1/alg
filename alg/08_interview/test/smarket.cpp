#include<iostream>
#include<cstdint>
#include<vector>
#include<set>
#include<map>
#include<unordered_map>
#include<math.h>

namespace interview {
// ************************************************* //
// *** Less than target subseq product in O(N^3) *** //
// ************************************************* //
std::uint32_t lt_target_ssp0(const std::vector<std::uint32_t>& vec, std::uint64_t target)
{
    std::uint32_t count = 0;
    for(std::uint32_t n=0; n!=vec.size(); ++n) // begin
    {
        for(std::uint32_t m=n+1; m!=vec.size()+1; ++m) // end
        {
            std::uint64_t product = 1;
            bool lt_target = true;

            for(std::uint32_t k=n; k!=m; ++k) 
            {
                product *= vec[k];                
                if (product > target) // avoid overflow 64-bits
                {
                    lt_target = false;
                    break;
                }
            }
            if (lt_target) ++count;
        }
    }
    return count;
}

// *********************************************** //
// *** Less than target subseq product in O(N) *** //
// *********************************************** //
// These two ceil-methods are verified to be the same.
//
std::uint64_t quotient_ceil_method0(std::uint64_t product, std::uint64_t target)
{
    return (std::uint64_t) ceil((double)product / target);  // BUG3
}

std::uint64_t quotient_ceil_method1(std::uint64_t product, std::uint64_t target)
{
    std::uint64_t ans = product / target; 
    if (product % target != 0) ++ans;  
    return ans;
}

std::uint32_t lt_target_ssp1(const std::vector<std::uint32_t>& vec, std::uint64_t target)
{
    std::map<std::uint64_t, std::uint32_t> hist;
    std::uint64_t product = 1;
    std::uint32_t count = 0;
    for(std::uint32_t n=0; n!=vec.size(); ++n)
    {
        // update product
        product *= vec[n];

        // update count 
        if (product <= target)
        {
            count += n+1; // BUG1
        }
        else
        {
            auto temp  = quotient_ceil_method1(product, target); 
            auto iter0 = hist.lower_bound(temp); 
            if (iter0 != hist.end())
            {
                count += n-iter0->second; // BUG2
            }
        }

        // update histogram
        auto iter1 = hist.find(product);
        if (iter1 == hist.end())
        {
            hist[product] = n;
        }
    }
    return count;
}

// ****************************** //
// *** Point puzzle in O(2^N) *** //
// ****************************** //
using hashmap = std::unordered_map<std::uint32_t, std::uint32_t>; // key = element, value = number
hashmap vec2hmap(const std::vector<std::uint32_t>& vec)
{
    hashmap hmap;
    for(const auto& x:vec)
    {
        if (auto iter=hmap.find(x); iter!=hmap.end())
        {
            iter->second++;
        }
        else
        {
            hmap[x] = 1;
        }
    }

    // *** Debug *** //
//  std::cout << "\nInput = ";
//  for(const auto& x:hmap) std::cout << x.first << ":" << x.second << ", "; 
    return hmap;
}

struct state
{
    std::uint32_t score;
    hashmap hmap;
};

std::uint32_t point_puzzle0(std::vector<std::uint32_t>& vec) // state-graph approach
{
    if (vec.empty()) return 0;

    std::uint32_t max_score = 0;
    std::vector<state> states0;
    states0.push_back({0, vec2hmap(vec)});

    while(!states0.empty())
    {
        std::vector<state> states1;
        for(auto& x:states0)
        {
            auto n = x.hmap.begin()->first;                                         // the begin element
            auto score0 = x.score;                                                  // score if ignore the begin element
            auto score1 = x.score + x.hmap.begin()->first * x.hmap.begin()->second; // score if select the begin element
            x.hmap.erase(x.hmap.begin());
            if (!x.hmap.empty()) states1.push_back({score0, x.hmap});               // BUG : Don't miss the empty checking.

            x.hmap.erase(n-1);
            x.hmap.erase(n+1);
            if (!x.hmap.empty()) states1.push_back({score1, std::move(x.hmap)});    // BUG : Don't miss the empty checking.

            if (max_score < score0) max_score = score0;
            if (max_score < score1) max_score = score1;
        }
        states0 = std::move(states1);
    }
    return max_score;
}

// **************************** //
// *** Point puzzle in O(N) *** //
// **************************** //
using histogram = std::vector<std::uint32_t>;
histogram vec2hist(const std::vector<std::uint32_t>& vec)
{   
    std::uint32_t max_number = 0;
    for(const auto& x:vec)
    {
        if (max_number < x)
            max_number = x;
    }

    histogram hist(max_number+1,0);
    for(const auto& x:vec) ++hist[x];

    // *** Debug *** //
//  std::cout << "\nInput = ";
//  for(const auto& x:hist) std::cout << x << ", "; 
    return hist;
} 

std::uint32_t point_puzzle1(std::vector<std::uint32_t>& vec)
{
    if (vec.empty()) return 0;

    auto hist = vec2hist(vec);
    std::uint32_t sub0 = 0;         // suboptimal excluding current item
    std::uint32_t sub1 = hist[0];   // suboptimal including current item
    std::uint32_t ans  = sub1;

    // No need to remove n-1 and n+1 explicitly, just skip them in subproblem
    for(std::uint32_t n=0; n!=hist.size(); ++n)
    {
        auto sub0_old = sub0;
        auto sub1_old = sub1;
        sub0 = std::max(sub0_old, sub1_old);
        sub1 = sub0_old + n*hist[n];
        ans  = std::max(ans, std::max(sub0, sub1));
    }
    return ans;
}

// *********************** //
// *** Test programmes *** //
// *********************** //
void test_smarket_subseq_product()
{
    std::uint32_t count_success = 0;
    std::uint32_t count_done = 0;
    std::uint32_t T = 1000;    // number of trials
    std::uint32_t N = 15;      // number of elements, overflow happens when N > 15

    for(std::uint32_t t=0; t!=T; ++t)
    {
        std::vector<std::uint32_t> vec;
        std::uint64_t target = 1;

        for(std::uint32_t n=0; n!=N; ++n)
        {
            vec.push_back(rand()%25+1); // Numbers are all positive.
            target *= 2;
        }

        auto ans0 = lt_target_ssp0(vec, target);
        auto ans1 = lt_target_ssp1(vec, target);

        ++count_done; // == t+1
        if (ans0 == ans1) ++count_success;
        std::cout << "\nTrial " << t << " : " 
                                << ans0 << ", "  
                                << ans1 << " success = " 
                                << count_success << "/" 
                                << count_done;
    }
}

void test_smarket_point_puzzle()
{
    std::uint32_t count_success = 0;
    std::uint32_t count_done = 0;
    std::uint32_t T = 1000; 
    std::uint32_t N = 40; 

    for(std::uint32_t t=0; t!=T; ++t)
    {
        std::vector<std::uint32_t> vec;
        for(std::uint32_t n=0; n!=N; ++n)
        {
            vec.push_back(rand()%20+1); 
        }

        auto ans0 = point_puzzle0(vec);
        auto ans1 = point_puzzle1(vec);

        ++count_done; // == t+1
        if (ans0 == ans1) ++count_success;
        std::cout << "\nTrial " << t << " : " 
                                << ans0 << ", "  
                                << ans1 << " success = " 
                                << count_success << "/" 
                                << count_done;
    }
}
}

