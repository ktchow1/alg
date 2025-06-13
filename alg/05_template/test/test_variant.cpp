#include<iostream>
#include<cassert>
#include<variant.h>
#include<utility.h>

std::uint32_t state = 0;

struct A{ std::uint8_t  m;  };
struct B{ std::uint16_t m;  };
struct C{ std::uint32_t m;  };
struct D{ std::uint64_t m;  };
struct E
{ 
    E()         { state = 0; }
   ~E()         { state = 1; }
    E(const E&) { state = 2; }
    E(E&&)      { state = 3; }

    std::uint64_t m0; 
    std::uint64_t m1; 
};


// ******************************* //
// *** Testing of alg::variant *** //
// ******************************* //
void test_variant_traits()
{
    static_assert(alg::max_size<A,A,A,B,A,A>::value ==  2, "incorrect alg::max_size");
    static_assert(alg::max_size<A,A,C,B,A,A>::value ==  4, "incorrect alg::max_size");
    static_assert(alg::max_size<A,D,C,B,A,A>::value ==  8, "incorrect alg::max_size");
    static_assert(alg::max_size<A,D,C,B,E,A>::value == 16, "incorrect alg::max_size");

    static_assert(alg::max_align<A,A,A,B,A,A>::value == 2, "incorrect alg::max_align");
    static_assert(alg::max_align<A,A,C,B,A,A>::value == 4, "incorrect alg::max_align");
    static_assert(alg::max_align<A,D,C,B,A,A>::value == 8, "incorrect alg::max_align");
    static_assert(alg::max_align<A,D,C,B,E,A>::value == 8, "incorrect alg::max_align");

    static_assert(alg::type_index<A,A,B,C,D,E>::value == 0, "incorrect alg::type_index");
    static_assert(alg::type_index<B,A,B,C,D,E>::value == 1, "incorrect alg::type_index");
    static_assert(alg::type_index<C,A,B,C,D,E>::value == 2, "incorrect alg::type_index");
    static_assert(alg::type_index<D,A,B,C,D,E>::value == 3, "incorrect alg::type_index");
    static_assert(alg::type_index<E,A,B,C,D,E>::value == 4, "incorrect alg::type_index");
    static_assert(alg::type_index<C,A,B,C,D,C>::value == 2, "incorrect alg::type_index");
    static_assert(alg::type_index<C,A,C,C,D,C>::value == 1, "incorrect alg::type_index");
    static_assert(alg::type_index<C,A,A,A,A,A>::value == 5, "incorrect alg::type_index"); // absent type

    static_assert(std::is_same_v<alg::type_of<0,A,B,C,D>::type, A>, "incorrect alg::type_of");
    static_assert(std::is_same_v<alg::type_of<1,A,B,C,D>::type, B>, "incorrect alg::type_of");
    static_assert(std::is_same_v<alg::type_of<2,A,B,C,D>::type, C>, "incorrect alg::type_of");
    static_assert(std::is_same_v<alg::type_of<3,A,B,C,D>::type, D>, "incorrect alg::type_of");

    static_assert( alg::one_of<A,A,B,C,D>, "incorrect alg::one_of");
    static_assert( alg::one_of<B,A,B,C,D>, "incorrect alg::one_of");
    static_assert( alg::one_of<C,A,B,C,D>, "incorrect alg::one_of");
    static_assert( alg::one_of<D,A,B,C,D>, "incorrect alg::one_of");
    static_assert(!alg::one_of<A,B,C,D,E>, "incorrect alg::one_of");
    static_assert(!alg::one_of<B,A,C,D,E>, "incorrect alg::one_of");
    static_assert(!alg::one_of<C,A,B,D,E>, "incorrect alg::one_of");
    static_assert(!alg::one_of<D,A,B,C,E>, "incorrect alg::one_of");

    static_assert( alg::one_of<A,A,B,C,D,E>, "incorrect alg::one_of");
    static_assert( alg::one_of<B,A,B,C,D,E>, "incorrect alg::one_of");
    static_assert( alg::one_of<C,A,B,C,D,E>, "incorrect alg::one_of");
    static_assert( alg::one_of<D,A,B,C,D,E>, "incorrect alg::one_of");
    print_summary("variant - alg::variant (traits and concept)", "succeeded");
}



void test_variant_runtime_dispatcher()
{
    alg::runtime_dispatcher<A,B,C,D,E> dispatcher;
    std::byte bytes0[sizeof(E)];
    std::byte bytes1[sizeof(E)];

    new (bytes0) E();
    assert(state == 0);
    dispatcher.copy(4, bytes0, bytes1);
    assert(state == 2);
    dispatcher.destroy(4, bytes0);
    assert(state == 1);
    dispatcher.move(4, bytes1, bytes0);
    assert(state == 3);
    dispatcher.destroy(4, bytes0);
    assert(state == 1);

    try                      
    {  
        dispatcher.destroy(100, bytes0);     
    }
    catch(std::exception& e)
    {
        state = 4;        
    }
    assert(state == 4);
    print_summary("variant - alg::variant (runtime dispatcher)", "succeeded");
}



void test_variant_full_test()
{
    std::variant<A,B,C,D,E> v;
    print_summary("variant - alg::variant", "succeeded");
}



void test_variant()
{
    test_variant_traits();
    test_variant_runtime_dispatcher();
    test_variant_full_test();
}
