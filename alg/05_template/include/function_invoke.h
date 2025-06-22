#pragma once



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

