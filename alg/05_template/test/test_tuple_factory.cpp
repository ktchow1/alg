#include<iostream>
#include<cassert>
#include<cstdint>
#include<tuple_factory.h>
#include<utility.h>


namespace toy_example2
{
    struct X {};

    X     named_x;
    X&    named_rx  = named_x;
    X&&   named_rrx = std::move(named_x);
    X   unnamed_x()   { return named_x; }
    X&  unnamed_rx()  { return named_x; } 
    X&& unnamed_rrx() { return std::move(named_x); } 
}
using namespace toy_example2;



// ********************************************************* //
// *** This test shows why we need std::forward_as_tuple *** //
// ********************************************************* //
void test_std_make_tuple_type()
{
    auto t0 = std::make_tuple      (named_x);
    auto t1 = std::tie             (named_x);
    auto t2 = std::forward_as_tuple(named_x);
    auto t3 = std::forward_as_tuple(std::move(named_x));
    auto s0 = std::make_tuple      (unnamed_x());
//  auto s1 = std::tie             (unnamed_x()); // expected : compile error
    auto s2 = std::forward_as_tuple(unnamed_x());
    auto s3 = std::forward_as_tuple(std::move(unnamed_x()));



    // For named x
    static_assert(std::is_same_v<decltype(std::make_tuple      (named_x)           ), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(std::tie             (named_x)           ), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(std::forward_as_tuple(named_x)           ), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(std::forward_as_tuple(std::move(named_x))), std::tuple<X&&>>, "deduce tuple type");

    static_assert(std::is_same_v<decltype(t0), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t1), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t2), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t3), std::tuple<X&&>>, "deduce tuple type");

    // For unnamed x
    static_assert(std::is_same_v<decltype(std::make_tuple      (unnamed_x())           ), std::tuple<X>>  , "deduce tuple type");
//  static_assert(std::is_same_v<decltype(std::tie             (unnamed_x())           ), std::tuple<***>>, "deduce tuple type"); // expected : cannot compile
    static_assert(std::is_same_v<decltype(std::forward_as_tuple(unnamed_x())           ), std::tuple<X&&>>, "deduce tuple type");
    static_assert(std::is_same_v<decltype(std::forward_as_tuple(std::move(unnamed_x()))), std::tuple<X&&>>, "deduce tuple type");

    static_assert(std::is_same_v<decltype(s0), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(s2), std::tuple<X&&>>, "deduce tuple type");
    static_assert(std::is_same_v<decltype(s3), std::tuple<X&&>>, "deduce tuple type");



    // For named tuple from named x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(t0)), X&>, "deduce tuple type");     // named std::tuple<X>>      
    static_assert(std::is_same_v<decltype(std::get<0>(t1)), X&>, "deduce tuple type");     // named std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(t2)), X&>, "deduce tuple type");     // named std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(t3)), X&>, "deduce tuple type");     // named std::tuple<X&&>>

    // For named tuple from unnamed x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(s0)), X&>, "deduce tuple type");     // named std::tuple<X>     
    static_assert(std::is_same_v<decltype(std::get<0>(s2)), X&>, "deduce tuple type");     // named std::tuple<X&&> 
    static_assert(std::is_same_v<decltype(std::get<0>(s3)), X&>, "deduce tuple type");     // named std::tuple<X&&>      



    // For unnamed tuple from named x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(std::make_tuple      (named_x)           )), X&&>, "deduce tuple type");          // unnamed std::tuple<X>>   
    static_assert(std::is_same_v<decltype(std::get<0>(std::tie             (named_x)           )), X&> , "deduce tuple type");          // unnamed std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(std::forward_as_tuple(named_x)           )), X&> , "deduce tuple type");          // unnamed std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(std::forward_as_tuple(std::move(named_x)))), X&&>, "deduce tuple type");          // unnamed std::tuple<X&&>>
     
    // For unnamed tuple from unnamed x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(std::make_tuple      (unnamed_x())           )), X&&>, "deduce tuple type");      // unnamed std::tuple<X>   
    static_assert(std::is_same_v<decltype(std::get<0>(std::forward_as_tuple(unnamed_x())           )), X&&>, "deduce tuple type");      // unnamed std::tuple<X&&> 
    static_assert(std::is_same_v<decltype(std::get<0>(std::forward_as_tuple(std::move(unnamed_x())))), X&&>, "deduce tuple type");      // unnamed std::tuple<X&&> 
  
    print_summary("tuple factory - std tuple type", "succeeded in compile time");
}



