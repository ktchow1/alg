#include<iostream>
#include<interview.h>

// *** 01_algorithm *** //
void test_arithmetic();
void test_bisection();
void test_stack();
void test_list();
void test_tree();
void test_tree_variant();
void test_sorting();

// *** 02_dynprog_vec *** //
void test_two_point_sum();
void test_two_point_diff();
void test_subseq_sum();
void test_string_problem();
void test_stack_problem();
void test_sorting_problem();

// *** 03_dynprog *** //
void test_dp_matrix_and_graph();
void test_dp_matrix_only();

// *** 04_fundalmental *** //
void test_cast();
void test_constexpr();
void test_constexpr_map();
void test_exception();
void test_memory_allocator();
void test_memory_deleter();
void test_memory_new_operator();
void test_memory_placement_new();
void test_memory_shared_ptr();
void test_memory_unique_ptr();
void test_optional();
void test_polymophism();
void test_rvalue();
void test_std_alg();
void test_std_container();
void test_std_layout();
void test_timer();
  
// *** 05_template *** //
void test_array();
void test_function();
void test_metaprog();
void test_optional_ref();
void test_shared_ptr();
void test_template();
void test_template_specialization(); 
void test_traits();
void test_tuple_idx_seq();
void test_tuple();
void test_type_erasure();
void test_type_erasure_example();
void test_unique_ptr();
void test_variadic();
void test_variant_basic();
void test_variant_overloading_lambda();  
void test_variant_visit();

// *** 06_threading *** //
void test_thread_create();
void test_thread_stop_source();
void test_thread_timing();
void test_spinlock_mutex();
void test_shared_mutex();
void test_semaphore();
void test_singleton();
void test_synchronization();
void test_promise_future();

// *** 06_threading (mpmcq & threadpool) *** //
void test_mpmcq();
void test_threadpool();

// *** 07_cpp20 *** //
void test_spaceship();
void test_range();
void test_concepts_apply();
void test_concepts_define();
void test_concepts_define_examples();



inline void banner(const std::string str)
{
    std::string s(str.size()+8,'*');
    std::cout << "\n";
    std::cout << "\n" << s;
    std::cout << "\n" << "*** " << str << " ***";
    std::cout << "\n" << s << std::flush;
}

int main()
{
//  banner("01_algorithm");
//  test_arithmetic();
//  test_bisection();
//  test_stack();
//  test_list();
//  test_tree();
//  test_tree_variant();
//  test_sorting();
//
//  banner("02_dynprog_vec");
//  test_two_point_sum();
//  test_two_point_diff();
//  test_subseq_sum();
//  test_string_problem();
//  test_stack_problem();
//  test_sorting_problem();
//    
//  banner("03_dynprog");
//  test_dp_matrix_and_graph();
//  test_dp_matrix_only();  
//  
//  banner("04_fundalmental");
//  test_cast();
//  test_constexpr();
//  test_constexpr_map();
//  test_exception();
//  test_memory_allocator();
//  test_memory_deleter();
//  test_memory_new_operator();
//  test_memory_placement_new();
//  test_memory_shared_ptr();
//  test_memory_unique_ptr();
//  test_optional();
//  test_polymophism();
//  test_rvalue();
//  test_std_alg();
//  test_std_container();
//  test_std_layout();
//  test_timer();
//
//  banner("05_template");
//  test_array();
//  test_function();
//  test_metaprog();
//  test_optional_ref();
//  test_shared_ptr();
//  test_template(); 
//  test_template_specialization(); 
//  test_traits();
//  test_tuple_idx_seq();
//  test_tuple();
//  test_type_erasure_example();
//  test_type_erasure();
//  test_unique_ptr();
//  test_variadic(); 
//  test_variant_basic();  
//  test_variant_overloading_lambda();  
//  test_variant_visit(); 
      
//  banner("06_threading");
//  test_thread_create();
//  test_thread_stop_source();
//  test_thread_timing();
//  test_spinlock_mutex();
    test_shared_mutex();
//  test_semaphore();
//  test_singleton();
//  test_synchronization();
//  test_promise_future();

//  banner("06_threading - mpmcq & threadpool");
//  test_mpmcq();
//  test_threadpool();

//  banner("07_cpp20");  
//  test_spaceship();
//  test_range();
//  test_concepts_apply();
//  test_concepts_define();
//  test_concepts_define_examples();

//  banner("08_interview");
//  interview::test_alphagrep_max_min_diff_in_subset();
//  interview::test_alphagrep_count_k_odd_subseq(); 
//  interview::test_alphagrep_count_decreasing_triplet();
//  interview::test_alphagrep_count_step_function();
//  interview::test_apt3();
//  interview::test_apt4();
//  interview::test_apt5();
//  interview::test_archax_min_missing_pos_num();
//  interview::test_citadel_matrix_spiral();
//  interview::test_citadel_K_merge();
//  interview::test_citadel_max_rook_sum();
//  interview::test_epoch_state();
//  interview::test_optiver_tree_checker();
//  interview::test_smarket_subseq_product();
//  interview::test_smarket_point_puzzle();
  
    std::cout << "\n\n\n";
    return 0;
}
