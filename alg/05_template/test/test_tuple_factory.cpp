#include<iostream>
#include<cassert>
#include<cstdint>
#include<tuple_factory.h>
#include<utility.h>



void why_we_need_3_tuple_factories()
{ 
    // Example 1
    {
        int i = 123;
        auto   x = i;     
        auto&  y = i;    
        auto&& z = i;   
        auto&& w = std::move(i); 
        
        x = 100; assert(i == 123);
        y = 200; assert(i == 200);
        z = 300; assert(i == 300);
        w = 400; assert(i == 400); // caller bears risk doing that ... 
    }

    // Example 2
    assert(toy_example::lvalue_impl_count == 0);
    assert(toy_example::rvalue_impl_count == 0);
    
    {
        std::uint32_t x = 123;

        toy_example::interface_by_copying(x); 
        assert(toy_example::lvalue_impl_count == 1);
        assert(toy_example::rvalue_impl_count == 0);
        toy_example::interface_by_copying(std::move(x)); 
        assert(toy_example::lvalue_impl_count == 2);
        assert(toy_example::rvalue_impl_count == 0);
        toy_example::interface_by_copying(std::uint32_t(123)); 
        assert(toy_example::lvalue_impl_count == 3);
        assert(toy_example::rvalue_impl_count == 0);
    }
    {
        std::uint32_t x = 123;

        toy_example::interface_by_lvalue_reference(x); 
        assert(toy_example::lvalue_impl_count == 4);
        assert(toy_example::rvalue_impl_count == 0);
    //  toy_example::interface_by_lvalue_reference(std::move(x));          // cannot compile for  xvalue
    //  toy_example::interface_by_lvalue_reference(std::uint32_t(123));    // cannot compile for prvalue
    }
    {
        std::uint32_t x = 123;

        toy_example::interface_by_perfect_forwarding_reference(x); 
        assert(toy_example::lvalue_impl_count == 5);
        assert(toy_example::rvalue_impl_count == 0);
        toy_example::interface_by_perfect_forwarding_reference(std::move(x)); 
        assert(toy_example::lvalue_impl_count == 5);
        assert(toy_example::rvalue_impl_count == 1);
        toy_example::interface_by_perfect_forwarding_reference(std::uint32_t(123)); 
        assert(toy_example::lvalue_impl_count == 5);
        assert(toy_example::rvalue_impl_count == 2);
    }
    print_summary("tuple factory - why we need 3 factories?", "succeeded");
}


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
    auto tup1 = alg::make_tuple(std::ref(b), std::ref(c), std::ref(i)); // Remark : We can make the assignment
    assert(std::get<0>(tup1).get()== true);                             // std::tuple<X,Y,Z> = std::tuple<std::reference_wrapper<X>, >
    assert(std::get<1>(tup1).get()== 'a');                              //                                std::reference_wrapper<Y>,
    assert(std::get<2>(tup1).get()== 123);                              //                                std::reference_wrapper<Z>>
                                                                        // because there is conversion operator from reference_wrapper<T> to T.
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
    assert(i0 == 123); // Remark : Tied variables is a temporary tuple, it cannot track source tuple<T> change.

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
    assert(i1 == 234); // Remark : Tied variables is a temporary tuple, it cannot track source tuple<T&> change.

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
    why_we_need_3_tuple_factories();
    test_make_tuple();
    test_tie();
    test_forward_as_tuple();
}
