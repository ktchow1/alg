#include<cstdint>
#include<vector>
#include<iterator>
#include<algorithm>


// Given a vector unsorted signed number, find the min positive number (> 0), that is missing.
//
// [0,1,2,4,5]   ---> 3
// [0,1,2,3,4,5] ---> 6
// [-1,1,2,4,5]  ---> 3
//
// Remark 1, for case like
//
// [1,2,3,4,5]   ---> 6


namespace arch
{
    // ************************** //
    // *** Method 1 : sorting *** //
    // ************************** //
    // time  = O(NlogN) 
    // space = O(1)     

    std::int32_t min_missing_pos_num_sort(std::vector<std::int32_t>& vec)
    {
        std::sort(vec.begin(), vec.end());

        std::int32_t next = 1;
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (vec[n] <= 0)      continue;
            if (vec[n] >  next)   return next;
            if (vec[n] == next)   ++next;
        //  if (vec[n] == next-1) duplicated number, we should proceed
        }
        return next; 
    }


    // **************************** //
    // *** Method 2 : histogram *** //
    // **************************** //
    // time  = O(N) 
    // space = O(N)     

    std::int32_t min_missing_pos_num_hist(const std::vector<std::int32_t>& vec)
    {
        std::vector<bool> hist(vec.size(), false);

        for(const auto& x:vec)
        {
            if (x <= 0)         continue;
            if (x > vec.size()) continue;

            hist[x-1] = true;
        }
        for(std::uint32_t n=0; n!=hist.size(); ++n)
        {
            if (!hist[n]) return n+1;
        }
        return vec.size()+1; // remark 1 
    }


    // ******************************* //
    // *** Method 3 : mark visited *** //
    // ******************************* //
    // time   = O(N) 
    // space  = O(1)     
    // assume = all num within +/-BOUND 
      
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

    std::int32_t min_missing_pos_num_mark(std::vector<std::int32_t>& vec)
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
        return vec.size()+1; // remark 1
    }


    // ****************************** //
    // *** Method 4 : cyclic sort *** //
    // ****************************** //
    // time   = O(N)
    // space  = O(1)
      
    std::int32_t min_missing_pos_num_cyclic_sort(std::vector<std::int32_t>& vec)
    {
        // step 1 : cyclic sort
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            while(vec[n]>0 && vec[n]<=vec.size() && vec[n] != vec[vec[n]-1])
            {
                std::swap(vec[n], vec[vec[n]-1]);
            }
        }

        // step 2 : find missing
        for(std::uint32_t n=0; n!=vec.size(); ++n) 
        {
            if (vec[n] != n+1) return n+1;
        }
        return vec.size()+1; // remark 1
    }

    // ************************************************************************************* //
    // Cyclic sort is like a histogram, 
    // * instead of hist[n] = count 
    // * instead of hist[n] = true / false
    // * it takes   hist[n] = (n+1)th possible value
    //
    // Hence it is O(N) re-ordering (more than a sorting).
    // If a number n does     exist, then it can be found in hist[n-1]  i.e. hist[n-1] == n
    // If a number n does not exist, then hist[n-1] = something else    i.e. hist[n-1] != n
    // 
    // Now we re-use input vec as the histogram, this is called in-place cyclic sort :
    // * start  from vec[0] 
    //   if vec[0] == 1, then it is in right place
    //   if vec[0] != 1, then put it in right place, by swapping vec[0] with vec[vec[0]-1]
    //
    // * repeat with new value of vec[0], it stops only :
    //   if vec[0] == 1 after a particular swap, or
    //   if vec[0] == vec[vec[0]-1] <--- imply there are duplicated number vec[vec[0]-1]
    //   in either case, proceed with vec[1] ...
    // ************************************************************************************* //
}
