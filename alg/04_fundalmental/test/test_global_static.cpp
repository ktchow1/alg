#include<cstdint>
#include<cassert>


// This is ok.
namespace alg 
{ 
    namespace global0
    {
        std::uint32_t global_var = 0;
    }

    namespace global1
    {
        std::uint32_t global_var = 0;
    }

    namespace global2
    {
        std::uint32_t global_var = 0;
    }

    void fct0() 
    {
        global0::global_var = 100;
        global1::global_var = 101;
        global2::global_var = 102;
    }

    void fct1() 
    {
        assert(global0::global_var == 100);
        assert(global1::global_var == 101);
        assert(global2::global_var == 102);
    }
}

// This is not ok.
void test_local_var_accessed_by_contained_function()
{
    std::uint32_t global_var = 0;
    
    struct visitor
    {
        void fct0()
        {
        //  global_var = 200; // ERROR : use of local variable with automatic storage from containing function
            alg::global0::global_var = 200;
        }

        void fct1()
        {
        //  assert(global_var == 200); // ERROR : use of local variable with automatic storage from containing function
            assert(alg::global0::global_var == 200);
        }
    };

    visitor v;
    v.fct0();
    v.fct1();
}
