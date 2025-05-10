#include<iostream>
#include<iomanip>
#include<archax.h>
#include<utility.h>


void test_archax()
{
    std::uint32_t num_trial = 5000;
    std::uint32_t size = 100;
    std::int32_t  min  = -3;
    std::int32_t  max  = 40;

    benchmark<1>("archax : method 1 vs 2",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_sort, _1),     
                 std::bind(archax::min_missing_pos_num_hist, _1),
                 num_trial); 
  
    benchmark<1>("archax : method 1 vs 3",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_sort, _1),     
                 std::bind(archax::min_missing_pos_num_mark, _1),
                 num_trial); 

    benchmark<1>("archax : method 1 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_sort,        _1),     
                 std::bind(archax::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 

    benchmark<1>("archax : method 2 vs 3",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_hist, _1),     
                 std::bind(archax::min_missing_pos_num_mark, _1),
                 num_trial); 

    benchmark<1>("archax : method 2 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_hist,        _1),     
                 std::bind(archax::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 

    benchmark<1>("archax : method 3 vs 4",           
                 std::bind(gen_random_vec<std::int32_t>, size, min, max), 
                 std::bind(archax::min_missing_pos_num_mark,        _1),     
                 std::bind(archax::min_missing_pos_num_cyclic_sort, _1),
                 num_trial); 
}
