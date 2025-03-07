#include<iostream>
#include<cassert>
#include<cstdint>
#include<cmath>
#include<vector>
#include<tuple.h>
#include<utility.h>


void test_construct()
{
    alg::tuple<std::uint32_t,std::uint32_t,std::uint32_t> tup0
    {
        1, 2, 3
    };

    alg:: tuple<std::uint32_t,std::string,std::string,double> tup1
    {
        1, "ABC", "DEF", 3.14
    };

    alg:: tuple<std::uint32_t,std::vector<int>,std::vector<double>,double,std::string> tup2
    {
        1, std::vector{11,12,13}, std::vector<double>{0.123,0.234}, 2.718238, "hello"
    };

    static_assert(alg::tuple_size<decltype(tup0)>::value == 3, "failed to tuple_size");
    static_assert(alg::tuple_size<decltype(tup1)>::value == 4, "failed to tuple_size");
    static_assert(alg::tuple_size<decltype(tup2)>::value == 5, "failed to tuple_size");

    assert(alg::get<0>(tup0) == 1);
    assert(alg::get<1>(tup0) == 2);
    assert(alg::get<2>(tup0) == 3);
    assert(alg::get<0>(tup1) == 1);
    assert(alg::get<1>(tup1) == "ABC");
    assert(alg::get<2>(tup1) == "DEF");
    assert(alg::get<3>(tup1) == 3.14);
    assert(alg::get<0>(tup2) == 1);
    assert(alg::get<1>(tup2)[0] == 11);
    assert(alg::get<2>(tup2)[0] == 0.123);
    assert(alg::get<3>(tup2) == 2.718238);
    assert(alg::get<4>(tup2) == "hello");

    // *** make alg::tuple *** //
    auto tup3 = alg::make_alg_tuple
    (
        std::uint32_t{1}, std::uint32_t{2}, std::uint32_t{3}
    );

    auto tup4 = alg::make_alg_tuple
    (
        std::uint32_t{1}, std::string{"ABC"}, std::string{"DEF"}, double{3.14}
    );

    auto tup5 = alg::make_alg_tuple
    (
        std::uint32_t{1}, std::vector{11,12,13}, std::vector<double>{0.123,0.234}, double{2.718238}, std::string{"hello"}
    );

    static_assert(alg::tuple_size<decltype(tup3)>::value == 3, "failed to tuple_size");
    static_assert(alg::tuple_size<decltype(tup4)>::value == 4, "failed to tuple_size");
    static_assert(alg::tuple_size<decltype(tup5)>::value == 5, "failed to tuple_size");

    assert(alg::get<0>(tup3) == 1);
    assert(alg::get<1>(tup3) == 2);
    assert(alg::get<2>(tup3) == 3);
    assert(alg::get<0>(tup4) == 1);
    assert(alg::get<1>(tup4) == "ABC");
    assert(alg::get<2>(tup4) == "DEF");
    assert(alg::get<3>(tup4) == 3.14);
    assert(alg::get<0>(tup5) == 1);
    assert(alg::get<1>(tup5)[0] == 11);
    assert(alg::get<2>(tup5)[0] == 0.123);
    assert(alg::get<3>(tup5) == 2.718238);
    assert(alg::get<4>(tup5) == "hello");
    print_summary("tuple - construct", "succeeded in compile time");
}


