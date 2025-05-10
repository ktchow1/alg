#include<iostream>
#include<iomanip>
#include<alphagrep.h>
#include<utility.h>


void test_alphagrep()
{
    // Question 4
    {
        std::uint32_t num_trial = 800;

        benchmark<1>("alphagrep : count_step_funcion",           
                     std::bind(gen_random_bool_vec, 80), 
                     std::bind(alphagrep::count_step_function,            _1),     
                     std::bind(alphagrep::count_step_function_exhaustive, _1),
                     num_trial); 
    }
    // Question 2
    {
        std::uint32_t num_trial = 800;
        std::uint32_t max_num_of_odd = 5;

        benchmark<1>("alphagrep : count_k_odd_subseq (2-ptrs)",           
                     std::bind(gen_random_vec<std::uint32_t>, 80, 0, 100), 
                     std::bind(alphagrep::count_k_odd_subseq_2_ptrs,     _1, max_num_of_odd),     
                     std::bind(alphagrep::count_k_odd_subseq_exhaustive, _1, max_num_of_odd),
                     num_trial); 

        benchmark<1>("alphagrep : count_k_odd_subseq (dynprog)",           
                     std::bind(gen_random_vec<std::uint32_t>, 80, 0, 100), 
                     std::bind(alphagrep::count_k_odd_subseq_dynprog,    _1, max_num_of_odd),     
                     std::bind(alphagrep::count_k_odd_subseq_exhaustive, _1, max_num_of_odd),
                     num_trial); 
    }
    // Question 2 - edge case
    {
        std::uint32_t num_trial = 100;
        std::uint32_t max_num_of_odd = 0;

        benchmark<1>("alphagrep : count_k_odd_subseq (2-ptrs), edge case",           
                     std::bind(gen_random_vec<std::uint32_t>, 80, 0, 100), 
                     std::bind(alphagrep::count_k_odd_subseq_2_ptrs,     _1, max_num_of_odd),     
                     std::bind(alphagrep::count_k_odd_subseq_exhaustive, _1, max_num_of_odd),
                     num_trial); 

        benchmark<1>("alphagrep : count_k_odd_subseq (dynprog), edge case",           
                     std::bind(gen_random_vec<std::uint32_t>, 80, 0, 100), 
                     std::bind(alphagrep::count_k_odd_subseq_dynprog,    _1, max_num_of_odd),     
                     std::bind(alphagrep::count_k_odd_subseq_exhaustive, _1, max_num_of_odd),
                     num_trial); 
    }
    // Question 3
    {
        std::uint32_t num_trial = 800;

        benchmark<1>("alphagrep : count_decreasing_triplet",           
                     std::bind(gen_random_vec<std::uint32_t>, 80, 0, 100), 
                     std::bind(alphagrep::count_decreasing_triplet_heuristic,  _1),     
                     std::bind(alphagrep::count_decreasing_triplet_exhaustive, _1),
                     num_trial); 
    }
    // Question 1
    {
        std::uint32_t num_trial = 800;
        std::uint32_t subseq_size = 6;

        benchmark<1>("alphagrep : max_min_diff_in_subseq (dynprog)", // even work when ans = 0          
                     std::bind(gen_random_mono_increase_vec, 40, 2, 30), 
                     std::bind(alphagrep::max_min_diff_in_subseq_iterative, _1, subseq_size),     
                     std::bind(alphagrep::max_min_diff_in_subseq_recursive, _1, subseq_size),
                     num_trial); 

        benchmark<1>("alphagrep : max_min_diff_in_subseq (bisection)", // cannot work when ans = 0          
                     std::bind(gen_random_mono_increase_vec, 40, 2, 30), 
                     std::bind(alphagrep::max_min_diff_in_subseq_bisection, _1, subseq_size),     
                     std::bind(alphagrep::max_min_diff_in_subseq_iterative, _1, subseq_size),
                     num_trial); 
    }
}


