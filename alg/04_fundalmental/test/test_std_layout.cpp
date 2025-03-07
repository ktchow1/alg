#include<iostream>
#include<cstdint>
#include<cassert>
#include<type_traits>
#include<string>
#include<utility.h>


// ********************************************************************************************************* //
// In the following, "data member" means "non-static data member".
// ********************************************************************************************************* //
// Definition for standard_layout 
// * no virtual table : no virtual base, no virtual function
// * main requirement : all data members have same access control  
// * recursive        : all base-classes are standard_layout 
// * recursive        : all data members are standard_layout  
// * extra            : all base-classes have no non-static member (a simplified statement)
//
// Definition for trivial 
// * no virtual table : no virtual base, no virtual function
// * main requirement : no user-defined (non-trivial) constructor / destructor / operator= <--- member initializer is considered as non-trivial 
// * recursive        : all base-classes are standard_layout 
// * recursive        : all data members are standard_layout  
//
// Definition of POD 
// = standard_layout AND trivial
//
// Definition of aggregate
// * no virtual table : all base-classes are non virtual
// * main requirement : no user-defined (non-trivial) constructor (yet member initializer is allowed)
// * main requirement : no protected / private data members       
// * non recursive    : can have non-aggregate base class
// * non recursive    : can have non-aggregate data members
//
//
// Layout : 
// standard_layout = contigious memory layout, with zero padding, no member re-ordering
// trivial         = contigious memory layout, with zero padding and member re-ordering
// aggregate       = unpredictable layout
//
// Properties : 
// standard_layout =     allow memcpy & memcmp,     allow reinterpret_cast to consume
// trivial         =     allow memcpy & memcmp, NOT allow reinterpret_cast to consume (due to reordering)
// aggregate       = not allow memcpy * memcmp,     allow aggregate initialization
// ********************************************************************************************************* //
namespace test
{
    class A {};
    class B {};
    class C {};
    class D { std::uint32_t m_d; };

