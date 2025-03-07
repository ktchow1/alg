#include<iostream>
#include<cstdint>
#include<cstring>
#include<cassert>
#include<string>
#include<unordered_map>
#include<utility.h>


// ************************************************************* //
// *** Compile time creation of std::map<std::string, value> *** //
// ************************************************************* //
namespace alg
{
    template<std::uint64_t N>
    constexpr std::uint64_t find_index(const std::array<const char*, N>& array, const char* key) // BUG1 : Don't declare constexpr for input arg, will result in compile error
    {
        for(std::uint64_t n=0; n!=N; ++n)
        {
            if (strcmp (key, array[n]) == 0)                   return n; // BUG2 : Don't declare constexpr for "if", will result in compile error
        //  if (strncmp(key, array[n], strlen(array[n])) == 0) return n; // Note : This works as well, but not necessary, as both lhs & rhs are null-terminate char*.
        }
        return N;
    }

    template<std::uint64_t N>
    constexpr const char* find_value(const std::array<std::pair<const char*, const char*>, N>& map, const char* key) 
    {
        for(std::uint64_t n=0; n!=N; ++n)
        {
            if (strcmp(key, map[n].first) == 0)                return map[n].second; 
        }
        return "";
    }

    template<std::uint64_t N>
    struct test_traits
    {
    };
}


