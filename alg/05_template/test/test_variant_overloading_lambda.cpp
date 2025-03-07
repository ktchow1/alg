#include<iostream>
#include<cstdint>
#include<cassert>
#include<string>
#include<variant>
#include<utility.h>


// ************************** // 
// *** Overloading lambda *** //
// ************************** // 
namespace alg { namespace overloading_lambda
{           
    bool        b = true;
    char        c = 'a';
    int         i = 123;
    std::string s = "abc";
    std::uint32_t invoked = 0;
    

    // ********************* //
    // *** original idea *** //
    // ********************* //
    void step1()
    {
        struct vb { void operator()(const bool& x)        const { invoked = 1; } };
        struct vc { void operator()(const char& x)        const { invoked = 2; } };
        struct vi { void operator()(const  int& x)        const { invoked = 3; } };
        struct vs { void operator()(const std::string& x) const { invoked = 4; } };
        struct hardcoded_visitor : public vb,vc,vi,vs
        {
            using vb::operator(); // without this using, there will be ambiguous error, but why?
            using vc::operator();
            using vi::operator();
            using vs::operator();
        };
        
        hardcoded_visitor visitor;
        visitor(b);
        assert(invoked == 1);
        visitor(c);
        assert(invoked == 2);
        visitor(i);
        assert(invoked == 3);
        visitor(s);
        assert(invoked == 4);
    }
    

    // ********************************* //
    // *** using lambda and variadic *** //
    // ********************************* //
    template<typename...Bs> struct variadic_visitor : public Bs... // cannot declare template class inside function, hence place it here
    {
        using Bs::operator()...;
    };

    void step2()
    {
        auto vb = [](const bool& x)        { invoked = 5; };
        auto vc = [](const char& x)        { invoked = 6; };
        auto vi = [](const  int& x)        { invoked = 7; };
        auto vs = [](const std::string& x) { invoked = 8; };
        
        variadic_visitor<decltype(vb), decltype(vc), decltype(vi), decltype(vs)> visitor; // will remove decltype in step3
        visitor(b);
        assert(invoked == 5);
        visitor(c);
        assert(invoked == 6);
        visitor(i);
        assert(invoked == 7);
        visitor(s);
        assert(invoked == 8);
    }


    // ********************************************************** //
    // *** using deduction guide and aggregate initialization *** //
    // ********************************************************** //
    // Three properties are applied in variadic inheritance :
    // 1. variadic template in base class 
    //    variadic template in "using" 
    // 2. CTAD and deduction guide 
    // 3. aggregate initialization of multi base classes
    // 
    //
    // Overloading lambda is consisted the following 2 lines :
    //

    template<typename...Bs> struct overloading_lambda_visitor : public Bs...          // <--- line 1
    {
        using Bs::operator()...;
    };
    template<typename...Bs> overloading_lambda_visitor(Bs...)                         // <--- line 2
                         -> overloading_lambda_visitor<Bs...>;
                     //  ^                             ^--- we have not defined constructor of visitor, 
                     //  |                                  this visitor can still be constructed, due to 
                     //  deduction guide                    aggregate initialization of multi base classes

    void step3()
    {
        overloading_lambda_visitor visitor
        {
            [](const bool& x)        { invoked =  9; },
            [](const char& x)        { invoked = 10; },
            [](const  int& x)        { invoked = 11; },
            [](const std::string& x) { invoked = 12; }
        };
        visitor(b);
        assert(invoked == 9);
        visitor(c);
        assert(invoked == 10);
        visitor(i);
        assert(invoked == 11);
        visitor(s);
        assert(invoked == 12);

        std::variant<bool,char,int,std::string> v0 = b;
        std::variant<bool,char,int,std::string> v1 = c;
        std::variant<bool,char,int,std::string> v2 = i;
        std::variant<bool,char,int,std::string> v3 = s;
        std::visit(visitor, v0);
        assert(invoked == 9);
        std::visit(visitor, v1);
        assert(invoked == 10);
        std::visit(visitor, v2);
        assert(invoked == 11);
        std::visit(visitor, v3);
        assert(invoked == 12);
    }
}}


void test_variant_overloading_lambda()
{
    alg::overloading_lambda::step1();
    alg::overloading_lambda::step2();
    alg::overloading_lambda::step3();
    print_summary("variant - overloading lambda", "succeeded");
}