// ****************************** //
// Repeat above experiment with :
// * alg::make_tuple
// * alg::tie
// * alg::forward_as_tuple
//
// Expect same result as above
// ****************************** //
void test_alg_make_tuple_type()
{
    auto t0 = alg::make_tuple      (named_x);
    auto t1 = alg::tie             (named_x);
    auto t2 = alg::forward_as_tuple(named_x);
    auto t3 = alg::forward_as_tuple(std::move(named_x));
    auto s0 = alg::make_tuple      (unnamed_x());
//  auto s1 = alg::tie             (unnamed_x()); // expected : compile error
    auto s2 = alg::forward_as_tuple(unnamed_x());
    auto s3 = alg::forward_as_tuple(std::move(unnamed_x()));



    // For named x
    static_assert(std::is_same_v<decltype(alg::make_tuple      (named_x)           ), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(alg::tie             (named_x)           ), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(alg::forward_as_tuple(named_x)           ), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(alg::forward_as_tuple(std::move(named_x))), std::tuple<X&&>>, "deduce tuple type");

    static_assert(std::is_same_v<decltype(t0), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t1), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t2), std::tuple<X&>> , "deduce tuple type");
    static_assert(std::is_same_v<decltype(t3), std::tuple<X&&>>, "deduce tuple type");

    // For unnamed x
    static_assert(std::is_same_v<decltype(alg::make_tuple      (unnamed_x())           ), std::tuple<X>>  , "deduce tuple type");
//  static_assert(std::is_same_v<decltype(alg::tie             (unnamed_x())           ), std::tuple<***>>, "deduce tuple type"); // expected : cannot compile
    static_assert(std::is_same_v<decltype(alg::forward_as_tuple(unnamed_x())           ), std::tuple<X&&>>, "deduce tuple type");
    static_assert(std::is_same_v<decltype(alg::forward_as_tuple(std::move(unnamed_x()))), std::tuple<X&&>>, "deduce tuple type");

    static_assert(std::is_same_v<decltype(s0), std::tuple<X>>  , "deduce tuple type");
    static_assert(std::is_same_v<decltype(s2), std::tuple<X&&>>, "deduce tuple type");
    static_assert(std::is_same_v<decltype(s3), std::tuple<X&&>>, "deduce tuple type");



    // For named tuple from named x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(t0)), X&>, "deduce tuple type");     // named std::tuple<X>>      
    static_assert(std::is_same_v<decltype(std::get<0>(t1)), X&>, "deduce tuple type");     // named std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(t2)), X&>, "deduce tuple type");     // named std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(t3)), X&>, "deduce tuple type");     // named std::tuple<X&&>>

    // For named tuple from unnamed x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(s0)), X&>, "deduce tuple type");     // named std::tuple<X>     
    static_assert(std::is_same_v<decltype(std::get<0>(s2)), X&>, "deduce tuple type");     // named std::tuple<X&&> 
    static_assert(std::is_same_v<decltype(std::get<0>(s3)), X&>, "deduce tuple type");     // named std::tuple<X&&>      



    // For unnamed tuple from named x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(alg::make_tuple      (named_x)           )), X&&>, "deduce tuple type");          // unnamed std::tuple<X>>   
    static_assert(std::is_same_v<decltype(std::get<0>(alg::tie             (named_x)           )), X&> , "deduce tuple type");          // unnamed std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(alg::forward_as_tuple(named_x)           )), X&> , "deduce tuple type");          // unnamed std::tuple<X&>> 
    static_assert(std::is_same_v<decltype(std::get<0>(alg::forward_as_tuple(std::move(named_x)))), X&&>, "deduce tuple type");          // unnamed std::tuple<X&&>>
     
    // For unnamed tuple from unnamed x <--- same result for decltype or decltype()
    static_assert(std::is_same_v<decltype(std::get<0>(alg::make_tuple      (unnamed_x())           )), X&&>, "deduce tuple type");      // unnamed std::tuple<X>   
    static_assert(std::is_same_v<decltype(std::get<0>(alg::forward_as_tuple(unnamed_x())           )), X&&>, "deduce tuple type");      // unnamed std::tuple<X&&> 
    static_assert(std::is_same_v<decltype(std::get<0>(alg::forward_as_tuple(std::move(unnamed_x())))), X&&>, "deduce tuple type");      // unnamed std::tuple<X&&> 
  
    print_summary("tuple factory - alg tuple type", "succeeded in compile time");
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


void test_tuple_factory()
{
    test_std_make_tuple_type();
    test_alg_make_tuple_type();
    test_make_tuple();
}
