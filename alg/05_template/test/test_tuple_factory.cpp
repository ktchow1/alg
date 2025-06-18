#include<iostream>
#include<cassert>
#include<cstdint>
#include<tuple_factory.h>
#include<utility.h>



void test_make_tuple() 
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

    // case 2 : make_tuple<std::reference_wrapper<T>> 
    b = true, c = 'a', i = 123;
    auto tup1 = alg::make_tuple(std::ref(b), std::ref(c), std::ref(i));
    assert(std::get<0>(tup1).get()== true);
    assert(std::get<1>(tup1).get()== 'a');
    assert(std::get<2>(tup1).get()== 123);

    b = false, c = 'b', i = 234;
    assert(std::get<0>(tup1).get()== false); 
    assert(std::get<1>(tup1).get()== 'b');
    assert(std::get<2>(tup1).get()== 234); // modify succeed

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
    assert(i0 == 123); // tied variables is a temporary tuple, it cannot track source tuple<T> change

    // case 4 : tie to tuple<std::reference_wrapper<T>>
    alg::tie(b1,c1,i1) = tup1;
    assert(b1 == false);
    assert(c1 == 'b');
    assert(i1 == 234);

    std::get<0>(tup1).get() = true;
    std::get<1>(tup1).get() = 'c';
    std::get<2>(tup1).get() = 345;
    assert(b1 == false);
    assert(c1 == 'b');
    assert(i1 == 234); // tied variables is a temporary tuple, it cannot track source tuple<T&> change

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

    print_summary("tuple factory - alg::make_tuple", "succeeded");
}


void test_tie()
{
    print_summary("tuple factory - alg::tie", "succeeded");
}


void test_forward_as_tuple()
{
    print_summary("tuple factory - alg::forward_as_tuple", "succeeded");
}


void test_tuple_factory()
{
    test_make_tuple();
    test_tie();
    test_forward_as_tuple();
}
