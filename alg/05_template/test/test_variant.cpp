#include<iostream>
#include<cassert>
#include<variant.h>
#include<utility.h>

std::uint32_t state = 0;

struct A{ std::uint8_t  m;  };
struct B{ std::uint16_t m;  };
struct C{ std::uint32_t m;  };
struct D{ std::uint64_t m;  };
struct E{ std::uint64_t m0; 
          std::uint64_t m1; };

struct X
{ 
    X()         { state = 0; }
   ~X()         { state = 1; }
    X(const X&) { state = 2; }
    X(X&&)      { state = 3; }
    X(std::uint32_t a, 
      std::uint32_t b, 
      std::uint32_t c)
    {
        state = 10;
    }
    std::uint32_t m[16];
};

struct Y
{ 
    Y()         { state = 4; }
   ~Y()         { state = 5; }
    Y(const Y&) { state = 6; }
    Y(Y&&)      { state = 7; }

    std::uint32_t m[16];
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
    static_assert(alg::type_index<X,A,B,C,D,E>::value == alg::variant<A,B,C,D,E>::monostate, "incorrect alg::type_index"); // absent type
    static_assert(alg::type_index<Y,A,B,C,D,E>::value == alg::variant<A,B,C,D,E>::monostate, "incorrect alg::type_index"); // absent type

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
    alg::runtime_dispatcher<A,B,X,C,D,Y,E> dispatcher;
    std::byte bytes0[std::max(sizeof(X), sizeof(Y))];
    std::byte bytes1[sizeof(X)];

    new (bytes0) X();
    std::size_t pos = 2;

    assert(state == 0); // construct
    dispatcher.copy(pos, bytes0, bytes1);
    assert(state == 2); // construct
    dispatcher.destroy(pos, bytes0);
    assert(state == 1); // destruct
    dispatcher.move(pos, bytes1, bytes0);
    assert(state == 3);
    dispatcher.destroy(pos, bytes0);
    assert(state == 1); // destruct

    new (bytes0) Y();
    pos = 5;

    assert(state == 4); // construct
    dispatcher.copy(pos, bytes0, bytes1);
    assert(state == 6); // construct
    dispatcher.destroy(pos, bytes0);
    assert(state == 5); // destruct
    dispatcher.move(pos, bytes1, bytes0);
    assert(state == 7);
    dispatcher.destroy(pos, bytes0);
    assert(state == 5); // destruct

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
    using V = alg::variant<A,B,C,X,Y>;
    X x;
    Y y;
    V vx{x};
    V vy{y};

    // Default construct and rebind with emplace
    {
        V v;
        assert((v.index() == V::monostate));
        assert(!v.is_type<A>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 101);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 102);
        
        v.emplace<X>(std::uint32_t{11}, std::uint32_t{12}, std::uint32_t{13});
        assert(state == 10); // rebine - dispatch to X's trinary constructor
        assert((v.index() == 3));
        assert(v.is_type<X>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 100);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 100);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 103);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 104);
    }

    // Constructed from T and rebind with T
    {
        V v(X{});
        assert((v.index() == 3));
        assert(v.is_type<X>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 100);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 100);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 103);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 104);

        v = y;
        assert(state == 6); // rebine - dispatch to Y's copy constructor
        assert((v.index() == 4));
        assert(v.is_type<Y>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 101);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 102);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 102);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 102);

        v = std::move(x);
        assert(state == 3); // rebine - dispatch to X's move constructor
        assert((v.index() == 3));
        assert(v.is_type<X>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 100);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 100);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 103);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 104);
    }

    // Constructed from variant<Ts...> and rebind with variant<Ts...>
    {
        V v(vx);
        assert((v.index() == 3));
        assert(v.is_type<X>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 100);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 100);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 103);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 104);

        v = vy;
        assert(state == 6); // rebine - dispatch to Y's copy constructor
        assert((v.index() == 4));
        assert(v.is_type<Y>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 101);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 102);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 102);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 102);

        v = std::move(vx);
        assert(state == 3); // rebine - dispatch to X's move constructor
        assert((v.index() == 3));
        assert(v.is_type<X>());

        state = 100;
        try { v.get<X>(); } catch(...) { state = 101; }
        assert(state = 100);
        try { v.get<3>(); } catch(...) { state = 102; }
        assert(state = 100);
        try { v.get<Y>(); } catch(...) { state = 103; }
        assert(state = 103);
        try { v.get<4>(); } catch(...) { state = 104; }
        assert(state = 104);
    }
    print_summary("variant - alg::variant", "succeeded");
}



void test_variant()
{
    test_variant_traits();
    test_variant_runtime_dispatcher();
    test_variant_full_test();
}
