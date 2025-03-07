#include<iostream>
#include<cstdint>
#include<cassert>
#include<rvalue.h>
#include<utility.h>

// *** containers under test *** //
#include<array>
#include<vector>
#include<list>
#include<map>
#include<unordered_map>



// ****************************************************************************************** //
// 1. vector/list do NOT require T to be default-constructible 
//    while array does   require T to be default-constructible
// 2. STL containers do NOT support T&.
// 3. when T is  std::reference, vector/list/array can    push by copy/move semantics
// 4. when T is purely copyable, vector/list/array cannot push by move semantics
// 5. when T is purely  movable, vector/list/array cannot push by copy semantics
// ****************************************************************************************** //
void test_std_container_vec_T_requirement()
{
    std::vector<alg::reference_member>      v0;
    std::vector<alg::purely_copyable>       v1;
    std::vector<alg::purely_movable>        v2;
    std::list  <alg::reference_member>      l0;
    std::list  <alg::purely_copyable>       l1;
    std::list  <alg::purely_movable>        l2;
//  std::array <alg::reference_member,10>   r0;  // compile error, T should be default-constructible
    std::array <alg::copyable,10>           r1;
    std::array <alg::movable,10>            r2;

    std::uint32_t         x{11};
    alg::reference_member a{x};
    alg::purely_copyable  b{22};
    alg::purely_movable   c{33};
    alg::copyable         B;
    alg::movable          C;


    v0.push_back(a);
    v0.push_back(alg::reference_member{x});
    v1.push_back(b);
//  v1.push_back(alg::purely_copyable{22});      // compile error, does not support move for non-movable T
//  v2.push_back(c);                             // compile error, does not support copy for non-copyable T
    v2.push_back(alg::purely_movable{33});

    l0.push_back(a);
    l0.push_back(alg::reference_member{x});
    l1.push_back(b);
//  l1.push_back(alg::purely_copyable{22});      // compile error, does not support move for non-movable T
//  l2.push_back(c);                             // compile error, does not support copy for non-copyable T
    l2.push_back(alg::purely_movable{33});

    r1[0] = B;
//  r1[0] = alg::copyable{};                     // compile error, does not support move for non-movable T
//  r2[0] = C;                                   // compile error, does not support copy for non-copyable T
    r2[0] = alg::movable{};
  
    print_summary("standard container - vec T requirement", "succeeded");
}


// ************************************************************************************************ //
// 1. map/unordered_map do NOT require T to be default-constructible
// 2. map/unordered_map do     require T to be copyable, but ok to be non-movable
// 3. for           map<T,value>, requires to provide auto T::operator<=>(const T&) const
// 4. for unordered_map<T,value>, requires to provide auto T::operator== (const T&) const and hash
//                                                                    ^
//                                                                    |
//        This is for resolving hash-collision. ----------------------+
// ************************************************************************************************ //
void test_std_container_map_T_requirement()
{
    std::map<alg::comparable,bool> m0;
    std::map<alg::copyable,  bool> m1;          // can declare, but cannot insert
    std::map<alg::movable,   bool> m2;          // can declare, but cannot insert

    m0.insert({alg::comparable{11},  true});                     
    m0.insert({alg::comparable{22}, false});                     
//  m1.insert({alg::comparable{11},  true});    // compile error, no T::operator<=>           
//  m1.insert({alg::comparable{22}, false});    // compile error, no T::operator<=>                               
//  m2.insert({alg::comparable{11},  true});    // compile error, no T::operator<=>           
//  m2.insert({alg::comparable{22}, false});    // compile error, no T::operator<=>           

    std::unordered_map<alg::hashable,bool, alg::hash> h0;
//  std::unordered_map<alg::copyable,bool> h1;  // cannot compile, no hash
//  std::unordered_map<alg::movable, bool> h2;  // cannot compile, no hash

    h0.insert({alg::hashable{"11"},  true});                     
    h0.insert({alg::hashable{"22"}, false});                     
    print_summary("standard container - map T requirement", "succeeded");
}


void test_std_container()
{
    test_std_container_vec_T_requirement();
    test_std_container_map_T_requirement();
}
