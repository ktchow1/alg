#include<iostream>
#include<cassert>
#include<rvalue.h>
#include<utility.h>



// ************** //
// *** Sample *** //
// ************** //
class sample
{
public:
    sample(int x, int y, int z) 
    {
        m_src = alg::object_source::default_constructed;
        ++num_instance;
    }

    // copyable
    sample(const sample& rhs)
    {
        m_src = alg::object_source::copy_constructed;
        ++num_instance;
    }
    sample& operator=(const sample& rhs)
    {
        m_src = alg::object_source::copy_assigned;
        ++num_instance;
        return *this;
    }

    // movable
    sample(sample&&) 
    {
        m_src = alg::object_source::move_constructed;
    }

    sample& operator=(sample&&)
    {
        m_src = alg::object_source::move_assigned;
        return *this;
    }

    alg::object_source m_src;
    static int num_instance;
};

int sample::num_instance = 0;


void test_return_rvalue()
{
    alg::rvalue_factory<sample> factory(10,11,12);
    assert(sample::num_instance == 1);

    static_assert(std::is_same_v<decltype(factory.create_const_lvalue()), const sample& >, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype(factory.create_lvalue()),             sample& >, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype(factory.create_xvalue()),             sample&&>, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype(factory.create_prvalue()),            sample  >, "rvalue - factory type check failed");

    // complex expression, should give same result
    static_assert(std::is_same_v<decltype((factory.create_const_lvalue())), const sample& >, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype((factory.create_lvalue())),             sample& >, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype((factory.create_xvalue())),             sample&&>, "rvalue - factory type check failed");
    static_assert(std::is_same_v<decltype((factory.create_prvalue())),            sample  >, "rvalue - factory type check failed");


    // ******************* //
    // *** Return type *** //
    // ******************* //
    const auto& x = factory.create_const_lvalue();
    assert(&x == factory.get_pointer());
    assert(x.m_src == alg::object_source::default_constructed); // refers to m_value inside rvalue_factory
    assert(sample::num_instance == 1);

    auto& y = factory.create_lvalue();
    assert(&y == factory.get_pointer());
    assert(y.m_src == alg::object_source::default_constructed); // refers to m_value inside rvalue_factory
    assert(sample::num_instance == 1);

    auto&& z = factory.create_xvalue();
    assert(&z == factory.get_pointer());
    assert(z.m_src == alg::object_source::default_constructed); // refers to m_value inside rvalue_factory
    assert(sample::num_instance == 1);

    auto w = factory.create_prvalue();
    assert(&w != factory.get_pointer());
    assert(w.m_src == alg::object_source::copy_constructed); 
    assert(sample::num_instance == 2);
  
    // ***************************************************** //
    // *** Named rvalue reference (z) is actually lvalue *** //
    // ***************************************************** //
    
    auto u = z; // invoke copying
    assert(&u != factory.get_pointer());
    assert(u.m_src == alg::object_source::copy_constructed); 
    assert(sample::num_instance == 3);

    auto v = std::move(z); // invoke moving 
    assert(&v != factory.get_pointer()); 
    assert(v.m_src == alg::object_source::move_constructed); 
    assert(sample::num_instance == 3);
    print_summary("rvalue - return rvalue", "succeeded");
}


void test_input_rvalue()
{
    alg::rvalue_factory<sample> factory(10,11,12);

    alg::binding_const_lvalue_ref<sample>(factory.create_const_lvalue());
    alg::binding_const_lvalue_ref<sample>(factory.create_lvalue());
    alg::binding_const_lvalue_ref<sample>(factory.create_xvalue());
    alg::binding_const_lvalue_ref<sample>(factory.create_prvalue());

//  alg::binding_lvalue_ref<sample>(factory.create_const_lvalue());
    alg::binding_lvalue_ref<sample>(factory.create_lvalue());
//  alg::binding_lvalue_ref<sample>(factory.create_xvalue());
//  alg::binding_lvalue_ref<sample>(factory.create_prvalue());
  
//  alg::binding_rvalue_ref<sample>(factory.create_const_lvalue());
//  alg::binding_rvalue_ref<sample>(factory.create_lvalue());
    alg::binding_rvalue_ref<sample>(factory.create_xvalue());
    alg::binding_rvalue_ref<sample>(factory.create_prvalue());

    alg::binding_value<sample>(factory.create_const_lvalue());
    alg::binding_value<sample>(factory.create_lvalue());
    alg::binding_value<sample>(factory.create_xvalue());
    alg::binding_value<sample>(factory.create_prvalue());
    print_summary("rvalue - input rvalue", "succeeded");
}


// ******************* //
// *** 4 questions *** //
// ******************* //
// Given an expression, ask 4 questions :
// * what is the type of the expression?
// * what is the valueness of the expression?
// * what function signature can be used to bind the expression?
// * if the function signature is template / auto, how to deduce the type?
//
//
// with name? |  type  |  valuenesss  | bound by
// -----------+--------+--------------+-------------
//   named    |   X    |  lvalue      | f(T&)
// unnamed    |   X    |  rvalue      | f(T&&) <--- this is relevant to implementation of alg::make_tuple / tie / forward_as_tuple
//   named    |   X&   |  lvalue      | f(T&)
// unnamed    |   X&   |  lvalue      | f(T&)
//   named    |   X&&  |  lvalue      | f(T&)
// unnamed    |   X&&  |  rvalue      | f(T&&) 