// ******************************************************** //
// *** Start from here, use std::tuple (not alg::tuple) *** //
// ******************************************************** //
void test_make_tuple_and_tie() // and structural_binding
{
    bool b,b0,b1;
    char c,c0,c1;
    int  i,i0,i1;
   
    // case 1 : make_tuple<T>
    b = true, c = 'a', i = 123;
    auto tup0 = alg::make_tuple(b,c,i);
    assert(std::get<0>(tup0) == true);
    assert(std::get<1>(tup0) == 'a');
    assert(std::get<2>(tup0) == 123);

    b = false, c = 'b', i = 234;
    assert(std::get<0>(tup0) == true);
    assert(std::get<1>(tup0) == 'a');
    assert(std::get<2>(tup0) == 123); // modify failed

    // case 2 : make_tuple<T&>
    b = true, c = 'a', i = 123;
    auto tup1 = alg::make_tuple<bool&,char&,int&>(b,c,i);
    assert(std::get<0>(tup1) == true);
    assert(std::get<1>(tup1) == 'a');
    assert(std::get<2>(tup1) == 123);

    b = false, c = 'b', i = 234;
    assert(std::get<0>(tup1) == false);
    assert(std::get<1>(tup1) == 'b');
    assert(std::get<2>(tup1) == 234); // modify succeed

    // case 3 : tie to tuple<T>
    alg::tie(b0,c0,i0) = tup0;
    assert(b0 == true);
    assert(c0 == 'a');
    assert(i0 == 123);

    std::get<0>(tup0) = false;
    std::get<1>(tup0) = 'b';
    std::get<2>(tup0) = 234;
    assert(b0 == true);
    assert(c0 == 'a');
    assert(i0 == 123); // tied variables cannot track tuple<T> change

    // case 4 : tie to tuple<T&>
    alg::tie(b1,c1,i1) = tup1;
    assert(b1 == false);
    assert(c1 == 'b');
    assert(i1 == 234);

    std::get<0>(tup1) = true;
    std::get<1>(tup1) = 'c';
    std::get<2>(tup1) = 345;
    assert(b1 == false);
    assert(c1 == 'b');
    assert(i1 == 234); // tied variables cannot track tuple<T&> change

    // case 5 : structural binding of variables to tuple<T>
    auto [b2,c2,i2] = tup0;
    assert(b2 == false);
    assert(c2 == 'b');
    assert(i2 == 234);

    std::get<0>(tup0) = true;
    std::get<1>(tup0) = 'c';
    std::get<2>(tup0) = 345;
    assert(b2 == false);
    assert(c2 == 'b');
    assert(i2 == 234); // modify failed

    // case 6 : structural binding of references to tuple<T>
    auto& [b3,c3,i3] = tup0;
    assert(b3 == true);
    assert(c3 == 'c');
    assert(i3 == 345);

    std::get<0>(tup0) = false;
    std::get<1>(tup0) = 'd';
    std::get<2>(tup0) = 456;
    assert(b3 == false);
    assert(c3 == 'd');
    assert(i3 == 456); // modify succeed
    print_summary("tuple - make and tie", "succeeded in compile time");
}


void test_size_and_element()
{
    using T0 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using T1 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>>;
    using T2 = std::tuple<char>;
    using T3 = std::tuple<>;
    using T4 = alg::tuple_size<T0>::type;
    using T5 = alg::tuple_size<T1>::type;
    using T6 = alg::tuple_size<T2>::type;
    using T7 = alg::tuple_size<T3>::type;

    static_assert(std::is_same_v<T4, std::integral_constant<std::size_t,5>>, "failed to tuple_size");
    static_assert(std::is_same_v<T5, std::integral_constant<std::size_t,4>>, "failed to tuple_size");
    static_assert(std::is_same_v<T6, std::integral_constant<std::size_t,1>>, "failed to tuple_size");
    static_assert(std::is_same_v<T7, std::integral_constant<std::size_t,0>>, "failed to tuple_size");

    using U0 = alg::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using U1 = alg::tuple<char, std::uint32_t, std::string, std::pair<double,double>>;
    using U2 = alg::tuple<char>;
    using U3 = alg::tuple<>;
    using U4 = alg::tuple_size<U0>::type;
    using U5 = alg::tuple_size<U1>::type;
    using U6 = alg::tuple_size<U2>::type;
    using U7 = alg::tuple_size<U3>::type;

    static_assert(std::is_same_v<U4, std::integral_constant<std::size_t,5>>, "failed to tuple_size");
    static_assert(std::is_same_v<U5, std::integral_constant<std::size_t,4>>, "failed to tuple_size");
    static_assert(std::is_same_v<U6, std::integral_constant<std::size_t,1>>, "failed to tuple_size");
    static_assert(std::is_same_v<U7, std::integral_constant<std::size_t,0>>, "failed to tuple_size");

    using X0 = alg::tuple_element<0,T0>::type;
    using X1 = alg::tuple_element<1,T0>::type;
    using X2 = alg::tuple_element<2,T0>::type;
    using X3 = alg::tuple_element<3,T0>::type;
    using X4 = alg::tuple_element<4,T0>::type;

    static_assert(std::is_same_v<X0, char>,                     "failed to tuple_element");
    static_assert(std::is_same_v<X1, std::uint32_t>,            "failed to tuple_element");
    static_assert(std::is_same_v<X2, std::string>,              "failed to tuple_element");
    static_assert(std::is_same_v<X3, std::pair<double,double>>, "failed to tuple_element");
    static_assert(std::is_same_v<X4, double>,                   "failed to tuple_element");

    using Y0 = alg::tuple_element2<0,T0>::type;
    using Y1 = alg::tuple_element2<1,T0>::type;
    using Y2 = alg::tuple_element2<2,T0>::type;
    using Y3 = alg::tuple_element2<3,T0>::type;
    using Y4 = alg::tuple_element2<4,T0>::type;

    static_assert(std::is_same_v<Y0, char>,                     "failed to tuple_element");
    static_assert(std::is_same_v<Y1, std::uint32_t>,            "failed to tuple_element");
    static_assert(std::is_same_v<Y2, std::string>,              "failed to tuple_element");
    static_assert(std::is_same_v<Y3, std::pair<double,double>>, "failed to tuple_element");
    static_assert(std::is_same_v<Y4, double>,                   "failed to tuple_element");
    print_summary("tuple - size and element", "succeeded in compile time");
}


