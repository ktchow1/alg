#pragma once
#include<iostream>
#include<algorithm>
#include<functional>
#include<optional>
#include<numeric> // std::accumulate is not in <algorithm>, but in <numeric>
#include<ranges>


// ******************************************* //
// *** Naive implementation of std::reduce *** //
// ******************************************* //
// Most likely, std::iterator_traits<ITER>::value_type == T, but NOT necessary.
//
namespace alg
{   
    template<typename ITER, typename T, typename REDUCE>
    T reduce(ITER begin, ITER end, T initial, const REDUCE& reduction_fct)  // <--- work
//  T reduce(ITER begin, ITER end, T initial,       REDUCE& reduction_fct)  // <--- cannot compile
//  T reduce(ITER begin, ITER end, T initial,       REDUCE  reduction_fct)  // <--- work
    {
        T ans = initial;
        for(auto iter=begin; iter!=end; ++iter)
        {
            ans = reduction_fct(ans, *iter);
        }
        return ans;
    }
}


// ********************************************* //
// *** Various c++ ways to do vector average *** //
// ********************************************* //
namespace alg
{   
    inline std::optional<double> average_naive(const std::vector<std::int32_t>& vec)
    {
        if (vec.empty()) return std::nullopt;

        std::int32_t sum = 0;
        for(const auto& x:vec) sum += x;
        return static_cast<double>(sum) / vec.size();
    }

    inline std::optional<double> average_for_each(const std::vector<std::int32_t>& vec)
    {
        if (vec.empty()) return std::nullopt;

        std::int32_t sum = 0;
        std::for_each(vec.begin(), vec.end(), [&sum](const auto& x) { sum += x; }); // <--- need to specify the operation
        return static_cast<double>(sum) / vec.size();
    }

    inline std::optional<double> average_accumulate(const std::vector<std::int32_t>& vec)
    {
        if (vec.empty()) return std::nullopt;

        std::int32_t sum = std::accumulate(vec.begin(), vec.end(), (std::int32_t)0); // <--- need to specify the initial
        return static_cast<double>(sum) / vec.size();
    }

    inline std::optional<double> average_reduce(const std::vector<std::int32_t>& vec)
    {
        if (vec.empty()) return std::nullopt;

        std::int32_t sum = std::reduce(vec.begin(), vec.end(), (std::int32_t)0, [](auto x, auto y) { return x+y; }); // <--- need to specify the initial and operation
        return static_cast<double>(sum) / vec.size();
    }

    inline std::optional<double> average_reduce2(const std::vector<std::int32_t>& vec)
    {
        if (vec.empty()) return std::nullopt;

        std::int32_t sum = alg::reduce(vec.begin(), vec.end(), (std::int32_t)0, [](auto x, auto y) { return x+y; }); // <--- need to specify the initial and operation
        return static_cast<double>(sum) / vec.size();
    }

    // ************************** //
    // *** For c++23 or above *** //
    // ************************** //
//  inline std::optional<double> average_ranges(const std::vector<std::int32_t>& vec)
//  {
//      if (vec.empty()) return std::nullopt;
//
//      std::int32_t sum = std::ranges::fold_left(vec, (std::int32_t)0, std::plus{});
//      return static_cast<double>(sum) / vec.size();
//  }
}


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
