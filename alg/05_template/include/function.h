#pragma once
#include<memory>


// ************************** //
// *** Interview question *** //
// ************************** //
// If we have an invocable argument as function input, 
// we can declare it in 2 ways, what are the differences?
//
// void invoker(std::function<void()>& fct)
// void invoker(auto& fct)
//
// Version 2 can bind to
// - function pointer
// - functor
// - lambda
// - std::function, while keeping their original types.
//
// Version 1 can also bind to above types, 
// but will convert all of them into std::function, which is slow.
//


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
                _fct();
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
                return _fct(std::forward<ARGS>(args)...);
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
