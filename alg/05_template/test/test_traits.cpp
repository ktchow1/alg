#include<cassert>
#include<vector>
#include<list>
#include<traits.h>
#include<utility.h>


void test_simple_traits()
{
    static_assert(alg::type2value_traits<bool>          ::value ==  0, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<char>          ::value ==  0, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<unsigned short>::value == +2, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<unsigned int>  ::value == +3, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<unsigned long> ::value == +4, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<short>         ::value == -2, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<int>           ::value == -3, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<long>          ::value == -4, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<std::string>   ::value ==  0, "failed to use type2value_traits");
    static_assert(alg::type2value_traits<std::list<int>>::value ==  0, "failed to use type2value_traits");

    static_assert(alg::type2value_traits_v<bool>           ==  0, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<char>           ==  0, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<unsigned short> == +2, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<unsigned int>   == +3, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<unsigned long>  == +4, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<short>          == -2, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<int>            == -3, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<long>           == -4, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<std::string>    ==  0, "failed to use type2value_traits_v");
    static_assert(alg::type2value_traits_v<std::list<int>> ==  0, "failed to use type2value_traits_v");

    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double, 0>::type, bool>,   "failed to use value2type_traits");
    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double, 5>::type, bool>,   "failed to use value2type_traits");
    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double,10>::type, int>,    "failed to use value2type_traits");
    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double,15>::type, int>,    "failed to use value2type_traits");
    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double,20>::type, double>, "failed to use value2type_traits");
    static_assert(alg::is_same_v<alg::value2type_traits<bool,int,double,25>::type, double>, "failed to use value2type_traits");

    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double, 0>, bool>,   "failed to use value2type_traits_t");
    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double, 5>, bool>,   "failed to use value2type_traits_t");
    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double,10>, int>,    "failed to use value2type_traits_t");
    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double,15>, int>,    "failed to use value2type_traits_t");
    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double,20>, double>, "failed to use value2type_traits_t");
    static_assert(alg::is_same_v<alg::value2type_traits_t<bool,int,double,25>, double>, "failed to use value2type_traits_t");
    print_summary(" simple traits", "succeeded in compile time");
}


void test_runtime_traits()
{
    // Maven interview : runtime traits
    auto p0 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 1);       
    auto p1 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 2);
    auto p2 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 3);
    auto p3 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 4);
    auto p4 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 5);
    auto p5 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create( 8);
    auto p6 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create(16);
    auto p7 = alg::value2type_hardcoded_runtime<1,2,4,8,16>::create(32);
    assert( dynamic_cast<alg::derivedN< 1>*>(p0.get()));
    assert( dynamic_cast<alg::derivedN< 2>*>(p1.get()));
    assert(!dynamic_cast<alg::derivedN< 3>*>(p2.get())); // if dynamic_cast fails, it returns nullptr
    assert( dynamic_cast<alg::derivedN< 4>*>(p3.get()));
    assert(!dynamic_cast<alg::derivedN< 5>*>(p4.get()));
    assert( dynamic_cast<alg::derivedN< 8>*>(p5.get()));
    assert( dynamic_cast<alg::derivedN<16>*>(p6.get()));
    assert(!dynamic_cast<alg::derivedN<32>*>(p7.get()));   

    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 1,1,2,4,8,16>::type, alg::derivedN< 1>>, "failed to use value2type_hardcoded_compiletime");       
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 2,1,2,4,8,16>::type, alg::derivedN< 2>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 3,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 4,1,2,4,8,16>::type, alg::derivedN< 4>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 5,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime< 8,1,2,4,8,16>::type, alg::derivedN< 8>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime<16,1,2,4,8,16>::type, alg::derivedN<16>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_hardcoded_compiletime<32,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");

    auto q0 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 1);       
    auto q1 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 2);
    auto q2 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 3);
    auto q3 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 4);
    auto q4 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 5);
    auto q5 = alg::value2type_variadic_runtime<1,2,4,8,16>::create( 8);
    auto q6 = alg::value2type_variadic_runtime<1,2,4,8,16>::create(16);
    auto q7 = alg::value2type_variadic_runtime<1,2,4,8,16>::create(32);
    assert( dynamic_cast<alg::derivedN< 1>*>(q0.get()));
    assert( dynamic_cast<alg::derivedN< 2>*>(q1.get()));
    assert(!dynamic_cast<alg::derivedN< 3>*>(q2.get()));
    assert( dynamic_cast<alg::derivedN< 4>*>(q3.get()));
    assert(!dynamic_cast<alg::derivedN< 5>*>(q4.get()));
    assert( dynamic_cast<alg::derivedN< 8>*>(q5.get()));
    assert( dynamic_cast<alg::derivedN<16>*>(q6.get()));
    assert(!dynamic_cast<alg::derivedN<32>*>(q7.get()));   

    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 1,1,2,4,8,16>::type, alg::derivedN< 1>>, "failed to use value2type_hardcoded_compiletime");       
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 2,1,2,4,8,16>::type, alg::derivedN< 2>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 3,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 4,1,2,4,8,16>::type, alg::derivedN< 4>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 5,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime< 8,1,2,4,8,16>::type, alg::derivedN< 8>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime<16,1,2,4,8,16>::type, alg::derivedN<16>>, "failed to use value2type_hardcoded_compiletime");
    static_assert(alg::is_convertible_v<int, int>            ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<int, double>         ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<double, int>         ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<double, std::string> == false, "failed to use is_convertible");

    static_assert(alg::is_same_v<alg::value2type_variadic_compiletime<32,1,2,4,8,16>::type,              void>, "failed to use value2type_hardcoded_compiletime");
    print_summary("runtime traits", "succeeded");
}