    // *********** //
    // *** POD *** //
    // *********** //
    class pod0 : public A,B,C 
    {
    public:
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class pod1 : public A,B,C 
    {
    private:
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    // *************************** //
    // *** Non standard_layout *** //
    // *************************** //
    class non_std_layout0 : public A,B,C 
    {
    public:
        std::uint32_t m_x;
                         
    private:           
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class non_std_layout1 : public A,B,C 
    {
    private:           
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
        non_std_layout0 m_w;
    };

    class non_std_layout2 : public A,B,C,D
    {
    private:           
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    // ******************* //
    // *** Non trivial *** //
    // ******************* //
    class non_trivial0 : public A,B,C 
    {
    public:
        explicit non_trivial0(std::uint32_t x) : m_x(x)
        {
        }

    private:
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class non_trivial1 : public A,B,C 
    {
    public:
        non_trivial1& operator=(const non_trivial1& rhs)
        {
            return *this;
        }

    private:
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class non_trivial2 : public A,B,C 
    {
    private:
        std::uint32_t m_x = 1;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class non_trivial3 : public A,B,C 
    {
    private:
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
        non_trivial0  m_w;
    };

    // *************************************** //
    // *** Non standard_layout non trivial *** //
    // *************************************** //
    class nothing0 : public A,B,C 
    {
    public:
        virtual void fct() {} 
                         
    private:           
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class nothing1 : public A,B,C 
    {
    private:           
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
        nothing0      m_w;
    };

    // ***************** //
    // *** Aggregate *** //
    // ***************** //
    struct aggA { std::uint32_t m_x; std::uint32_t m_y; };
    struct aggB { std::uint32_t m_x; std::uint32_t m_y; };
    struct aggC { std::uint32_t m_x; std::uint32_t m_y; };

    class aggregate0 : public aggA // make it non standard_layout
    {
    public:
        ~aggregate0(){} // make it non trivial

    public:
        std::uint32_t m_x; 
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    class aggregate1 : public aggA 
    {
    public:
        ~aggregate1(){} 

    public:
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };

    // ********************* //
    // *** Non aggregate *** //
    // ********************* //
    class non_aggregate0 : public aggA
    {
    public:
        non_aggregate0(){} // make it non aggregate
        ~non_aggregate0(){} 

    public:
        std::uint32_t m_x = 1;
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };

    class non_aggregate1 : public aggA
    {
    public:
        ~non_aggregate1(){} 

    private: // make it non aggregate
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };

    class non_aggregate2 : public aggA, aggB, aggC // make it non aggregate
    {
    public:
        ~non_aggregate2(){} 

    public:
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };

    // ************************* //
    // *** Aggregate - again *** //
    // ************************* //
    // aggregate having non aggregate base class
    // aggregate having non aggregate data member

    class aggregate2 : public non_aggregate0 
    {
    public:
        ~aggregate2(){} 

    public:
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };

    class aggregate3 : public aggA
    {
    public:
        ~aggregate3(){} 

    public:
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
        non_aggregate0 m_w; 
    };
    
    class aggregate4 : public aggA, public aggB, public aggC // multi inheritance
    {
    public:
        ~aggregate4(){} 

    public:
        std::uint32_t m_x = 1; 
        std::uint32_t m_y = 2;
        std::uint32_t m_z = 3;
    };
}

void test_std_layout()
{
    // *** pod *** //
    static_assert(std::is_standard_layout_v<test::pod0>            ==  true, "failed pod");
    static_assert(std::is_trivial_v<test::pod0>                    ==  true, "failed pod");

    static_assert(std::is_standard_layout_v<test::pod1>            ==  true, "failed pod");
    static_assert(std::is_trivial_v<test::pod1>                    ==  true, "failed pod");

    // *** non standard_layout *** //
    static_assert(std::is_standard_layout_v<test::non_std_layout0> == false, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_std_layout0>         ==  true, "failed standard_layout");

    static_assert(std::is_standard_layout_v<test::non_std_layout1> == false, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_std_layout1>         ==  true, "failed standard_layout");

    static_assert(std::is_standard_layout_v<test::non_std_layout2> == false, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_std_layout2>         ==  true, "failed standard_layout");

    // *** non trivial *** //
    static_assert(std::is_standard_layout_v<test::non_trivial0>    ==  true, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_trivial0>            == false, "failed standard_layout");

    static_assert(std::is_standard_layout_v<test::non_trivial1>    ==  true, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_trivial1>            == false, "failed standard_layout");

    static_assert(std::is_standard_layout_v<test::non_trivial2>    ==  true, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_trivial2>            == false, "failed standard_layout");

    static_assert(std::is_standard_layout_v<test::non_trivial3>    ==  true, "failed standard_layout");
    static_assert(std::is_trivial_v<test::non_trivial3>            == false, "failed standard_layout");

    // *** non standard_layout & non trivial *** //
    static_assert(std::is_standard_layout_v<test::nothing0>        == false, "failed non standard_layout non trivial");
    static_assert(std::is_trivial_v<test::nothing0>                == false, "failed non standard_layout non trivial");

    static_assert(std::is_standard_layout_v<test::nothing1>        == false, "failed non standard_layout non trivial");
    static_assert(std::is_trivial_v<test::nothing1>                == false, "failed non standard_layout non trivial");
 
    // *** aggregate *** //
    static_assert(std::is_standard_layout_v<test::aggregate0>      == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::aggregate0>              == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::aggregate0>            ==  true, "failed aggregate");

    static_assert(std::is_standard_layout_v<test::aggregate1>      == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::aggregate1>              == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::aggregate1>            ==  true, "failed aggregate");

    {
        test::aggregate0 agg0{11,12,21,22,23}; // initialize in order of base members and derived members
        assert(agg0.test::aggA::m_x == 11); 
        assert(agg0.test::aggA::m_y == 12); 
        assert(agg0.m_x == 21);
        assert(agg0.m_y == 22);
        assert(agg0.m_z == 23);
    }
    {
        test::aggregate1 agg1{11,12,21,22,23}; // this is ok
        assert(agg1.test::aggA::m_x == 11); 
        assert(agg1.test::aggA::m_y == 12); 
        assert(agg1.m_x == 21);
        assert(agg1.m_y == 22);
        assert(agg1.m_z == 23);
    }
    {
        test::aggregate1 agg1{{11,12},21,22,23}; // this is also ok
        assert(agg1.test::aggA::m_x == 11); 
        assert(agg1.test::aggA::m_y == 12); 
        assert(agg1.m_x == 21);
        assert(agg1.m_y == 22);
        assert(agg1.m_z == 23);
    }
    //  test::aggregate1 agg1{{11,12,21},22,23};   // cannot compile
    //  test::aggregate1 agg1{{11,12},{21,22,23}}; // cannot compile

    // *** non-aggregate *** //
    static_assert(std::is_standard_layout_v<test::non_aggregate0>  == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::non_aggregate0>          == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::non_aggregate0>        == false, "failed aggregate");

    static_assert(std::is_standard_layout_v<test::non_aggregate1>  == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::non_aggregate1>          == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::non_aggregate1>        == false, "failed aggregate");

    static_assert(std::is_standard_layout_v<test::non_aggregate2>  == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::non_aggregate2>          == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::non_aggregate2>        == false, "failed aggregate");

    // *** aggregate - again *** //
    static_assert(std::is_standard_layout_v<test::aggregate2>      == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::aggregate2>              == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::aggregate2>            ==  true, "failed aggregate");

    static_assert(std::is_standard_layout_v<test::aggregate3>      == false, "failed aggregate");
    static_assert(std::is_trivial_v<test::aggregate3>              == false, "failed aggregate");
    static_assert(std::is_aggregate_v<test::aggregate3>            ==  true, "failed aggregate");

    {
        test::aggregate2 agg2{{},21,22,23}; // non_aggregate0 supports default constructor
        assert(agg2.test::non_aggregate0::m_x == 1); 
        assert(agg2.test::non_aggregate0::m_y == 2); 
        assert(agg2.test::non_aggregate0::m_z == 3); 
        assert(agg2.m_x == 21);
        assert(agg2.m_y == 22);
        assert(agg2.m_z == 23);
    }
    //  test::aggregate2 agg2{21,22,23};            // cannot compile : missing non_aggregate0
    //  test::aggregate2 agg2{{11,12,13},21,22,23}; // cannot compile : non_aggregate0 does not support this constructor

    {
        test::aggregate3 agg3{{11,12},21,22,23,{}};
        assert(agg3.test::aggA::m_x == 11);
        assert(agg3.test::aggA::m_y == 12);
        assert(agg3.m_x == 21);
        assert(agg3.m_y == 22);
        assert(agg3.m_z == 23);
        assert(agg3.m_w.m_x == 1); 
        assert(agg3.m_w.m_y == 2); 
        assert(agg3.m_w.m_z == 3); 
    }
    {
        test::aggregate3 agg3{11,12,21,22,23};
        assert(agg3.test::aggA::m_x == 11);
        assert(agg3.test::aggA::m_y == 12);
        assert(agg3.m_x == 21);
        assert(agg3.m_y == 22);
        assert(agg3.m_z == 23);
        assert(agg3.m_w.m_x == 1); 
        assert(agg3.m_w.m_y == 2); 
        assert(agg3.m_w.m_z == 3); 
    }
    {
        test::aggregate3 agg3{{11},21,22,23,{}}; 
        assert(agg3.test::aggA::m_x == 11);
        assert(agg3.test::aggA::m_y == 0);
        assert(agg3.m_x == 21);
        assert(agg3.m_y == 22);
        assert(agg3.m_z == 23);
        assert(agg3.m_w.m_x == 1); 
        assert(agg3.m_w.m_y == 2); 
        assert(agg3.m_w.m_z == 3); 
    }
    //  test::aggregate3 agg3{{11,12},21,22,23,{11,12,13}}; // cannot compile
    {
        test::aggregate4 agg4{{11,12},{21,22},{31,32},41,42,43};
        assert(agg4.test::aggA::m_x == 11); 
        assert(agg4.test::aggA::m_y == 12); 
        assert(agg4.test::aggB::m_x == 21); 
        assert(agg4.test::aggB::m_y == 22); 
        assert(agg4.test::aggC::m_x == 31); 
        assert(agg4.test::aggC::m_y == 32); 
        assert(agg4.m_x == 41);
        assert(agg4.m_y == 42);
        assert(agg4.m_z == 43);
    }
    print_summary("standard layout", "succeeded");
}

