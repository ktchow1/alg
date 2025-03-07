#pragma once
#include<memory>
#include<cstdint>
#include<string>
#include<vector>

// Please refer to "test_type_erasure_example.cpp" for step-by-step to the ideas of type erasure.
//
//
// Objective : to design a non-template class
// * which can wrap any type T, without inheritance relationship
// * which can invoke common interface functions
// 
//
// Making use of 3 classes : 
// 1. concept (or base),    which is non-template-class
// 2. model   (or wrapper), which is template-class of T, all inherited from concept
// 3. type erasure class,   which is non-template-class, with template-constructor of T
//    (a) it contains a concept*
//    (b) it initializes concept* = new model<T>(...) in its template-constructor of T <--- This is how the type is erased (hidden).
// Type T does not exist in type erasure class, except in constructor, hence T is type erased.
//
//
// Applications :
// 1. std::shared_ptr<T>        <--- DELETER type is type erased from std::shared_ptr<T>
//                                   DELETER type is template parameter for template constructor of std::shared_ptr<T>
// 2. std::function<R(ARGS...)> <--- FUNCTOR type is type erased from std::function<R(ARGS...)>
//                                   FUNCTOR type is template parameter for template constructor of std::function<R(ARGS...)>
// The common interface functions :
// 1. std::shared_ptr<T>        <--- DELETER::operator()(void*)
// 2. std::function<R(ARSG...)  <--- FUNCTOR::operator()(ARGS...) -> R
//
// 
// In the following example :
// type_erase_memfct  is NOT a template of T, but its constructor is a template of T
// type_erase_functor is NOT a template of T, but its constructor is a template of T


// **************************************** //
// *** Type erasure for member-function *** //
// **************************************** //
namespace alg
{
    class type_erase_memfct
    {
        class base // aka conept
        {
        public:
            virtual ~base() = default;
            virtual std::uint32_t shared_interface(const std::string& str) const = 0;
        };

        template<typename T>
        class wrapper : public base // aka model
        {
        public:
            // ********************************************************* //
            // takes T as arg to support both 
            // construction with lvalue arg, i.e. one copy plus one move
            // construction wiht rvalue arg, i.e. two moves
            // with keyword explicit here to avoid implicit conversion
            // ********************************************************* //
            explicit wrapper(T object) : _object(std::move(object))
            {
            }

            std::uint32_t shared_interface(const std::string& str) const override
            {
                return _object.function(str);
            }
            
        private:
            T _object; // Remark 1 : instance of T
        };


    public:
        type_erase_memfct() = default;

        // ******************************************************************************* //
        // * 1st requires is require-clause
        // * 2nd requires is require-expression
        // * no explicit keyword to support implicit conversion from U to type_erasure
        // * universal reference to support construction with both lvalue / rvalue arg
        // * std::decay_t to support non-const lvalue targered_object, which otherwise ... 
        //
        //   fail in wrapper<T> construction 
        //        in _object(std::move(object) 
        //   with T = non-const raw type      <--- no such problem in type_erasure_example
        // ******************************************************************************* //
        template<typename U> 
        requires requires(U&& u, std::string str) 
        { 
            { u.function(str) } -> std::same_as<std::uint32_t>; 
        }
        type_erase_memfct(U&& targeted_object) : base_ptr(new wrapper<std::decay_t<U>>(std::forward<U>(targeted_object))) 
        {
            // This initialization is the core of type erasure :
            // it hides the type U into a non-template base_ptr
            // in template-constructor of non-template type-erasure class
        }
        
        inline std::uint32_t shared_interface(const std::string& str) const 
        {
            if (base_ptr) return base_ptr->shared_interface(str);
            else throw std::runtime_error("type_erase_memfct not initialized");
            // construction with 1st constructor, it will throw
            // construction with 2nd constructor will never throw
        }

    private:
        std::shared_ptr<base> base_ptr; // Remark 2 : pointer to base
    };
}


// ******************************** //
// *** Type erasure for functor *** //
// ******************************** //
namespace alg
{
    class type_erase_functor
    {
        class base
        {
        public:
            virtual ~base() = default;
            virtual std::uint32_t shared_interface(const std::string& str) const = 0;
        };

        template<typename T>
        class wrapper : public base
        {
        public:
            explicit wrapper(T object) : _object(std::move(object)) 
            {
            }

            std::uint32_t shared_interface(const std::string& str) const override
            {
                return _object(str); 
            }

        private:
            T _object; 
        };

    public:
        type_erase_functor() = default;

        template<typename U> 
        requires requires(U&& u, std::string str) 
        { 
            { u(str) } -> std::same_as<std::uint32_t>; 
        }
        type_erase_functor(U&& targeted_object) : base_ptr(new wrapper<std::decay_t<U>>(std::forward<U>(targeted_object)))
        {
        }
        
        inline std::uint32_t shared_interface(const std::string& str) const 
        {
            if (base_ptr) return base_ptr->shared_interface(str);
            else throw std::runtime_error("type_erase_functor not initialized");
        }

    private:
        std::shared_ptr<base> base_ptr;

    };
}
