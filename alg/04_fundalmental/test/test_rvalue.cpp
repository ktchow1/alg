#include<iostream>
#include<cassert>
#include<rvalue.h>
#include<utility.h>


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
    test_compiler_generated_constructor();
    test_universal_reference_deduction();
}
