#include<iostream>
#include<iomanip>
#include<cassert>
#include<dp_matrix_and_graph.h>
#include<utility.h>


void test_coin_change()
{
    std::uint32_t num_trial;
           
    num_trial = 100;
    benchmark<1>("min_coin_change --------- recursive vs iterative (graph) with ans=inf",
                 std::bind(gen_random_coins, 3, 2, 9), 
                 std::bind(alg::min_coin_change_recursive_in_graph, _1, 25),
                 std::bind(alg::min_coin_change_iterative_in_graph, _1, 25),      
                 num_trial);
 
    num_trial = 1000;
    benchmark<1>("min_coin_change --------- recursive vs iterative (matrix)", 
                 std::bind(gen_random_coins, 4, 2, 30), 
                 std::bind(alg::min_coin_change_recursive_in_matrix, _1, 100),
                 std::bind(alg::min_coin_change_iterative_in_matrix, _1, 100),      
                 num_trial);
          
    num_trial = 1000;
    benchmark<1>("min_coin_change --------- recursive vs iterative (matrix) with ans=inf", 
                 std::bind(gen_random_coins, 2, 7, 19), 
                 std::bind(alg::min_coin_change_recursive_in_matrix, _1, 73),
                 std::bind(alg::min_coin_change_iterative_in_matrix, _1, 73),      
                 num_trial);
  
    num_trial = 1000;
    benchmark<1>("min_coin_change --------- graph vs matrix (iterative)",           
                 std::bind(gen_random_coins, 8, 1, 60), 
                 std::bind(alg::min_coin_change_iterative_in_graph,  _1, 600),      
                 std::bind(alg::min_coin_change_iterative_in_matrix, _1, 600),
                 num_trial);

    num_trial = 1000;
    benchmark<1>("min_coin_change --------- graph vs matrix (iterative) with ans=inf",           
                 std::bind(gen_random_coins, 2, 7, 19), 
                 std::bind(alg::min_coin_change_iterative_in_graph,  _1, 173),      
                 std::bind(alg::min_coin_change_iterative_in_matrix, _1, 173),
                 num_trial);
    
    num_trial = 1000;
    benchmark<1>("count_coin_change ------- recursive vs iterative (matrix)", 
                 std::bind(gen_random_coins, 8, 1, 30), 
                 std::bind(alg::count_coin_change_recursive_in_matrix, _1, 100),
                 std::bind(alg::count_coin_change_iterative_in_matrix, _1, 100),      
                 num_trial);
}


void test_knapsack()
{
    std::uint32_t num_trial = 1000;
    benchmark<1>("knapack ----------------- graph vs matrix (iterative)",           
                 std::bind(gen_random_objects, 15, 10, 40, 1, 80), 
                 std::bind(alg::knapsack_iterative_in_graph,  _1, 400),      
                 std::bind(alg::knapsack_iterative_in_matrix, _1, 400),
                 num_trial); 
}


void test_job_schedule()
{
    std::uint32_t num_trial = 1000;
    benchmark<1>("job_schedule ------------ graph vs matrix (iterative)",           
                 std::bind(gen_random_jobs, 30, 2, 20, 2, 30, 10, 250), 
                 std::bind(alg::job_schedule_iterative_in_graph,  _1),      
                 std::bind(alg::job_schedule_iterative_in_matrix, _1),
                 num_trial); 
}


void test_equal_partition()
{
    std::uint32_t num_trial = 1000;
    benchmark<1>("equal_partition --------- graph vs matrix (iterative)",           
                 std::bind(gen_random_vec<std::uint32_t>, 25, 2, 50), 
                 std::bind(alg::equal_partition_iterative_in_graph,  _1),      
                 std::bind(alg::equal_partition_iterative_in_matrix, _1), 
                 num_trial);  
}


void test_box_stacking()
{
    std::uint32_t num_trial = 1000;
    benchmark<1>("box_stacking ------------ graph vs matrix (iterative)",           
                 std::bind(gen_random_boxes, 80, 5, 50), 
                 std::bind(alg::box_stacking_iterative_in_graph,  _1),      
                 std::bind(alg::box_stacking_iterative_in_matrix, _1), 
                 num_trial); 
}


void test_bin_packing()
{
    std::uint32_t num_trial    = 100;
    std::uint32_t num_obj_min  =  10; 
    std::uint32_t num_obj_max  =  20;
    std::uint32_t size_obj_min =  50;
    std::uint32_t size_obj_max = 100;
    std::uint32_t size_bin_min = 100;
    std::uint32_t size_bin_max = 200;  

    benchmark<1>("bin_packing ------------- graph vs matrix (iterative)",           
                 std::bind(gen_random_bins,  num_obj_min,  num_obj_max,
                                            size_obj_min, size_obj_max,
                                            size_bin_min, size_bin_max),
                 std::bind(alg::bin_packing_iterative_in_graph,  _1),      
                 std::bind(alg::bin_packing_iterative_in_matrix, _1), 
                 num_trial); 
}


void test_dp_matrix_and_graph()
{
    test_coin_change();
    test_knapsack();
    test_job_schedule();
    test_equal_partition();
    test_box_stacking();
    test_bin_packing();
}
