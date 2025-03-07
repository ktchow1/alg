#include<iostream>
#include<utility.h>
#include<subseq.h>


void test_subseq_sum()
{
    std::uint32_t num_trial = 1000;

    benchmark<1>("max_subseq_sum",           
                 std::bind(gen_random_vec<std::int32_t>, 50, -30, +30), 
                 std::bind(alg::max_subseq_sum,     _1),      
                 std::bind(alg::max_subseq_sum_bmk, _1), 
                 num_trial); 

    benchmark<1>("max_subseq_prd",           
                 std::bind(gen_random_vec<std::int32_t>, 10, -11, 11), 
                 std::bind(alg::max_subseq_prd,     _1),      
                 std::bind(alg::max_subseq_prd_bmk, _1), 
                 num_trial); 

    benchmark<1>("max_non_contiguous_subseq_sum",           
                 std::bind(gen_random_vec<std::int32_t>, 50, -30, +30), 
                 std::bind(alg::max_non_contiguous_subseq_sum,     _1),      
                 std::bind(alg::max_non_contiguous_subseq_sum_bmk, _1), 
                 num_trial); 

    benchmark<1>("max_non_contiguous_subseq_sum",           
                 std::bind(gen_random_vec<std::int32_t>, 50, -30, +0), // all negative
                 std::bind(alg::max_non_contiguous_subseq_sum,     _1),     
                 std::bind(alg::max_non_contiguous_subseq_sum_bmk, _1),
                 num_trial); 
    
    benchmark<1>("count_target_subseq_sum",           
                 std::bind(gen_random_vec<std::int32_t>, 30, -20, +20), 
                 std::bind(alg::count_target_subseq_sum,     _1, 20),
                 std::bind(alg::count_target_subseq_sum_bmk, _1, 20),  
                 num_trial); 

    benchmark<1>("count_target_divisible_subseq_sum", 
                 std::bind(gen_random_vec<std::int32_t>, 30, -20, +20), 
                 std::bind(alg::count_target_divisible_subseq_sum,     _1, 8),
                 std::bind(alg::count_target_divisible_subseq_sum_bmk, _1, 8),
                 num_trial); 

    benchmark<1>("longest_target_subseq_sum", 
                 std::bind(gen_random_vec<std::int32_t>, 30, -20, +20), 
                 std::bind(alg::longest_target_subseq_sum,     _1, 20),
                 std::bind(alg::longest_target_subseq_sum_bmk, _1, 20),
                 num_trial); 

    benchmark<1>("count_less_than_target_subseq_sum", 
                 std::bind(gen_random_vec<std::uint32_t>, 30, 1, 60), 
                 std::bind(alg::count_less_than_target_subseq_sum,     _1, 100),
                 std::bind(alg::count_less_than_target_subseq_sum_bmk, _1, 100),
                 num_trial); 

    benchmark<1>("count_less_than_target_subseq_prd", 
                 std::bind(gen_random_vec<std::uint32_t>, 10, 1, 12), 
                 std::bind(alg::count_less_than_target_subseq_prd,     _1, 1234),
                 std::bind(alg::count_less_than_target_subseq_prd_bmk, _1, 1234), 
                 num_trial); 
    
}


