#include<cstdint>
#include<numeric>   // for std::iota
#include<vector>
#include<algorithm> // for std::next_permutation

#include<matrix.h>
#include<sorting.h>


// *********************** //
// *** 2020 Citadel HK *** //
// *********************** //
namespace citadel
{
    enum class direction 
    {
        L2R, DOWN, R2L, UP
    };

    template<typename T>
    void linear_tranverse(const alg::matrix<T>& mat, 
                          std::uint32_t y, 
                          std::uint32_t x, 
                          direction dir,
                          std::uint32_t sz, 
                          std::vector<T>& ans)
    {
        if (dir == direction::L2R)   for(std::uint32_t n=0; n!=sz; ++n) ans.push_back(mat(y, x+n));
        if (dir == direction::DOWN)  for(std::uint32_t n=0; n!=sz; ++n) ans.push_back(mat(y+n, x));
        if (dir == direction::R2L)   for(std::uint32_t n=0; n!=sz; ++n) ans.push_back(mat(y, x-n));
        if (dir == direction::UP)    for(std::uint32_t n=0; n!=sz; ++n) ans.push_back(mat(y-n, x));
    }

    template<typename T> 
    std::vector<T> spiral_traverse(const alg::matrix<T>& mat)
    {
        std::uint32_t co_y = 0;
        std::uint32_t co_x = 0;
        std::uint32_t sz_y = mat.size_y();
        std::uint32_t sz_x = mat.size_x();
        std::vector<T> ans;
        
        while(sz_y >= 2 && sz_x >= 2)
        {
            linear_tranverse(mat, co_y,        co_x,        direction::L2R , sz_x-1, ans);
            linear_tranverse(mat, co_y,        co_x+sz_x-1, direction::DOWN, sz_y-1, ans);
            linear_tranverse(mat, co_y+sz_y-1, co_x+sz_x-1, direction::R2L,  sz_x-1, ans);
            linear_tranverse(mat, co_y+sz_y-1, co_x,        direction::UP,   sz_y-1, ans);
            co_y += 1;
            co_x += 1;
            sz_y -= 2;
            sz_x -= 2;
        }
        if (sz_x == 1)
        {
            linear_tranverse(mat, co_y, co_x, direction::DOWN, sz_y, ans);
        }
        else if (sz_y == 1)
        {
            linear_tranverse(mat, co_y, co_x, direction::L2R, sz_x, ans);
        }
        return ans;
    }
}


// *********************** //
// *** 2021 Citadel UK *** //
// *********************** //
namespace citadel
{
    template<typename ITER, typename OITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
    void K_merge_pairwise(const std::vector<std::pair<ITER,ITER>>& ranges, OITER oiter)
    {
        if (ranges.size() == 0) return;

        // ************************ //
        // Step 1 : initialize acc0 //
        // ************************ //
        // Use 2 auxilary vectors :
        // - acc0 = accumulator before merge
        // - acc1 = accumulator  after merge
        //
        using vector_t = std::vector<typename std::iterator_traits<ITER>::value_type>;
        vector_t acc0(ranges[0].first, ranges[0].second);


        // ************************************** //
        // Step 2 : acc1 = merge(acc0, ranges[n]) //
        // ************************************** //
        for(std::uint32_t n=1; n!=ranges.size(); ++n) // works also for ranges.size() == 1
        {
            vector_t acc1;
            alg::merge<ITER, OITER, CMP>
            (   
                acc0.begin(), 
                acc0.end(), 
                ranges[n].first,
                ranges[n].second, 
                std::back_inserter(acc1)
            );
            acc0 = std::move(acc1);
        }


        // ***************************** //
        // Step 3 : copy to final output //
        // ***************************** //
        for(const auto& x:acc0)
        {
            *oiter = x;
            ++oiter;
        }
    }


    template<typename ITER, typename OITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
    void K_merge_all_at_once(const std::vector<std::pair<ITER,ITER>>& ranges, OITER oiter)
    {
        if (ranges.size() == 0) return;
        auto work_ranges = ranges;

        while(true)
        {
            // ********************************************* //
            // why use ITER*? 
            // 1. it points to any ranges' begin
            // 2. it helps to move ranges' begin towards end
            // ********************************************* //
            ITER* iter_ptr = nullptr; 
            for(auto& range:work_ranges)
            {
                if (range.first != range.second)
                {       
                    if (!iter_ptr) // i.e. first valid range in this loop
                    {
                        iter_ptr = &(range.first);
                    }
                    else if (CMP{}(*(range.first), **iter_ptr)) // BUG 1 : forget sorting data in test
                    {                                           // BUG 2 : forget using CMP
                        iter_ptr = &(range.first);              // BUG 3 : reverse order of values 
                    }
                }
            } 

            // ******************************* //
            // *** Update opt_iter or exit *** //
            // ******************************* //
            if (iter_ptr) 
            {
                *oiter = **iter_ptr;
                ++oiter;
                ++(*iter_ptr);
            }
            else return; // all ranges reach end
        }
    }
    

