#include<iostream>
#include<cstdint>
#include<cmath>
#include<vector>
#include<map>
#include<unordered_map>
#include<algorithm>


// ******************** //
// *** Maximization *** //
// ******************** //
namespace alg
{
    std::int32_t max_subseq_sum(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()==0) return 0;

        std::int32_t sub = vec[0]; // include the last element    
        std::int32_t ans = sub;    
        for(std::uint32_t n=1; n!=vec.size(); ++n)
        {
            sub = std::max(sub + vec[n], vec[n]);
            ans = std::max(ans, sub);
        }
        return ans;
    }

    std::int32_t max_subseq_prd(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()==0) return 0;

        std::int32_t sub0 = vec[0]; // most-positive   
        std::int32_t sub1 = vec[0]; // most-negative   
        std::int32_t ans  = sub0;    
        for(std::uint32_t n=1; n!=vec.size(); ++n)
        {
            auto tmp0 = sub0 * vec[n];
            auto tmp1 = sub1 * vec[n];
            sub0 = std::max(std::max(tmp0, tmp1), vec[n]);
            sub1 = std::min(std::min(tmp0, tmp1), vec[n]);
            ans  = std::max(ans, sub0);
        }
        return ans;
    }


    // If no empty subseq is allowed, then the answer should be equivalent to :
    // * sum of positive numbers, if there exists one or more positive numbers
    // * least negative number, if there exists no positive number.
    // Hence, no dynprog is needed.
    //
    std::int32_t max_non_contiguous_subseq_sum(const std::vector<std::int32_t>& vec) 
    {
        if (vec.size()==0) return 0;

        std::int32_t sub0 = 0;      // exclude the last element  
        std::int32_t sub1 = vec[0]; // include the last element  
        std::int32_t ans  = sub1;    
        for(std::uint32_t n=1; n!=vec.size(); ++n)
        {
            auto tmp0 = sub0;
            auto tmp1 = sub1;
            sub0 = std::max(tmp0, tmp1);
            sub1 = std::max(std::max(tmp0 + vec[n], tmp1 + vec[n]), vec[n]);
            ans  = std::max(ans, sub1);
        }
        return ans;
    }

    std::int32_t max_strictly_non_contiguous_subseq_sum(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()==0) return 0;

        std::int32_t sub0 = 0;      // exclude the last element  
        std::int32_t sub1 = vec[0]; // include the last element  
        std::int32_t ans  = sub1;    
        for(std::uint32_t n=1; n!=vec.size(); ++n)
        {
            auto tmp0 = sub0;
            auto tmp1 = sub1;
            sub0 = std::max(tmp0, tmp1);
            sub1 = std::max(tmp0 + vec[n], vec[n]);
            ans  = std::max(ans, sub1);
        }
        return ans;
    }

    std::int32_t max_puzzle_game(const std::vector<std::int32_t>& vec)
    {
        // step 1 : build histogram
        std::unordered_map<std::int32_t, std::uint32_t> hist;    
        for(const auto& x:vec)
        {
            if (auto iter=hist.find(x); iter!=hist.end())
            {
                ++iter->second;
            }
            else
            {
                hist[x] = 1;
            }
        }

        // step 2 : dynprog
        std::int32_t max  = *std::max_element(vec.begin(), vec.end());
        std::int32_t min  = *std::min_element(vec.begin(), vec.end());
        std::int32_t sub0 = 0;   // exclude the last element
        std::int32_t sub1 = min; // include the last element
        std::int32_t ans;
        for(std::uint32_t n=min+1; n<=max; ++n)
        {
            auto tmp0 = sub0;
            auto tmp1 = sub1;
            if (auto iter=hist.find(n); iter!=hist.end())
            {
                auto x = iter->first*iter->second;
                sub0 = std::max(tmp0, tmp1); 
                sub1 = std::max(tmp0 + x, x);
                ans  = std::max(ans, sub1);
            }
            else
            {
                sub0 = std::max(tmp0, tmp1); 
                sub1 = 0;
                ans  = std::max(ans, sub1); // should be no change
            }
        }
        return ans;
    }
}



// *********************************************** //
// *** Maximization - benchmark implementation *** //
// *********************************************** //
namespace alg
{
    std::int32_t max_subseq_sum_bmk(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()==0) return 0;
          
        std::int32_t ans = std::numeric_limits<std::int32_t>::min();
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 0;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum += vec[m];
                ans = std::max(ans, cum);
            }
        }
        return ans;
    }

    std::int32_t max_subseq_prd_bmk(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()==0) return 0;

        std::int32_t ans = std::numeric_limits<std::int32_t>::min();
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 1;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum *= vec[m];
                ans = std::max(ans, cum);
            }
        }
        return ans;
    }

    std::int32_t max_non_contiguous_subseq_sum_bmk(const std::vector<std::int32_t>& vec) 
    {
        if (vec.size()==0) return 0;

        // case 1 : least negative number
        if (std::all_of(vec.begin(), vec.end(), [](std::int32_t x) { return x < 0; }))
        {
            return *std::max_element(vec.begin(), vec.end()); 
        }
        // case 2 : sum of positive numbers
        else 
        {
            std::int32_t ans = 0;
            std::for_each(vec.begin(), vec.end(), [&ans](std::int32_t x)
            {
                if (x >= 0) ans += x;
            });
            return ans;
        }
    }

    // no benchmark for  
    // * max_strictly_non_contiguous_subseq_sum
    // * max_puzzle_game
}



