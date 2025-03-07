#include<iostream>
#include<vector>
#include<algorithm>


namespace alg
{  
    // *************************** //
    // Using combination of :
    // 1. inner loop - quick sort 
    // 2. outer loop - bisection
    // *************************** //
    std::uint32_t order_statistics(const std::vector<std::uint32_t>& vec, std::uint32_t kth_order)
    {
        if (kth_order >= vec.size()) return 0;

        std::vector<std::uint32_t> v(vec);
        std::uint32_t lower = 0;
        std::uint32_t upper = v.size()-1;
        while(lower!=upper)
        {
            std::uint32_t i = lower;
            std::uint32_t j = upper;
            while(i!=j)
            {
                if (v[i] <= v[j])
                {
                    --j;
                }
                else
                {
                    std::uint32_t temp = v[i];
                    v[i] = v[j];
                    v[j] = v[i+1];
                    v[i+1] = temp;
                    ++i;
                }
            }
            if (i < kth_order)
            {
            //  lower = i;   // BUG : infinity loop, as v[i] is in correct position
                lower = i+1; // Will lower > upper? No, since i < kth_order
            }
            else if (i > kth_order)
            {
            //  upper = i;   // BUG : infinity loop, as v[i] is in correct position
                upper = i-1; // Will lower > upper? No, since i > kth_order
            }
            else return v[i];
        }
        return v[lower];
    }

    // *********************************** //
    // Using : 
    // 1. bubble sort
    // 2. logic to skip useless comparison
    // *********************************** //
    std::uint32_t min_number_adjacent_swap(const std::vector<std::uint32_t>& orig_pos)
    {
        std::uint32_t num_swap = 0;
        for(std::uint32_t n=0; n!=orig_pos.size(); ++n)
        {
            std::uint32_t m0 = 0;
            if (orig_pos[n] > 2) m0 = orig_pos[n]-2;

            for(std::uint32_t m=m0; m!=n; ++m) // m < n, i.e. orig_pos[m] overtook orig_pos[n]
            {
                if (orig_pos[m] > orig_pos[n]) ++num_swap;
                // when this happens, 
                // m > orig_pos[m]-2
                //   > orig_pos[n]-2 <--- optimization is possible
            }
        }
        return num_swap;
    }
}


// ******************************** //
// *** Benchmark implementation *** //
// ******************************** //
namespace alg
{
    std::uint32_t order_statistics_bmk(const std::vector<std::uint32_t>& vec, std::uint32_t kth_order)
    {
        std::vector<std::uint32_t> sorted_vec(vec);
        std::sort(sorted_vec.begin(), sorted_vec.end());
        return sorted_vec[kth_order];
    }

    std::uint32_t min_number_adjacent_swap_bmk_UL(const std::vector<std::uint32_t>& orig_pos)
    {
        std::uint32_t num_swap = 0;
        for(std::uint32_t n=0; n!=orig_pos.size(); ++n)
        {
            for(std::uint32_t m=0; m!=n; ++m) // m < n, i.e. orig_pos[m] overtook orig_pos[n]
            {
                if (orig_pos[m] > orig_pos[n]) ++num_swap;
                // when this happens, 
                // m > orig_pos[m]-2
                //   > orig_pos[n]-2 <--- optimization is possible
            }
        }
        return num_swap;
    }

    std::uint32_t min_number_adjacent_swap_bmk_UR(const std::vector<std::uint32_t>& orig_pos)
    {
        std::uint32_t num_swap = 0;
        for(std::uint32_t n=0; n!=orig_pos.size(); ++n)
        {
            for(std::uint32_t m=n+1; m!=orig_pos.size(); ++m) // m > n, i.e. orig_pos[n] overtook orig_pos[m] 
            {
                if (orig_pos[m] < orig_pos[n]) ++num_swap;
                // when this happens, 
                // n > orig_pos[n]-2
                //   > orig_pos[m]-2 <--- no optimization
            }
        }
        return num_swap;
    }
}
