#include<iostream>
#include<cassert>
#include<functional>
#include<variant>
#include<tuple>
#include<map>
#include<string>
#include<sstream>
#include<utility.h>


std::string visitor_invoked;
// *************************************************************************************************** //
// Variant vs tuple
//
// std::variant<T0,T1,T2> =     sum type, i.e. feasible set = set of T0 + set of T1 + set of T2 + ...
// std::tuple<T0,T1,T2>   = product type, i.e. feasible set = set of T0 * set of T1 * set of T2 * ...
// 
// std::variant works with std::visit (and overloading lambda)
// std::tuple works with std::apply
// *************************************************************************************************** //
//
void test_variant_vs_tuple()
{
    bool        b = true;
    char        c = 'c';
    int         i = 12345;
    std::string s = "str";

    std::variant<bool,char,int,std::string> v0 = b;
    std::variant<bool,char,int,std::string> v1 = c;
    std::variant<bool,char,int,std::string> v2 = i;
    std::variant<bool,char,int,std::string> v3 = s;

    std::tuple<bool,char,int,std::string> t0{b,c,i,s};
    auto t1 = std::make_tuple(b,c,i,s);


    // ********************** //
    // *** variant access *** //
    // ********************** //
    auto variant_access_method1 = [](const auto& v, const std::string& name)
    {
        try
        {
            std::stringstream ss;
            if      (v.index()==0) ss << "variant " << name << " is bool, value = " << std::get<0>(v);
            else if (v.index()==1) ss << "variant " << name << " is char, value = " << std::get<1>(v);
            else if (v.index()==2) ss << "variant " << name << " is int,  value = " << std::get<2>(v);
            else if (v.index()==3) ss << "variant " << name << " is str,  value = " << std::get<3>(v);
            else                   ss << "variant " << name << " is unknown type";
            visitor_invoked = ss.str();
        }
        catch(const std::bad_variant_access& ex) { std::cout << "\nbad variant access for " << name; }
    };

    auto variant_access_method2 = [](const auto& v, const std::string& name)
    {
        try
        {
            std::stringstream ss;
            if      (std::holds_alternative<bool>(v))        ss << "variant " << name << " is bool, value = " << std::get<bool>(v);
            else if (std::holds_alternative<char>(v))        ss << "variant " << name << " is char, value = " << std::get<char>(v);
            else if (std::holds_alternative<int>(v))         ss << "variant " << name << " is int,  value = " << std::get<int>(v);
            else if (std::holds_alternative<std::string>(v)) ss << "variant " << name << " is str,  value = " << std::get<std::string>(v);
            else                                             ss << "variant " << name << " is unknown type";
            visitor_invoked = ss.str();
        }
        catch(const std::bad_variant_access& ex) { std::cout << "\nbad variant access for " << name; }
    };

    auto variant_access_method3 = [](const auto& v, const std::string& name)
    {
        try
        { 
            std::stringstream ss;
            //       Don't forget the address operator ---v
            if      (const auto& iter = std::get_if<bool>(&v);        iter) ss << "variant " << name << " is bool, value = " << *iter;
            else if (const auto& iter = std::get_if<char>(&v);        iter) ss << "variant " << name << " is char, value = " << *iter;
            else if (const auto& iter = std::get_if<int>(&v);         iter) ss << "variant " << name << " is int,  value = " << *iter;
            else if (const auto& iter = std::get_if<std::string>(&v); iter) ss << "variant " << name << " is str,  value = " << *iter;
            else                                                            ss << "variant " << name << " is unknown type";
            visitor_invoked = ss.str();
        }
        catch(const std::bad_variant_access& ex) { std::cout << "\nbad variant access for " << name; }
    };


    variant_access_method1(v0, "v0");   assert(visitor_invoked == "variant v0 is bool, value = 1");
    variant_access_method1(v1, "v1");   assert(visitor_invoked == "variant v1 is char, value = c");
    variant_access_method1(v2, "v2");   assert(visitor_invoked == "variant v2 is int,  value = 12345");
    variant_access_method1(v3, "v3");   assert(visitor_invoked == "variant v3 is str,  value = str");
    variant_access_method2(v0, "v0");   assert(visitor_invoked == "variant v0 is bool, value = 1");
    variant_access_method2(v1, "v1");   assert(visitor_invoked == "variant v1 is char, value = c");
    variant_access_method2(v2, "v2");   assert(visitor_invoked == "variant v2 is int,  value = 12345");
    variant_access_method2(v3, "v3");   assert(visitor_invoked == "variant v3 is str,  value = str");
    variant_access_method3(v0, "v0");   assert(visitor_invoked == "variant v0 is bool, value = 1");
    variant_access_method3(v1, "v1");   assert(visitor_invoked == "variant v1 is char, value = c");
    variant_access_method3(v2, "v2");   assert(visitor_invoked == "variant v2 is int,  value = 12345");
    variant_access_method3(v3, "v3");   assert(visitor_invoked == "variant v3 is str,  value = str");
    print_summary("variant - variant access", "succeeded");


    // ********************* //
    // *** variant visit *** //
    // ********************* //
    struct visitor
    {
        explicit visitor(const std::string& name) : m_name(name) {}
        void operator()(const bool& b)        { m_ss << "variant " << m_name << " is bool, value = " << b; visitor_invoked = m_ss.str(); } 
        void operator()(const char& c)        { m_ss << "variant " << m_name << " is char, value = " << c; visitor_invoked = m_ss.str(); } 
        void operator()(const int&  i)        { m_ss << "variant " << m_name << " is int,  value = " << i; visitor_invoked = m_ss.str(); } 
        void operator()(const std::string& s) { m_ss << "variant " << m_name << " is str,  value = " << s; visitor_invoked = m_ss.str(); } 

        std::string m_name;
        std::stringstream m_ss;
    };

    std::visit(visitor{"v0"}, v0);   assert(visitor_invoked == "variant v0 is bool, value = 1");
    std::visit(visitor{"v1"}, v1);   assert(visitor_invoked == "variant v1 is char, value = c");
    std::visit(visitor{"v2"}, v2);   assert(visitor_invoked == "variant v2 is int,  value = 12345");
    std::visit(visitor{"v3"}, v3);   assert(visitor_invoked == "variant v3 is str,  value = str");
    print_summary("variant - variant visit", "succeeded");
    

    // ******************** //
    // *** tuple access *** //
    // ******************** //
    auto tuple_access = [](const auto& t, const std::string& name)
    {
        std::stringstream ss;
        ss << "tuple " << name 
           << " has value " << std::get<0>(t)
                    << ", " << std::get<1>(t)
                    << ", " << std::get<2>(t)
                    << ", " << std::get<3>(t);
        visitor_invoked = ss.str();
    };

    tuple_access(t0, "t0");   assert(visitor_invoked == "tuple t0 has value 1, c, 12345, str");
    tuple_access(t1, "t1");   assert(visitor_invoked == "tuple t1 has value 1, c, 12345, str");
    print_summary("variant - tuple access", "succeeded");


    // ******************* //
    // *** tuple apply *** //
    // ******************* //
    auto function = [](const bool& b, 
                       const char& c, 
                       const int&  i,
                       const std::string& s,
                       const std::string& name)
    {
        std::stringstream ss;
        ss << "tuple " << name 
           << " has value " << b
                    << ", " << c
                    << ", " << i
                    << ", " << s;
        visitor_invoked = ss.str();
    };

    using namespace std::placeholders;
    std::apply(std::bind(function, _1, _2, _3, _4, "t0"), t0);
    assert(visitor_invoked == "tuple t0 has value 1, c, 12345, str");
    std::apply(std::bind(function, _1, _2, _3, _4, "t1"), t1);
    assert(visitor_invoked == "tuple t1 has value 1, c, 12345, str");
    print_summary("variant - tuple apply", "succeeded");
}


