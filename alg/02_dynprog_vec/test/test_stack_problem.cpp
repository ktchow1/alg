#include<iostream>
#include<utility.h>
#include<stack_problem.h>

  
void test_stack_problem()
{
    std::uint32_t num_trial = 10000;

    benchmark<1>("shortest_unsorted_subseq",           
                 std::bind(gen_random_partial_sorted_vec<std::uint32_t>, 300, 0, 200), 
                 std::bind(alg::shortest_unsorted_subseq,     _1),         
                 std::bind(alg::shortest_unsorted_subseq_bmk, _1),
                 num_trial); 

    benchmark<1>("count_stroke_in_histogram",           
                 std::bind(gen_random_vec<std::uint32_t>, 300, 0, 50), 
                 std::bind(alg::count_stroke_in_histogram,     _1),       
                 std::bind(alg::count_stroke_in_histogram_bmk, _1),
                 num_trial); 

    benchmark<1>("biggest_rect_in_hist",           
                 std::bind(gen_random_vec<std::uint32_t>, 100, 0, 80),
                 std::bind(alg::biggest_rect_in_hist,     _1), 
                 std::bind(alg::biggest_rect_in_hist_bmk, _1),
                 num_trial); 

    benchmark<1>("total_trapped_water",           
                 std::bind(gen_random_vec<std::uint32_t>, 50, 0, 30),
                 std::bind(alg::total_trapped_water, _1),   
                 std::bind(alg::total_trapped_water, _1), // no benchmark
                 num_trial);  
}
  
