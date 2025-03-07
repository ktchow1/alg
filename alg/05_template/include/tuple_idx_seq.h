#pragma once

// ******************************************************************************** //
// Conclusion
// 1. In template struct declaration :
// -  do not put parameter after parameter-pack
// -  do not put fomrula, like N-1, or sizeof...(Ns)
// 2. In template struct body : 
// -  we can put parameter after parameter-pack
// -  we can put formula, like N-1, or sizeof...(Ns)
// 3. As the pack expands, the tail remains unchanged, see the illustrative comment below.
// 4. Use std::size_t instead of std::uint32_t, as the latter is not compatiable with std::tuple_size
// 5. To get the type  of selected items in tuple, use variadic Ts... ans std::tuple<Ts...>
//    To get the value of selected items in tuple, use variatic Ns... and std::make_tuple
//
// [Summary]
//
//     idx_seq            |                        |   my_tuple
//     idx_seq_generator  |  tuple_size            |   my_tuple_size
// inv_idx_seq_generator  |  tuple_element         |   my_tuple_element
// off_idx_seq_generator  |                        |   get
//                        |                        |   make_my_tuple
// -----------------------+------------------------+--------------------
//                        |  shuffle               |   
// push_front/push_back   |  push_front/push_back  |
// pop_front(failed)      |  pop_front(failed)     |
// reverse                |  reverse               |
// odd_pick               |  odd_pick              |
// dedupe                 |  tuple_cat             |
// dot_product            |  tuple_apply           |
// ******************************************************************************** //
namespace alg 
{
    template<std::size_t ...Ns> struct idx_seq 
    {
    };
}


// ********************************* //
// *** Increasing index sequence *** // 
// ********************************* //
namespace alg 
{
    // 1. keep a counter, decrement it 
    // 2. build the sequence by either 
    //    starting from zero, with formula sizeof...(Ns)
    //    starting from N,    with formula N-1
    // 3. remove counter when it reaches zero, as specialization
    //
    // [Approach 1]
    // idx_seq_generator<4>::type = idx_seq_generator<3,0> ::type
    //                            = idx_seq_generator<2,0,1> ::type
    //                            = idx_seq_generator<1,0,1,2> ::type
    //                            = idx_seq_generator<0,0,1,2,3> ::type
    //                            = idx_seq<0,1,2,3>  ^ ^--+--^    
    //                                                |    |
    //                                                |    +--- output
    //                                                +-------- counter
    template<std::size_t N, std::size_t ...Ns> 
    struct idx_seq_generator
    {
        using type = typename idx_seq_generator<N-1,Ns...,sizeof...(Ns)>::type; 
    };  
    template<std::size_t ...Ns> 
    struct idx_seq_generator<0,Ns...>
    {
        using type = idx_seq<Ns...>; 
    };

    // [Approach 2]
    // idx_seq_generator2<4>::type = idx_seq_generator2<3,3> ::type
    //
    //                             = idx_seq_generator2<2,2,3> ::type
    //                             = idx_seq_generator2<1,1,2,3> ::type
    //                             = idx_seq_generator2<0,0,1,2,3> ::type
    //                             = idx_seq<0,1,2,3>   ^ ^--+--^    
    //                                                  |    |
    //                                                  |    +--- output
    //                                                  +-------- counter
      
    template<std::size_t N, std::size_t ...Ns> 
    struct idx_seq_generator2
    {
        using type = typename idx_seq_generator2<N-1,N-1,Ns...>::type;
    };

    template<std::size_t ...Ns> 
    struct idx_seq_generator2<0,Ns...>
    {
        using type = idx_seq<Ns...>;          
    };
}


// ******************************* //
// *** Inverted index sequence *** //
// ******************************* //
namespace alg 
{
    // [Approach 1]
    // inv_idx_seq_generator<4>::type = inv_idx_seq_generator<3,0> ::type
    //                                = inv_idx_seq_generator<2,1,0> ::type
    //                                = inv_idx_seq_generator<1,2,1,0> ::type
    //                                = inv_idx_seq_generator<0,3,2,1,0> ::type 
    //                                = idx_seq<3,2,1,0>

    template<std::size_t N, std::size_t ...Ns> 
    struct inv_idx_seq_generator
    {
        using type = typename inv_idx_seq_generator<N-1, sizeof...(Ns), Ns...>::type;
    };

    template<std::size_t ...Ns>
    struct inv_idx_seq_generator<0,Ns...>
    {
        using type = idx_seq<Ns...>;    
    }; 
    /*
    template<std::size_t N, std::size_t ...Ns>
    struct inv_idx_seq_generator_failed<N,N-1,Ns...> // <--- do not put formula here
    {
        using type = idx_seq<Ns...>; 
    };  */

