#include<iostream>
#include<cassert>
#include<cstdint>
#include<string>
#include<concepts>

// *** alg *** //
#include<utility.h>



// ***************************** //
// *** Applying std concepts *** //
// ***************************** //
// There are 4 ways to apply concept :
// 1. as requires-clause after template parameter   (supports conjunction / disjunction of multiple concepts)
// 2. as requires-clause after function prototype   (supports conjunction / disjunction of multiple concepts)
// 3. as requires-clause in template parameter list (does not support conjunction / disjunction) <--- 1st template parameter is the one on which constraint applied
// 4. as requires-requires pattern, which is a requires-clause plus a requires-expression
//
// 
// requires-clause      : apply concept to template class or template function
// requires-expresssion : define a new concept
//
//
// Format of a requires-clause : 
// requires std::xxx<T>          <--- where xxx is a concept
// requires std::xxx<T,U,...>    <--- where xxx is a concept
//    
namespace test
{
    // ********************************************** //
    // *** std concepts with 1 template parameter *** //
    // ********************************************** //
    struct type_A { type_A() = delete; type_A(std::uint32_t){}                                    void special_fct(){} };
    struct type_B { type_B(){}         type_B(const type_B&) = delete; type_B(type_B&&){}         void special_fct(){} };
    struct type_C { type_C(){}         type_C(const type_C&){}         type_C(type_C&&) = delete; void special_fct(){} };

    template<typename T, typename U> requires std::default_initializable<T>
    void tester_no_A_method1(const T&, const U&) {}
    template<typename T, typename U> requires std::copy_constructible<T>
    void tester_no_B_method1(const T&, const U&) {}
    template<typename T, typename U> requires std::move_constructible<T>
    void tester_no_C_method1(const T&, const U&) {}

    template<typename T, typename U> 
    void tester_no_A_method2(const T&, const U&) requires std::default_initializable<T> {}
    template<typename T, typename U> 
    void tester_no_B_method2(const T&, const U&) requires std::copy_constructible<T> {}
    template<typename T, typename U> 
    void tester_no_C_method2(const T&, const U&) requires std::move_constructible<T> {}

    template<std::default_initializable T, typename U> 
    void tester_no_A_method3(const T&, const U&) {}
    template<std::copy_constructible T, typename U> 
    void tester_no_B_method3(const T&, const U&) {}
    template<std::move_constructible T, typename U> 
    void tester_no_C_method3(const T&, const U&) {}


    // *********************************************** //
    // *** std concepts with 2 template parameters *** //
    // *********************************************** //
    // 1. order of template parameter in std::is_base_of<B,D> is different from 
    //    order of template parameter in std::derived_from<D,B>
    // 2. when using std::derived_from<D,B> to replace typename in method 3,
    //    simply use std::derived_from<B>
      
    struct type_D : public type_B 
    {
    private:
        void special_fct(); // overwritten type_B::special_fct(), so that type_D cannot fulfill concept type_ABC
    };

    template<typename T, typename U> requires std::derived_from<U, type_B>
    void tester_derived_from_B_method1(const T&, const U&) {}
    template<typename T, typename U> requires std::derived_from<U, T>
    void tester_derived_from_T_method1(const T&, const U&) {}

    template<typename T, typename U> 
    void tester_derived_from_B_method2(const T&, const U&) requires std::derived_from<U, type_B> {}
    template<typename T, typename U>
    void tester_derived_from_T_method2(const T&, const U&) requires std::derived_from<U, T> {}

    template<typename T, std::derived_from<type_B> U> 
    void tester_derived_from_B_method3(const T&, const U&) {}
    template<typename T, std::derived_from<T> U>
    void tester_derived_from_T_method3(const T&, const U&) {}


    // *********************************************** //
    // *** std concepts with 3 template parameters *** //
    // *********************************************** //
    template<typename T, typename U, typename F> requires std::invocable<F, std::string, T>
    void tester_invoke_binary_method1(const T&, const U&, const F&) {}
    template<typename T, typename U, typename F> requires std::invocable<F, std::string, T, U>
    void tester_invoke_ternary_method1(const T&, const U&, const F&) {}

    template<typename T, typename U, typename F>
    void tester_invoke_binary_method2(const T&, const U&, const F&) requires std::invocable<F, std::string, T> {}
    template<typename T, typename U, typename F>
    void tester_invoke_ternary_method2(const T&, const U&, const F&) requires std::invocable<F, std::string, T, U> {}

