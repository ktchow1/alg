#include<iostream>
#include<cassert>
#include<cstdint>
#include<memory>
#include<vector>
#include<utility.h>


namespace alg
{
    std::vector<std::string> deleter_invoked;

    template<typename T> 
    struct deleter
    {
        void operator()(T* ptr)
        { 
            deleter_invoked.push_back("deleter");
            ptr->~T();
        }
    };


    // ************************************************************ //
    // *** Allocate memory for smart pointer using stack memory *** //
    // ************************************************************ //
    template<typename T, std::uint32_t N>
    class stack_pool
    {
        class deleter
        {
        public:
            deleter(stack_pool<T,N>& pool, std::uint32_t index) : m_pool(pool), m_index(index)
            {
            }

            void operator()(T* ptr)
            {
                std::stringstream ss;
                ss << "deleter for stack_pool, index " << m_index; 
                deleter_invoked.push_back(ss.str());

                if (m_index < N)
                {
                    m_pool.m_flags[m_index] = true;
                }
            }

            stack_pool<T,N>& m_pool;
            std::uint32_t m_index;
        };

    public:
        static const std::uint32_t size = N;
        friend class deleter;

        stack_pool()
        {
            for(std::uint32_t n=0; n!=N; ++n)
            {
                m_flags[n] = true;
            }
        }

        template<typename... ARGS> 
        auto make_unique(ARGS&&... args)
        {
            for(std::uint32_t n=0; n!=N; ++n)
            {
                if (m_flags[n])    
                {
                    m_flags[n] = false;

                    std::unique_ptr<T, deleter> ptr(nullptr, deleter{*this, n});
                    ptr.reset
                    (
                        new (&m_impl[n]) T{std::forward<ARGS>(args)...} // Todo : can we move placement new into constructor?
                    );   
                    return ptr;
                }
            }

            // index N will not trigger deletion
            return std::unique_ptr<T, deleter>{nullptr, deleter{*this, N}};
        }

        bool is_available(std::uint32_t n) const noexcept
        {
            return m_flags[n];
        }

    private:
        T    m_impl[N];
        bool m_flags[N]; // true for available
    }; 
}


// ************************************************** //
// *** Test deleter for unique_ptr and stack_pool *** //
// ************************************************** //
void test_deleter_for_unique_ptr()
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };


    alg::deleter_invoked.clear();
    {
        auto ptr = std::unique_ptr<A, alg::deleter<A>>(new A, alg::deleter<A>{});
    }
    
    assert(alg::deleter_invoked[0] == "deleter"); 
    print_summary("deleter for unique_ptr", "succeeded");
}


void test_deleter_for_stack_pool()
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    alg::deleter_invoked.clear();
    {
        alg::stack_pool<A,5> pool;
        auto a0 = pool.make_unique((std::uint32_t)11, (std::uint32_t)12, (std::uint32_t)13);
        auto a1 = pool.make_unique((std::uint32_t)21, (std::uint32_t)22, (std::uint32_t)23);
        {
            auto a2 = pool.make_unique((std::uint32_t)31, (std::uint32_t)32, (std::uint32_t)33);
            auto a3 = pool.make_unique((std::uint32_t)41, (std::uint32_t)42, (std::uint32_t)43);

            assert(!pool.is_available(0));
            assert(!pool.is_available(1));
            assert(!pool.is_available(2));
            assert(!pool.is_available(3));
            assert( pool.is_available(4));
        }

        auto a4(std::move(a1)); // no allocation
        assert(!pool.is_available(0));
        assert(!pool.is_available(1));
        assert( pool.is_available(2));
        assert( pool.is_available(3));
        assert( pool.is_available(4));

        auto a5 = pool.make_unique((std::uint32_t)51,(std::uint32_t)52,(std::uint32_t)53);
        auto a6 = pool.make_unique((std::uint32_t)61,(std::uint32_t)62,(std::uint32_t)63);
        auto a7 = pool.make_unique((std::uint32_t)71,(std::uint32_t)72,(std::uint32_t)73);
        auto a8 = pool.make_unique((std::uint32_t)81,(std::uint32_t)82,(std::uint32_t)83);

        assert(!pool.is_available(0));
        assert(!pool.is_available(1));
        assert(!pool.is_available(2));
        assert(!pool.is_available(3));
        assert(!pool.is_available(4));
        assert(a4 != nullptr);
        assert(a5 != nullptr);
        assert(a6 != nullptr);
        assert(a7 != nullptr);
        assert(a8 == nullptr);
    }

    std::uint32_t n = 0;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 3");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 2");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 4");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 3");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 2");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 1");     ++n;
    assert(alg::deleter_invoked[n] == "deleter for stack_pool, index 0");     ++n;
    print_summary("deleter for stack_pool", "succeeded");
}


void test_memory_deleter()
{
    test_deleter_for_unique_ptr();
    test_deleter_for_stack_pool();
}