void test_shuffle()
{
    using T   = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using X0  = alg::shuffle_tuple <T,3,2,1>::type;
    using X1  = alg::shuffle_tuple2<T,alg::idx_seq<3,2,1>>::type;
    using ANS = std::tuple<std::pair<double,double>, std::string, std::uint32_t>;

    static_assert(std::is_same_v<X0, ANS>, "failed to shuffle_tuple");
    static_assert(std::is_same_v<X1, ANS>, "failed to shuffle_tuple");
    static_assert(std::is_same_v<std::tuple_size<T >::type, std::integral_constant<std::size_t,5>>, "failed to tuple_size");
    static_assert(std::is_same_v<std::tuple_size<X0>::type, std::integral_constant<std::size_t,3>>, "failed to tuple_size");
    static_assert(std::is_same_v<std::tuple_size<X1>::type, std::integral_constant<std::size_t,3>>, "failed to tuple_size");
    static_assert(std::tuple_size<T >::value == 5, "failed to tuple_size");
    static_assert(std::tuple_size<X0>::value == 3, "failed to tuple_size");
    static_assert(std::tuple_size<X1>::value == 3, "failed to tuple_size");

    // *** Factory *** //
    auto t  = T{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141), 0.98765};
    auto x2 = alg::make_shuffle_tuple(t, alg::idx_seq<3,2,1>{}); // remember to construct an object here

    static_assert(std::is_same_v<decltype(x2),ANS>, "failed to make_shuffle_tuple");
    assert(std::get<1>(t) == std::get<2>(x2));
    assert(std::get<2>(t) == std::get<1>(x2));
    print_summary("tuple - shuffle and element", "succeeded in compile time");
}


void test_push_front()
{
    using T    = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using X0   = alg::push_front_tuple<T,  std::string, char>::type;
    using X1   = alg::push_front_tuple<X0, std::string, char>::type;
    using X2   = alg::push_front_tuple<std::tuple<>, std::string, char>::type;
    using ANS0 = std::tuple<char, std::string, char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS1 = std::tuple<char, std::string, char, std::string, char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS2 = std::tuple<char, std::string>;

    static_assert(std::is_same_v<X0, ANS0>, "failed to push_front_tuple");
    static_assert(std::is_same_v<X1, ANS1>, "failed to push_front_tuple");
    static_assert(std::is_same_v<X2, ANS2>, "failed to push_front_tuple");

    // *** Factory *** //
    auto  t    = T{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141), 0.98765};
    auto  x3   = alg::make_push_front_tuple(t, std::string{"hello-world"});
    auto  x4   = alg::make_push_front_tuple(t, std::string{"hello-world"}, 'A', 'B', 'C');
    auto  x5   = alg::make_push_front_tuple(std::tuple<>{}, std::string{"hello-world"}, 'A');
    using ANS3 = std::tuple                  <std::string, char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS4 = std::tuple<char, char, char, std::string, char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS5 = std::tuple            <char, std::string>;

    static_assert(std::is_same_v<decltype(x3), ANS3>, "failed to make_push_front_tuple");
    static_assert(std::is_same_v<decltype(x4), ANS4>, "failed to make_push_front_tuple");
    static_assert(std::is_same_v<decltype(x5), ANS5>, "failed to make_push_front_tuple");
    print_summary("tuple - push front", "succeeded in compile time");
}


