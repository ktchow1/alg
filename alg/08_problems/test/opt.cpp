#include<cassert>
#include<opt.h>
#include<utility.h>


// ********************************** //
// This is a test-driven development. //
// ********************************** //
void test_opt()
{
    for(std::uint32_t n=0; n!=55; ++n)
    {
        std::string str;
        std::string expected;
        
        // Suite 0 : Normal case (with single child or both children)
        if (n==0)   { str = "(A,B) (A,C) (B,D) (C,F)";                              expected = "(A(B(D))(C(F)))";       }
        if (n==1)   { str = "(A,B) (A,C) (B,D) (C,F) (C,G)";                        expected = "(A(B(D))(C(F)(G)))";    }
        if (n==2)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,G)";                  expected = "(A(B(D)(E))(C(F)(G)))"; }

        // Suite 1 : single error
        if (n==3)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C G)";                  expected = "E1"; } // E1 
        if (n==4)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (B,E)";                  expected = "E2"; } // E2
        if (n==5)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (B,X)";                  expected = "E3"; } // E3
        if (n==6)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (X,Y)";                  expected = "E4"; } // E4
        if (n==7)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,A)";                  expected = "E5"; } // E5a 
        if (n==8)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C)";                  expected = "E5"; } // E5b
        if (n==9)   { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,D)";                  expected = "E5"; } // E5c

        // Suite 2 : double errors
        if (n==10)  { str = "[A,B) (A,C) (B,D) (B,E) (C,F) (B,E)";                  expected = "E1"; } // E1,E2
        if (n==11)  { str = "(A B) (A,C) (B,D) (B,E) (C,F) (B,X)";                  expected = "E1"; } // E1,E3
        if (n==12)  { str = "(A,B] (A,C) (B,D) (B,E) (C,F) (X,Y)";                  expected = "E1"; } // E1,E4
        if (n==13)  { str = "(A,B),(A,C) (B,D) (B,E) (C,F) (D,A)";                  expected = "E1"; } // E1,E5a 
        if (n==14)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C) ";                 expected = "E1"; } // E1,E5b
        if (n==15)  { str = " (A,B) (A,C) (B,D) (B,E) (C,F) (C,D)";                 expected = "E1"; } // E1,E5c
                                                                                    // no E5d unless there is E4
        // when duplicated edge is not related to E3,E4,E5
        if (n==16)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (B,X) (A,C)";            expected = "E2"; } // E2,E3
        if (n==17)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (X,Y) (A,C)";            expected = "E2"; } // E2,E4
        if (n==18)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,A) (A,C)";            expected = "E2"; } // E2,E5a 
        if (n==19)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C) (A,C)";            expected = "E2"; } // E2,E5b
        if (n==20)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,D) (A,C)";            expected = "E2"; } // E2,E5c

        // when duplcated edge is ALSO the edge that causes E3,E4,E5 
        if (n==21)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (B,X) (B,X)";            expected = "E2"; } // E2,E3
        if (n==22)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (X,Y) (X,Y)";            expected = "E2"; } // E2,E4
        if (n==23)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,A) (D,A)";            expected = "E2"; } // E2,E5a 
        if (n==24)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C) (D,C)";            expected = "E2"; } // E2,E5b
        if (n==25)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,D) (C,D)";            expected = "E2"; } // E2,E5c
        
        if (n==26)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (X,Y) (B,I)";            expected = "E3"; } // E3,E4
        if (n==27)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,A) (B,I)";            expected = "E3"; } // E3,E5a 
        if (n==28)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C) (B,I)";            expected = "E3"; } // E3,E5b
        if (n==29)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,D) (B,I)";            expected = "E3"; } // E3,E5c

        if (n==30)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,A) (X,Y)";            expected = "E4"; } // E4,E5a [FAIL-1] 
        if (n==31)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (D,C) (X,Y)";            expected = "E4"; } // E4,E5b
        if (n==32)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (C,D) (X,Y)";            expected = "E4"; } // E4,E5c
        if (n==33)  { str = "(A,B) (A,C) (B,D) (B,E) (C,F) (X,Y) (Y,F)";            expected = "E4"; } // E4,E5d

        // Suite 3 : triple errors (not involve E1 as I know how its implemented)
        if (n==34)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (X,Y) (D,G) (D,G)";      expected = "E2"; } // E2,E3,E4 
        if (n==35)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (D,A) (D,A)";            expected = "E2"; } // E2,E3,E5a
        if (n==36)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (D,C) (D,C)";            expected = "E2"; } // E2,E3,E5b [FAIL-2]
        if (n==37)  { str = "(A,B) (A,C) (B,D) (D,E) (X,Y) (D,A) (D,A)";            expected = "E2"; } // E2,E4,E5a
        if (n==38)  { str = "(A,B) (A,C) (B,D) (D,E) (X,Y) (D,C) (D,C)";            expected = "E2"; } // E2,E4,E5b
        if (n==39)  { str = "(A,B) (A,C) (B,D) (D,E) (X,Y) (Y,C) (Y,C)";            expected = "E2"; } // E2,E4,E5d
        if (n==40)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (X,Y) (D,A)";            expected = "E3"; } // E3,E4,E5a
        if (n==41)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (X,Y) (D,C)";            expected = "E3"; } // E3,E4,E5b
        if (n==42)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (X,Y) (D,Y)";            expected = "E3"; } // E3,E4,E5d

        // Suite 4 : quad errors
        if (n==43)  { str = "(A,B) (A,C) (B,D) (D,E) (D,F) (X,Y) (D,Y) (D,Y)";      expected = "E2"; } // E2,E3,E4,E5d [FAIL-2]
        
        // Suite 5 : minimal node-set with E2,E3,E4,E5
        if (n==44)  { str = "(A,B)";                                                expected = "(A(B))"; }
        if (n==45)  { str = "(a,a)";                                                expected = "E1"; } // E1
        if (n==46)  { str = "(A,B) (A,B)";                                          expected = "E2"; } // E2
        if (n==47)  { str = "(A,B) (A,C) (A,D)";                                    expected = "E3"; } // E3
        if (n==48)  { str = "(A,B) (C,D)";                                          expected = "E4"; } // E4
        if (n==49)  { str = "(A,A)";                                                expected = "E5"; } // E5a
        if (n==50)  { str = "(A,B) (B,A)";                                          expected = "E5"; } // E5b
        if (n==51)  { str = "(A,B) (B,C) (A,C)";                                    expected = "E5"; } // E5c
        if (n==52)  { str = "(A,B) (C,B)";                                          expected = "E4"; } // E4,E5d
        if (n==53)  { str = "(";                                                    expected = "E1"; } // E1
        if (n==54)  { str = "";                                                     expected = "E5"; } // E5

        opt::tree_checker chk;
        chk.load(str);
        auto answer = chk.get_output();

        assert(answer == expected);

        // *********************************************** //
        // *** For debug, please turn on the following *** //
        // *********************************************** //
    //  chk.debug(); 
    }
    print_summary("opt : tree checker", "succeeded");
}

