#include<iostream>
#include<cassert>
#include<utility.h>


// ***************** //
// *** Alignment *** //
// ***************** //
// Memory alignment of a POD is the requirement of memory address on which a POD is placed, it is :
// * cpu core dependent and
// * compiler dependent 
// 
// Given a POD, it is placed in memory address as multiple of alignof(POD), where
// * alignof(primitive) = sizeof(primitive) 
// * alignof(POD)      != sizeof(POD)                <--- in general 
// * alignof(POD)       = max(alignof(POD::member))
// 
// For example :
// * alignof(char)                              = 1
// * alignof(std::uint32_t)                     = 4
// * alignof(struct {char,char,char,char,char}) = 1
// * alignof(struct {char,char,std::uint32_t }) = 4
//
//
//
// ****************************************** //
// *** Address of member and zero padding *** //
// ****************************************** //
// Address of each member in POD is :
//
//    addr_of(mem[n]) = (min wrt k) max[ addr_of(mem[n-1]) + sizeof(mem[n-1]), k * alignof(mem[n]) ]
//
// then zeros are padded to the end of mem[n-1] in this range : 
//
//    addr_of(mem[n-1]) + sizeof(mem[n-1]) <---> addr_of(mem[n])
//
//
//
// ******************* //
// *** Size of POD *** //
// ******************* //
// Size of POD is :
//
//    sizeof(POD) = (min wrt k) max[ addr_of(mem[n]) + sizeof(mem[n]), k * alignof(POD) ]
//
//

template<typename T0, typename T1>                           struct   bi_class { T0 m0; T1 m1;               };
template<typename T0, typename T1, typename T2>              struct  tri_class { T0 m0; T1 m1; T2 m2;        };
template<typename T0, typename T1, typename T2, typename T3> struct quad_class { T0 m0; T1 m1; T2 m2; T3 m3; };

template<typename POD>
void check_bi_class(std::uint32_t align, std::uint32_t size, std::uint32_t m0, std::uint32_t m1)
{
    POD x;

    assert(alignof(POD) == align);
    assert( sizeof(POD) ==  size);
    assert((std::uint64_t)(&x.m0) - (std::uint64_t)(&x) == m0);
    assert((std::uint64_t)(&x.m1) - (std::uint64_t)(&x) == m1);
}

template<typename POD>
void check_tri_class(std::uint32_t align, std::uint32_t size, std::uint32_t m0, std::uint32_t m1, std::uint32_t m2)
{
    POD x;

    assert(alignof(POD) == align);
    assert( sizeof(POD) ==  size);
    assert((std::uint64_t)(&x.m0) - (std::uint64_t)(&x) == m0);
    assert((std::uint64_t)(&x.m1) - (std::uint64_t)(&x) == m1);
    assert((std::uint64_t)(&x.m2) - (std::uint64_t)(&x) == m2);
}

template<typename POD>
void check_quad_class(std::uint32_t align, std::uint32_t size, std::uint32_t m0, std::uint32_t m1, std::uint32_t m2, std::uint32_t m3)
{
    POD x;

    assert(alignof(POD) == align);
    assert( sizeof(POD) ==  size);
    assert((std::uint64_t)(&x.m0) - (std::uint64_t)(&x) == m0);
    assert((std::uint64_t)(&x.m1) - (std::uint64_t)(&x) == m1);
    assert((std::uint64_t)(&x.m2) - (std::uint64_t)(&x) == m2);
    assert((std::uint64_t)(&x.m3) - (std::uint64_t)(&x) == m3);
}

using A12     =   bi_class<std:: uint8_t, std::uint16_t>;
using A21     =   bi_class<std::uint16_t, std:: uint8_t>;

using B124    =  tri_class<std:: uint8_t, std::uint16_t, std::uint32_t>;
using B142    =  tri_class<std:: uint8_t, std::uint32_t, std::uint16_t>;
using B214    =  tri_class<std::uint16_t, std:: uint8_t, std::uint32_t>;
using B241    =  tri_class<std::uint16_t, std::uint32_t, std:: uint8_t>;
using B412    =  tri_class<std::uint32_t, std:: uint8_t, std::uint16_t>;
using B421    =  tri_class<std::uint32_t, std::uint16_t, std:: uint8_t>;

using C1248   = quad_class<std:: uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
using C1284   = quad_class<std:: uint8_t, std::uint16_t, std::uint64_t, std::uint32_t>;
using C1428   = quad_class<std:: uint8_t, std::uint32_t, std::uint16_t, std::uint64_t>;
using C1482   = quad_class<std:: uint8_t, std::uint32_t, std::uint64_t, std::uint16_t>;
using C1824   = quad_class<std:: uint8_t, std::uint64_t, std::uint16_t, std::uint32_t>;
using C1842   = quad_class<std:: uint8_t, std::uint64_t, std::uint32_t, std::uint16_t>;