    // ********************** //
    // *** Benchmark algo *** //
    // ********************** //
    template<typename ITER, typename OITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
    void K_merge_all_in_one(const std::vector<std::pair<ITER,ITER>>& ranges, OITER oiter)
    {
        // *********************** //
        // Step 1 : concaternation //
        // *********************** //
        using vector_t = std::vector<typename std::iterator_traits<ITER>::value_type>;
        vector_t all_in_one;

        for(const auto range:ranges)
        {
            for(auto iter=range.first; iter!=range.second; ++iter)
            {
                all_in_one.push_back(*iter);
            }
        }

        // ************************ //
        // Step 2 : sort and output //
        // ************************ //
        std::sort
        (
            all_in_one.begin(), 
            all_in_one.end(), CMP{}
        );
        for(const auto& x:all_in_one) 
        {
            *oiter = x;
            ++oiter;
        }
    }
}


// ******************************************** //
// *** 2022 Citadel UK - Assignment problem *** //
// ******************************************** //
// Given cost matrix :
// * row n = agent n
// * col m = task m
//
// Find a task assignment to minimize total cost :
// * method 1 : exhaustive iteration    O(N!)
// * method 2 : exhaustive recursion    O(N!)  much slower than iteration (as there are duplications)
// * method 3 : Hungarian               O(N^3) non-doable from scratch
// * method 4 : simulated annealing     O(N)   non-optimal
//
// The output "ans" is a vector, such that :
// * ans[n] = task assigned to agent n
//
namespace citadel
{
    std::uint32_t find_cost(const alg::matrix<std::uint32_t>& cost_mat, const std::vector<std::uint32_t>& assignment)
    {
        if (cost_mat.size_y() != assignment.size()) return 0;

        std::uint32_t cost = 0;
        for(std::uint32_t n=0; n!=assignment.size(); ++n)
        {
            cost += cost_mat(n, assignment[n]);
        }
        return cost;
    }

    std::uint32_t assignment_by_exhaustive_iteration(const alg::matrix<std::uint32_t>& cost_mat)
    {
        if (cost_mat.size_y() != cost_mat.size_x()) return 0;
        if (cost_mat.size_y() == 0)                 return 0;

        std::vector<std::uint32_t> perm(cost_mat.size_y());
        std::iota(perm.begin(), perm.end(), 0); // filled as [0,1,2,...]

        std::vector<std::uint32_t> min_perm = perm;
        std::uint32_t min_cost = find_cost(cost_mat, perm);

        while(std::next_permutation(perm.begin(), perm.end()))
        {
            auto cost = find_cost(cost_mat, perm);
            if (min_cost > cost)
            {
                min_cost = cost;
                min_perm = perm; // cannot use std::move
            }
        }
    //  return min_perm; // alternatively, we can return the assignment
        return min_cost;
    }

  
    std::uint32_t assignment_by_exhaustive_recursive(const alg::matrix<std::uint32_t>& cost_mat)
    {   
        // ********************* //
        // *** Boundary case *** //
        // ********************* //
        if (cost_mat.size_y() != cost_mat.size_x()) return 0;
        if (cost_mat.size_y() == 0)                 return 0;
        if (cost_mat.size_y() == 1)                 return cost_mat(0,0);

        // ***************** //
        // *** Recursion *** //
        // ***************** //
        std::uint32_t min_cost = std::numeric_limits<std::uint32_t>::max();

        // Agent 0 picks one of the task ...
        for(std::uint32_t x=0; x!=cost_mat.size_x(); ++x) 
        {
            alg::matrix<std::uint32_t>  sub_cost_mat(cost_mat.size_y()-1, cost_mat.size_x()-1);
            for(std::uint32_t yy=0; yy!=sub_cost_mat.size_y(); ++yy)
            {
                for(std::uint32_t xx=0; xx!=x; ++xx)
                {
                    sub_cost_mat(yy,xx) = cost_mat(yy+1,xx);
                }
                for(std::uint32_t xx=x; xx!=sub_cost_mat.size_x(); ++xx)
                {
                    sub_cost_mat(yy,xx) = cost_mat(yy+1,xx+1);
                }
            }

            auto sub_min = assignment_by_exhaustive_recursive(sub_cost_mat);
            if (min_cost > cost_mat(0,x) + sub_min) 
                min_cost = cost_mat(0,x) + sub_min;
        }
        return min_cost;
    }
}
