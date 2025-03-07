#include<iostream>
#include<cstdint>
#include<cmath>
#include<vector>
#include<unordered_map>
#include<algorithm>
#include<stdexcept>


// ******************** //
// *** Maximization *** //
// ******************** //
namespace alg
{
    std::int32_t max_2_point_sum_distance(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()<2) return 0;
        const std::int32_t min_limit = std::numeric_limits<std::int32_t>::min();

        // *** [step 1] find max(vec[n]) *** //
        auto max_iter = std::max_element(vec.begin(), vec.end());
        std::uint32_t max_index = std::distance(vec.begin(), max_iter);

        // *** [step 2] find max(vec[n]+n) *** //
        std::int32_t max_value0 = min_limit;
        for(std::uint32_t n=max_index+1; n!=vec.size(); ++n)
        {
            max_value0 = std::max(max_value0, vec[n] + (std::int32_t)n);
        }

        // *** [step 3] find max(vec[n]-n) *** //
        std::int32_t max_value1 = min_limit;
        for(std::uint32_t n=0; n!=max_index; ++n)
        {
            max_value1 = std::max(max_value1, vec[n] - (std::int32_t)n);
        }
        
        // *** [step 4] return max of these cases *** //
        // remark 1-4 : ignoring these cases will fail occasionally
        //
        if  (max_value0 != min_limit && max_value1 != min_limit)
        {
            std::int32_t ans = max_value0 + max_value1;
            ans = std::max(ans, max_value0 + *max_iter - (std::int32_t)max_index); // remark 1
            ans = std::max(ans, *max_iter + (std::int32_t)max_index + max_value1); // remark 2
            return ans;
        }
        else if (max_value0 != min_limit) // remark 3
        {
            return max_value0 + *max_iter - max_index;
        }
        else if (max_value1 != min_limit) // remark 4
        {
            return *max_iter + max_index + max_value1;
        }
        else
        {
            throw std::runtime_error("impossible, must be a bug");
        }   
    }


    // *********************** //
    // *** Equal digit sum *** //
    // *********************** //
    std::uint32_t digit_sum(std::uint32_t x)
    {
        std::uint32_t ans = 0;
        while(x>0)
        {
            ans += x%10;
            x /= 10;
        }
        return ans;
    }

    std::uint32_t max_2_point_sum_with_equal_digit_sum(const std::vector<std::uint32_t>& vec)
    {
        std::unordered_map<std::uint32_t, std::uint32_t> hist;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            auto sum = digit_sum(vec[n]); // can further optimize if we cache all digit_sum
            if (auto iter=hist.find(sum); iter!=hist.end())
            {
                ans = std::max(ans, vec[n] + iter->second);
            }
            if (auto iter=hist.find(sum); iter!=hist.end()) 
            {
                iter->second = std::max(iter->second, vec[n]);
            } 
            else 
            {
                hist[sum] = vec[n];
            }
        }
        return ans;
    }


    // *************************** //
    // *** Non attacking rooks *** //
    // *************************** //
    // Todo
    std::uint32_t max_2_point_sum_with_non_attacking_rooks(const std::vector<std::uint32_t>& vec)
    {
        std::uint32_t ans = 0;
        return ans;
    }
}



// *********************************************** //
// *** Maximization - benchmark implementation *** //
// *********************************************** //
namespace alg
{
    std::int32_t max_2_point_sum_distance_bmk(const std::vector<std::int32_t>& vec)
    {
        if (vec.size()<2) return 0;

        std::int32_t ans = std::numeric_limits<std::int32_t>::min();
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                ans = std::max(ans, vec[n]+vec[m]+(std::int32_t)m-(std::int32_t)n);
            }
        }
        return ans;
    }

    std::uint32_t max_2_point_sum_with_equal_digit_sum_bmk(const std::vector<std::uint32_t>& vec)
    {
        if (vec.size()<2) return 0;

        std::uint32_t ans = std::numeric_limits<std::uint32_t>::min();
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                if (digit_sum(vec[n]) == digit_sum(vec[m]))
                {
                    ans = std::max(ans, vec[n]+vec[m]);
                }
            }
        }
        return ans;
    }

    // Todo
    std::uint32_t max_2_point_sum_with_non_attacking_rooks_bmk(const std::vector<std::uint32_t>& vec)
    {
        std::uint32_t ans = 0;
        return ans;
    }
}



// **************** //
// *** Counting *** //
// **************** //
namespace alg
{
    bool check_target_2_point_sum_in_1_sorted_vec(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<2) return false;