    template<typename T, typename U, std::invocable<std::string,T> F>
    void tester_invoke_binary_method3(const T&, const U&, const F&) {}
    template<typename T, typename U, std::invocable<std::string,T,U> F>
    void tester_invoke_ternary_method3(const T&, const U&, const F&) {}


    // ******************************************************* //
    // *** std concepts with abbreviated template function *** //
    // ******************************************************* //
    // Abbreviated template function is function with "auto" argument.
    // Concept can be applied to "auto" argument like the following.
    //

    auto abbrev_fct_invoker(const std::invocable<std::string, std::uint32_t, std::uint32_t, std::uint32_t> auto& fct, std::uint32_t id) 
    {
        return fct(std::string{"abbrev fct invoker : "}, id, id+1, id+2);
    }


    // ******************************************* //
    // *** simple concept - no requires-clause *** //
    // ******************************************* //
    struct type_E {};

    template<typename T>  constexpr bool is_type_ABCD         = false;
    template<>            constexpr bool is_type_ABCD<type_A> = true;
    template<>            constexpr bool is_type_ABCD<type_B> = true;
    template<>            constexpr bool is_type_ABCD<type_C> = true;
    template<>            constexpr bool is_type_ABCD<type_D> = true;

    template<typename T>
    concept type_ABCD = is_type_ABCD<T>;

    template<typename T, typename U> requires type_ABCD<T>
    void tester_type_ABCD_method1(const T&, const U&) {}

    template<typename T, typename U> 
    void tester_type_ABCD_method2(const T&, const U&) requires type_ABCD<T> {}

    template<type_ABCD T, typename U> 
    void tester_type_ABCD_method3(const T&, const U&) {}


    // *** SFINAE *** //
    template<typename T, typename std::enable_if<is_type_ABCD<T>, int>::type = 0>
    std::uint32_t tester_sfinae_method1(const T&)
    {
        return 11;
    };

    template<typename T, typename std::enable_if<!is_type_ABCD<T>, int>::type = 0>
    std::uint32_t tester_sfinae_method1(const T&)
    {
        return 12;
    };

    template<typename T>
    constexpr std::uint32_t tester_sfinae_method2(const T&)
    {
        return 22;
    };

    template<type_ABCD T>
    std::uint32_t tester_sfinae_method2(const T&)
    {
        return 21;
    };


    // ************************************************************* //
    // *** simple concept - support method 4 "requires-requires" *** //
    // ************************************************************* //
    template<typename T>
    concept type_ABC = requires(T x)
    {
        x.special_fct();
    };

    template<typename T, typename U> requires type_ABC<T>
    void tester_type_ABC_method1(const T&, const U&) {}

    template<typename T, typename U> 
    void tester_type_ABC_method2(const T&, const U&) requires type_ABC<T> {}

    template<type_ABC T, typename U> 
    void tester_type_ABC_method3(const T&, const U&) {}

    template<typename T, typename U> requires requires(T x) { x.special_fct(); }
    void tester_type_ABC_method4(const T&, const U&) {}
}


// *************************************************** //
// *** Comment lines cannot compile due to concept *** //
// *************************************************** //
void test_concepts_apply_constraint_on_1_para()
{
//  test::tester_no_A_method1(test::type_A{123}, std::string{"abc"});
    test::tester_no_A_method1(test::type_B{},    std::string{"abc"});
    test::tester_no_A_method1(test::type_C{},    std::string{"abc"});
    test::tester_no_B_method1(test::type_A{123}, std::string{"abc"});
//  test::tester_no_B_method1(test::type_B{},    std::string{"abc"});
//  test::tester_no_B_method1(test::type_C{},    std::string{"abc"}); // std::copy_constructible = requires copy_constructible AND move_constructible
    test::tester_no_C_method1(test::type_A{123}, std::string{"abc"});
    test::tester_no_C_method1(test::type_B{},    std::string{"abc"}); // std::move_constructible = requires move_constructible only
//  test::tester_no_C_method1(test::type_C{},    std::string{"abc"});

//  test::tester_no_A_method2(test::type_A{123}, std::string{"abc"});
    test::tester_no_A_method2(test::type_B{},    std::string{"abc"});
    test::tester_no_A_method2(test::type_C{},    std::string{"abc"});
    test::tester_no_B_method2(test::type_A{123}, std::string{"abc"});
//  test::tester_no_B_method2(test::type_B{},    std::string{"abc"});
//  test::tester_no_B_method2(test::type_C{},    std::string{"abc"}); // std::copy_constructible = requires copy_constructible AND move_constructible
    test::tester_no_C_method2(test::type_A{123}, std::string{"abc"});
    test::tester_no_C_method2(test::type_B{},    std::string{"abc"}); // std::move_constructible = requires move_constructible only
//  test::tester_no_C_method2(test::type_C{},    std::string{"abc"});
  
//  test::tester_no_A_method3(test::type_A{123}, std::string{"abc"});
    test::tester_no_A_method3(test::type_B{},    std::string{"abc"});
    test::tester_no_A_method3(test::type_C{},    std::string{"abc"});
    test::tester_no_B_method3(test::type_A{123}, std::string{"abc"});
//  test::tester_no_B_method3(test::type_B{},    std::string{"abc"});
//  test::tester_no_B_method3(test::type_C{},    std::string{"abc"}); // std::copy_constructible = requires copy_constructible AND move_constructible
    test::tester_no_C_method3(test::type_A{123}, std::string{"abc"});
    test::tester_no_C_method3(test::type_B{},    std::string{"abc"}); // std::move_constructible = requires move_constructible only
//  test::tester_no_C_method3(test::type_C{},    std::string{"abc"});
    print_summary("coocepts - apply constraint on 1 para", "succeeded");
}


