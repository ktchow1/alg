#include<iostream>
#include<utility.h>
#include<string_problem.h>

  
void test_string_problem()
{
    std::uint32_t num_trial = 10000;
  
    benchmark<1>("longest_non_duplicate_substr",           
                 std::bind(gen_random_str, 20, 26), 
                 std::bind(alg::longest_non_duplicated_substr,     _1),      
                 std::bind(alg::longest_non_duplicated_substr_bmk, _1),
                 num_trial); 

    benchmark<1>("longest_odd_palindrome_substr",           
                 std::bind(gen_random_palindrome, 200, 5), 
                 std::bind(alg::longest_odd_palindrome_substr,     _1),
                 std::bind(alg::longest_odd_palindrome_substr_bmk, _1),
                 num_trial); 


    // **************************** //
    // *** For repeat and debug *** //
    // **************************** //
/*    
    std::string str = "cbabacccbcccccccbcabc";
    std::uint32_t ans = alg::longest_odd_palindrome_substr_optimized(str);
    std::cout << "\n[REPEAT CASE] ans = " << ans; */
}
  
