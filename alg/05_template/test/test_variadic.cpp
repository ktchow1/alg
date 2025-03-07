#include<iostream>
#include<sstream>
#include<cassert>
#include<string>
#include<traits.h>
#include<variadic.h>
#include<utility.h>


void test_parameter_pack_expansion()
{
    std::stringstream ss;
    alg::ostream_in_old_way(ss, "row = ", 1, 2, 3, 4, std::string{"ABC"}, std::string{"DEF"}, std::string{"GHIJ"}, std::string{"KLMN"});

    assert(ss.str() == "row = 1234ABCDEFGHIJKLMN");
    print_summary("variadic - parameter pack expansion", "succeeded");
}


void test_parameter_pack_nested_expansion()
{
    std::stringstream ss;
    alg::ostream_nested(ss, 1, 2, 3, 4, std::string{"ABC"}, std::string{"DEF"}, std::string{"GHIJ"}, std::string{"KLMN"});
    std::string ans = std::string{"row 0 = 1234ABCDEFGHIJKLMN + 1, "}    +
                      std::string{"row 1 = 1234ABCDEFGHIJKLMN + 2, "}    +
                      std::string{"row 2 = 1234ABCDEFGHIJKLMN + 3, "}    + 
                      std::string{"row 3 = 1234ABCDEFGHIJKLMN + 4, "}    + 
                      std::string{"row 4 = 1234ABCDEFGHIJKLMN + ABC, "}  +
                      std::string{"row 5 = 1234ABCDEFGHIJKLMN + DEF, "}  +
                      std::string{"row 6 = 1234ABCDEFGHIJKLMN + GHIJ, "} + 
                      std::string{"row 7 = 1234ABCDEFGHIJKLMN + KLMN, "};

    assert(ss.str() == ans);
    print_summary("variadic - parameter pack nested expansion", "succeeded");
}


void test_fold_expansion()
{
    std::stringstream ss0;
    alg::ostream_by_ostream_op(ss0, 1, 2, 3, 4, "ABC", "DEF", "GHIJ", "KLMN");

    std::stringstream ss1;
    alg::ostream_by_comma_op(ss1, 1, 2, 3, 4, "ABC", "DEF", "GHIJ", "KLMN");

    std::stringstream ss2;
    alg::ostream_by_comma_op2(ss2, 1, 2, 3, 4, "ABC", "DEF", "GHIJ", "KLMN");

    std::stringstream ss3;
    alg::ostream_by_comma_op_decorated(ss3, 1, 2, 3, 4, "ABC", "DEF", "GHIJ", "KLMN");

    std::stringstream ss4;
    alg::ostream_by_comma_op_decorated2(ss4, 1, 2, 3, 4, "ABC", "DEF", "GHIJ", "KLMN");

    std::stringstream ss5;
    (ss5 << 1, ss5 << 2, ss5 << 3, ss5 << 4, ss5 << "ABC", ss5 << "DEF", ss5 << "GHIJ", ss5 << "KLMN");

    assert(ss0.str() == "1234ABCDEFGHIJKLMN");
    assert(ss1.str() == "1234ABCDEFGHIJKLMN");
    assert(ss2.str() == "1234ABCDEFGHIJKLMN");
    assert(ss3.str() == "1, 2, 3, 4, ABC, DEF, GHIJ, KLMN.");
    assert(ss4.str() == "1, 2, 3, 4, ABC, DEF, GHIJ, KLMN.");
    assert(ss5.str() == "1234ABCDEFGHIJKLMN");
    print_summary("variadic - fold expansion", "succeeded");


    // ************************************************ //
    // *** Fold-expression-with-comma vs Param-pack *** //
    // ************************************************ //
    std::stringstream ss;
    alg::double_up_and_ostream(ss, 1, 2, 3, 4, std::string{"ABC"}, std::string{"DEF"}, std::string{"GHIJ"}, std::string{"KLMN"});
    std::string ans = std::string{"\nparameter-pack-expansion , 2, 4, 6, 8, ABCABC, DEFDEF, GHIJGHIJ, KLMNKLMN."} +
                      std::string{"\nfold-expression-of-comma0, 2."} +
                      std::string{"\nfold-expression-of-comma0, 4."} +
                      std::string{"\nfold-expression-of-comma0, 6."} +
                      std::string{"\nfold-expression-of-comma0, 8."} +
                      std::string{"\nfold-expression-of-comma0, ABCABC."} +
                      std::string{"\nfold-expression-of-comma0, DEFDEF."} +
                      std::string{"\nfold-expression-of-comma0, GHIJGHIJ."} +
                      std::string{"\nfold-expression-of-comma0, KLMNKLMN."} +
                      std::string{"\nfold-expression-of-comma1, KLMNKLMN."};

    assert(ss.str() == ans);
    print_summary("variadic - fold expansion with comma vs param pack", "succeeded");
}


// ********************** //
// *** Other variadic *** //
// ********************** //
template<int N> struct base 
{
    explicit base(int id) : m_id(id) 
    {
    }

    void fct()  
    { 
        std::stringstream ss;
        ss << "This is base" << N << " with id " << m_id;
        m_log = ss.str(); 
    }

    int m_id = 0;
    std::string m_log;
};

void test_other_variadic()
{
    static_assert(alg::multiple_inheritance<base<1>>                        ::num_of_base == 1, "failed to use multiple inheritance");
    static_assert(alg::multiple_inheritance<base<1>,base<2>>                ::num_of_base == 2, "failed to use multiple inheritance");
    static_assert(alg::multiple_inheritance<base<1>,base<2>,base<4>>        ::num_of_base == 3, "failed to use multiple inheritance");
    static_assert(alg::multiple_inheritance<base<1>,base<2>,base<4>,base<8>>::num_of_base == 4, "failed to use multiple inheritance");

    alg::multiple_inheritance<base<1>,base<2>,base<4>,base<8>> derived(12345);
    assert(derived.base<1>::m_id == 12345);
    assert(derived.base<2>::m_id == 12345);
//  assert(derived.base<3>::m_id == 12346); // compile error as expected, no this base class : base<3>
    assert(derived.base<4>::m_id == 12345);
    assert(derived.base<8>::m_id == 12345);

    assert(derived.base<1>::m_log == "");
    assert(derived.base<2>::m_log == "");
    assert(derived.base<4>::m_log == "");
    assert(derived.base<8>::m_log == "");
    derived.run();
    assert(derived.base<1>::m_log == "This is base1 with id 12345");
    assert(derived.base<2>::m_log == "This is base2 with id 12345");
    assert(derived.base<4>::m_log == "This is base4 with id 12345");
    assert(derived.base<8>::m_log == "This is base8 with id 12345");

    // pack indexing will be available in c++26
//  static_assert(alg::is_same_v<typename alg::pack_indexing<0,bool,char,int,std::string>::type,        bool>, "failed to use pack indexing"); 
//  static_assert(alg::is_same_v<typename alg::pack_indexing<1,bool,char,int,std::string>::type,        char>, "failed to use pack indexing"); 
//  static_assert(alg::is_same_v<typename alg::pack_indexing<2,bool,char,int,std::string>::type,         int>, "failed to use pack indexing"); 
//  static_assert(alg::is_same_v<typename alg::pack_indexing<3,bool,char,int,std::string>::type, std::string>, "failed to use pack indexing"); 
    print_summary("variadic - other expansion", "succeeded");
}


void test_variadic()
{
    test_parameter_pack_expansion();
    test_parameter_pack_nested_expansion();
    test_fold_expansion();
    test_other_variadic();
}
