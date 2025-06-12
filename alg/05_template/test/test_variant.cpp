#include<iostream>
#include<cassert>
#include<variant.h>
#include<utility.h>


struct A{ std::uint8_t  m;  };
struct B{ std::uint16_t m;  };
struct C{ std::uint32_t m;  };
struct D{ std::uint64_t m;  };
struct E{ std::uint64_t m0; 
          std::uint64_t m1; };


// ******************************* //
// *** Testing of alg::variant *** //
// ******************************* //
void test_variant()
{
    static_assert(alg::find_max_size<A,A,A,B,A,A>::value ==  2, "incorrect alg::find_max_size");
    static_assert(alg::find_max_size<A,A,C,B,A,A>::value ==  4, "incorrect alg::find_max_size");
    static_assert(alg::find_max_size<A,D,C,B,A,A>::value ==  8, "incorrect alg::find_max_size");
    static_assert(alg::find_max_size<A,D,C,B,E,A>::value == 16, "incorrect alg::find_max_size");

    static_assert(alg::find_max_align<A,A,A,B,A,A>::value == 2, "incorrect alg::find_max_align");
    static_assert(alg::find_max_align<A,A,C,B,A,A>::value == 4, "incorrect alg::find_max_align");
    static_assert(alg::find_max_align<A,D,C,B,A,A>::value == 8, "incorrect alg::find_max_align");
    static_assert(alg::find_max_align<A,D,C,B,E,A>::value == 8, "incorrect alg::find_max_align");

    static_assert(alg::find_type_index<A,A,B,C,D,E>::value == 0, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<B,A,B,C,D,E>::value == 1, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<C,A,B,C,D,E>::value == 2, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<D,A,B,C,D,E>::value == 3, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<E,A,B,C,D,E>::value == 4, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<C,A,B,C,D,C>::value == 2, "incorrect alg::find_type_index");
    static_assert(alg::find_type_index<C,A,C,C,D,C>::value == 1, "incorrect alg::find_type_index");
    print_summary("variant - alg::variant (helper)", "succeeded");


    std::variant<A,B,C,D,E> v;


    print_summary("variant - alg::variant", "succeeded");
}
