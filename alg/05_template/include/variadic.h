#pragma once

// Note the terms: 
// * parameter pack
// * parameter pack expansion
// * fold expression 
// * fold expression expansion
//
// comma operator : (a,b) means 
// * evaluate a, ignore value return from a
// * evaluate b, return value return from b


// *********************************** //
// *** 1. Parameter pack expansion *** //
// *********************************** //
namespace alg 
{
    template<typename OS, typename...ARGS> 
    OS& ostream_in_old_way(OS& os, ARGS&&...args)
    {
        return os;
    }

    template<typename OS, typename ARG, typename...ARGS> 
    OS& ostream_in_old_way(OS& os, ARG&& arg, ARGS&&...args)
    {
        os << arg;
        return ostream_in_old_way(os, std::forward<ARGS>(args)...);
    }
}


// ****************************************** //
// *** 2. Parameter pack nested expansion *** //
// ****************************************** //
// Please note :
// implementation of alg::make_tuple and 
// implementation of alg::tie 
// are NOT nested expansion.:wa
//
namespace alg 
{
    template<typename OS, typename...ARGS> 
    OS& ostream_nested(OS& os, ARGS&&...args)
    {
        std::uint32_t n = 0;
        (       // outer bracket for comma fold expression
            (   // inner bracket for comma fold expression
                ostream_in_old_way(os, "row ", n++, " = ", std::forward<ARGS>(args)...) 
                << " + " 
                << args 
                << ", "
            )
            ,...
        );
        return os;
    }
}


// ************************** //
// *** 3. Fold expression *** //
// ************************** //
// Fold expression == Reduce (fold) parameter pack
//
// ***************************************************************************************** //
// This is normal parameter-pack-expansion, it is NOT fold expansion : 
// 0. fct(x, y, args...)    = fct(x, y, arg0, arg1, arg2, arg3) 
//
// There are 4 forms of fold expansion :
// 1. (args OP ...)         =         arg0  OP (arg1  OP (arg2  OP  arg3))
// 2. (... OP args)         =       ((arg0  OP  arg1) OP  arg2) OP  arg3  
// 3. (args OP ... OP x)    =         arg0  OP (arg1  OP (arg2  OP (arg3 OP x))) 
// 4. (x OP ... OP args)    = (((x OP arg0) OP  arg1) OP  arg2) OP  arg3  
//
// Note
// * universal bracket () that brackets everything is needed
// * one extra bracket () that brackets args may also needed if args is a complex expression 
// ***************************************************************************************** //

namespace alg 
{
    // Case 4 with << as OP (cases 1-3 do NOT work for <<).
    template<typename OS, typename...ARGS> 
    OS& ostream_by_ostream_op(OS& os, ARGS&&...args)
    {
    //   os    ... << std::forward<ARGS>(args);  // compile error, bracket is a must
        (os << ... << std::forward<ARGS>(args)); 
        return os;
    }

    // Case 1 with comma as OP.
    template<typename OS, typename...ARGS>
    OS& ostream_by_comma_op(OS& os, ARGS&&...args)
    {
    //   (os << args), ... ; // compile error, outer bracket is a must
    //  ( os << args , ...); // compile error, inner bracket is a must
        ((os << args), ...); 
        return os;
    }

    // Case 2 with comma as OP.
    template<typename OS, typename...ARGS>
    OS& ostream_by_comma_op2(OS& os, ARGS&&...args)
    {
        (..., (os << args));
        return os;
    }

    // Case 1 with comma as OP.
    template<typename OS, typename...ARGS>
    OS& ostream_by_comma_op_decorated(OS& os, ARGS&&...args)
    {
        std::size_t n=0;
    //  ((os << args <<  ++n==sizeof...(ARGS)? "." : ", " ), ...); // compile error, bracket around ?: operator is a must
        ((os << args << (++n==sizeof...(ARGS)? "." : ", ")), ...);
        return os;
    }

    // Case 2 with comma as OP.
    template<typename OS, typename...ARGS>
    OS& ostream_by_comma_op_decorated2(OS& os, ARGS&&...args)
    {
        std::size_t n=0;
        (..., (os << args << (++n==sizeof...(ARGS)? "." : ", ")));
        return os;
    }

    // ******************************************************************************** //
    // *** Difference between parameter-pack-expansion & fold-expression-with-comma *** //
    // ******************************************************************************** //
    // These 2 scenarios look similar, lets compare them :
    // * case 0       with normal parameter-pack-expansion
    // * case 1 and 2 with fold-expression-of-comma
    //
    //   case 0       f(x, y, g(args)   ...)  = f(x, y, g(arg0), g(arg1), g(arg2), g(arg3));
    //   case 1      (f(x, y, g(args)), ...)  = f(x, y, g(arg0)), f(x, y, g(arg1)), f(x, y, g(arg2)), f(x, y, g(arg3));
    //   case 1       f(x, y,(g(args) , ...)) = g(arg3)
    //                                          g(arg2)
    //                                          g(arg1)
    //                                          g(arg0)
    //                                          f(x, y, g(arg3))
    // where f = ostream_by_comma_op
    // where g = double_up

    template<typename T>
    T double_up(const T& x)
    {
        return x + x;
    }

    template<typename OS, typename...ARGS>
    void double_up_and_ostream(OS& os, ARGS&&...args)
    {
        ostream_by_comma_op_decorated(os, "\nparameter-pack-expansion ", double_up(args)   ...);
       (ostream_by_comma_op_decorated(os, "\nfold-expression-of-comma0", double_up(args)), ...);
        ostream_by_comma_op_decorated(os, "\nfold-expression-of-comma1",(double_up(args),  ...));
    }
}


// **************** //
// *** 4. Other *** //
// **************** //
// variadic base expansion
// variadic using expansion
// variadic sizeof expansion
// pack indexing of types      (c++26) 
// pack indexing of parameters (c++26)

namespace alg
{
    template<typename...Bs>
    struct multiple_inheritance : public Bs...
    {
        using Bs::fct...; // assuming each base class has fct() 
        static constexpr int num_of_base = sizeof...(Bs); 

        explicit multiple_inheritance(int n) : Bs(n)... // assuming each base class can be constructed by int
        {
        }

        void run() noexcept
        {
            // fold expression with comma operator
            (..., (Bs::fct()));
        }
    };

    // pack indexing will be available in c++26
//  template<int N, typename...Ts>
//  struct pack_indexing
//  {
//      using type = Ts...[N];
//  };
}
