#pragma once
#include<atomic>  
#include<concepts>
#include<functional>
#include<cstdint>

namespace alg
{
    template<typename T>
    class default_deleter
    {
    public:
        void operator()(T* ptr) const
        {
            delete ptr;
        }
    };

    template<typename T>
    class default_deleter_with_ptr_type
    {
    public:
        using ptr_type = T*;

        void operator()(void* ptr) const
        {
            delete static_cast<ptr_type>(ptr);
        }
    };
}


// *************************************************** //
// *** With deleter as an extra template parameter *** //
// *************************************************** //
// Remarks about shared_ptr in general :
//
// 1. _ptr should be accessed by single dereference, NOT double or more dereference
// 2. _mgr is nullptr when _ptr is
//    _mgr is no null when _ptr is not
// 3. put deleter in manager, all shared_ptr pointing to same resouce re-use same deleter
//    provide default type for DELETER
//    provide default value for deleter
// 4. race between increment and decrement is solved by atomic count
//    race between assigning and accessing _ptr is not thread-safe
//    concurrent-read is fine, concurrent-write or concurrent-read-write is not ok
// 5. for copy & move assignment,     add this != rhs checking 
//    for copy & move constructor,    move some initialization to member initializer 
//    for move semantics,             add noexcept 
// 6. deleter is passed in different ways in constructor :
//    alg::simple_shared_ptr<> as const DELETER&
//    alg::       shared_ptr<> as universal reference of DELETER, both are ok ...

namespace alg
{
    template<typename T, typename DELETER = std::function<void(T*)>> 
    class simple_shared_ptr
    {	
        struct manager 
        {
            std::atomic<std::uint32_t> _count;
            DELETER _deleter;
        };

    public: 
        explicit simple_shared_ptr(T* ptr = nullptr, const DELETER& deleter = default_deleter<T>()) 
               : _ptr(ptr)
               , _mgr(nullptr)
        {
            if (_ptr)
            {
                _mgr = new manager(1, deleter);
            }
        }												
                                            
        ~simple_shared_ptr()
        {
            decrement();
        }
                                        
        // *** Copy semantics *** //
        simple_shared_ptr(const simple_shared_ptr<T,DELETER>& rhs) // BUG : Don't miss const here
        {
            _ptr = rhs._ptr;
            _mgr = rhs._mgr;
            increment();
        }

        simple_shared_ptr<T,DELETER>& operator=(const simple_shared_ptr<T,DELETER>& rhs)
        {
            if (this != &rhs)
            {
                decrement();
                _mgr = rhs._mgr;
                _ptr = rhs._ptr;
                increment();
            }
            return *this;
        }

        // *** Move semantics *** //
        simple_shared_ptr(simple_shared_ptr<T,DELETER>&& rhs) noexcept
        {
            _ptr = rhs._ptr;
            _mgr = rhs._mgr;
            rhs._ptr = nullptr;
            rhs._mgr = nullptr;
        }	

        simple_shared_ptr<T,DELETER>& operator=(simple_shared_ptr<T,DELETER>&& rhs) noexcept
        {
            if (this != &rhs)
            {
                decrement();
                _ptr = rhs._ptr;
                _mgr = rhs._mgr;
                rhs._ptr = nullptr;
                rhs._mgr = nullptr;
            }
            return *this;
        }

    public:
        bool unique()             const { return  _mgr && (_mgr->_count == 1);       } 
        std::uint32_t ref_count() const { return (_mgr ?   _mgr->_count.load() : 0); }
        operator bool()           const { return (_ptr!=nullptr); }
        const T& operator *()     const { return *_ptr;  }
        const T* operator->()     const { return  _ptr;  }
        T& operator *()                 { return *_ptr;  }
        T* operator->()                 { return  _ptr;  }

    private:    
        void increment()
        {
            if (_mgr) ++(_mgr->_count); 
        }

        void decrement()
        {	
            if (_mgr && --(_mgr->_count)==0)
            {
                _mgr->_deleter(_ptr);	
                delete _mgr;	
                _ptr = nullptr;
                _mgr = nullptr;
            }
        }

    private:    
        T* _ptr; 
        manager* _mgr;
    };
     
    template<typename T, typename...ARGS>
    simple_shared_ptr<T> make_simple_shared(ARGS&&...args)
    {
        return simple_shared_ptr<T>(new T(std::forward<ARGS>(args)...));
    }
}


// ************************************ //
// *** With deleter as type erasure *** //
// ************************************ //
// Objective :
// * remove DELETER type from alg::shared_ptr::manager
// * so we have alg::shared_ptr<T> 
//   instead of alg::shared_ptr<T,DELETER>
//
// 1. T and U are resource type, where type erasure is irrelevant
//    DELETER is  deleter  type, where type erasure applie
//
// 2. as alg::shared_ptr<T> is not a template of DELETER (type-erased), 
//    so alg::shared_ptr<T>::manager cannot be a template of DELETER as well
//    but both constructors should be template with DELETER is parameter
//
// 3. unlike 
//    alg::function,           which contains std::shared_ptr<callable_concept>
//    alg::shared_ptr::manager cannot contain std::shared_ptr<deleter_concept>, as it introduces recursive dependency, hence ...
//    alg::shared_ptr::manager manages deleter_concept* by itself
//
// 4. deleter_concept* is initialized in :
//    constructor of alg::shared_ptr<T>, then in 
//    constructor of alg::shared_ptr<T>::manager, which knows the type U
//    by picking U as the type for deleter, Wintermute's problem is thus solved
//
// 5. why need to introduce U to deleter_model? 
// - U is necessary, as the common interface is deleter_concept::operator()(void* ptr), NOT operator()(U* ptr)
// - U seems redundant, as it may be deduced from DELETER (we limit the freedom of U by requires-requires clause)
// - to remove U from deleter_model, we can :
//   (a) replace "default_deleter" by "default_deleter_with_ptr_type" 
//   (b) replace some implementation accordingly 
//   (c) but seems not very convenient
  
