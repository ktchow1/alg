#include<iostream>
#include<cstdint>
#include<cassert>
#include<memory>
#include<utility.h>


// **************************************** //
// Linking two inheritance trees of classes 
// **************************************** //
// Like below, we want :
// omsA is always binded to parserA
// omsB is always binded to parserB
// omsC is always binded to parserC
//
namespace test
{
    std::uint32_t parser_invoked = 0;
    std::uint32_t oms_invoked = 0;


    // ************************** //
    // *** inheritance tree 1 *** //
    // ************************** //
    struct parser
    {
        virtual void invoke() const = 0;
    };

    struct parserA : public parser 
    { 
        void invoke() const { parser_invoked = 1; } 
    };

    struct parserB : public parser 
    { 
        void invoke() const { parser_invoked = 2; } 
    };

    struct parserC : public parser 
    { 
        void invoke() const { parser_invoked = 3; } 
    };


    // ************************** //
    // *** inheritance tree 2 *** //
    // ************************** //
    struct oms 
    {
        virtual std::unique_ptr<parser> get_parser() const = 0;
        virtual void invoke() const = 0;

        void run()
        {
            get_parser()->invoke();
            invoke();
        } 
    };

    struct omsA : public oms 
    { 
        std::unique_ptr<parser> get_parser() const { return std::make_unique<parserA>(); } 
        void invoke() const { oms_invoked = 1; };
    };

    struct omsB : public oms 
    { 
        std::unique_ptr<parser> get_parser() const { return std::make_unique<parserB>(); } 
        void invoke() const { oms_invoked = 2; };
    };

    struct omsC : public oms 
    { 
        std::unique_ptr<parser> get_parser() const { return std::make_unique<parserC>(); } 
        void invoke() const { oms_invoked = 3; };
    };
}


void test_polymophism()
{
    std::unique_ptr<test::oms> pA(std::make_unique<test::omsA>());
    std::unique_ptr<test::oms> pB(std::make_unique<test::omsB>());
    std::unique_ptr<test::oms> pC(std::make_unique<test::omsC>());

    pA->run();
    assert(test::parser_invoked == 1);
    assert(test::oms_invoked == 1);

    pB->run();
    assert(test::parser_invoked == 2);
    assert(test::oms_invoked == 2);

    pC->run();
    assert(test::parser_invoked == 3);
    assert(test::oms_invoked == 3);

    print_summary("polymorphism", "succeeded");
}