        std::uint32_t n = 0;
        std::uint32_t m = vec.size()-1;
        while(n!=m)
        {
            std::int32_t sum = vec[n]+vec[m];
            if (sum == target) return true;
            if (sum <  target) ++n;
            else               --m;
        }
        return false;
    }

    bool check_target_2_point_sum_in_2_sorted_vec(const std::vector<std::int32_t>& vec0, 
                                                  const std::vector<std::int32_t>& vec1,
                                                  std::int32_t target)
    {
        if (vec0.size()==0) return false;
        if (vec1.size()==0) return false;

        std::uint32_t n = 0;
        std::uint32_t m = vec1.size()-1;
        while(n!=vec0.size()-1 && m!=0)
        {
            std::int32_t sum = vec0[n]+vec1[m];
            if (sum == target) return true;
            if (sum <  target) ++n;
            else               --m;
        }
        while(n!=vec0.size()-1)
        {
            std::int32_t sum = vec0[n]+vec1[m];
            if (sum == target) return true;
            if (sum <  target) ++n;
            else               return false;
        }

        while(m!=0)
        {
            std::int32_t sum = vec0[n]+vec1[m];
            if (sum == target) return true;
            if (sum <  target) return false;
            else               --m;
        }

        // BUG : Dont miss this case, when n=vec0.size()-1 and m=0
        std::int32_t sum = vec0[n]+vec1[m];
        if (sum == target) return true;
        else               return false;
    }

    std::uint32_t count_target_2_point_sum(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<2) return false;
        std::unordered_map<std::int32_t,std::uint32_t> hist;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (auto iter=hist.find(target-vec[n]); iter!=hist.end()) 
            {
                ans += iter->second;
            }

            // Cache vec[n], ensure index_in_hist < n
            if (auto iter=hist.find(vec[n]); iter!=hist.end()) 
            {
                ++iter->second;
            }
            else
            {
                hist[vec[n]] = 1;
            }
        }
        return ans;
    }

    std::uint32_t count_target_3_point_sum(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<3) return false;
        std::unordered_map<std::int32_t,std::uint32_t> hist;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                if (auto iter=hist.find(target-vec[n]-vec[m]); iter!=hist.end()) 
                {
                    ans += iter->second;
                }
            }

            // cache vec[n], not vec[m], ensure index_in_hist < n < m
            if (auto iter=hist.find(vec[n]); iter!=hist.end()) 
            {
                ++iter->second;
            }
            else
            {
                hist[vec[n]] = 1; 
            }
        }
        return ans;
    }
    
    std::uint32_t count_target_4_point_sum(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<4) return false;
        std::unordered_map<std::int32_t,std::uint32_t> hist;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                for(std::uint32_t k=m+1; k!=vec.size(); ++k)
                {
                    if (auto iter=hist.find(target-vec[n]-vec[m]-vec[k]); iter!=hist.end()) 
                    {
                        ans += iter->second;
                    }
                }
            }

            // cache vec[n], not vec[m], not vec[k], ensure index_in_hist < n < m < k
            if (auto iter=hist.find(vec[n]); iter!=hist.end()) 
            {
                ++iter->second;
            }
            else
            {
                hist[vec[n]] = 1; 
            }
        }
        return ans;
    }
}



// ******************************************* //
// *** Counting - benchmark implementation *** //
// ******************************************* //
namespace alg
{
    bool check_target_2_point_sum_in_1_sorted_vec_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<2) return false;

        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                if (vec[n]+vec[m] == target) return true;
            }
        }
        return false;
    }

    bool check_target_2_point_sum_in_2_sorted_vec_bmk(const std::vector<std::int32_t>& vec0, 
                                                      const std::vector<std::int32_t>& vec1,
                                                      std::int32_t target)
    {
        if (vec0.size()==0) return false;
        if (vec1.size()==0) return false;

        for(std::uint32_t n=0; n!=vec0.size(); ++n)
        {
            for(std::uint32_t m=0; m!=vec1.size(); ++m)
            {
                if (vec0[n]+vec1[m] == target) return true;
            }
        }
        return false;
    }

    std::uint32_t count_target_2_point_sum_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<2) return false;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                if (vec[n]+vec[m] == target) ++ans;
            }
        }
        return ans;
    }

    std::uint32_t count_target_3_point_sum_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<3) return false;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                for(std::uint32_t k=m+1; k!=vec.size(); ++k)
                {
                    if (vec[n]+vec[m]+vec[k] == target) ++ans;
                }
            }
        }
        return ans;
    }
    
    std::uint32_t count_target_4_point_sum_bmk(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        if (vec.size()<4) return false;

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=vec.size(); ++m)
            {
                for(std::uint32_t k=m+1; k!=vec.size(); ++k)
                {
                    for(std::uint32_t l=k+1; l!=vec.size(); ++l)
                    {
                        if (vec[n]+vec[m]+vec[k]+vec[l] == target) ++ans;
                    }
                }
            }
        }
        return ans;
    }
}

