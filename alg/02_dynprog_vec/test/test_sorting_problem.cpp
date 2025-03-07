#include<iostream>
#include<utility.h>
#include<sorting_problem.h>

  
void test_sorting_problem()
{
    std::uint32_t num_trial = 10000;
    std::uint32_t kth_order = 99;

    benchmark<1>("order_statistics",           
                 std::bind(gen_random_vec<std::uint32_t>, 200, 0, 100), 
                 std::bind(alg::order_statistics,     _1, kth_order),       
                 std::bind(alg::order_statistics_bmk, _1, kth_order),
                 num_trial); 
    
    benchmark<1>("min_number_adjacent_swap",           
                 std::bind(gen_random_swapped_order, 200),
                 std::bind(alg::min_number_adjacent_swap,        _1),   
                 std::bind(alg::min_number_adjacent_swap_bmk_UR, _1),
                 num_trial); 
}
  