void test_push_back()
{
    using T    = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using X0   = alg::push_back_tuple<T,  std::string, char>::type;
    using X1   = alg::push_back_tuple<X0, std::string, char>::type;
    using X2   = alg::push_back_tuple<std::tuple<>, std::string, char>::type;
    using ANS0 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double, std::string, char>;
    using ANS1 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double, std::string, char, std::string, char>;
    using ANS2 = std::tuple<std::string, char>;

    static_assert(std::is_same_v<X0, ANS0>, "failed to push_back_tuple");
    static_assert(std::is_same_v<X1, ANS1>, "failed to push_back_tuple");
    static_assert(std::is_same_v<X2, ANS2>, "failed to push_back_tuple");

    // *** Factory *** //
    auto  t    = T{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141), 0.98765};
    auto  x3   = alg::make_push_back_tuple(t, std::string{"hello-world"});
    auto  x4   = alg::make_push_back_tuple(t, std::string{"hello-world"}, 'A', 'B', 'C');
    auto  x5   = alg::make_push_back_tuple(std::tuple<>{}, std::string{"hello-world"}, 'A');
    using ANS3 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double, std::string>;
    using ANS4 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double, std::string, char, char, char>;
    using ANS5 = std::tuple<std::string, char>;

    static_assert(std::is_same_v<decltype(x3), ANS3>, "failed to make_push_back_tuple");
    static_assert(std::is_same_v<decltype(x4), ANS4>, "failed to make_push_back_tuple");
    static_assert(std::is_same_v<decltype(x5), ANS5>, "failed to make_push_back_tuple");
    print_summary("tuple - push back", "succeeded in compile time");
}


void test_pop_front()
{
    using T    = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using X0   = alg::pop_N_front_tuple<T, 0>::type;
    using X1   = alg::pop_N_front_tuple<T, 1>::type;
    using X2   = alg::pop_N_front_tuple<T, 2>::type;
    using ANS0 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS1 = std::tuple<std::uint32_t, std::string, std::pair<double,double>, double>;
    using ANS2 = std::tuple<std::string, std::pair<double,double>, double>;
    
    static_assert(std::is_same_v<X0, ANS0>, "failed to pop_N_front_tuple");
    static_assert(std::is_same_v<X1, ANS1>, "failed to pop_N_front_tuple");
    static_assert(std::is_same_v<X2, ANS2>, "failed to pop_N_front_tuple");
    print_summary("tuple - pop front", "succeeded in compile time");
}


void test_reverse()
{    
    using T   = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, std::vector<double>, double>;
    using X0  = alg::reverse_tuple<T >::type;
    using X1  = alg::reverse_tuple<X0>::type;
    using X2  = alg::reverse_tuple2<T >::type;
    using X3  = alg::reverse_tuple2<X2>::type;
    using ANS = std::tuple<double, std::vector<double>, std::pair<double,double>, std::string, std::uint32_t, char>;

    static_assert(std::is_same_v<X0, ANS>, "failed to reverse_tuple");
    static_assert(std::is_same_v<X1, T>,   "failed to reverse_tuple");
    static_assert(std::is_same_v<X2, ANS>, "failed to reverse_tuple2");
    static_assert(std::is_same_v<X3, T>,   "failed to reverse_tuple2");

    // *** Factory *** //
    auto  t  = T{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141), std::vector<double>{0.1,0.2,0.3,0.4}, 0.98765};
    auto  x4 = alg::make_reverse_tuple_helper(t, alg::idx_seq<0,1,3,4,5>{}); 
    auto  x5 = alg::make_reverse_tuple(t); 
    using T4 = std::tuple<double, std::vector<double>, std::string, std::uint32_t, char>;
    using T5 = ANS;

    static_assert(std::is_same_v<decltype(x4), T4>, "failed to make_reverse_tuple");
    assert(std::get<0>(t) == std::get<4>(x4));
    assert(std::get<1>(t) == std::get<3>(x4));
    assert(std::get<2>(t) == std::get<2>(x4));

    static_assert(std::is_same_v<decltype(x5), T5>, "failed to make_reverse_tuple");
    assert(std::get<0>(t) == std::get<5>(x5));
    assert(std::get<1>(t) == std::get<4>(x5));
    assert(std::get<2>(t) == std::get<3>(x5));
    print_summary("tuple - reverse", "succeeded in compile time");
}