void test_concepts_apply_constraint_on_2_para()
{
//  test::tester_derived_from_B_method1(test::type_B{}, test::type_A{123});
    test::tester_derived_from_B_method1(test::type_B{}, test::type_D{});
    test::tester_derived_from_T_method1(test::type_B{}, test::type_D{});
//  test::tester_derived_from_T_method1(test::type_C{}, test::type_D{});

//  test::tester_derived_from_B_method2(test::type_B{}, test::type_A{123});
    test::tester_derived_from_B_method2(test::type_B{}, test::type_D{});
    test::tester_derived_from_T_method2(test::type_B{}, test::type_D{});
//  test::tester_derived_from_T_method2(test::type_C{}, test::type_D{});

//  test::tester_derived_from_B_method3(test::type_B{}, test::type_A{123});
    test::tester_derived_from_B_method3(test::type_B{}, test::type_D{});
    test::tester_derived_from_T_method3(test::type_B{}, test::type_D{});
//  test::tester_derived_from_T_method3(test::type_C{}, test::type_D{});
    print_summary("coocepts - apply constraint on 2 para", "succeeded");
}


void test_concepts_apply_constraint_on_3_para()
{
//  test::tester_invoke_binary_method1 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, const std::string&){});
    test::tester_invoke_binary_method1 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
//  test::tester_invoke_ternary_method1(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
    test::tester_invoke_ternary_method1(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t, const std::string&){});

//  test::tester_invoke_binary_method2 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, const std::string&){});
    test::tester_invoke_binary_method2 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
//  test::tester_invoke_ternary_method2(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
    test::tester_invoke_ternary_method2(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t, const std::string&){});

//  test::tester_invoke_binary_method3 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, const std::string&){});
    test::tester_invoke_binary_method3 (std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
//  test::tester_invoke_ternary_method3(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t){});
    test::tester_invoke_ternary_method3(std::uint32_t{123}, std::string{"abc"}, [](const std::string&, std::uint32_t, const std::string&){});
    print_summary("coocepts - apply constraint on 3 para", "succeeded");
}

struct abbrev_functor
{
    auto operator()(const std::string& s, std::uint32_t x, std::uint32_t y, std::uint32_t z) const
    {
        std::stringstream ss;
        ss << "[functor] ";
        ss << s;
        ss << x << ", ";
        ss << y << ", ";
        ss << z;
        return ss.str();
    }
};

void test_concepts_apply_constraint_on_abbrev_fct_template()
{
    auto ans1 = test::abbrev_fct_invoker(abbrev_functor{}, std::uint32_t{10});
    auto ans2 = test::abbrev_fct_invoker([](const std::string& s, std::uint32_t x, std::uint32_t y, std::uint32_t z)
    {
        std::stringstream ss;
        ss << "[lambda] ";
        ss << s;
        ss << x << ", ";
        ss << y << ", ";
        ss << z;
        return ss.str();
    }
    , std::uint32_t{20});

    assert(ans1 == std::string{"[functor] abbrev fct invoker : 10, 11, 12"});
    assert(ans2 == std::string{"[lambda] abbrev fct invoker : 20, 21, 22"});
    print_summary("coocepts - apply constraint on abbrev fct template", "succeeded");
}


