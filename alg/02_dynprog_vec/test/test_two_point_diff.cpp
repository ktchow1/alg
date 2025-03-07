#include<iostream>
#include<utility.h>
#include<two_point_diff.h>


void test_two_point_diff()
{
    std::uint32_t num_trial = 1000;

    benchmark<1>("max_profit",           
                 std::bind(gen_random_vec<std::int32_t>, 30, 100, 500), 
                 std::bind(alg::max_profit,     _1),     
                 std::bind(alg::max_profit_bmk, _1),
                 num_trial); 
    
    benchmark<1>("count_target_profit", 
                 std::bind(gen_random_vec<std::int32_t>, 100, 100, 300),
                 std::bind(alg::count_target_profit,     _1, 40),     
                 std::bind(alg::count_target_profit_bmk, _1, 40),
                 num_trial); 

    benchmark<1>("count_target_abs_profit", 
                 std::bind(gen_random_vec<std::int32_t>, 100, 100, 300),  
                 std::bind(alg::count_target_abs_profit,     _1, 40),      
                 std::bind(alg::count_target_abs_profit_bmk, _1, 40), 
                 num_trial); 
}
