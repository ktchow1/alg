#include<iostream>
#include<cassert>
#include<cstdint>
#include<vector>
#include<list>
#include<concepts>

// *** alg *** //
#include<concepts.h>
#include<utility.h>




// ***************************** //
// *** Defining alg concepts *** //
// ***************************** //
// Concept is related to traits. The former is usually built on top of the latter.
//
// Naming in STL : 
// * std::is_xxx_able  <T> = traits
// * std::is_xxx_able_v<T> = std::is_xxx_able<T>::value
// * std::is_xxx_able_t<T> = std::is_xxx_able<T>::type
// * std::   xxx_able  <T> = concept based on std::is_xxx_able<T>
//
//
// Concept is defined as conjunction or disjunction of constraints. There are 7 ways to define constraints : 
// 1. value traits in class template  
// 2. value traits in variable template  
// 3. requires expression -     type requirement
// 4. requires expression -   simple requirement
// 5. requires expression - compound requirement (with brace {})
// 6. requires expression -   nested requirement
// 7. other concepts                             <--- this is same as 6? just put concept in different places
//
namespace test
{
    struct typeK       { using type = std::uint32_t;   std::uint32_t foo() { return 123;   } };
    struct typeL       { using type = std::uint32_t;   std::uint32_t fct() { return 123;   } };
    struct typeM       { using type = std::uint32_t;   std::string   fct() { return "ABC"; } };
    struct typeN       {                               std::string   fct() { return "ABC"; } };
  
    template<typename T> struct is_KLM        { static constexpr bool value = false;  };
    template<>           struct is_KLM<typeK> { static constexpr bool value =  true;  };
    template<>           struct is_KLM<typeL> { static constexpr bool value =  true;  };
    template<>           struct is_KLM<typeM> { static constexpr bool value =  true;  };

    template<typename T> constexpr bool is_LMN        = false;
    template<>           constexpr bool is_LMN<typeL> =  true;
    template<>           constexpr bool is_LMN<typeM> =  true;
    template<>           constexpr bool is_LMN<typeN> =  true;

    template<typename T>
    concept concept_method1 = is_KLM<T>::value;

    template<typename T>
    concept concept_method2 = is_LMN<T>;

    template<typename T>
    concept concept_method3 = 
    requires
    {
        typename T::type;
        // <--- can be multi lines
    };

    template<typename T>
    concept concept_method4 = 
    requires(T x) // <--- can be multi arguments
    {
        x.fct();
    };

    template<typename T>
    concept concept_method5 = 
    requires(T x)
    {
        { x.fct() } -> std::same_as<std::string>;
    //  { x.fct() } ->              std::string;  // BUG : compile error
    };

    template<typename T>
    concept concept_method6 = 
    requires(T x)
    {
        { x.fct() } -> std::same_as<std::string>;
        requires concept_method1<T>;
        requires concept_method2<T>;
    };

    template<typename T>
    concept concept_method7 = 
    concept_method1<T> &&
    concept_method2<T> &&
    requires(T x)
    {
        { x.fct() } -> std::same_as<std::string>;
    };
}


// ************** //
// *** Test 1 *** //
// ************** //
namespace test
{
    template<typename T, typename U> requires concept_method1<T>
    void tester_method1(const T&, const U&) {}

    template<typename T, typename U> requires concept_method2<T>
    void tester_method2(const T&, const U&) {}

    template<typename T, typename U> requires concept_method3<T>
    void tester_method3(const T&, const U&) {}

    template<typename T, typename U> requires concept_method4<T>
    void tester_method4(const T&, const U&) {}

    template<typename T, typename U> requires concept_method5<T>
    void tester_method5(const T&, const U&) {}

    template<typename T, typename U> requires concept_method6<T>
    void tester_method6(const T&, const U&) {}
     
    template<typename T, typename U> requires concept_method7<T>
    void tester_method7(const T&, const U&) {}
}

void test_concepts_define()
{
//  test::tester_method1(std::string{"x"}, std::string{"abc"});
    test::tester_method1(test::typeK{},    std::string{"abc"});
    test::tester_method1(test::typeL{},    std::string{"abc"});
    test::tester_method1(test::typeM{},    std::string{"abc"});
//  test::tester_method1(test::typeN{},    std::string{"abc"});
  
//  test::tester_method2(std::string{"x"}, std::string{"abc"});
//  test::tester_method2(test::typeK{},    std::string{"abc"});
    test::tester_method2(test::typeL{},    std::string{"abc"});
    test::tester_method2(test::typeM{},    std::string{"abc"});
    test::tester_method2(test::typeN{},    std::string{"abc"});

//  test::tester_method3(std::string{"x"}, std::string{"abc"});
    test::tester_method3(test::typeK{},    std::string{"abc"});
    test::tester_method3(test::typeL{},    std::string{"abc"});
    test::tester_method3(test::typeM{},    std::string{"abc"});
//  test::tester_method3(test::typeN{},    std::string{"abc"});

//  test::tester_method4(std::string{"x"}, std::string{"abc"});
//  test::tester_method4(test::typeK{},    std::string{"abc"});
    test::tester_method4(test::typeL{},    std::string{"abc"});
    test::tester_method4(test::typeM{},    std::string{"abc"});
    test::tester_method4(test::typeN{},    std::string{"abc"});

//  test::tester_method5(std::string{"x"}, std::string{"abc"});
//  test::tester_method5(test::typeK{},    std::string{"abc"});
//  test::tester_method5(test::typeL{},    std::string{"abc"});
    test::tester_method5(test::typeM{},    std::string{"abc"});
    test::tester_method5(test::typeN{},    std::string{"abc"});
  
//  test::tester_method6(std::string{"x"}, std::string{"abc"});
//  test::tester_method6(test::typeK{},    std::string{"abc"});
//  test::tester_method6(test::typeL{},    std::string{"abc"});
    test::tester_method6(test::typeM{},    std::string{"abc"});
//  test::tester_method6(test::typeN{},    std::string{"abc"});

//  test::tester_method7(std::string{"x"}, std::string{"abc"});
//  test::tester_method7(test::typeK{},    std::string{"abc"});
//  test::tester_method7(test::typeL{},    std::string{"abc"});
    test::tester_method7(test::typeM{},    std::string{"abc"});
//  test::tester_method7(test::typeN{},    std::string{"abc"});
  
    print_summary("coocepts - define concept", "succeeded");
}


