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
    std::cout << "\nbi_class";
    std::cout << "\n" << alignof(A01)  << " " << sizeof(A01);
    std::cout << "\n" << alignof(A10)  << " " << sizeof(A10);

    std::cout << "\ntri_class";
    std::cout << "\n" << alignof(B012) << " " << sizeof(B012);
    std::cout << "\n" << alignof(B021) << " " << sizeof(B021);
    std::cout << "\n" << alignof(B102) << " " << sizeof(B102);
    std::cout << "\n" << alignof(B120) << " " << sizeof(B120);
    std::cout << "\n" << alignof(B201) << " " << sizeof(B201);
    std::cout << "\n" << alignof(B210) << " " << sizeof(B210);

    std::cout << "\nquad_class";
    std::cout << "\n" << alignof(C0123) << " " << sizeof(C0123);
    std::cout << "\n" << alignof(C0132) << " " << sizeof(C0132);
    std::cout << "\n" << alignof(C0213) << " " << sizeof(C0213);
    std::cout << "\n" << alignof(C0231) << " " << sizeof(C0231);
    std::cout << "\n" << alignof(C0312) << " " << sizeof(C0312);
    std::cout << "\n" << alignof(C0321) << " " << sizeof(C0321);
                         
    std::cout << "\nquad_class";
    std::cout << "\n" << alignof(C1023) << " " << sizeof(C1023);
    std::cout << "\n" << alignof(C1032) << " " << sizeof(C1032);
    std::cout << "\n" << alignof(C1203) << " " << sizeof(C1203);
    std::cout << "\n" << alignof(C1230) << " " << sizeof(C1230);
    std::cout << "\n" << alignof(C1302) << " " << sizeof(C1302);
    std::cout << "\n" << alignof(C1320) << " " << sizeof(C1320);
                         
    std::cout << "\nquad_class";
    std::cout << "\n" << alignof(C2013) << " " << sizeof(C2013);
    std::cout << "\n" << alignof(C2031) << " " << sizeof(C2031);
    std::cout << "\n" << alignof(C2103) << " " << sizeof(C2103);
    std::cout << "\n" << alignof(C2130) << " " << sizeof(C2130);
    std::cout << "\n" << alignof(C2301) << " " << sizeof(C2301);
    std::cout << "\n" << alignof(C2310) << " " << sizeof(C2310);
                         
    std::cout << "\nquad_class";
    std::cout << "\n" << alignof(C3012) << " " << sizeof(C3012);
    std::cout << "\n" << alignof(C3021) << " " << sizeof(C3021);
    std::cout << "\n" << alignof(C3102) << " " << sizeof(C3102);
    std::cout << "\n" << alignof(C3120) << " " << sizeof(C3120);
    std::cout << "\n" << alignof(C3201) << " " << sizeof(C3201);
    std::cout << "\n" << alignof(C3210) << " " << sizeof(C3210);

    print_summary("memory alignment for POD", "succeeded");
}
