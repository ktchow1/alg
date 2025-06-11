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
// ******************** //
// *** Zero padding *** //
// ******************** //
// Zeros are padded in order to fulfill alignof(POD). The algorithm of zero padding is like : 
//
//
//
//
//
// ******************* //
// *** Size of POD *** //
// ******************* //
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

using A01     =   bi_class<std:: uint8_t, std::uint16_t>;
using A10     =   bi_class<std::uint16_t, std:: uint8_t>;

using B012    =  tri_class<std:: uint8_t, std::uint16_t, std::uint32_t>;
using B021    =  tri_class<std:: uint8_t, std::uint32_t, std::uint16_t>;
using B102    =  tri_class<std::uint16_t, std:: uint8_t, std::uint32_t>;
using B120    =  tri_class<std::uint16_t, std::uint32_t, std:: uint8_t>;
using B201    =  tri_class<std::uint32_t, std:: uint8_t, std::uint16_t>;
using B210    =  tri_class<std::uint32_t, std::uint16_t, std:: uint8_t>;

using C0123   = quad_class<std:: uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;
using C0132   = quad_class<std:: uint8_t, std::uint16_t, std::uint64_t, std::uint32_t>;
using C0213   = quad_class<std:: uint8_t, std::uint32_t, std::uint16_t, std::uint64_t>;
using C0231   = quad_class<std:: uint8_t, std::uint32_t, std::uint64_t, std::uint16_t>;
using C0312   = quad_class<std:: uint8_t, std::uint64_t, std::uint16_t, std::uint32_t>;
using C0321   = quad_class<std:: uint8_t, std::uint64_t, std::uint32_t, std::uint16_t>;

using C1023   = quad_class<std::uint16_t, std:: uint8_t, std::uint32_t, std::uint64_t>;
using C1032   = quad_class<std::uint16_t, std:: uint8_t, std::uint64_t, std::uint32_t>;
using C1203   = quad_class<std::uint16_t, std::uint32_t, std:: uint8_t, std::uint64_t>;
using C1230   = quad_class<std::uint16_t, std::uint32_t, std::uint64_t, std:: uint8_t>;
using C1302   = quad_class<std::uint16_t, std::uint64_t, std:: uint8_t, std::uint32_t>;
using C1320   = quad_class<std::uint16_t, std::uint64_t, std::uint32_t, std:: uint8_t>;

using C2013   = quad_class<std::uint32_t, std:: uint8_t, std::uint16_t, std::uint64_t>;
using C2031   = quad_class<std::uint32_t, std:: uint8_t, std::uint64_t, std::uint16_t>;
using C2103   = quad_class<std::uint32_t, std::uint16_t, std:: uint8_t, std::uint64_t>;
using C2130   = quad_class<std::uint32_t, std::uint16_t, std::uint64_t, std:: uint8_t>;
using C2301   = quad_class<std::uint32_t, std::uint64_t, std:: uint8_t, std::uint16_t>;
using C2310   = quad_class<std::uint32_t, std::uint64_t, std::uint16_t, std:: uint8_t>;

using C3012   = quad_class<std::uint64_t, std:: uint8_t, std::uint16_t, std::uint32_t>;
using C3021   = quad_class<std::uint64_t, std:: uint8_t, std::uint32_t, std::uint16_t>;
using C3102   = quad_class<std::uint64_t, std::uint16_t, std:: uint8_t, std::uint32_t>;
using C3120   = quad_class<std::uint64_t, std::uint16_t, std::uint32_t, std:: uint8_t>;
using C3201   = quad_class<std::uint64_t, std::uint32_t, std:: uint8_t, std::uint16_t>;
using C3210   = quad_class<std::uint64_t, std::uint32_t, std::uint16_t,  std::uint8_t>;

void test_memory_alignment()
{
    check_bi_class<A01>(2, 4, 0, 2);
    check_bi_class<A10>(2, 4, 0, 2);

    check_tri_class<B012>(4,  8, 0, 2, 4);
    check_tri_class<B021>(4, 12, 0, 4, 8);
    check_tri_class<B102>(4,  8, 0, 2, 4);
    check_tri_class<B120>(4, 12, 0, 4, 8);
    check_tri_class<B201>(4,  8, 0, 4, 6);
    check_tri_class<B210>(4,  8, 0, 4, 6);

    check_quad_class<C0123>(8, 16, 0, 2,  4,  8);
    check_quad_class<C0132>(8, 24, 0, 2,  8, 16);
    check_quad_class<C0213>(8, 24, 0, 4,  8, 16);
    check_quad_class<C0231>(8, 24, 0, 4,  8, 16);
    check_quad_class<C0312>(8, 24, 0, 8, 16, 20);
    check_quad_class<C0321>(8, 24, 0, 8, 16, 20);
                         
    check_quad_class<C1023>(8, 16, 0, 2,  4,  8);
    check_quad_class<C1032>(8, 24, 0, 2,  8, 16);
    check_quad_class<C1203>(8, 24, 0, 4,  8, 16);
    check_quad_class<C1230>(8, 24, 0, 4,  8, 16);
    check_quad_class<C1302>(8, 24, 0, 8, 16, 20);
    check_quad_class<C1320>(8, 24, 0, 8, 16, 20);
                         
    check_quad_class<C2013>(8, 16, 0, 4,  6,  8);
    check_quad_class<C2031>(8, 24, 0, 4,  8, 16);
    check_quad_class<C2103>(8, 16, 0, 4,  6,  8);
    check_quad_class<C2130>(8, 24, 0, 4,  8, 16);
    check_quad_class<C2301>(8, 24, 0, 8, 16, 18);
    check_quad_class<C2310>(8, 24, 0, 8, 16, 18);
                         
    check_quad_class<C3012>(8, 16, 0, 8, 10, 12);
    check_quad_class<C3021>(8, 24, 0, 8, 12, 16);
    check_quad_class<C3102>(8, 16, 0, 8, 10, 12);
    check_quad_class<C3120>(8, 24, 0, 8, 12, 16);
    check_quad_class<C3201>(8, 16, 0, 8, 12, 14);
    check_quad_class<C3210>(8, 16, 0, 8, 12, 14);

    print_summary("memory alignment for POD", "succeeded");
}
