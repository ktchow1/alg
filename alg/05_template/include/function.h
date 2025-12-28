#pragma once
#include<functional>
#include<memory>


// ************************************** //
// *** std::bind is NOT std::function *** //
// ************************************** //
// std::function is a type-erased wrapper, which needs to know whole signature.
// std::bind is a templated callable object, which needs to know all arg types.
// std::bind's return type is NOT std::function.
//
//
//               |   std::function       std::bind
// --------------+-------------------------------------------
// what is it    |   type erasure        templated callable                 
// arg    type   |   known in compile    known in compile 
// return type   |   known in compile    deduced
// performance   |   slower              faster
//



// *************************************************** // 
// *** alg::function can bind to various callables *** //
// *************************************************** // 
//
//                               |              direct function call      standardised call of std::invoke
// ------------------------------+---------------------------------------------------------------------------
// 1. function pointer           |              (*f)(arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 2. functor (rvalue)           |                f (arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
//    functor (lvalue)           |                f (arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 3. member pointer             |          (obj.*f)(arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 4. lambda                     |                f (arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 5. std::function              |                f (arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 6. std::bind                  |                f (arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
// 7. std::reference_wrapper<F>  |           f.get()(arg0,arg1,arg2)        std::invoke(f,arg0,arg1,arg2)
//                               | or simply      f (arg0,arg1,arg2)
//
//
// Remark 1 : 
// Implementation with direct function call has different forms for different callables, hence not suitable for alg::function.
// Implementation with standardised call can handle all callables, the complexity is delegated to std::invoke.
//
// Remark 2 : 
// alg::simple_function does not support member pointer, even with std::invoke,
// because it is nullary and no placeholder for passing the object.
//
// Remark 3 : 
// There is std::reference_wrapper<T>::operator() for calling wrapped callable directly.



// *************************************** //
// *** nullary function returning void *** //
// *************************************** //
namespace alg
{
    class simple_function
    {
        class callable_concept
        {
        public:
            virtual ~callable_concept() = default;
            virtual void call() const = 0;
        };

        template<typename F>
        class callable_model : public callable_concept
        {
        public:
            explicit callable_model(F fct) : _fct(std::move(fct))
            {
            }

            void call() const override
            {
                std::invoke(_fct);

                 // The following implementation does not support :
                 // * member pointer
                 // * std::reference_wrapper
                   
            //  _fct();
            }

        private:
            F _fct;
        };

    public:
        simple_function() = default; 

        template<typename F>
        simple_function(F&& fct) : _fct_ptr(new callable_model<std::decay_t<F>>(std::forward<F>(fct)))
        {
        } 

        inline void operator()() const    
        {  
            if (_fct_ptr) _fct_ptr->call();
            else throw std::runtime_error("simple_function not initialized");
        }

    private:
        // use unique_ptr if we want simple_function to be non-copyable
        // use shared_ptr if we want simple_function to be copyable
        std::shared_ptr<callable_concept> _fct_ptr;
    };
}


// ********************************** //
// *** N-ary function returning R *** //
// ********************************** //
// Implement std::function for R(ARGS...)
//
namespace alg
{
    template<typename R, typename...ARGS>
    class function
    {
        class callable_concept
        {
        public:
            virtual ~callable_concept() = default;
            virtual R call(ARGS&&...args) const = 0;
        };

        template<typename F>
        class callable_model : public callable_concept
        {
        public:
            explicit callable_model(F fct) : _fct(std::move(fct)) 
            {
            }

            R call(ARGS&&...args) const override
            {
                return std::invoke(_fct, std::forward<ARGS>(args)...);
            //  return _fct(std::forward<ARGS>(args)...);
            }

        private:
            F _fct;
        };

    public:
        function() = default; 

        template<typename F>
        function(F&& fct) : _fct_ptr(new callable_model<std::decay_t<F>>(std::forward<F>(fct)))
        {
        }  

        inline R operator()(ARGS&&...args) const
        {  
            if (_fct_ptr) return _fct_ptr->call(std::forward<ARGS>(args)...);
            else throw std::runtime_error("function not initialized");
        }

    private:
        std::shared_ptr<callable_concept> _fct_ptr;
    };
}