void test_constexpr_map() 
{
    std::uint32_t function_invoked = 0;


    // *************************************** //
    // *** Part 1 - Using string in switch *** //
    // *************************************** //
    // No, switch statement supports const int only. Alternatively : 

    static const std::unordered_map<std::string, std::function<void()>> switch_by_str = 
    {
        {"method1", [&](){ function_invoked = 1; }},
        {"method2", [&](){ function_invoked = 2; }},
        {"method3", [&](){ function_invoked = 3; }}
    };

    switch_by_str.at("method1")();
    assert(function_invoked == 1);
    switch_by_str.at("method2")();
    assert(function_invoked == 2);
    switch_by_str.at("method3")();
    assert(function_invoked == 3);



    // ************************************************ //
    // *** Part 2 - Declaring constexpr std::string *** //
    // ************************************************ //
    // No, we cannot declare std::string in compile time, as it needs dynamic allocation in runtime. Alternatively :

    // method 1
//  constexpr       char* s  = "Hello world"; // compile error : cannot convert "xxx" into char*, we need const char*
    constexpr const char* s0 = "Hello world"; // compile ok
    constexpr const char* s1 = "Hello-world"; 
    constexpr const char* s2 = "Hello world"; 

    // method 2
    constexpr std::array<char,11> s3 = {'H','e','l','l','o',' ','w','o','r','l','d'};
    constexpr std::array<char,11> s4 = {'H','e','l','l','o','-','w','o','r','l','d'};
    constexpr std::array<char,11> s5 = {'H','e','l','l','o',' ','w','o','r','l','d'};

    // method x
//  constexpr std::string s("Hello world");   // compile error


    // To compare char*, use strcmp
    static_assert(s0 != s1,                       "failed to construct char* in compile time");
    static_assert(s0 == s2,                       "failed to construct char* in compile time"); // note : compiler reuses the same instance for same string
    static_assert(strlen(s0) == 11,               "failed to get size from char*");
    static_assert(strlen(s1) == 11,               "failed to get size from char*");
    static_assert(strlen(s2) == 11,               "failed to get size from char*");
    static_assert(strcmp(s0, "Hello world") == 0, "failed to compare char* in compile time");
    static_assert(strcmp(s1, "Hello world") != 0, "failed to compare char* in compile time");
    static_assert(strcmp(s2, "Hello world") == 0, "failed to compare char* in compile time"); 

    // To compare array, use ==
    static_assert(&s3 != &s4,             "failed to construct array in compile time");
    static_assert(&s3 != &s5,             "failed to construct array in compile time");
    static_assert(s3.data() != s4.data(), "failed to construct array in compile time");
    static_assert(s3.data() != s5.data(), "failed to construct array in compile time");
    static_assert(s3.size() == 11,        "failed to get size from array in compile time");
    static_assert(s4.size() == 11,        "failed to get size from array in compile time");
    static_assert(s5.size() == 11,        "failed to get size from array in compile time");
    static_assert(s3 != s4,               "failed to compare array in compile time");
    static_assert(s3 == s5,               "failed to compare array in compile time");

    // To compare between char* & array, use strncmp
    assert(strncmp(s3.data(), "Hello world", s3.size()) == 0);
    assert(strncmp(s4.data(), "Hello world", s4.size()) != 0);
    assert(strncmp(s5.data(), "Hello world", s5.size()) == 0);
    assert(strncmp(s3.data(), s0,            s3.size()) == 0);
//  static_assert(strncmp(s3.data(), "Hello world", s3.size()) == 0, "xxxx"); // note : this expression is not compile time constant (not sure why?)



    // ************************************************************* //
    // *** Part 3 - Declaring constexpr std::vector<std::string> *** //
    // ************************************************************* //
    // No, as both std::vector and std::string cannot be declared constexpr. Alternatively :

    constexpr std::array<const char*,5> array_str =
    {
        "a_1",
        "b_02",
        "c_003",
        "d_0004",
        "e_00005"
    };

    // Since both lhs & rhs of strcmp are null terminated, no need to use strncmp.
    static_assert(strcmp(array_str[0],       "a_1"    ) == 0, "failed to compare str in array");
    static_assert(strcmp(array_str[1],       "b_02"   ) == 0, "failed to compare str in array");
    static_assert(strcmp(array_str[2],       "c_003"  ) == 0, "failed to compare str in array");
    static_assert(strcmp(array_str[3],       "d_0004" ) == 0, "failed to compare str in array");
    static_assert(strcmp(array_str[4],       "e_00005") == 0, "failed to compare str in array");
    static_assert(strcmp(array_str[0],       "a_*"    ) != 0, "failed to compare str in array");
    static_assert(strcmp(array_str[1],       "b_*2"   ) != 0, "failed to compare str in array");
    static_assert(strcmp(array_str[2],       "c_*03"  ) != 0, "failed to compare str in array");
    static_assert(strcmp(array_str[3],       "d_*004" ) != 0, "failed to compare str in array");
    static_assert(strcmp(array_str[4],       "e_*0005") != 0, "failed to compare str in array");
    static_assert(alg::find_index(array_str, "a_1"    ) == 0, "failed to find str in array");
    static_assert(alg::find_index(array_str, "b_02"   ) == 1, "failed to find str in array");
    static_assert(alg::find_index(array_str, "c_003"  ) == 2, "failed to find str in array");
    static_assert(alg::find_index(array_str, "d_0004" ) == 3, "failed to find str in array");
    static_assert(alg::find_index(array_str, "e_00005") == 4, "failed to find str in array");
    static_assert(alg::find_index(array_str, "a_*"    ) == 5, "failed to find str in array");
    static_assert(alg::find_index(array_str, "b_*2"   ) == 5, "failed to find str in array");
    static_assert(alg::find_index(array_str, "c_*03"  ) == 5, "failed to find str in array");
    static_assert(alg::find_index(array_str, "d_*004" ) == 5, "failed to find str in array");
    static_assert(alg::find_index(array_str, "e_*0005") == 5, "failed to find str in array"); 

    // We can even initiate template class with constexpr function.
    alg::test_traits<alg::find_index(array_str, "a_1")> dummy;

    static_assert(std::is_same_v<alg::test_traits<alg::find_index(array_str, "a_1")>,     alg::test_traits<0>>, "failed to instantiate template with constexpr function");
    static_assert(std::is_same_v<alg::test_traits<alg::find_index(array_str, "b_02")>,    alg::test_traits<1>>, "failed to instantiate template with constexpr function");
    static_assert(std::is_same_v<alg::test_traits<alg::find_index(array_str, "c_003")>,   alg::test_traits<2>>, "failed to instantiate template with constexpr function");
    static_assert(std::is_same_v<alg::test_traits<alg::find_index(array_str, "d_0004")>,  alg::test_traits<3>>, "failed to instantiate template with constexpr function");
    static_assert(std::is_same_v<alg::test_traits<alg::find_index(array_str, "e_00005")>, alg::test_traits<4>>, "failed to instantiate template with constexpr function");



    // ************************************************************ //
    // *** Part 4 - Declaring constexpr std::map<std::string,V> *** //
    // ************************************************************ //
    // No, as both std::map and std::string cannot be declared constexpr. Alternatively :

    constexpr std::array<std::pair<const char*, const char*>,5> map_str =
    {
        // BUG3 : without specifying std::pair<...> below, there will be compile error "too many initializers" 
        std::pair<const char*, const char*>{"a_1",     "A1"},
        std::pair<const char*, const char*>{"b_02",    "B2"},
        std::pair<const char*, const char*>{"c_003",   "C3"},
        std::pair<const char*, const char*>{"d_0004",  "D4"},
        std::pair<const char*, const char*>{"e_00005", "E5"} 
    };

    static_assert(strcmp(map_str[0].first,        "a_1"    ) == 0, "failed to compare str in map");
    static_assert(strcmp(map_str[1].first,        "b_02"   ) == 0, "failed to compare str in map");
    static_assert(strcmp(map_str[2].first,        "c_003"  ) == 0, "failed to compare str in map");
    static_assert(strcmp(map_str[3].first,        "d_0004" ) == 0, "failed to compare str in map");
    static_assert(strcmp(map_str[4].first,        "e_00005") == 0, "failed to compare str in map");
    static_assert(strcmp(map_str[0].first,        "a_*"    ) != 0, "failed to compare str in map");
    static_assert(strcmp(map_str[1].first,        "b_*2"   ) != 0, "failed to compare str in map");
    static_assert(strcmp(map_str[2].first,        "c_*03"  ) != 0, "failed to compare str in map");
    static_assert(strcmp(map_str[3].first,        "d_*004" ) != 0, "failed to compare str in map");
    static_assert(strcmp(map_str[4].first,        "e_*0005") != 0, "failed to compare str in map");
    static_assert(strcmp(alg::find_value(map_str, "a_1"    ), "A1") == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "b_02"   ), "B2") == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "c_003"  ), "C3") == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "d_0004" ), "D4") == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "e_00005"), "E5") == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "a_*"    ), ""  ) == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "b_*2"   ), ""  ) == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "c_*03"  ), ""  ) == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "d_*004" ), ""  ) == 0, "failed to find str in map");
    static_assert(strcmp(alg::find_value(map_str, "e_*0005"), ""  ) == 0, "failed to find str in map");

    print_summary("constexpr std::map<std::string>", "succeeded");
}