// **************** //
// *** Counting *** //
// **************** //
namespace alg
{
    std::uint32_t count_target_subseq_sum(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        std::unordered_map<std::int32_t, std::uint32_t> hist;
        hist[0] = 1; // do not miss this

        std::int32_t  cum = 0;
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            cum += vec[n];
            if (auto iter=hist.find(cum-target); iter!=hist.end())
            {
                ans += iter->second;
            }
            if (auto iter=hist.find(cum); iter!=hist.end())
            {
                ++iter->second;
            }
            else 
            {
                hist[cum] = 1;
            }
        }
        return ans;
    }

    std::uint32_t count_target_divisible_subseq_sum(const std::vector<std::int32_t>& vec, std::uint32_t target)
    {
        std::vector<std::uint32_t> hist(target, 0); 
        hist[0] = 1; // do not miss this

        std::int32_t  cum = 0;
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            cum += vec[n];
            auto mod = cum % target;
            ans += hist[mod];
            ++hist[mod];
        }
        return ans;
    }

    std::uint32_t longest_target_subseq_sum(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        std::unordered_map<std::int32_t, std::int32_t> index; 
        index[0] = -1; // do not miss this

        std::int32_t  cum = 0;
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            cum += vec[n];
            if (auto iter=index.find(cum-target); iter!=index.end())
            {
                ans = std::max(ans, n-iter->second);
            }
            if (auto iter=index.find(cum); iter==index.end())
            {
                index[cum] = n;
            }
        }
        return ans;
    }

    // ************************************************ //
    // 1. Only count less-than-target (use upper_bound)
    //    Don't count equal-to-target (not lower_bound)
    // 2. Input numbers are positive (can be zero).
    // ************************************************ //
    std::uint32_t count_less_than_target_subseq_sum(const std::vector<std::uint32_t>& vec, std::uint32_t target)
    {
        std::map<std::uint32_t, std::int32_t> index; // unlike prev, need ordered map here
        index[0] = -1;                               

        std::uint32_t cum = 0;
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            cum += vec[n]; 
            if (cum < target)
            {
                ans += n+1;
            }
        //  else if (auto iter=index.lower_bound(cum-target); iter!=index.end()) // for count_less_than_or_equal_target_subseq_sum
            else if (auto iter=index.upper_bound(cum-target); iter!=index.end()) 
            {
                ans += n-iter->second;  
            }
            if (index.find(cum)==index.end()) 
            {
                index[cum] = n; 
            }
        //  else implies vec[n]==0, no need to update index
        }
        return ans;
    }

    // ******************************************************** //
    // 1. Only count less-than-target (use upper_bound)
    //    Don't count equal-to-target (not lower_bound)
    // 2. Input numbers are strictly positive (cannot be zero).
    //    We set cum = 1, unlike max_subseq_prd_bmk.
    // ******************************************************** //
    std::uint32_t count_less_than_target_subseq_prd(const std::vector<std::uint32_t>& vec, std::uint64_t target)
    {
        std::map<std::uint64_t, std::int32_t> index;
        index[0] = -1; 

        std::uint64_t cum = 1; 
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            cum *= vec[n];
            if (cum < target)
            {
                ans += n+1;
            }        
        //  else if (auto iter=index.lower_bound(std::ceil((double)cum/target)); iter!=index.end())  // <--- incorrect 
        //  else if (auto iter=index.upper_bound(std::ceil((double)cum/target)); iter!=index.end())  // <--- incorrect
        //  else if (auto iter=index.lower_bound(cum/target); iter!=index.end())                     // <--- incorrect 
            else if (auto iter=index.upper_bound(cum/target); iter!=index.end())                     // <---   correct (why? see remark)
            {
                ans += n-iter->second;
            }
            if (index.find(cum)==index.end())
            {
                index[cum] = n; 
            }
        //  else implies vec[n]==1, no need to update index
        }
        return ans;
    }

    // Remark : Consider target = 10
    //
    // cum  | cum / target | min n in hist, such that cum / n < target 
    // -----+--------------+-------------------------------------------
    // 28   | 2.8          | 3     as 28 / 3 < 10
    // 29   | 2.9          | 3     as 29 / 3 < 10
    // 30   | 3.0          | 4     as 30 / 4 < 10
    // 31   | 3.1          | 4     as 31 / 4 < 10
    // 32   | 3.2          | 4     as 32 / 4 < 10
    //
    // hence we have : n = upper_bound(cum/target)
}



// ******************************************* //
// *** Counting - benchmark implementation *** //
// ******************************************* //
namespace alg
{
    std::uint32_t count_target_subseq_sum_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()==0) return 0;
          
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 0;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum += vec[m];
                if (cum == target) ++ans;
            }
        }
        return ans;
    }

    std::uint32_t count_target_divisible_subseq_sum_bmk(const std::vector<std::int32_t>& vec, std::uint32_t target)
    {
        if (vec.size()==0) return 0;
          
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 0;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum += vec[m];
                if (cum % target == 0) ++ans;
            }
        }
        return ans;
    }

    std::uint32_t longest_target_subseq_sum_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()==0) return 0;
          
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 0;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum += vec[m];
                if (cum == target && ans < m-n+1) ans = m-n+1;
            }
        }
        return ans;
    }

    std::uint32_t count_less_than_target_subseq_sum_bmk(const std::vector<std::uint32_t>& vec, std::uint32_t target)
    {
        if (vec.size()==0) return 0;
          
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::int32_t cum = 0;
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum += vec[m];
                if (cum < target) ++ans;
            }
        }
        return ans;
    }

    std::uint32_t count_less_than_target_subseq_prd_bmk(const std::vector<std::uint32_t>& vec, std::uint64_t target)
    {
        if (vec.size()==0) return 0;
          
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            std::uint64_t cum = 1; // initialized as 1 for product
            for(std::uint32_t m=n; m!=vec.size(); ++m) 
            {
                cum *= vec[m];
                if (cum < target) ++ans;
            }
        }
        return ans;
    }
}
