#pragma once
#include<cstdint>
#include<cstring>
#include<new>      // for placement new  
#include<utility>  // for std::forward



namespace incorrect
{
    // ********************************** // 
    // *** Incorrect implementation 1 *** //
    // ********************************** // 
    // 1. force T to be default constructible
    // 2. each T is constructed twice :
    //    once by T{}
    //    once by T{args...}
    // 3. emplace does not involve copy / move, it is inplace construction
    //
    template<typename T, std::uint32_t N>
    class fixed_size_array
    {
    public:
        template<typename...ARGS> void emplace_back(ARGS&&... args)
        {
            m_impl[m_size] = T{std::forward<ARGS>(args)...};
            ++m_size;
        }

    private:
        std::uint32_t m_size;
        T m_impl[N];
    };

    // ********************************** // 
    // *** Incorrect implementation 2 *** //
    // ********************************** // 
    // same problems as 1
    //
    template<typename T, std::uint32_t N>
    class fixed_size_array2
    {
    public:
        template<typename...ARGS> void emplace_back(ARGS&&... args)
        {
            new (m_impl + m_size) T{std::forward<ARGS>(args)...};
            ++m_size;
        }

    private:
        std::uint32_t m_size;
        T m_impl[N];
    };
}


// ********************************** //
// *** Copyable and movable array *** //
// ********************************** //
namespace alg
{
    template<typename T>
    class array
    {
    public:
        explicit array(std::uint32_t capacity) : m_capacity(capacity), m_size(0), m_ptr(new char[capacity * sizeof(T)])
        {
            // unit of capacity is number of T
            // unit of size is also number of T 
        }

        array(const array<T>& rhs) : m_capacity(rhs.m_capacity), m_size(rhs.m_size), m_ptr(new char[m_capacity * sizeof(T)])
        {
            std::memcpy(m_ptr, rhs.m_ptr, m_capacity * sizeof(T));
        }

        array<T>& operator=(const array<T>& rhs) 
        {
            destruct_and_delete_ptr(); // remove this line to see if valgrind can detect leakage
            m_capacity = rhs.m_capacity;
            m_size = rhs.m_size;
            m_ptr = new char[m_capacity * sizeof(T)];
            std::memcpy(m_ptr, rhs.m_ptr, m_capacity * sizeof(T));
            return *this;
        }

        array(array<T>&& rhs) : m_capacity(rhs.m_capacity), m_size(rhs.m_size), m_ptr(rhs.m_ptr) 
        {
            rhs.m_capacity = 0;
            rhs.m_size = 0;
            rhs.m_ptr = nullptr;
        }

        array<T>& operator=(array<T>&& rhs) 
        {
            destruct_and_delete_ptr(); // remove this line to see if valgrind can detect leakage
            m_capacity = rhs.m_capacity;
            m_size = rhs.m_size; 
            m_ptr = rhs.m_ptr;
            rhs.m_capacity = 0;
            rhs.m_size = 0;
            rhs.m_ptr = nullptr;
            return *this;
        }

        ~array()
        {
            destruct_and_delete_ptr();
            m_capacity = 0;
            m_size = 0;
            m_ptr = nullptr;
        }

    public:
        // Techniques 
        // 1. variadic template
        // 2. universal reference forwarding
        // 3. placement new
        // 4. reinterpret_cast
        template<typename...Ts>
        void emplace(Ts&&...args) 
        {
            new (&reinterpret_cast<T*>(m_ptr)[m_size]) T(std::forward<Ts>(args)...); // custom constructor
            ++m_size;
        }

        // 5. remove reference is needed
        template<typename U>
        requires std::same_as<std::decay_t<T>,std::decay_t<U>> 
        void push_back(U&& u) 
        {
            new (&reinterpret_cast<T*>(m_ptr)[m_size]) T(std::forward<U>(u)); // copy or move constructor
            ++m_size;
        }

        const T& operator[](std::uint32_t n) const noexcept
        {
            return reinterpret_cast<const T*>(m_ptr)[n];
        }

        T& operator[](std::uint32_t n) noexcept
        {
            return reinterpret_cast<T*>(m_ptr)[n];
        }
        
        auto capacity() const noexcept
        {
            return m_capacity;
        }

        auto size() const noexcept
        {
            return m_size;
        }
        
        auto empty() const noexcept
        {
            return m_size == 0;
        }


    private:
        void destruct_and_delete_ptr() noexcept
        {
            if (m_ptr) 
            {
                for(std::uint32_t n=0; n!=m_size; ++n)
                {
                    reinterpret_cast<T*>(m_ptr)[n].~T();
                }
                delete [] m_ptr;
            }
        }

    private:
        std::uint32_t m_capacity;
        std::uint32_t m_size;
        char* m_ptr;
    };
}