void test_sfinae_traits()
{
    static_assert(alg::is_same_v<alg::integral_constant<std::uint8_t, 123>  ::value_type, std::uint8_t>,  "failed to use integral_constant");
    static_assert(alg::is_same_v<alg::integral_constant<std::uint16_t,1234> ::value_type, std::uint16_t>, "failed to use integral_constant");
    static_assert(alg::is_same_v<alg::integral_constant<std::uint32_t,12345>::value_type, std::uint32_t>, "failed to use integral_constant");
    static_assert(alg::is_same_v<alg::integral_constant<std::int8_t, -123>  ::value_type, std::int8_t>,   "failed to use integral_constant");
    static_assert(alg::is_same_v<alg::integral_constant<std::int16_t,-1234> ::value_type, std::int16_t>,  "failed to use integral_constant");
    static_assert(alg::is_same_v<alg::integral_constant<std::int32_t,-12345>::value_type, std::int32_t>,  "failed to use integral_constant");
    static_assert(alg::integral_constant<std::uint8_t, 123>  ::value == 123,   "failed to use integral_constant");
    static_assert(alg::integral_constant<std::uint16_t,1234> ::value == 1234,  "failed to use integral_constant");
    static_assert(alg::integral_constant<std::uint32_t,12345>::value == 12345, "failed to use integral_constant");
    static_assert(alg::integral_constant<std::int8_t, -123>  ::value ==-123,   "failed to use integral_constant");
    static_assert(alg::integral_constant<std::int16_t,-1234> ::value ==-1234,  "failed to use integral_constant");
    static_assert(alg::integral_constant<std::int32_t,-12345>::value ==-12345, "failed to use integral_constant");

    static_assert(alg::is_same_v<alg::remove_const<      int       >::type,       int     >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<const int       >::type,       int     >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<      int&      >::type,       int&    >, "failed to use remove_const"); // cannot remove const of pointee
    static_assert(alg::is_same_v<alg::remove_const<const int&      >::type, const int&    >, "failed to use remove_const"); // cannot remove const of pointee
    static_assert(alg::is_same_v<alg::remove_const<      int&&     >::type,       int&&   >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<      int*      >::type,       int*    >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<const int*      >::type, const int*    >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<const int* const>::type, const int*    >, "failed to use remove_const");
    static_assert(alg::is_same_v<alg::remove_const<      int* const>::type,       int*    >, "failed to use remove_const"); // cannot remove const of pointee

    static_assert(alg::is_same_v<alg::remove_ref<      int       >::type,       int       >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<const int       >::type, const int       >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<      int&      >::type,       int       >, "failed to use remove_ref"); 
    static_assert(alg::is_same_v<alg::remove_ref<const int&      >::type, const int       >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<      int&&     >::type,       int       >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<      int*      >::type,       int*      >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<const int*      >::type, const int*      >, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<const int* const>::type, const int* const>, "failed to use remove_ref");
    static_assert(alg::is_same_v<alg::remove_ref<      int* const>::type,       int* const>, "failed to use remove_ref");

    static_assert(alg::is_same_v<alg::remove_ptr<      int       >::type,       int       >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<const int       >::type, const int       >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<      int&      >::type,       int&      >, "failed to use remove_ptr"); 
    static_assert(alg::is_same_v<alg::remove_ptr<const int&      >::type, const int&      >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<      int&&     >::type,       int&&     >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<      int*      >::type,       int       >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<const int*      >::type, const int       >, "failed to use remove_ptr");
    static_assert(alg::is_same_v<alg::remove_ptr<const int* const>::type, const int       >, "failed to use remove_ptr"); // remove const in pointer as well
    static_assert(alg::is_same_v<alg::remove_ptr<      int* const>::type,       int       >, "failed to use remove_ptr"); // remove const in pointer as well

    static_assert(alg::is_same_v<alg::decay<      int       >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<const int       >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<      int&      >::type, int>, "failed to use decay"); 
    static_assert(alg::is_same_v<alg::decay<const int&      >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<      int&&     >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<      int*      >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<const int*      >::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<const int* const>::type, int>, "failed to use decay");
    static_assert(alg::is_same_v<alg::decay<      int* const>::type, int>, "failed to use decay");

    static_assert(alg::is_ptr_v<      int       > == false, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<const int       > == false, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<      int&      > == false, "failed to use is_ptr"); 
    static_assert(alg::is_ptr_v<const int&      > == false, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<      int&&     > == false, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<      int*      > ==  true, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<const int*      > ==  true, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<const int* const> ==  true, "failed to use is_ptr");
    static_assert(alg::is_ptr_v<      int* const> ==  true, "failed to use is_ptr");

    // alg::is_array
    static_assert(alg::is_array_v<      int       > == false, "failed to use is_array");
    static_assert(alg::is_array_v<const int       > == false, "failed to use is_array");
    static_assert(alg::is_array_v<      int&      > == false, "failed to use is_array"); 
    static_assert(alg::is_array_v<const int&      > == false, "failed to use is_array");
    static_assert(alg::is_array_v<      int&&     > == false, "failed to use is_array");
    static_assert(alg::is_array_v<      int*      > == false, "failed to use is_array");
    static_assert(alg::is_array_v<const int*      > == false, "failed to use is_array");
    static_assert(alg::is_array_v<const int* const> == false, "failed to use is_array");
    static_assert(alg::is_array_v<      int* const> == false, "failed to use is_array");

    static_assert(alg::is_array_v<      std::array<int,8>       > == true, "failed to use is_array");
    static_assert(alg::is_array_v<const std::array<int,8>       > == true, "failed to use is_array");
    static_assert(alg::is_array_v<      std::array<int,8>&      > == true, "failed to use is_array"); 
    static_assert(alg::is_array_v<const std::array<int,8>&      > == true, "failed to use is_array");
    static_assert(alg::is_array_v<      std::array<int,8>&&     > == true, "failed to use is_array");
    static_assert(alg::is_array_v<      std::array<int,8>*      > == true, "failed to use is_array");
    static_assert(alg::is_array_v<const std::array<int,8>*      > == true, "failed to use is_array");
    static_assert(alg::is_array_v<const std::array<int,8>* const> == true, "failed to use is_array");
    static_assert(alg::is_array_v<      std::array<int,8>* const> == true, "failed to use is_array");

    // alg::is_vec::impl does NOT work when there is adornment 
    static_assert(alg::is_vec_impl<      std::vector<int>       >::value ==  true, "failed to use is_vec");
    static_assert(alg::is_vec_impl<const std::vector<int>       >::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<      std::vector<int>&      >::value == false, "failed to use is_vec"); 
    static_assert(alg::is_vec_impl<const std::vector<int>&      >::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<      std::vector<int>&&     >::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<      std::vector<int>*      >::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<const std::vector<int>*      >::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<const std::vector<int>* const>::value == false, "failed to use is_vec");
    static_assert(alg::is_vec_impl<      std::vector<int>* const>::value == false, "failed to use is_vec");

    // alg::is_vec 
    static_assert(alg::is_vec_v<      int       > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const int       > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      int&      > == false, "failed to use is_vec"); 
    static_assert(alg::is_vec_v<const int&      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      int&&     > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      int*      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const int*      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const int* const> == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      int* const> == false, "failed to use is_vec");

    static_assert(alg::is_vec_v<      std::vector<int>       > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::vector<int>       > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::vector<int>&      > == true, "failed to use is_vec"); 
    static_assert(alg::is_vec_v<const std::vector<int>&      > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::vector<int>&&     > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::vector<int>*      > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::vector<int>*      > == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::vector<int>* const> == true, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::vector<int>* const> == true, "failed to use is_vec");

    static_assert(alg::is_vec_v<      std::list<int>       > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::list<int>       > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::list<int>&      > == false, "failed to use is_vec"); 
    static_assert(alg::is_vec_v<const std::list<int>&      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::list<int>&&     > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::list<int>*      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::list<int>*      > == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<const std::list<int>* const> == false, "failed to use is_vec");
    static_assert(alg::is_vec_v<      std::list<int>* const> == false, "failed to use is_vec");

    // alg::is_vec_naive
    static_assert(alg::is_vec_naive_v<      int       > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const int       > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      int&      > == false, "failed to use is_vec_naive"); 
    static_assert(alg::is_vec_naive_v<const int&      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      int&&     > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      int*      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const int*      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const int* const> == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      int* const> == false, "failed to use is_vec_naive");

    static_assert(alg::is_vec_naive_v<      std::vector<int>       > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::vector<int>       > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::vector<int>&      > == true, "failed to use is_vec_naive"); 
    static_assert(alg::is_vec_naive_v<const std::vector<int>&      > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::vector<int>&&     > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::vector<int>*      > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::vector<int>*      > == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::vector<int>* const> == true, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::vector<int>* const> == true, "failed to use is_vec_naive");

    static_assert(alg::is_vec_naive_v<      std::list<int>       > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::list<int>       > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::list<int>&      > == false, "failed to use is_vec_naive"); 
    static_assert(alg::is_vec_naive_v<const std::list<int>&      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::list<int>&&     > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::list<int>*      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::list<int>*      > == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<const std::list<int>* const> == false, "failed to use is_vec_naive");
    static_assert(alg::is_vec_naive_v<      std::list<int>* const> == false, "failed to use is_vec_naive");

    // alg::is_vec_except_pointer
    static_assert(alg::is_vec_except_pointer_v<      int       > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const int       > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      int&      > == false, "failed to use is_vec_except_pointer"); 
    static_assert(alg::is_vec_except_pointer_v<const int&      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      int&&     > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      int*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const int*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const int* const> == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      int* const> == false, "failed to use is_vec_except_pointer");

    static_assert(alg::is_vec_except_pointer_v<      std::vector<int>       > == true,  "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::vector<int>       > == true,  "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::vector<int>&      > == true,  "failed to use is_vec_except_pointer"); 
    static_assert(alg::is_vec_except_pointer_v<const std::vector<int>&      > == true,  "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::vector<int>&&     > == true,  "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::vector<int>*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::vector<int>*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::vector<int>* const> == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::vector<int>* const> == false, "failed to use is_vec_except_pointer");

    static_assert(alg::is_vec_except_pointer_v<      std::list<int>       > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::list<int>       > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::list<int>&      > == false, "failed to use is_vec_except_pointer"); 
    static_assert(alg::is_vec_except_pointer_v<const std::list<int>&      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::list<int>&&     > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::list<int>*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::list<int>*      > == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<const std::list<int>* const> == false, "failed to use is_vec_except_pointer");
    static_assert(alg::is_vec_except_pointer_v<      std::list<int>* const> == false, "failed to use is_vec_except_pointer");

    // inheritance
    struct base {};
    struct derived : public base {};
    struct derder0 : public derived {};
    struct derder1 : public derived {};
    struct derpriv : private base {}; // failed to handle private inheritance

    static_assert(alg::is_inherit_v<base,    base>    ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<base,    derived> ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<base,    derder0> ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<base,    derder1> ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derived, derder0> ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derived, derder1> ==  true, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<double,  int>     == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<base,    int>     == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derived, int>     == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derder0, int>     == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derived, base>    == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derder0, base>    == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derder1, base>    == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derder0, derived> == false, "failed to use is_inherit");
    static_assert(alg::is_inherit_v<derder1, derived> == false, "failed to use is_inherit");
//  std::cout <<  alg::is_inherit_v<base,    derpriv>; // cannot compile

    static_assert(alg::is_same_v<alg::condition< true,              int,std::string>::type, int>,         "failed to use condition");
    static_assert(alg::is_same_v<alg::condition<false,              int,std::string>::type, std::string>, "failed to use condition");
    static_assert(alg::is_same_v<alg::condition<alg::is_ptr_v<int*>,int,std::string>::type, int>,         "failed to use condition");
    static_assert(alg::is_same_v<alg::condition<alg::is_ptr_v<int >,int,std::string>::type, std::string>, "failed to use condition");

    static_assert(alg::is_target_types_v<std::string,bool> == false, "failed to use is_target_types");
    static_assert(alg::is_target_types_v<std::string,char> == false, "failed to use is_target_types");
    static_assert(alg::is_target_types_v<std::string, int> == false, "failed to use is_target_types");
    static_assert(alg::is_target_types_v<bool,std::string> ==  true, "failed to use is_target_types");
    static_assert(alg::is_target_types_v<char,std::string> ==  true, "failed to use is_target_types");
    static_assert(alg::is_target_types_v< int,std::string> ==  true, "failed to use is_target_types");
    static_assert(alg::is_target_types_v       <bool,bool> == false, "failed to use is_target_types");
    static_assert(alg::is_target_types_v       <char,char> == false, "failed to use is_target_types");
    static_assert(alg::is_target_types_v       < int, int> ==  true, "failed to use is_target_types");
    
    // assert is a macro, hence need to add () to ensure the whole arg as single arg
    assert((alg::sfinae_fct_method1<std::string,bool>() == false));
    assert((alg::sfinae_fct_method1<std::string,char>() == false));
    assert((alg::sfinae_fct_method1<std::string, int>() == false));
    assert((alg::sfinae_fct_method1<bool,std::string>() ==  true));
    assert((alg::sfinae_fct_method1<char,std::string>() ==  true));
    assert((alg::sfinae_fct_method1< int,std::string>() ==  true));
    assert((alg::sfinae_fct_method1       <bool,bool>() == false));
    assert((alg::sfinae_fct_method1       <char,char>() == false));
    assert((alg::sfinae_fct_method1       < int, int>() ==  true));

    assert((alg::sfinae_fct_method2<std::string,bool>() == false));
    assert((alg::sfinae_fct_method2<std::string,char>() == false));
    assert((alg::sfinae_fct_method2<std::string, int>() == false));
    assert((alg::sfinae_fct_method2<bool,std::string>() ==  true));
    assert((alg::sfinae_fct_method2<char,std::string>() ==  true));
    assert((alg::sfinae_fct_method2< int,std::string>() ==  true));
    assert((alg::sfinae_fct_method2       <bool,bool>() == false));
    assert((alg::sfinae_fct_method2       <char,char>() == false));
    assert((alg::sfinae_fct_method2       < int, int>() ==  true));

    assert((alg::sfinae_fct_method3<std::string,bool>() == false));
    assert((alg::sfinae_fct_method3<std::string,char>() == false));
    assert((alg::sfinae_fct_method3<std::string, int>() == false));
    assert((alg::sfinae_fct_method3<bool,std::string>() ==  true));
    assert((alg::sfinae_fct_method3<char,std::string>() ==  true));
    assert((alg::sfinae_fct_method3< int,std::string>() ==  true));
    assert((alg::sfinae_fct_method3       <bool,bool>() == false));
    assert((alg::sfinae_fct_method3       <char,char>() == false));
    assert((alg::sfinae_fct_method3       < int, int>() ==  true));

    assert((alg::sfinae_fct_method4<std::string,bool>() == false));
    assert((alg::sfinae_fct_method4<std::string,char>() == false));
    assert((alg::sfinae_fct_method4<std::string, int>() == false));
    assert((alg::sfinae_fct_method4<bool,std::string>() ==  true));
    assert((alg::sfinae_fct_method4<char,std::string>() ==  true));
    assert((alg::sfinae_fct_method4< int,std::string>() ==  true));
    assert((alg::sfinae_fct_method4       <bool,bool>() == false));
    assert((alg::sfinae_fct_method4       <char,char>() == false));
    assert((alg::sfinae_fct_method4       < int, int>() ==  true));

    // example 1 : perfect_forwarding
    std::string str;
    assert((alg::perfect_forwarding(str) == true));
    assert((alg::perfect_forwarding(std::move(str)) == false));
    assert((alg::perfect_forwarding(std::string{ }) == false));

    // example 2 : sfinae in class member function
    alg::sfinae_class<int>           x0;
    alg::sfinae_class<std::string>   x1;
    alg::sfinae_class<alg::target_A> x2;
    alg::sfinae_class<alg::target_B> x3;
    assert(x0.fct() == false);
    assert(x1.fct() == false);
    assert(x2.fct() == true);
    assert(x3.fct() == true);

    static_assert(alg::is_incrementable_v<int>                               == false, "failed to use is_incrementable"); // note : this is not incrementable
    static_assert(alg::is_incrementable_v<int*>                              == false, "failed to use is_incrementable");
    static_assert(alg::is_incrementable_v<std::string>                       == false, "failed to use is_incrementable");
    static_assert(alg::is_incrementable_v<std::list<int>::iterator>          ==  true, "failed to use is_incrementable");
    static_assert(alg::is_incrementable_v<std::vector<int >::iterator>       ==  true, "failed to use is_incrementable");
    static_assert(alg::is_incrementable_v<std::vector<int*>::iterator>       ==  true, "failed to use is_incrementable");
    static_assert(alg::is_incrementable_v<std::vector<int*>::const_iterator> ==  true, "failed to use is_incrementable");

    static_assert(alg::has_value_v<int>                               == false, "failed to use is_incrementable");
    static_assert(alg::has_value_v<int*>                              == false, "failed to use is_incrementable");
    static_assert(alg::has_value_v<std::string>                       ==  true, "failed to use is_incrementable");
    static_assert(alg::has_value_v<std::list<int>::iterator>          ==  true, "failed to use is_incrementable");
    static_assert(alg::has_value_v<std::vector<int >::iterator>       ==  true, "failed to use is_incrementable");
    static_assert(alg::has_value_v<std::vector<int*>::iterator>       ==  true, "failed to use is_incrementable");
    static_assert(alg::has_value_v<std::vector<int*>::const_iterator> ==  true, "failed to use is_incrementable");

    static_assert(alg::is_convertible_v<int, int>            ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<int, double>         ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<double, int>         ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<double, std::string> == false, "failed to use is_convertible");

    struct src {};
    struct dst { dst() = default; dst(const src&){}     }; 
    struct src2{ operator dst() const { return dst{}; } };

    static_assert(alg::is_convertible_v<dst, dst>  ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<dst, src>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<dst, src2> == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src, src>  ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src, src2> == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src, dst>  ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src2,dst>  ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src2,src>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src2,src2> ==  true, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<dst, int>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src, int>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<src2,int>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<int, dst>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<int, src>  == false, "failed to use is_convertible");
    static_assert(alg::is_convertible_v<int, src2> == false, "failed to use is_convertible");

    static_assert(alg::is_base_of_v<base,    base>    ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<base,    derived> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<base,    derder0> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<base,    derder1> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derived, derder0> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derived, derder1> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<double,  int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<base,    int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derived, int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derder0, int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derived, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derder0, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derder1, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derder0, derived> == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<derder1, derived> == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v<base,    derpriv> == false, "failed to use is_base_of"); // cannot detect private inheritance

    static_assert(alg::is_base_of_v2<base,    base>    ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<base,    derived> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<base,    derder0> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<base,    derder1> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derived, derder0> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derived, derder1> ==  true, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<double,  int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<base,    int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derived, int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derder0, int>     == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derived, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derder0, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derder1, base>    == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derder0, derived> == false, "failed to use is_base_of");
    static_assert(alg::is_base_of_v2<derder1, derived> == false, "failed to use is_base_of");
//  static_assert(alg::is_base_of_v2<base,    derpriv> == false, "failed to use is_base_of"); // cannot even compile, no matter whether we put true / false in static_assert

    // example 3 : sfinae in inheritance  
    struct sfinae_derived : public alg::sfinae_base{};
    struct sfinae_derder0 : public sfinae_derived{};
    struct sfinae_derder1 : public sfinae_derived{};

    assert(alg::sfinae_inheritance_method1(int{123})                  == false);
    assert(alg::sfinae_inheritance_method1(std::string{"123"})        == false);
    assert(alg::sfinae_inheritance_method1(std::vector<int>{1,2,3,4}) == false);
    assert(alg::sfinae_inheritance_method1(sfinae_derived{})          == true);
    assert(alg::sfinae_inheritance_method1(sfinae_derder0{})          == true);
    assert(alg::sfinae_inheritance_method1(sfinae_derder1{})          == true);

    assert(alg::sfinae_inheritance_method2(int{123})                  == false);
    assert(alg::sfinae_inheritance_method2(std::string{"123"})        == false);
    assert(alg::sfinae_inheritance_method2(std::vector<int>{1,2,3,4}) == false);
    assert(alg::sfinae_inheritance_method2(sfinae_derived{})          == true);
    assert(alg::sfinae_inheritance_method2(sfinae_derder0{})          == true);
    assert(alg::sfinae_inheritance_method2(sfinae_derder1{})          == true);
    print_summary(" sfinae traits", "succeeded");
}


void test_traits()
{
    test_simple_traits();
    test_runtime_traits();
    test_sfinae_traits();
}