// ************** //
// *** Test 2 *** //
// ************** //
namespace test
{
    template<typename T, typename U> 
    void tester_container(const T&, const U&) requires alg::container<T> {}

    template<typename T, typename U>
    void tester_incrementable(const T&, const U&) requires alg::incrementable<T> {}

    template<typename T, typename U>
    void tester_decrementable(const T&, const U&) requires alg::decrementable<T> {}

    template<typename T, typename U>
    void tester_hashable(const T&, const U&) requires alg::hashable<T> {}

    template<typename T, typename U>
    void tester_non_commutative_addable(const T&, const U&) requires alg::non_commutative_addable<T,U> {}

    template<typename T, typename U>
    void tester_commutative_addable(const T&, const U&) requires alg::commutative_addable<T,U> {}

    template<typename T, typename U>
    void tester_derived_from(const T&, const U&) requires alg::derived_from<T,U> {}

    template<typename T, typename...ARGS>
    void tester_invocable(const T&, ARGS&&...args) requires alg::invocable<T,ARGS...> {}

    template<typename T, typename...ARGS>
    void tester_invocable2(const T&, ARGS&&...args) requires alg::invocable2<T,ARGS...> {}
}


namespace // Why use anonymous namespace? To make the following local to translation unit
{
    struct self_add
    {
        self_add& operator++() { return *this; }
    };

    struct hash_pod
    {
    };

    struct add_A 
    {
        const add_A& operator+(const add_A& rhs) { return *this; }
    };

    struct add_B 
    {
        const add_B& operator+(const add_B& rhs) { return *this; }
    };

    const add_A& operator+(const add_A& a, const add_B& b)
    {
        return a;
    }

    void just_a_fct(bool, std::uint32_t, const std::string&)
    {
    }

    struct just_a_fctor
    {
        void operator()(bool, std::uint32_t, const std::string&){}
    };
}

namespace std
{
    template<> struct hash<hash_pod>
    {
        std::size_t operator()(const hash_pod&) { return std::size_t{}; }
    };
}


void test_concepts_define_examples()
{
//  test::tester_container(std::uint32_t{123},  std::string{"abc"});
//  test::tester_container(std::pair{123,123},  std::string{"abc"});
    test::tester_container(std::string{"abc"},  std::string{"abc"});
    test::tester_container(std::vector<char>{}, std::string{"abc"});
    test::tester_container(std::list  <char>{}, std::string{"abc"});

    test::tester_incrementable(std::uint32_t{123},  std::string{"abc"});
//  test::tester_incrementable(std::string{"abc"},  std::string{"abc"});
//  test::tester_incrementable(std::vector<char>{}, std::string{"abc"});
    test::tester_incrementable(self_add{},          std::string{"abc"});

    test::tester_hashable(std::uint32_t{123},  std::string{"abc"});
    test::tester_hashable(std::string{"abc"},  std::string{"abc"});
//  test::tester_hashable(std::vector<char>{}, std::string{"abc"});
//  test::tester_hashable(self_add{},          std::string{"abc"});
    test::tester_hashable(hash_pod{},          std::string{"abc"});
    
    test::tester_non_commutative_addable(std::uint32_t{123},  std::uint32_t{123});
//  test::tester_non_commutative_addable(std::uint32_t{123},  std::string{"abc"});
    test::tester_non_commutative_addable(std::string{"abc"},  std::string{"abc"});
//  test::tester_non_commutative_addable(std::vector<char>{}, std::vector<char>{});

    test::tester_non_commutative_addable(add_A{}, add_A{});
    test::tester_non_commutative_addable(add_B{}, add_B{});
    test::tester_non_commutative_addable(add_A{}, add_B{});
//  test::tester_non_commutative_addable(add_B{}, add_A{});
    test::tester_commutative_addable(add_A{}, add_A{});
    test::tester_commutative_addable(add_B{}, add_B{});
    test::tester_commutative_addable(add_A{}, add_B{});
    test::tester_commutative_addable(add_B{}, add_A{});

    test::tester_invocable ([](bool, std::uint32_t, const std::string&){}, bool{true}, std::uint32_t{123}, std::string{"abc"});
    test::tester_invocable (&just_a_fct,                                   bool{true}, std::uint32_t{123}, std::string{"abc"});
    test::tester_invocable ( just_a_fctor{},                               bool{true}, std::uint32_t{123}, std::string{"abc"});
    test::tester_invocable2([](bool, std::uint32_t, const std::string&){}, bool{true}, std::uint32_t{123}, std::string{"abc"});
    test::tester_invocable2(&just_a_fct,                                   bool{true}, std::uint32_t{123}, std::string{"abc"});
    test::tester_invocable2( just_a_fctor{},                               bool{true}, std::uint32_t{123}, std::string{"abc"});

    print_summary("coocepts - define concept (examples)", "succeeded");
}

