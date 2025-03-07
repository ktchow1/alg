#include<iostream>
#include<cstdint>
#include<cassert>
#include<utility.h>

// ********************************************************************* //
// 1. define == and <=> (for homogenous input)
//    T ==  T that returns bool
//    T <=> T that returns ordering
//
// 2. define == and <=> (for heterogenous input)
//    T ==  X that returns bool
//    T <=> X that returns ordering
//
// 3. rewriting of secondary operator (for homogenous input)
//    T != T that returns !(T == T)
//    T >= T that returns (T <=> T) >= 0
//    T <= T that returns (T <=> T) <= 0
//    T >  T that returns (T <=> T) >  0
//    T <  T that returns (T <=> T) <  0
//
// 4. rewriting of secondary operator (for heterogenous input)
//    T != X that returns !(X == T)
//    T >= X that returns (X <=> T) >= 0
//    T <= X that returns (X <=> T) <= 0
//    T >  X that returns (X <=> T) >  0
//    T <  X that returns (X <=> T) <  0
//
// 5. reversing of primary operator (for heterogenous input)     
//    X ==  T that returns T == X
//    X !=  T that returns T != X
//    X <=> T that returns 0 <=> (T <=> X) 
//    X >=  T that returns T <= X
//    X <=  T that returns T >= X
//    X >   T that returns T <  X
//    X <   T that returns T >  X
//
// As a result 4 operators into step 1&2 becomes 21 operators in step 5.
// ********************************************************************* //
// Solve 3 problems : 
// 1. no need declare 21 operators
// 2. rewrite ensure 
//    <= and >  are complement 
//    >= and <  are complement
//    == and != are complement
// 3. elementwise comparison is faster
// ********************************************************************* //


namespace test
{
    std::uint32_t op_invoked = 0;

    struct sampleA
    {
        std::uint32_t m_x;
    };

    struct sampleB
    {
        std::uint32_t m_y;
    };

    struct sampleC
    {
        std::uint32_t m_z;
    };

    struct sample
    {
        inline bool operator==(const sample& rhs) const
        {
            op_invoked = 11;
            return m_x == rhs.m_x && 
                   m_y == rhs.m_y &&
                   m_z == rhs.m_z; 
        }

        inline bool operator==(const sampleA& rhs) const
        {
            op_invoked = 12;
            return m_x == rhs.m_x;
        }

        inline bool operator==(const sampleB& rhs) const
        {
            op_invoked = 13;
            return m_y == rhs.m_y;
        }

        inline bool operator==(const sampleC& rhs) const
        {
            op_invoked = 14;
            return m_z == rhs.m_z;
        }

        // ******************************************************* //
        // *** Typical element-wise spaceship operator for POD *** //
        // ******************************************************* //
        // 1. spaceship operator returns ordering, does NOT return bool
        // 2. ordering cannot be static_cast into bool / int
        // 3. ordering can    be converted   into bool by comparison with zero
        //
        inline auto operator<=>(const sample& rhs) const
        {
            op_invoked = 21;
            if (auto tmp = m_x <=> rhs.m_x; tmp != 0) return tmp;
            if (auto tmp = m_y <=> rhs.m_y; tmp != 0) return tmp;
            return         m_z <=> rhs.m_z;
        }

        inline auto operator<=>(const sampleA& rhs) const
        {
            op_invoked = 22;
            return m_x <=> rhs.m_x;
        }

        inline auto operator<=>(const sampleB& rhs) const
        {
            op_invoked = 23;
            return m_y <=> rhs.m_y;
        }
        
