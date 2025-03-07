#pragma once
#include<iostream>
#include<algorithm>
#include<optional>


// ********************************************************************************************************* //
// Given a container of sorted values : 
// * std::lower_bound finds the first iter, before which, when we insert value, will keep all values sorted
// * std::upper_bound finds the  last iter, before which, when we insert value, will keep all values sorted
//
// Equivalently, we can also view like this : 
// * if value does not exist in original container, there will be no difference between lower_upper_bound.
// * if value does     exist in original container, then ...
//   std::lower_bound will return iter pointing to first existing value
//   std::upper_bound will return iter pointing to element right after last existing value
//
// For example : insert 5 into [1,1,2,3,3,5,5,5,5,5,7,8,9,9,9]
//                                        ^         ^---------- std::upper_bound returns this
//                                        +-------------------- std::lower_bound returns this 
// ********************************************************************************************************* //
namespace alg
{
    template<typename ITER>
    ITER lower_bound(ITER begin, ITER end, typename std::iterator_traits<ITER>::value_type& value)
    {
        // 1. check edge case
        if (begin == end) return end;
        ITER last =  end - 1;

        // 3. chech stop condition
        while(std::distance(begin, last) > 1)
        {
            // 4. mid point
            ITER mid = begin;
            std::advance(mid, std::distance(begin, last)/2);

            // 5. bisection (equality comes with <)
            if (value <= *mid) last  = mid; 
            else               begin = mid; 
        }

        // 6. answer
        if      (value <= *begin) return begin;
        else if (value <= *last)  return last;
        else                      return end;
    }


    template<typename ITER>
    ITER upper_bound(ITER begin, ITER end, typename std::iterator_traits<ITER>::value_type& value)
    {
        // 1. check edge case
        if (begin == end) return end;
        ITER last =  end - 1;
        ITER cbegin = begin;

        // 3. chech stop condition
        while(std::distance(begin, last) > 1)
        {
            // 4. mid point
            ITER mid = begin;
            std::advance(mid, std::distance(begin, last)/2);

            // 5. bisection (equality comes with >)
            if (value >= *mid) begin = mid; 
            else               last  = mid; 
        }

        // 6. answer
        if      (value >= *last)  return last  + 1; // BUG : remember this
        else if (value >= *begin) return begin + 1; // BUG : remember this
        else                      return cbegin;    // BUG : remember this
    }
}


namespace alg
{
    template<typename ITER>
    bool next_permutation(ITER begin, ITER end)
    {
        // ************************** //
        // *** Step 1 : find drop *** //
        // ************************** //
        ITER drop_iter = end;

        for(ITER iter=end-1; iter!=begin; --iter) 
        {
            if (*(iter-1) < *iter)
            {
                drop_iter = iter-1;
                break; // BUG1 : Don't miss this
            }
        }
        if (drop_iter == end) return false;


        // ********************************************************* //
        // *** Step 2 : find min element greater than *drop_iter *** //
        // ********************************************************* //
        ITER swap_iter = end;

        for(ITER iter=drop_iter+1; iter!=end; ++iter)
        {
            if (*iter > *drop_iter)
            {
                if (swap_iter == end || 
                   *swap_iter > *iter)
                {
                    swap_iter = iter;
                }
            }
        }
        if (swap_iter == end) return false;
        std::swap(*drop_iter, *swap_iter);


        // ********************************************** //
        // *** Step 3 : sort RHS subseq from the drop *** //
        // ********************************************** //
    //  std::sort(drop_iter,   end); // BUG2  
        std::sort(drop_iter+1, end); 
        return true;
    }
}


// ********************************************* //
// *** A naive version hashmap, which can be *** //
// *** extended to lockfree multi-thread     *** //
// ********************************************* //
namespace alg
{
    template<typename K, typename V> 
    class hashmap
    {
    private:
        struct cell
        {
            bool m_flag;
            K    m_key;
            V    m_value;
        };

    public:
        explicit hashmap(std::uint32_t size) // size to be power of 2 for the mask to work
                        : m_size(size), 
                          m_mask(size-1), 
                          m_impl(new cell[size])
        {
            for(std::uint32_t n=0; n!=size; ++n) 
            {
                m_impl[n].m_flag = false;
            }
        }

        ~hashmap()
        {
            if (m_impl) delete [] m_impl;
        }

        bool set(const K& key, const V& value)
        {
            auto hash = std::hash<K>{}(key);        
            for(std::uint32_t n=0; n!=m_size; ++n)
            {
                auto m = (hash+n) & m_mask;   // Why "+n"? To resolve hash collision by looking at next available cell
                if (m_impl[m].m_flag == false ||
                    m_impl[m].m_key  == key)
                {
                    m_impl[m].m_flag  = true;
                    m_impl[m].m_key   = key;
                    m_impl[m].m_value = value; 
                    return true; 
                }
            }
            return false;
        }

        std::optional<V> get(const K& key) const
        {        
            auto hash = std::hash<K>{}(key);        
            for(std::uint32_t n=0; n!=m_size; ++n)
            {
                auto m = (hash+n) & m_mask;
                if      (m_impl[m].m_flag == false) { return std::nullopt;                          }
                else if (m_impl[m].m_key  == key)   { return std::make_optional(m_impl[m].m_value); }
                else continue;
            }
            return std::nullopt;
        }

    private:
        std::uint32_t m_size;
        std::uint32_t m_mask;
        cell*         m_impl;
    };
}