// ********************** //
// *** Variant vs POD *** //
// ********************** //
namespace test
{
    struct pod
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::string   m_z;
        std::string   m_w;
    };

    class pod_map
    {
    public:
        pod_map()
        {
            using namespace std::string_literals;
            m_impl.emplace("0001"s, pod {11, 12, "AAA"s, "aaa"s});
            m_impl.emplace("0002"s, pod {21, 22, "BBB"s, "bbb"s});
            m_impl.emplace("0003"s, pod {31, 32, "CCC"s, "ccc"s});
            m_impl.emplace("0004"s, pod {41, 42, "DDD"s, "ddd"s});
            m_impl.emplace("0005"s, pod {51, 52, "EEE"s, "eee"s});
            m_impl.emplace("0006"s, pod {61, 62, "FFF"s, "fff"s});
        }

        template<typename MEMPTR, typename VARIANT>
        void update(const std::string& key, MEMPTR mptr, const VARIANT& variant) noexcept
        {
            auto iter = m_impl.find(key);
            if (iter != m_impl.end())
            {
                using TEMP = std::remove_cvref_t<decltype(std::declval<pod>().*mptr)>;
                iter->second.*mptr = std::get<TEMP>(variant);
            }
        }

        pod& get(const std::string& key)
        {
            return m_impl[key];
        }

    private:
        std::map<std::string, pod> m_impl;
    };
}

void test_variant_vs_pod()
{
    test::pod_map m;

    std::variant<std::uint32_t, std::string> v;
    using namespace std::string_literals;

    v = 99U;
    m.update("0001"s, &test::pod::m_x, v);
    m.update("0002"s, &test::pod::m_y, v);
    v = "XXX"s;
    m.update("0003"s, &test::pod::m_z, v);
    m.update("0004"s, &test::pod::m_w, v);

    assert(m.get("0001"s).m_x == 99);
    assert(m.get("0001"s).m_y == 12);
    assert(m.get("0001"s).m_z == "AAA"s);
    assert(m.get("0001"s).m_w == "aaa"s);

    assert(m.get("0002"s).m_x == 21);
    assert(m.get("0002"s).m_y == 99);
    assert(m.get("0002"s).m_z == "BBB"s);
    assert(m.get("0002"s).m_w == "bbb"s);

    assert(m.get("0003"s).m_x == 31);
    assert(m.get("0003"s).m_y == 32);
    assert(m.get("0003"s).m_z == "XXX"s);
    assert(m.get("0003"s).m_w == "ccc"s);

    assert(m.get("0004"s).m_x == 41);
    assert(m.get("0004"s).m_y == 42);
    assert(m.get("0004"s).m_z == "DDD"s);
    assert(m.get("0004"s).m_w == "XXX"s);

    print_summary("variant - to POD", "succeeded");
}


void test_variant_basic()
{
    test_variant_vs_tuple();
    test_variant_vs_pod();
}
