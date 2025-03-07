#include<iostream>
#include<cassert>
#include<type_erasure.h>
#include<utility.h>


namespace type_erasure
{
    std::string invoked = "";

    class A
    {
    public: 
        A(std::uint32_t x, std::uint32_t y) : _x(x), _y(y) {}
        inline std::uint32_t function  (const std::string& str) const { invoked = "A::function";   return 0; }
        inline std::uint32_t operator()(const std::string& str) const { invoked = "A::operator()"; return 0; }

    private:
        std::uint32_t _x;
        std::uint32_t _y;
    };

    class B
    {
    public: 
        B(std::uint32_t x, std::uint32_t y, std::uint32_t z) : _x(x), _y(y), _z(z) {}
        inline std::uint32_t function  (const std::string& str) const { invoked = "B::function";   return 0; }
        inline std::uint32_t operator()(const std::string& str) const { invoked = "B::operator()"; return 0; }

    private:
        std::uint32_t _x;
        std::uint32_t _y;
        std::uint32_t _z;
    };

    class C
    {
    public: 
        C(std::string x, std::string y, std::string z) : _x(x), _y(y), _z(z) {}
        inline std::uint32_t function  (const std::string& str) const { invoked = "C::function";   return 0; }
        inline std::uint32_t operator()(const std::string& str) const { invoked = "C::operator()"; return 0; }

    private:
        std::string _x;
        std::string _y;
        std::string _z;
    };

    inline std::uint32_t f(const std::string& str)
    {
        invoked = "global f";
        return 0;
    }

    inline std::uint32_t g(const std::string& str)
    {
        invoked = "global g";
        return 0;
    }
}


void test_type_erasure()
{
    using namespace type_erasure;

    // ******************************** //
    // *** Test type erasure memfct *** //
    // ******************************** //
    alg::type_erase_memfct x0(A{1,2});
    alg::type_erase_memfct x1(B{1,2,3});
    alg::type_erase_memfct x2(C{"1","2","3"});
    x0.shared_interface("xxx");
    assert(invoked == "A::function");
    x1.shared_interface("xxx");
    assert(invoked == "B::function");
    x2.shared_interface("xxx");
    assert(invoked == "C::function");
    print_summary("type erasure - memfct", "succeeded");


    // ********************************* //
    // *** Test type erasure functor *** //
    // ********************************* //
    alg::type_erase_functor y0(A{1,2});
    alg::type_erase_functor y1(B{1,2,3});
    alg::type_erase_functor y2(C{"1","2","3"});
    alg::type_erase_functor y3(f);
    alg::type_erase_functor y4(g);
    alg::type_erase_functor y5(+[](const std::string& str) -> std::uint32_t
    {
        invoked = "lambda prefixed with +";
        return 0; 
    });
    y0.shared_interface("xxx");
    assert(invoked == "A::operator()");
    y1.shared_interface("xxx");
    assert(invoked == "B::operator()");
    y2.shared_interface("xxx");
    assert(invoked == "C::operator()");
    y3.shared_interface("xxx");
    assert(invoked == "global f");
    y4.shared_interface("xxx");
    assert(invoked == "global g");
    y5.shared_interface("xxx");
    assert(invoked == "lambda prefixed with +");
    print_summary("type erasure - functor", "succeeded");


    // ***************************************** //
    // *** Test type erasure vector<functor> *** //
    // ***************************************** //
    std::vector<alg::type_erase_functor> vec;

    // 1. rvalue type_erasure
    vec.push_back(alg::type_erase_functor(A{1,2}));
    vec.push_back(alg::type_erase_functor(B{1,2,3}));
    vec.push_back(alg::type_erase_functor(C{"1","2","3"}));
    vec.push_back(alg::type_erase_functor(f));
    vec.push_back(alg::type_erase_functor(g));
    vec.push_back(alg::type_erase_functor(+[](const std::string& str) -> std::uint32_t
    {
        invoked = "lambda prefixed with +";
        return 0; 
    }));
    
    // 2. lvalue type_erasure
    vec.push_back(y0);
    vec.push_back(y1);
    vec.push_back(y2);
    vec.push_back(y3);
    vec.push_back(y4);
    vec.push_back(y5);

    // 3. rvalue raw type
    vec.push_back(A{1,2});
    vec.push_back(B{1,2,3});
    vec.push_back(C{"1","2","3"});
    vec.push_back(f);
    vec.push_back(g);
    vec.push_back(+[](const std::string& str) -> std::uint32_t
    {
        invoked = "lambda prefixed with +";
        return 0; 
    });

    // 4. lvalue raw type (this part is the reason why std::decay_t is needed in type_erasure)
    auto z0 = A{1,2};
    auto z1 = B{1,2,3};
    auto z2 = C{"1","2","3"};
    auto z3 = &f;
    auto z4 = &g;
    auto z5 = +[](const std::string& str) -> std::uint32_t
    {
        invoked = "lambda prefixed with +";
        return 0; 
    };
    vec.push_back(z0);
    vec.push_back(z1);
    vec.push_back(z2);
    vec.push_back(z3);
    vec.push_back(z4);
    vec.push_back(z5);

    // execution
    std::uint32_t n = 0;
    for(const auto& x:vec)
    {
        x.shared_interface("xxx");
    
        if      (n%6 == 0) assert(invoked == "A::operator()");
        else if (n%6 == 1) assert(invoked == "B::operator()");
        else if (n%6 == 2) assert(invoked == "C::operator()");
        else if (n%6 == 3) assert(invoked == "global f");
        else if (n%6 == 4) assert(invoked == "global g");
        else if (n%6 == 5) assert(invoked == "lambda prefixed with +");
        ++n;
    }
    print_summary("type erasure - vector<functor>", "succeeded");
}