namespace toy_example
{
    struct X{};
    X     named_x;
    X&    named_rx  = named_x;
    X&&   named_rrx = std::move(named_x);
    X   unnamed_x()   { return named_x; }
    X&  unnamed_rx()  { return named_x; } 
    X&& unnamed_rrx() { return std::move(named_x); } 

    std::uint32_t lvalue_count = 0;
    std::uint32_t rvalue_count = 0;
    template<typename T> void fct(T&)  { ++lvalue_count; }
    template<typename T> void fct(T&&) { ++rvalue_count; }
}

void test_bind_rvalue()
{
    using namespace toy_example;

    fct(  named_x);        assert(lvalue_count == 1 && rvalue_count == 0);
    fct(unnamed_x());      assert(lvalue_count == 1 && rvalue_count == 1);
    fct(  named_rx);       assert(lvalue_count == 2 && rvalue_count == 1);
    fct(unnamed_rx());     assert(lvalue_count == 3 && rvalue_count == 1);
    fct(  named_rrx);      assert(lvalue_count == 4 && rvalue_count == 1);
    fct(unnamed_rrx());    assert(lvalue_count == 4 && rvalue_count == 2); // <--- proved the above table is correct

    print_summary("rvalue - bind rvalue", "succeeded");
}


void test_compiler_generated_constructor()
{
    static_assert( std::is_default_constructible<alg::member_initialized_DC>::value);
    static_assert( std::   is_copy_constructible<alg::member_initialized_DC>::value);
    static_assert( std::   is_copy_assignable   <alg::member_initialized_DC>::value);
    static_assert( std::   is_move_assignable   <alg::member_initialized_DC>::value);
    static_assert( std::   is_move_assignable   <alg::member_initialized_DC>::value);
    static_assert( std::   is_destructible      <alg::member_initialized_DC>::value);

    static_assert(!std::is_default_constructible<alg::explicitly_defined_DC>::value);
    static_assert( std::   is_copy_constructible<alg::explicitly_defined_DC>::value);
    static_assert( std::   is_copy_assignable   <alg::explicitly_defined_DC>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_DC>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_DC>::value);
    static_assert( std::   is_destructible      <alg::explicitly_defined_DC>::value);

    static_assert(!std::is_default_constructible<alg::explicitly_defined_CC>::value);
    static_assert(!std::   is_copy_constructible<alg::explicitly_defined_CC>::value);
    static_assert( std::   is_copy_assignable   <alg::explicitly_defined_CC>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_CC>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_CC>::value);
    static_assert( std::   is_destructible      <alg::explicitly_defined_CC>::value);

    static_assert( std::is_default_constructible<alg::explicitly_defined_CA>::value);
    static_assert( std::   is_copy_constructible<alg::explicitly_defined_CA>::value);
    static_assert(!std::   is_copy_assignable   <alg::explicitly_defined_CA>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_CA>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_CA>::value);
    static_assert( std::   is_destructible      <alg::explicitly_defined_CA>::value);

    static_assert(!std::is_default_constructible<alg::explicitly_defined_MC>::value);
    static_assert(!std::   is_copy_constructible<alg::explicitly_defined_MC>::value);
    static_assert(!std::   is_copy_assignable   <alg::explicitly_defined_MC>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_MC>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_MC>::value);
    static_assert( std::   is_destructible      <alg::explicitly_defined_MC>::value);

    static_assert( std::is_default_constructible<alg::explicitly_defined_MA>::value);
    static_assert(!std::   is_copy_constructible<alg::explicitly_defined_MA>::value);
    static_assert(!std::   is_copy_assignable   <alg::explicitly_defined_MA>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_MA>::value);
    static_assert(!std::   is_move_assignable   <alg::explicitly_defined_MA>::value);
    static_assert( std::   is_destructible      <alg::explicitly_defined_MA>::value);

    static_assert(!std::is_default_constructible<alg::explicitly_defined_DD>::value);
    static_assert(!std::   is_copy_constructible<alg::explicitly_defined_DD>::value);
    static_assert( std::   is_copy_assignable   <alg::explicitly_defined_DD>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_DD>::value);
    static_assert( std::   is_move_assignable   <alg::explicitly_defined_DD>::value);
    static_assert(!std::   is_destructible      <alg::explicitly_defined_DD>::value);
    print_summary("rvalue - compiler generated constructor", "succeeded");
}


void test_universal_reference_deduction()
{
    alg::universal_reference_deduction();
    print_summary("rvalue - universal reference deduction", "succeeded");
}


void test_rvalue()
{
    test_return_rvalue();
    test_input_rvalue();
    test_bind_rvalue();
    test_compiler_generated_constructor();
    test_universal_reference_deduction();
}