        inline auto operator<=>(const sampleC& rhs) const
        {
            op_invoked = 24;
            return m_z <=> rhs.m_z;
        }

        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
}


void test_spaceship()
{
    test::sample  s {15,25,35};
    test::sample  s0{15,25,34};
    test::sample  s1{15,25,35};
    test::sample  s2{15,25,36};
    test::sampleA a0{14};
    test::sampleA a1{15};
    test::sampleA a2{16};
    test::sampleB b0{24};
    test::sampleB b1{25};
    test::sampleB b2{26};
    test::sampleC c0{34};
    test::sampleC c1{35};
    test::sampleC c2{36};


    // reverse of primary (heterogenous ==), return bool
    assert(!(a0 == s) && test::op_invoked == 12);          test::op_invoked = 0;
    assert( (a1 == s) && test::op_invoked == 12);          test::op_invoked = 0;
    assert(!(a2 == s) && test::op_invoked == 12);          test::op_invoked = 0;
       
    assert(!(b0 == s) && test::op_invoked == 13);          test::op_invoked = 0;
    assert( (b1 == s) && test::op_invoked == 13);          test::op_invoked = 0;
    assert(!(b2 == s) && test::op_invoked == 13);          test::op_invoked = 0;

    assert(!(c0 == s) && test::op_invoked == 14);          test::op_invoked = 0;
    assert( (c1 == s) && test::op_invoked == 14);          test::op_invoked = 0;
    assert(!(c2 == s) && test::op_invoked == 14);          test::op_invoked = 0;

    // reverse of primary (heterogenous <=>), return ordering
    assert( (a0 <=> s) <  0 && test::op_invoked == 22);    test::op_invoked = 0;
    assert( (a1 <=> s) == 0 && test::op_invoked == 22);    test::op_invoked = 0;
    assert( (a2 <=> s) >  0 && test::op_invoked == 22);    test::op_invoked = 0;
       
    assert( (b0 <=> s) <  0 && test::op_invoked == 23);    test::op_invoked = 0;
    assert( (b1 <=> s) == 0 && test::op_invoked == 23);    test::op_invoked = 0;
    assert( (b2 <=> s) >  0 && test::op_invoked == 23);    test::op_invoked = 0;

    assert( (c0 <=> s) <  0 && test::op_invoked == 24);    test::op_invoked = 0;
    assert( (c1 <=> s) == 0 && test::op_invoked == 24);    test::op_invoked = 0;
    assert( (c2 <=> s) >  0 && test::op_invoked == 24);    test::op_invoked = 0;

    // rewrite of secondary (homogenous !=), return bool
    assert( (s0 != s) && test::op_invoked == 11);          test::op_invoked = 0;
    assert(!(s1 != s) && test::op_invoked == 11);          test::op_invoked = 0;
    assert( (s2 != s) && test::op_invoked == 11);          test::op_invoked = 0;

    // rewrite of secondary (heterogenous !=), return bool
    assert( (a0 != s) && test::op_invoked == 12);          test::op_invoked = 0;
    assert(!(a1 != s) && test::op_invoked == 12);          test::op_invoked = 0;
    assert( (a2 != s) && test::op_invoked == 12);          test::op_invoked = 0;
    
    assert( (b0 != s) && test::op_invoked == 13);          test::op_invoked = 0;
    assert(!(b1 != s) && test::op_invoked == 13);          test::op_invoked = 0;
    assert( (b2 != s) && test::op_invoked == 13);          test::op_invoked = 0;

    assert( (c0 != s) && test::op_invoked == 14);          test::op_invoked = 0;
    assert(!(c1 != s) && test::op_invoked == 14);          test::op_invoked = 0;
    assert( (c2 != s) && test::op_invoked == 14);          test::op_invoked = 0;

    // rewrite of secondary (homogenous <,<=,>,>=), return bool
    assert( (s0 <  s) && test::op_invoked == 21);          test::op_invoked = 0;
    assert( (s1 <= s) && test::op_invoked == 21);          test::op_invoked = 0;
    assert( (s2 >  s) && test::op_invoked == 21);          test::op_invoked = 0;

    // rewrite of secondary (heterogenous <,<=,>,>=), return bool
    assert( (a0 <  s) && test::op_invoked == 22);          test::op_invoked = 0;
    assert( (a1 <= s) && test::op_invoked == 22);          test::op_invoked = 0;
    assert( (a2 >  s) && test::op_invoked == 22);          test::op_invoked = 0;

    assert( (b0 <  s) && test::op_invoked == 23);          test::op_invoked = 0;
    assert( (b1 <= s) && test::op_invoked == 23);          test::op_invoked = 0;
    assert( (b2 >  s) && test::op_invoked == 23);          test::op_invoked = 0;

    assert( (c0 <  s) && test::op_invoked == 24);          test::op_invoked = 0;
    assert( (c1 <= s) && test::op_invoked == 24);          test::op_invoked = 0;
    assert( (c2 >  s) && test::op_invoked == 24);          test::op_invoked = 0;
    print_summary("spaceship operator", "succeeded");
}