void test_odd_pick()
{    
    using T   = std::tuple<char, char,
                           std::uint32_t, std::uint32_t,
                           std::string, std::string, 
                           std::pair<double,double>, std::pair<double,double>,
                           std::vector<double>, std::vector<double>,
                           double, double>;
    using X0   = alg::odd_pick_tuple<T >::type;
    using X1   = alg::odd_pick_tuple<X0>::type;
    using ANS0 = std::tuple<char, std::uint32_t, std::string, std::pair<double,double>, std::vector<double>, double>;
    using ANS1 = std::tuple<char, std::string, std::vector<double>>;

    static_assert(std::is_same_v<X0, ANS0>, "failed to odd_pick_tuple");
    static_assert(std::is_same_v<X1, ANS1>, "failed to odd_pick_tuple");

    // *** Factory *** //
    auto  t  = T{'a', 'a', 12345, 23456, 
                 "abcdf", "wxyz", 
                 std::make_pair(3.1415, 1.4141), 
                 std::make_pair(3.1415, 1.4141), 
                 std::vector<double>{0.1,0.2,0.3,0.4},
                 std::vector<double>{0.1,0.2,0.3,0.4},
                 0.98765, 0.98765};
    auto  x2   = alg::make_odd_pick_tuple(t); 
    auto  x3   = alg::make_odd_pick_tuple(x2); 
    using ANS2 = ANS0;
    using ANS3 = ANS1;

    static_assert(std::is_same_v<decltype(x2), ANS2>, "failed to make_reverse_tuple");
    assert(std::get<0>(t) == std::get<0>(x2));
    assert(std::get<2>(t) == std::get<1>(x2));
    assert(std::get<4>(t) == std::get<2>(x2));
      
    static_assert(std::is_same_v<decltype(x3), ANS3>, "failed to make_reverse_tuple");
    assert(std::get<0>(t) == std::get<0>(x3));
    assert(std::get<4>(t) == std::get<1>(x3));
    print_summary("tuple - odd pick", "succeeded in compile time");
}


void test_cat()
{
    using T0  = std::tuple<char, std::uint32_t, std::string, std::pair<double, double>>;
    using T1  = std::tuple<char, std::uint32_t, std::string>;
    using T2  = alg::tuple_cat<T0,T1>::type;
    using ANS = std::tuple<char, std::uint32_t, std::string, std::pair<double, double>, char, std::uint32_t, std::string>;
    static_assert(std::is_same_v<T2, ANS>, "failed to tuple_cat");

    // *** Factory *** //
    auto  x0 = T0{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141)};
    auto  x1 = T1{'a', 12345, "wxyz"};
    auto  x2 = alg::make_tuple_cat(x0, x1);

    static_assert(std::is_same_v<decltype(x2), ANS>, "failed to make_tuple_cat");
    assert(std::get<0>(x0) == std::get<0>(x2));
    assert(std::get<1>(x0) == std::get<1>(x2));
    assert(std::get<2>(x0) == std::get<2>(x2));
    assert(std::get<0>(x1) == std::get<4>(x2));
    assert(std::get<1>(x1) == std::get<5>(x2));
    assert(std::get<2>(x1) == std::get<6>(x2));
    print_summary("tuple - cat", "succeeded in compile time");
}


void test_zip()
{
    using T0  = std::tuple<char, std::uint32_t, std::string, std::pair<double, double>>;
    using T1  = std::tuple<std::pair<double, double>, std::string, std::uint32_t, char>;
    using T2  = alg::tuple_zip<T0,T1>::type;
    using ANS = std::tuple<std::pair<char, std::pair<double, double>>,
                           std::pair<std::uint32_t, std::string>,
                           std::pair<std::string, std::uint32_t>,
                           std::pair<std::pair<double, double>, char>>;
    static_assert(std::is_same_v<T2, ANS>, "failed to tuple_zip");

    // *** Factory *** //
    auto  x0 = T0{'a', std::uint32_t{12345}, std::string{"wxyz"}, std::pair<double,double>(3.1415, 1.4141)};
    auto  x1 = T1{std::pair<double,double>(3.1415, 1.4141), std::string{"wxyz"}, std::uint32_t{12345}, 'a'};
    auto  x2 = alg::make_tuple_zip(x0, x1);

    static_assert(std::is_same_v<decltype(x2), ANS>, "failed to make_tuple_zip");
    assert(std::get<0>(x0) == std::get<0>(x2).first);
    assert(std::get<1>(x0) == std::get<1>(x2).first);
    assert(std::get<2>(x0) == std::get<2>(x2).first);
    assert(std::get<3>(x0) == std::get<3>(x2).first);
    assert(std::get<0>(x1) == std::get<0>(x2).second);
    assert(std::get<1>(x1) == std::get<1>(x2).second);
    assert(std::get<2>(x1) == std::get<2>(x2).second);
    assert(std::get<3>(x1) == std::get<3>(x2).second);
    print_summary("tuple - zip", "succeeded in compile time");


    // *** simple zip *** //
    struct A{};
    struct B{};
    struct C{};
    struct D{};
    using ANS2 = std::tuple<std::pair<A,B>,
                            std::pair<B,C>,
                            std::pair<C,D>,
                            std::pair<D,A>>;
    static_assert(std::is_same_v<alg::simple_zip<A,B,C,D>::with<B,C,D,A>::type, ANS2>, "failed to do simple zip");
    print_summary("simple - zip", "succeeded in compile time");
}


