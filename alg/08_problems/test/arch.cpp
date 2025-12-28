#include<iostream>
#include<iomanip>
#include<arch.h>
#include<utility.h>


void test_arch()
{
    std::uint32_t num_trial = 5000;
    std::uint32_t size = 100;
    std::int32_t  min  = -3;
    std::int32_t  max  = 40;

    benchmark<1>("arch : method 1 vs 2",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_sort, _1),     
                 std::bind(arch::min_missing_pos_num_hist, _1),
                 num_trial); 
  
    benchmark<1>("arch : method 1 vs 3",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_sort, _1),     
                 std::bind(arch::min_missing_pos_num_mark, _1),
                 num_trial); 

    benchmark<1>("arch : method 1 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_sort,        _1),     
                 std::bind(arch::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 

    benchmark<1>("arch : method 2 vs 3",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_hist, _1),     
                 std::bind(arch::min_missing_pos_num_mark, _1),
                 num_trial); 

    benchmark<1>("arch : method 2 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_hist,        _1),     
                 std::bind(arch::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 

    benchmark<1>("arch : method 3 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(arch::min_missing_pos_num_mark,        _1),     
                 std::bind(arch::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 
}