using C2148   = quad_class<std::uint16_t, std:: uint8_t, std::uint32_t, std::uint64_t>;
using C2184   = quad_class<std::uint16_t, std:: uint8_t, std::uint64_t, std::uint32_t>;
using C2418   = quad_class<std::uint16_t, std::uint32_t, std:: uint8_t, std::uint64_t>;
using C2481   = quad_class<std::uint16_t, std::uint32_t, std::uint64_t, std:: uint8_t>;
using C2814   = quad_class<std::uint16_t, std::uint64_t, std:: uint8_t, std::uint32_t>;
using C2841   = quad_class<std::uint16_t, std::uint64_t, std::uint32_t, std:: uint8_t>;

using C4128   = quad_class<std::uint32_t, std:: uint8_t, std::uint16_t, std::uint64_t>;
using C4182   = quad_class<std::uint32_t, std:: uint8_t, std::uint64_t, std::uint16_t>;
using C4218   = quad_class<std::uint32_t, std::uint16_t, std:: uint8_t, std::uint64_t>;
using C4281   = quad_class<std::uint32_t, std::uint16_t, std::uint64_t, std:: uint8_t>;
using C4812   = quad_class<std::uint32_t, std::uint64_t, std:: uint8_t, std::uint16_t>;
using C4821   = quad_class<std::uint32_t, std::uint64_t, std::uint16_t, std:: uint8_t>;

using C8124   = quad_class<std::uint64_t, std:: uint8_t, std::uint16_t, std::uint32_t>;
using C8142   = quad_class<std::uint64_t, std:: uint8_t, std::uint32_t, std::uint16_t>;
using C8214   = quad_class<std::uint64_t, std::uint16_t, std:: uint8_t, std::uint32_t>;
using C8241   = quad_class<std::uint64_t, std::uint16_t, std::uint32_t, std:: uint8_t>;
using C8412   = quad_class<std::uint64_t, std::uint32_t, std:: uint8_t, std::uint16_t>;
using C8421   = quad_class<std::uint64_t, std::uint32_t, std::uint16_t,  std::uint8_t>;


using A11     =   bi_class<std::uint8_t, std::uint8_t>;
using B111    =  tri_class<std::uint8_t, std::uint8_t, std::uint8_t>;
using C1111   = quad_class<std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t>;

void test_memory_alignment()
{
    check_bi_class<A12>(2, 4, 0, 2);
    check_bi_class<A21>(2, 4, 0, 2);

    check_tri_class<B124>(4,  8, 0, 2, 4);
    check_tri_class<B142>(4, 12, 0, 4, 8);
    check_tri_class<B214>(4,  8, 0, 2, 4);
    check_tri_class<B241>(4, 12, 0, 4, 8);
    check_tri_class<B412>(4,  8, 0, 4, 6);
    check_tri_class<B421>(4,  8, 0, 4, 6);

    check_quad_class<C1248>(8, 16, 0, 2,  4,  8);
    check_quad_class<C1284>(8, 24, 0, 2,  8, 16);
    check_quad_class<C1428>(8, 24, 0, 4,  8, 16);
    check_quad_class<C1482>(8, 24, 0, 4,  8, 16);
    check_quad_class<C1824>(8, 24, 0, 8, 16, 20);
    check_quad_class<C1842>(8, 24, 0, 8, 16, 20);
                         
    check_quad_class<C2148>(8, 16, 0, 2,  4,  8);
    check_quad_class<C2184>(8, 24, 0, 2,  8, 16);
    check_quad_class<C2418>(8, 24, 0, 4,  8, 16);
    check_quad_class<C2481>(8, 24, 0, 4,  8, 16);
    check_quad_class<C2814>(8, 24, 0, 8, 16, 20);
    check_quad_class<C2841>(8, 24, 0, 8, 16, 20);
                         
    check_quad_class<C4128>(8, 16, 0, 4,  6,  8);
    check_quad_class<C4182>(8, 24, 0, 4,  8, 16);
    check_quad_class<C4218>(8, 16, 0, 4,  6,  8);
    check_quad_class<C4281>(8, 24, 0, 4,  8, 16);
    check_quad_class<C4812>(8, 24, 0, 8, 16, 18);
    check_quad_class<C4821>(8, 24, 0, 8, 16, 18);
                         
    check_quad_class<C8124>(8, 16, 0, 8, 10, 12);
    check_quad_class<C8142>(8, 24, 0, 8, 12, 16);
    check_quad_class<C8214>(8, 16, 0, 8, 10, 12);
    check_quad_class<C8241>(8, 24, 0, 8, 12, 16);
    check_quad_class<C8412>(8, 16, 0, 8, 12, 14);
    check_quad_class<C8421>(8, 16, 0, 8, 12, 14);

    check_bi_class<A11>(1, 2, 0, 1);
    check_tri_class<B111>(1, 3, 0, 1, 2);
    check_quad_class<C1111>(1, 4, 0, 1, 2, 3);

    print_summary("memory alignment for POD", "succeeded");
}