namespace alg
{
    template <typename T> 
    class shared_ptr
    {
        class manager 
        {
            class deleter_concept
            {
            public:
                virtual ~deleter_concept() = default;
                virtual void operator()(void* ptr) const = 0;
            };

            template <typename U, typename DELETER> 
            requires requires(U* ptr, DELETER deleter)
            {
                deleter(ptr);
            }
            class deleter_model : public deleter_concept
            {
            public:
                explicit deleter_model(DELETER d) : _deleter(std::move(d)) 
                {
                }

                void operator()(void* ptr) const override 
                {
                    _deleter(static_cast<U*>(ptr)); 
                }

            private:
                DELETER _deleter;
            };

        public:
            template<typename U, typename DELETER> // not duplicate the requires-requires in model?
            manager(U* ptr, DELETER&& deleter) 
                  : _count(1)
                  , _deleter_ptr(new deleter_model<std::decay_t<U>,
                                                   std::decay_t<DELETER>>(std::forward<DELETER>(deleter)))
            {
            }

        public:
            std::atomic<std::uint32_t> _count;
            deleter_concept* _deleter_ptr;
        };

    public:
        // Input U* ptr in next constructor cannot be nullptr as default, 
        // otherwise, type U cannot be auto-deduced from constructor,
        // as a result, we need to add this default constructor.
        shared_ptr() 
               : _ptr(nullptr)
               , _mgr(nullptr)
        {
        }

        template <typename U, typename DELETER = default_deleter<U>> 
        requires std::derived_from<U,T>
        explicit shared_ptr(U* ptr, DELETER&& deleter = default_deleter<U>()) 
               : _ptr(ptr)
               , _mgr(nullptr)
        {
            if (_ptr)
            {
                _mgr = new manager(ptr, std::forward<DELETER>(deleter));
            }
        }

        ~shared_ptr()
        {
            decrement(); 
        }

        // *** Copy semantics *** //
        shared_ptr(const shared_ptr<T>& rhs)
        {
            _ptr = rhs._ptr;
            _mgr = rhs._mgr; 
            increment();
        }

        shared_ptr<T>& operator=(const shared_ptr<T>& rhs)
        {
            if (this != &rhs)
            {
                decrement(); 
                _ptr = rhs._ptr;
                _mgr = rhs._mgr; 
                increment();
            }
            return *this;
        }

        // *** Move semantics *** //
        shared_ptr(shared_ptr<T>&& rhs) noexcept
        {
            _ptr = rhs._ptr;
            _mgr = rhs._mgr; 
            rhs._ptr = nullptr;
            rhs._mgr = nullptr; 
        }

        shared_ptr<T>& operator=(shared_ptr<T>&& rhs) noexcept
        {
            if (this != &rhs)
            {
                decrement();
                _ptr = rhs._ptr;
                _mgr = rhs._mgr; 
                rhs._ptr = nullptr;
                rhs._mgr = nullptr; 
            }
            return *this;
        }

    public: 
        bool unique()             const { return  _mgr && (_mgr->_count.load() == 1); } 
        std::uint32_t ref_count() const { return (_mgr ?   _mgr->_count.load() :  0); }
        operator bool()           const { return (_ptr!=nullptr); }
        const T& operator *()     const { return *_ptr;  }
        const T* operator->()     const { return  _ptr;  }
        const T* get()            const { return  _ptr;  }
        T& operator *()                 { return *_ptr;  }
        T* operator->()                 { return  _ptr;  }

    private:
        void increment()
        {
            if (_mgr) ++(_mgr->_count); 
        }

        void decrement()
        {
            if (_mgr && --(_mgr->_count) == 0)
            {
                (*(_mgr->_deleter_ptr))(_ptr); 
                delete _mgr->_deleter_ptr; // <---- please valgrind this memleak
                delete _mgr;
                _ptr = nullptr;
                _mgr = nullptr;
            }
        }

    /*  
     *  Run valgrind by 
     *  $ valgrind --tool=memcheck --leak-check-full --show-leak-kinds=all --verbose ./build/debug/Test
     *
     *  with    delete _mgr->_deleter_ptr : no error
     *  without delete _mgr->_deleter_ptr : error, leak detected
     */ 

    private:
        T* _ptr; 
        manager* _mgr; 
    };
     
    template<typename T, typename...ARGS>
    shared_ptr<T> make_shared(ARGS&&...args)
    {
        return shared_ptr<T>(new T (std::forward<ARGS>(args)...));
    } 

    // DST = provided by caller
    // SRC = deduced from input arg
    template<typename DST, typename SRC> 
    shared_ptr<DST> dynamic_pointer_cast(const shared_ptr<SRC>& src_ptr)
    {
        DST* ptr = dynamic_cast<DST*>(src_ptr.get());
        if (ptr) 
        {
            return shared_ptr<DST>(ptr);
        }
        else
        {
            return shared_ptr<DST>();
        }
    }
}

