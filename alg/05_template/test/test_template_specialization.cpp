#include<iostream>
#include<cassert>
#include<cstdint>
#include<vector>
#include<string>
#include<tuple>
#include<utility.h>


// ************************************* //
// *** Template class specialization *** //
// ************************************* //
// When used as type-type mapping, template struct is like a multivariate function.
//
// We define specialization like below :
// (a) make sure specialization is a subset of generic template
// (b) feasible-set defined by line 3 is a subset of feasible-set defined by line 1
// (c) feasible-set defined by line 2 is not related feasible-set defined by line 1
//
template<typename T0, typename T1, typename T2> // <--- line 1
struct sample_class
{
    using output_type = std::tuple<T0,T1,T2>;
};

template<typename U0, typename U1, typename U2, typename U3>              // <--- line 2
struct sample_class<std::pair<U0,U1>, std::pair<U1,U2>, std::pair<U2,U3>> // <--- line 3
{
    using output_type = std::tuple<U0,U1,U2,U3>;
}; 

class A{};
class B{};
class C{};
class D{};

void test_class_template_specialization()
{
    using Y0 = sample_class<A,B,C>::output_type;
    static_assert(std::is_same_v<Y0, std::tuple<A,B,C>>, "type mapping incorrect");

    using Y1 = sample_class<std::pair<A,B>,
                            std::pair<B,C>, 
                            std::pair<C,D>>::output_type;
    static_assert(std::is_same_v<Y1, std::tuple<A,B,C,D>>, "type mapping incorrect");

    using Y2 = sample_class<std::pair<A,B>,
                            std::pair<C,D>, 
                            std::pair<B,D>>::output_type;
    static_assert(std::is_same_v<Y2, std::tuple<std::pair<A,B>,
                                                std::pair<C,D>, 
                                                std::pair<B,D>>>, "type mapping incorrect");

    print_summary("   class template specialization", "succeeded in compile time");
}


// **************************************** // 
// *** Template function specialization *** //
// **************************************** // 
// As template functions are just function overload, 
// we can put parameter after parameter-pack (see line A).
//

std::uint32_t which_overload = 0;

template<typename...T>  
void sample_function(const T&... x)
{
    which_overload = 1;
}

template<typename T0, typename T1, typename T2> 
void sample_function(const T0& x0, const T1& x1, const T2& x2)
{
    which_overload = 2;
}

template<typename T0, typename T1, typename T2, typename T3>  
void sample_function(const T0& x0, const T1& x1, const T2& x2, const T3& x3)
{
    which_overload = 3;
}

template<typename...T>
void sample_function(const std::tuple<T...>& tup) 
{
    which_overload = 4;
}

template<typename...T, typename U> // <--- line A
void sample_function(const std::tuple<T...>& tup, const U& x) 
{
    which_overload = 5;
}

void test_function_template_specialization()
{
    sample_function(std::uint32_t{1});
    assert(which_overload == 1);
    sample_function(std::uint32_t{1}, std::uint32_t{1});
    assert(which_overload == 1);
    sample_function(std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1});
    assert(which_overload == 2);
    sample_function(std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1});
    assert(which_overload == 3);
    sample_function(std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1}, std::uint32_t{1});
    assert(which_overload == 1);
    sample_function(std::make_tuple(1,2));  
    assert(which_overload == 4);
    sample_function(std::make_tuple(1,2,3)); 
    assert(which_overload == 4);
    sample_function(std::make_tuple(1,2), 12.3);  
    assert(which_overload == 5);
    sample_function(std::make_tuple(1,2,3), "123"); 
    assert(which_overload == 5);
    print_summary("function template specialization", "succeeded");
}


// ******************************************************* // 
// *** Template function specialization for char array *** //
// ******************************************************* // 
namespace test
{
    template<typename T>
    std::uint32_t fct(const T& x)
    {
        return 0;
    } 

    template<std::uint32_t N>
    std::uint32_t fct(const char(&x)[N])
    {
        return 1;
    } 
}

void test_function_template_specialization_for_char_array()
{
    const char  ac0[ 6] = "12345";
    const char  ac1[ 8] = "1234567";       
    const char  ac2[10] = "123456789";  // reserve 1 char for null char
    const char* pc0     = "12345";
    const char* pc1     = "1234567";       
    const char* pc2     = "123456789";  // refer to 04_fundalmental/test_constexpr_map.cpp for more about const char*

    assert(test::fct(std::uint32_t{}) == 0);
    assert(test::fct(std::uint64_t{}) == 0);
    assert(test::fct(std::string{})   == 0);
    assert(test::fct(std::vector<int>{1,2,3,4,5}) == 0);
    assert(test::fct(ac0) == 1);
    assert(test::fct(ac1) == 1);
    assert(test::fct(ac2) == 1);
    assert(test::fct(pc0) == 0);
    assert(test::fct(pc1) == 0);
    assert(test::fct(pc2) == 0); 

    print_summary("function template specialization for char array", "succeeded");
}


void test_template_specialization()
{
    test_class_template_specialization();
    test_function_template_specialization();
    test_function_template_specialization_for_char_array();
}
