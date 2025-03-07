#include<iostream>
#include<metaprog.h>
#include<utility.h>


void test_metaprog()
{
    static_assert(alg::less_than<2,4>() == true);
    static_assert(alg::less_than<8,4>() == false);
    static_assert(alg::abs_diff<2,4>() == 2);
    static_assert(alg::abs_diff<8,4>() == 4);
    static_assert(alg::abs_diff2<200,400>() == 200);
    static_assert(alg::abs_diff2<800,400>() == 400);
    static_assert(alg::min<2,4>() == 2);
    static_assert(alg::max<8,4>() == 8);

    static_assert(alg::factorial<2>::value ==     2);
    static_assert(alg::factorial<3>::value ==     6);
    static_assert(alg::factorial<4>::value ==    24);
    static_assert(alg::factorial<5>::value ==   120);
    static_assert(alg::factorial<6>::value ==   720);
    static_assert(alg::factorial<7>::value ==  5040);
    static_assert(alg::factorial<8>::value == 40320);    
    static_assert(alg::fibonacci<3>::value ==     2);
    static_assert(alg::fibonacci<4>::value ==     3);
    static_assert(alg::fibonacci<5>::value ==     5);
    static_assert(alg::fibonacci<6>::value ==     8);
    static_assert(alg::fibonacci<7>::value ==    13);
    static_assert(alg::fibonacci<8>::value ==    21);

    static_assert(alg::min_item<1>::value == 1);
    static_assert(alg::min_item<1,2>::value == 1);
    static_assert(alg::min_item<2,3,4>::value == 2);
    static_assert(alg::min_item<3,4,5,6>::value == 3);
    static_assert(alg::min_item<4,5,6,7,8>::value == 4);
    static_assert(alg::max_item<1>::value == 1);
    static_assert(alg::max_item<1,2>::value == 2);
    static_assert(alg::max_item<2,3,4>::value == 4);
    static_assert(alg::max_item<3,4,5,6>::value == 6);
    static_assert(alg::max_item<4,5,6,7,8>::value == 8);

    static_assert(alg::max_profit_buy_at_begin<8,12>::value == 4);
    static_assert(alg::max_profit_buy_at_begin<2,8,12>::value == 10);
    static_assert(alg::max_profit_buy_at_begin<5,2,8,12>::value == 7);
    static_assert(alg::max_profit_buy_at_begin<5,2,8,18,12>::value == 13);
    static_assert(alg::max_profit_buy_at_begin<5,2,8,18,12,10>::value == 13);
    static_assert(alg::max_profit_buy_at_begin<8,5,2,8,18,12,10>::value == 10);

    static_assert(alg::max_profit<8,12>::value == 4);
    static_assert(alg::max_profit<2,8,12>::value == 10);
    static_assert(alg::max_profit<5,2,8,12>::value == 10);
    static_assert(alg::max_profit<5,2,8,18,12>::value == 16);
    static_assert(alg::max_profit<5,2,8,18,12,10>::value == 16);
    static_assert(alg::max_profit<8,5,2,8,18,12,10>::value == 16);

    static_assert(alg::max_subseq_sum<-10,-10,4,-10,-10,8,-10,-10>::value == 8);
    static_assert(alg::max_subseq_sum<-10,-10,4,4,1,-10,8,-10,-10>::value == 9);
    static_assert(alg::max_subseq_sum<-10,-10,4,4,1,-1,5,-10,8,-10,-10>::value == 13);

    static_assert(alg::reverse_boolean<false>::value              == 0, "failed to reverse boolean");
    static_assert(alg::reverse_boolean< true>::value              == 1, "failed to reverse boolean");
    static_assert(alg::reverse_boolean<false, false, true>::value == 4, "failed to reverse boolean");
    static_assert(alg::reverse_boolean<false,  true, true>::value == 6, "failed to reverse boolean");
    static_assert(alg::reverse_boolean<true,   true, true>::value == 7, "failed to reverse boolean");
    static_assert(alg::reverse_boolean<false, false, true, true>::value == 12, "failed to reverse boolean");
    print_summary("metaprogramming", "succeeded in compile time");
}