    // [Approach 2]
    // inv_idx_seq_generator2<4>::type = inv_idx_seq_generator2<3,3> ::type
    //                                 = inv_idx_seq_generator2<2,3,2> ::type
    //                                 = inv_idx_seq_generator2<1,3,2,1> ::type
    //                                 = inv_idx_seq_generator2<0,3,2,1,0> ::type
    //                                 = idx_seq<3,2,1,0>  
      
    template<std::size_t N, std::size_t ...Ns> 
    struct inv_idx_seq_generator2
    {
        using type = typename inv_idx_seq_generator2<N-1, Ns..., N-1>::type;
    };

    template<std::size_t ...Ns>
    struct inv_idx_seq_generator2<0,Ns...>
    {
        using type = idx_seq<Ns...>; 
    }; 
}


// ***************************** //
// *** Offset index sequence *** //
// ***************************** //
//  create type idx_seq<N0,N0+1,...,N-1>, please ensure N0 <= N-1
//
namespace alg 
{
    // [Approach 1]
    template<std::size_t N0, std::size_t N1, std::size_t ...Ns> 
    struct off_idx_seq_generator                               // <--- This is interface & recursion.
    {
        using type = typename off_idx_seq_generator<N0,N1-1,Ns...,N0+sizeof...(Ns)>::type;
    };

    template<std::size_t N0, std::size_t ...Ns> 
    struct off_idx_seq_generator<N0,N0,Ns...>                  // <--- This is boundary case.
    {
        using type = idx_seq<Ns...>;          
    };

    // [Approach 2]
    template<std::size_t N0, std::size_t N1, std::size_t ...Ns> 
    struct off_idx_seq_generator2                              // <--- This is interface & recursion.
    {
        using type = typename off_idx_seq_generator2<N0,N1-1,N1-1,Ns...>::type;
    };

    template<std::size_t N0, std::size_t ...Ns> 
    struct off_idx_seq_generator2<N0,N0,Ns...>                 // <--- This is boundary case.
    {
        using type = idx_seq<Ns...>;          
    };
}


// ******************************************* //
// *** Push front & back to index sequence *** //
// ******************************************* //
namespace alg 
{
    template<typename T, std::size_t...Ns> 
    struct push_front_idx_seq                                  // <--- This is interface.
    {    
    };  
      
    template<std::size_t...TUP_Ns, std::size_t N, std::size_t...Ns> 
    struct push_front_idx_seq<idx_seq<TUP_Ns...>,N,Ns...>      // <--- This is recursion (with change in interface, support 2 parameter packs).
    {
        using type = typename push_front_idx_seq<idx_seq<N,TUP_Ns...>,Ns...>::type;
    };

    template<std::size_t...TUP_Ns> 
    struct push_front_idx_seq<idx_seq<TUP_Ns...>>              // <--- This is boundary case.
    {
        using type = idx_seq<TUP_Ns...>;
    };

    template<typename T, std::size_t...Ns> 
    struct push_back_idx_seq                                   // <--- This is interface.
    { 
    };
     
    template<std::size_t...TUP_Ns, std::size_t N, std::size_t...Ns> 
    struct push_back_idx_seq<idx_seq<TUP_Ns...>,N,Ns...>       // <--- This is recursion (with change in interface, support 2 parameter packs).
    {
        using type = typename push_back_idx_seq<idx_seq<TUP_Ns...,N>,Ns...>::type;
    };

    template<std::size_t...TUP_Ns> 
    struct push_back_idx_seq<idx_seq<TUP_Ns...>>               // <--- This is boundary case.
    {
        using type = idx_seq<TUP_Ns...>;
    };
}


// ********************************************* //
// *** Pop front & back from index sequence  *** //
// ********************************************* //
// Pop is implemented by 2 separate classes : 
// one for single pop
// one for multi pop
// I tried to implement in 1 class, in the same way as push
// unfortunately compilation error : ambiguous.
//
namespace alg 
{
    template<typename T> 
    struct pop_front_idx_seq
    {    
    };

    template<std::size_t TUP_N, std::size_t...TUP_Ns> 
    struct pop_front_idx_seq<idx_seq<TUP_N,TUP_Ns...>>  
    {
        using type = idx_seq<TUP_Ns...>;
    };

    template<typename T, std::size_t N> 
    struct pop_N_front_idx_seq
    {    
        using type = typename pop_N_front_idx_seq<typename pop_front_idx_seq<T>::type, N-1>::type;
    };

    template<typename T> 
    struct pop_N_front_idx_seq<T,0>
    {    
        using type = T;
    }; 
}


