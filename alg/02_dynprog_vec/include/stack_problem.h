#include<iostream>
#include<limits>
#include<stack>
#include<vector>
#include<algorithm>


namespace alg
{
    std::uint32_t shortest_unsorted_subseq(const std::vector<std::uint32_t>& vec)
    {
        // *************** //
        // *** Forward *** //
        // *************** //
        std::stack<std::uint32_t> s0;
        bool push0 = true;

        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            const auto& x = vec[n];
            while(!s0.empty() && x < s0.top())
            {
                push0 = false;
                s0.pop();
            }
            if (push0)
            {
                s0.push(x);
            }
        }
        if (s0.size()==vec.size()) return 0;

        // **************** //
        // *** Backward *** //
        // **************** //
        std::stack<std::uint32_t> s1;
        bool push1 = true;

        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            const auto& x = vec[vec.size()-n-1];
            while(!s1.empty() && x > s1.top())
            {
                push1 = false;
                s1.pop();
            }
            if (push1)
            {
                s1.push(x);
            }
        }
        if (s1.size()==vec.size()) return 0; // <--- should never happen

        return vec.size() - s0.size() - s1.size();
    }

    // ******************************************************************** //
    // The while-loop inside count_stroke_in_histogram() is similar to ...
    // the while-loop inside shortest_unsorted_subseq()
    // ******************************************************************** //
    std::uint32_t count_stroke_in_histogram(const std::vector<std::uint32_t>& vec)
    {
        std::stack<std::uint32_t> s;
        std::uint32_t ans = 0;

        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            const auto& x = vec[n];
            if (s.empty())
            {
                if (x > 0) 
                {
                    ans += x;
                    s.push(x);
                }
            }
            else if (x > s.top())
            {
                if (x > 0) 
                {
                    ans += x-s.top();
                    s.push(x);
                }
            }
            else
            {
                while(!s.empty() && x <= s.top()) 
                {
                    s.pop();
                }
                if (x > 0) 
                {
                    s.push(x);
                }
            }
        }
        return ans;
    }

    // ************************************************************************************** //
    // [Explanation about biggest rect in hist]
    // ************************************************************************************** //
    // To define a rect in hist, we need 3 parameters :
    // * LHS edge of rect
    // * RHS edge of rect
    // * height of rect
    //  
    // In O(N2) exhaustive search :
    // * iterate vec[n] as LHS edge in outer for-loop
    // * iterate vec[m] as RHS edge in inner for-loop
    // * find max height such that h <= vec[n,n+1,...,m]
    //
    // In O(N) dynprog search :
    // * iterate vec[n] as height
    // * extend LHS-edge to as far as possible, keeping height const at vec[n]
    // * extend RHS-edge to as far as possible, keeping height const at vec[n]
    // 
    // When we are at vec[n], using it as height, if hist is increasing, then :
    // * LHS-edge is vec[n]
    // * RHS-edge is vec[vec.size()-1]
    //
    // When we are at vec[n], using it as height, if hist is decreasing (local crest), then :
    // * remove the crest, because when using any vec[m] inside crest as height ...
    // * RHS-edge cannot be extended to vec[n]       or beyond (as vec[m] > vec[n])
    // * LHS-edge cannot be extended to vec[s.top()] or beyond 
    // 
    // Hence we need to :
    // * consider all rect inside crest, with height vec[m]
    // * extract crest with help of stack s
    // * flatten crest by const height vec[n]
    // * unlike other algo, we store index n (instead of value vec[n]) in stack
    //
    // Consider the animation :
    // * invert the histogram profile
    // * water pumping from LHS and fill the trough
    // * once 1st trough is full, water flows into 2nd trough
    // * once 2nd trough is full, water flows ...
    // * some troughs may merge to form larger trough
    // * water will escape from RHS edge
    // * remove all water-filled troughs (flatten them)
    // * perform final scan of whole monotonic landscape
    // ************************************************************************************** //
    // A bin in hist is defined as :
    //
    // LHS edge = s.next_top()+1
    // RHS edge = s.top()
    // height   = vec[s.top()]
    //
    // where s stores index n
    // ************************************************************************************** //
    //
    std::uint32_t biggest_rect_in_hist(const std::vector<std::uint32_t>& vec)
    {
        std::stack<std::uint32_t> s; // for index, not for value
        std::uint32_t ans = 0;

        // ***************************** //
        // *** Step 1 : Remove crest *** //
        // ***************************** //
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            while(!s.empty() && vec[n] < vec[s.top()])
            {
                std::uint32_t m = s.top();
                s.pop();

                if (s.empty())
                {
                    std::uint32_t area = vec[m] * ((n-1)+1); // BUG : cannot replace n-1 by m, see remark 1
                    ans = std::max(ans, area);
                }
                else
                {
                    std::uint32_t area = vec[m] * ((n-1)-s.top()); // BUG : cannot replace n-1 by m, see remark 1
                    ans = std::max(ans, area);
                }
            }
            // [Remark 1]
            // It may involve multiple pop in while-loop, 
            // m works in the 1st pop, but not for other.

            s.push(n); 
        }

        // *************************** //
        // *** Step 2 : Final scan *** //
        // *************************** //
        while(!s.empty())
        {
            std::uint32_t m = s.top();
            s.pop();

            if (s.empty())
            {
                std::uint32_t area = vec[m] * ((vec.size()-1)+1);
                ans = std::max(ans, area);
            }
            else
            {
                std::uint32_t area = vec[m] * ((vec.size()-1)-s.top());
                ans = std::max(ans, area);
            }
        }
        return ans;
    }

    // ********************************** //
    // *** No benchmark for this algo *** //
    // ********************************** //
    std::uint32_t total_trapped_water(const std::vector<std::uint32_t>& vec)
    {
        std::vector<std::uint32_t> LHS_profile(vec.size());
        std::vector<std::uint32_t> RHS_profile(vec.size());

        std::uint32_t LHS_max = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            LHS_max = std::max(LHS_max,vec[n]);
            LHS_profile[n] = LHS_max;
        }

        std::uint32_t RHS_max = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            std::uint32_t m = vec.size()-1-n;
            RHS_max = std::max(RHS_max,vec[m]);
            RHS_profile[m] = RHS_max;
        }

        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            ans += std::min(LHS_profile[n], RHS_profile[n]) - vec[n];
        }
        return ans;
    }
}


