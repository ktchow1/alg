#include<iostream>
#include<utility.h>
#include<two_point_sum.h>


void test_two_point_sum()
{
    std::uint32_t num_trial = 1000;

    benchmark<1>("max_2_point_sum_distance",           
                 std::bind(gen_random_vec<std::int32_t>, 100, -100, +100),
                 std::bind(alg::max_2_point_sum_distance,     _1),     
                 std::bind(alg::max_2_point_sum_distance_bmk, _1),
                 num_trial);

    benchmark<1>("max_2_point_sum_with_equal_digit_sum",           
                 std::bind(gen_random_vec<std::uint32_t>, 50, 0, +1000), 
                 std::bind(alg::max_2_point_sum_with_equal_digit_sum,     _1),     
                 std::bind(alg::max_2_point_sum_with_equal_digit_sum_bmk, _1),
                 num_trial); 

    benchmark<1>("check_target_2_point_sum_in_1_sorted_vec", 
                 std::bind(gen_random_sorted_vec<std::int32_t>, 100, -6000, +6000),  
                 std::bind(alg::check_target_2_point_sum_in_1_sorted_vec,     _1, 40),      
                 std::bind(alg::check_target_2_point_sum_in_1_sorted_vec_bmk, _1, 40),          
                 num_trial); 
  
    benchmark<2>("check_target_2_point_sum_in_2_sorted_vec", 
                 std::bind(gen_random_sorted_vec<std::int32_t>, 100, -6000, +6000),
                 std::bind(alg::check_target_2_point_sum_in_2_sorted_vec,     _1, _2, 40),      
                 std::bind(alg::check_target_2_point_sum_in_2_sorted_vec_bmk, _1, _2, 40),          
                 num_trial);    
    
    benchmark<1>("count_target_2_point_sum", 
                 std::bind(gen_random_vec<std::int32_t>, 50, -40, +40), 
                 std::bind(alg::count_target_2_point_sum,     _1, 50),     
                 std::bind(alg::count_target_2_point_sum_bmk, _1, 50),         
                 num_trial); 
    
    benchmark<1>("count_target_3_point_sum", 
                 std::bind(gen_random_vec<std::int32_t>, 50, -40, +40),  
                 std::bind(alg::count_target_3_point_sum,     _1, 75),     
                 std::bind(alg::count_target_3_point_sum_bmk, _1, 75),         
                 num_trial); 

    benchmark<1>("count_target_4_point_sum", 
                 std::bind(gen_random_vec<std::int32_t>, 50, -40, +40),  
                 std::bind(alg::count_target_4_point_sum,     _1, 100),      
                 std::bind(alg::count_target_4_point_sum_bmk, _1, 100),          
                 num_trial);  
}
