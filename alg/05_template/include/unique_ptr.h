#pragma once
#include <functional>
#include <utility>

namespace alg 
{
    template <typename T, typename DELETER = std::function<void(T*)>> 
    class unique_ptr 
    {
    public:
        explicit unique_ptr(T* ptr = nullptr, const DELETER& deleter = [](T* ptr){ delete ptr; }) 
               : _ptr(ptr)
               , _deleter(deleter)
        {
        }

        ~unique_ptr() 
        {
            decrement();
        }


        // *** Copy semantics *** //
        unique_ptr(const unique_ptr<T,DELETER>&) = delete;
        unique_ptr<T,DELETER>& operator=(const unique_ptr<T,DELETER>&) = delete;


        // *** Move semantics *** //
        unique_ptr(unique_ptr<T,DELETER>&& rhs) noexcept 
        {
            _ptr         = rhs._ptr;
            _deleter     = std::move(rhs._deleter);
            rhs._ptr     = nullptr;
        //  rhs._deleter = unchanged
        }

        unique_ptr<T,DELETER>& operator=(unique_ptr<T,DELETER>&& rhs) noexcept 
        {
            if (this != &rhs) 
            {
                decrement();
                _ptr         = rhs._ptr;
                _deleter     = std::move(rhs._deleter);
                rhs._ptr     = nullptr; 
            //  rhs._deleter = unchanged
            }
            return *this;
        }


    public:
        operator bool()           const { return (_ptr!=nullptr); }
        const T& operator *()     const { return *_ptr; }
        const T* operator->()     const { return  _ptr; }
        T& operator *()                 { return *_ptr; }
        T* operator->()                 { return  _ptr; }


    public:
        void reset(T* ptr = nullptr) 
        {
            decrement();
            _ptr = ptr;
        }

        T* release() // This is the decrement function in shared_ptr.
        {
            T* old_ptr = _ptr;
            _ptr = nullptr; 
            return old_ptr;
        }

    private:
        void decrement()
        {
            if (_ptr) _deleter(_ptr); 
        }

    private:
        T* _ptr; 
        DELETER _deleter;
    };

    template<typename T, typename...ARGS>
    unique_ptr<T> make_unique(ARGS&&...args)
    {
        return unique_ptr<T>(new T (std::forward<ARGS>(args)...));
    }
}