// ****************************** //
// *** Reverse index sequence *** //
// ****************************** //
// Reversing index sequence is different from constructing an inverted index sequence.
// idx_seq<> is used as boundary case in the below.
// idx_seq<N> can also be used as boundary case as well.
//
namespace alg 
{
    template<typename T> 
    struct reverse_idx_seq                                     // <--- This is interface.
    {
    };

    template<std::size_t N, std::size_t...Ns>
    struct reverse_idx_seq<idx_seq<N,Ns...>>                   // <--- This is recursion.
    {
        using type = typename push_back_idx_seq<typename reverse_idx_seq<idx_seq<Ns...>>::type, N>::type;
    };

    template<>
    struct reverse_idx_seq<idx_seq<>>                          // <--- This is boundary case.
    {
        using type = idx_seq<>; 
    };
}


// ******************************* //
// *** Odd pick index sequence *** //
// ******************************* //
namespace alg 
{
    template<typename T> 
    struct odd_pick_idx_seq                                    // <--- This is interface.
    {
    };

    template<std::size_t N0, std::size_t N1, std::size_t...Ns>
    struct odd_pick_idx_seq<idx_seq<N0,N1,Ns...>>              // <--- This is recursion.
    {
        using type = typename push_front_idx_seq<typename odd_pick_idx_seq<idx_seq<Ns...>>::type, N0>::type;
    };

    template<>
    struct odd_pick_idx_seq<idx_seq<>>                         // <--- This is boundary case for even size.
    {
        using type = idx_seq<>; 
    };

    template<std::size_t N0>
    struct odd_pick_idx_seq<idx_seq<N0>>                       // <--- This is boundary case for odd size.
    {
        using type = idx_seq<N0>;
    };
}


// ***************************** //
// *** Dedupe index sequence *** //
// ***************************** //
namespace alg 
{
    template<typename T>
    struct dedupe_idx_seq                                      // <--- This is interface.
    {
    };

    template<std::size_t N, std::size_t...Ns>
    struct dedupe_idx_seq<idx_seq<N,N,Ns...>>                  // <--- This is recursion.
    {
        using type = typename dedupe_idx_seq<idx_seq<N,Ns...>>::type;
    }; 

    template<std::size_t N0, std::size_t N1, std::size_t...Ns>
    struct dedupe_idx_seq<idx_seq<N0,N1,Ns...>>                // <--- This is recursion.
    {
        using type = typename push_front_idx_seq<typename dedupe_idx_seq<idx_seq<N1,Ns...>>::type, N0>::type;
    };

    template<>
    struct dedupe_idx_seq<idx_seq<>>                           // <--- This is boundary case for even size.
    {
        using type = idx_seq<>;
    };

    template<std::size_t N>
    struct dedupe_idx_seq<idx_seq<N>>                          // <--- This is boundary case for odd size.
    {
        using type = idx_seq<N>;
    };
}  


// *************************** //
// *** Dot product and zip *** //
// *************************** //
namespace alg 
{
    template<typename T0, typename T1>
    struct dot_product
    {
    };

    template<std::size_t...Ns0, std::size_t...Ns1>
    struct dot_product<idx_seq<Ns0...>, idx_seq<Ns1...>>
    {
        using type = idx_seq<Ns0*Ns1...>;
    };

    // Zip product is a generalized form of dot product. 
    // Zip product handles multiple index sequences.
    template<typename...ISs>                                   // <--- general interface for any number of idx_seq
    struct zip_product
    {
    };

    template<typename IS>                                      // <--- interface for 1 idx_seq
    struct zip_product<IS>           
    {
        using type = IS;
    };

    template<typename IS0, typename IS1, typename...ISs>       // <--- interface for 2 or more idx_seq
    struct zip_product<IS0,IS1,ISs...>     
    {
        using type = typename zip_product<typename dot_product<IS0,IS1>::type, ISs...>::type;
    };
}

// ********************************************************************************* //
// For index sequence, we can get reversed type :
//
// reverse_idx_seq<idx_seq<1,2,3,4,5>>::type == idx_seq<5,4,3,2,1>
//
// For tuple, since it has members, we can get reversed type AND reversed object :
// 
// reverse_tuple<std::tuple<A,B,C,D,E>>::type == std::tuple<E,D,C,B,A>    <--- reverse the type 
// reverse_tuple_object(t0) == t1                                         <--- reverse the type and the value 
//
// where t0 is an object of std::tuple<A,B,C,D,E>
//       t1 is an object of std::tuple<E,D,C,B,A>
//
// Remark : reverse of tuple object is easier than reverse of tuple type, because :
// 1. there is an input object (see tuple.h), we can operate with
// 2. there is a factory std::make_tuple<>() which can resolve T... 
// ********************************************************************************* //