void test_concepts_vs_SFINAE()
{
//  test::tester_type_ABCD_method1(std::uint32_t{"abc"}, std::string{"abc"});
    test::tester_type_ABCD_method1(test::type_A{123},    std::string{"abc"});
    test::tester_type_ABCD_method1(test::type_B{},       std::string{"abc"});
    test::tester_type_ABCD_method1(test::type_C{},       std::string{"abc"});
    test::tester_type_ABCD_method1(test::type_D{},       std::string{"abc"});
//  test::tester_type_ABCD_method1(test::type_E{},       std::string{"abc"});

//  test::tester_type_ABCD_method2(std::uint32_t{"abc"}, std::string{"abc"});
    test::tester_type_ABCD_method2(test::type_A{123},    std::string{"abc"});
    test::tester_type_ABCD_method2(test::type_B{},       std::string{"abc"});
    test::tester_type_ABCD_method2(test::type_C{},       std::string{"abc"});
    test::tester_type_ABCD_method2(test::type_D{},       std::string{"abc"});
//  test::tester_type_ABCD_method2(test::type_E{},       std::string{"abc"});

//  test::tester_type_ABCD_method3(std::uint32_t{"abc"}, std::string{"abc"});
    test::tester_type_ABCD_method3(test::type_A{123},    std::string{"abc"});
    test::tester_type_ABCD_method3(test::type_B{},       std::string{"abc"});
    test::tester_type_ABCD_method3(test::type_C{},       std::string{"abc"});
    test::tester_type_ABCD_method3(test::type_D{},       std::string{"abc"});
//  test::tester_type_ABCD_method3(test::type_E{},       std::string{"abc"});

    // SFINAE 
    assert(test::tester_sfinae_method1( test::type_A {123}) == 11);
    assert(test::tester_sfinae_method1(std::uint32_t {123}) == 12);
    assert(test::tester_sfinae_method2( test::type_A {123}) == 21);
    assert(test::tester_sfinae_method2(std::uint32_t {123}) == 22);
    print_summary("coocepts - vs SFINAE", "succeeded");
}


void test_concepts_requires_requires()
{
//  test::tester_type_ABC_method1(std::uint32_t{"abc"},  std::string{"abc"});
    test::tester_type_ABC_method1(test::type_A{123},     std::string{"abc"});
    test::tester_type_ABC_method1(test::type_B{},        std::string{"abc"});
    test::tester_type_ABC_method1(test::type_C{},        std::string{"abc"});
//  test::tester_type_ABC_method1(test::type_D{},        std::string{"abc"}); 
//  test::tester_type_ABC_method1(test::type_E{},        std::string{"abc"});

//  test::tester_type_ABC_method2(std::uint32_t{"abc"},  std::string{"abc"});
    test::tester_type_ABC_method2(test::type_A{123},     std::string{"abc"});
    test::tester_type_ABC_method2(test::type_B{},        std::string{"abc"});
    test::tester_type_ABC_method2(test::type_C{},        std::string{"abc"});
//  test::tester_type_ABC_method2(test::type_D{},        std::string{"abc"}); 
//  test::tester_type_ABC_method2(test::type_E{},        std::string{"abc"});
  
//  test::tester_type_ABC_method3(std::uint32_t{"abc"},  std::string{"abc"});
    test::tester_type_ABC_method3(test::type_A{123},     std::string{"abc"});
    test::tester_type_ABC_method3(test::type_B{},        std::string{"abc"});
    test::tester_type_ABC_method3(test::type_C{},        std::string{"abc"});
//  test::tester_type_ABC_method3(test::type_D{},        std::string{"abc"}); 
//  test::tester_type_ABC_method3(test::type_E{},        std::string{"abc"});
    
//  test::tester_type_ABC_method4(std::uint32_t{"abc"},  std::string{"abc"});
    test::tester_type_ABC_method4(test::type_A{123},     std::string{"abc"});
    test::tester_type_ABC_method4(test::type_B{},        std::string{"abc"});
    test::tester_type_ABC_method4(test::type_C{},        std::string{"abc"});
//  test::tester_type_ABC_method4(test::type_D{},        std::string{"abc"}); 
//  test::tester_type_ABC_method4(test::type_E{},        std::string{"abc"}); 
    print_summary("coocepts - requires requires", "succeeded");
}


void test_concepts_apply()
{
    test_concepts_apply_constraint_on_1_para();
    test_concepts_apply_constraint_on_2_para();
    test_concepts_apply_constraint_on_3_para();
    test_concepts_apply_constraint_on_abbrev_fct_template();
    test_concepts_vs_SFINAE();
    test_concepts_requires_requires();
}