void test_apply()
{
    char          ans_c = 'x';
    std::uint32_t ans_n = 0;
    std::string   ans_s = "";
    std::pair<double,double> ans_p{0,0};

    using T = std::tuple<char, std::uint32_t, std::string, std::pair<double, double>>;
    auto  t = T{'a', 12345, "wxyz", std::make_pair(3.1415, 1.4141)};
    auto  f = [&](char c, std::uint32_t n, const std::string& s, const std::pair<double,double>& p)
    {
        ans_c = c;
        ans_n = n;
        ans_s = s;
        ans_p = p;
    };
    alg::apply(t,f);

    assert(ans_c == 'a');
    assert(ans_n == 12345);
    assert(ans_s == "wxyz");
    assert(std::abs(ans_p.first  - 3.1415) < 0.000001);
    assert(std::abs(ans_p.second - 1.4141) < 0.000001);
    print_summary("tuple - apply", "succeeded");
}


void test_cherry_pick()
{
    std::vector<std::string> v = {"111", "222", "333", "444", "555"};
    std::tuple<std::uint32_t, 
               std::uint32_t, 
               std::string, 
               std::string, 
               std::string> t{111, 222, "333", "444", "555"};

    // ************************************* //
    // *** Pick defined as template type *** //
    // ************************************* //
    { 
        auto t0 = alg::cherry_pick_from_vector<decltype(v),0,2,4>(v); // use decltype to avoid typing
        assert(std::get<0>(t0) == "111");
        assert(std::get<1>(t0) == "333");
        assert(std::get<2>(t0) == "555");

        auto t1 = alg::cherry_pick_from_tuple<decltype(t),0,2,4>(t);
        assert(std::get<0>(t1) ==  111 );
        assert(std::get<1>(t1) == "333");
        assert(std::get<2>(t1) == "555");

        auto t2 = alg::cherry_pick_from_vector<decltype(v),4,1,0>(v);
        assert(std::get<0>(t2) == "555");
        assert(std::get<1>(t2) == "222");
        assert(std::get<2>(t2) == "111");

        auto t3 = alg::cherry_pick_from_tuple<decltype(t),4,1,0>(t);
        assert(std::get<0>(t3) == "555");
        assert(std::get<1>(t3) ==  222 );
        assert(std::get<2>(t3) ==  111 );
    }

    // ************************************** //
    // *** Pick defined as input argument *** //
    // ************************************** //
    { 
        auto t0 = alg::cherry_pick_from_vector2(v, std::index_sequence<0,2,4>{}); 
        assert(std::get<0>(t0) == "111");
        assert(std::get<1>(t0) == "333");
        assert(std::get<2>(t0) == "555");

        auto t1 = alg::cherry_pick_from_tuple2 (t, std::index_sequence<0,2,4>{});
        assert(std::get<0>(t1) ==  111 );
        assert(std::get<1>(t1) == "333");
        assert(std::get<2>(t1) == "555");

        auto t2 = alg::cherry_pick_from_vector2(v, std::index_sequence<4,1,0>{});
        assert(std::get<0>(t2) == "555");
        assert(std::get<1>(t2) == "222");
        assert(std::get<2>(t2) == "111");

        auto t3 = alg::cherry_pick_from_tuple2 (t, std::index_sequence<4,1,0>{});
        assert(std::get<0>(t3) == "555");
        assert(std::get<1>(t3) ==  222 );
        assert(std::get<2>(t3) ==  111 );
    }
    print_summary("tuple - cherry pick", "succeeded");
}


void test_tuple()
{
    test_construct();
    test_make_tuple_and_tie(); // and structural_binding
    test_size_and_element();
    test_shuffle();
    test_push_front();
    test_push_back();
    test_pop_front();
    test_reverse();
    test_odd_pick();
    test_cat();
    test_zip();
    test_apply(); 
    test_cherry_pick(); 
}

