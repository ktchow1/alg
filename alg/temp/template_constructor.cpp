It is possble to have template constructor for non-template class. However type T should be deduce-able from constructor arguments.

class A
{
public:
    template<typename T>
    A(int, const T&) 
    {        
        if      (std::is_same<T,int>::value)         std::cout << "\nconstructor with int"; 
        else if (std::is_same<T,double>::value)      std::cout << "\nconstructor with double"; 
        else if (std::is_same<T,std::string>::value) std::cout << "\nconstructor with string"; 
        else                                         std::cout << "\nconstructor with other";   
    }
};

void test()
{
    A a0(1,1);                   // int
    A a1(1,1.234);               // double
    A a2(1,std::string{"1234"}); // string
    A a3(1,"1234");              // other, not sure what is it ... NOT char*, NOT char[]
}

However, T cannot be non-deduce-able from constructor arguments.

class A
{
public:
    template<typename T>
    A(int) 
    {        
        if      (std::is_same<T,int>::value)         std::cout << "\nconstructor with int"; 
        else if (std::is_same<T,double>::value)      std::cout << "\nconstructor with double"; 
        else if (std::is_same<T,std::string>::value) std::cout << "\nconstructor with string"; 
        else                                         std::cout << "\nconstructor with other";   
    }
};

void test()
{
    A      a0(1);    // compile error : couldn't deduce template parameter T
    A<int> a1(1);    // compile error : A is not a template class
}