// ******************************** //
// *** Benchmark implementation *** //
// ******************************** //
namespace alg
{
    std::uint32_t shortest_unsorted_subseq_bmk(const std::vector<std::uint32_t>& vec)
    {
        std::vector<std::uint32_t> sorted_vec(vec);
        std::sort(sorted_vec.begin(), sorted_vec.end());

        // *************** //
        // *** Forward *** //
        // *************** //
        std::uint32_t unsorted_begin = vec.size();
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (vec[n] != sorted_vec[n])
            {
                unsorted_begin = n;
                break;
            }
        } 
        if (unsorted_begin == vec.size()) return 0;

        // **************** //
        // *** Backward *** //
        // **************** //
        std::uint32_t unsorted_last = vec.size();
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (vec[vec.size()-n-1] != sorted_vec[vec.size()-n-1])
            {
                unsorted_last = vec.size()-n-1;
                break;
            }
        }
        if (unsorted_last == vec.size()) return 0; // <--- should never happen

        return unsorted_last - unsorted_begin + 1;
    }

    std::uint32_t count_stroke_in_histogram_bmk(const std::vector<std::uint32_t>& vec)
    {
        std::uint32_t ans = 0;
        std::uint32_t lvl = 1;

        while(true)
        {
            bool is_empty = true;
            bool is_stroke = false;

            for(std::uint32_t n=0; n!=vec.size(); ++n)
            {
                if (!is_stroke)
                {
                    if (vec[n] >= lvl)
                    {
                        is_empty = false;
                        is_stroke = true;
                        ++ans;
                    }
                    else
                    {
                        // do nothing
                    }
                }
                else
                {
                    if (vec[n] >= lvl)
                    {
                        is_empty = false;
                    }
                    else
                    {
                        is_stroke = false;
                    }
                }
            }
            if (is_empty) break;
            ++lvl;
        }
        return ans;
    }

    std::uint32_t biggest_rect_in_hist_bmk(const std::vector<std::uint32_t>& vec)
    {
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=vec.size(); ++n)                        // n = LHS-edge of rect (inclusive)
        {
            std::uint32_t h = std::numeric_limits<std::uint32_t>::max(); 
            for(std::uint32_t m=n; m!=vec.size(); ++m)                    // m = RHS-edge of rect (inclusive)
            {
                h = std::min(h, vec[m]);                                  // h = min(vec[n], vec[n+1], ..., vec[m])

                std::uint32_t area = h * (m-n+1);
                ans = std::max(ans, area);
            }
        }
        return ans;
    }
}
