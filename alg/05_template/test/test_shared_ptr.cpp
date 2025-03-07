#include<iostream>
#include<memory>
#include<cassert>
#include<typeinfo> 
#include<shared_ptr.h> 
#include<type_erasure.h> 
#include<utility.h> 


// ************************** //
// *** Normal inheritance *** //
// ************************** //
class base
{
public:
    base(bool debug) : _debug(debug)   { if (_debug) std::cout << "\nbase constructor"; }
    virtual ~base()                    { if (_debug) std::cout << "\nbase destructor";  }
    virtual void memfct() const        {             std::cout << "\nbase memfct";      }

protected:
    bool _debug;
};

class derived0 : public base
{
public:
    derived0(bool debug) : base(debug) { if (_debug) std::cout << "\nderived0 constructor";  }
   ~derived0() override                { if (_debug) std::cout << "\nderived0 destructor";   }
    void memfct() const override       {             std::cout << "\nderived0 memfct";       }
};

class derived1 : public base
{
public:
    derived1(bool debug) : base(debug) { if (_debug) std::cout << "\nderived1 constructor";  }
   ~derived1() override                { if (_debug) std::cout << "\nderived1 destructor";   }
    void memfct() const override       {             std::cout << "\nderived1 memfct";       }
};


// ***************************************************************************** //
// *** Abnormal inheritance - virtual is omitted in destructor intentionally *** //
// ***************************************************************************** //
class no_virtual_base
{
public:
    no_virtual_base(bool debug) : _debug(debug)              { if (_debug) std::cout << "\nno_virtual_base constructor"; }
   ~no_virtual_base()                                        { if (_debug) std::cout << "\nno_virtual_base destructor";  }
    virtual void memfct() const                              {             std::cout << "\nno_virtual_base memfct";      }

protected:
    bool _debug;
};

class no_virtual_derived0 : public no_virtual_base
{
public:
    no_virtual_derived0(bool debug) : no_virtual_base(debug) { if (_debug) std::cout << "\nno_virtual_derived0 constructor";  }
   ~no_virtual_derived0()                                    { if (_debug) std::cout << "\nno_virtual_derived0 destructor";   }
    void memfct() const override                             {             std::cout << "\nno_virtual_derived0 memfct";       }
};

class no_virtual_derived1 : public no_virtual_base
{
public:
    no_virtual_derived1(bool debug) : no_virtual_base(debug) { if (_debug) std::cout << "\nno_virtual_derived1 constructor";  }
   ~no_virtual_derived1()                                    { if (_debug) std::cout << "\nno_virtual_derived1 destructor";   }
    void memfct() const override                             {             std::cout << "\nno_virtual_derived1 memfct";       }
};


// *************** //
// *** Deleter *** //
// *************** //
// T = base type
// U = derived type

template<typename U> 
class custom_deleter
{
public:
    // template T is introduced to make alg::simple_shared_ptr<derived> work ...
    template<typename T> requires std::derived_from<U,T>
    void operator()(T* ptr) const
    {
        [[maybe_unused]] U* dummy_ptr;
        
        std::cout << "\ncustom_deleter operator(" << typeid(dummy_ptr).name() << ")";
        delete dynamic_cast<U*>(ptr);
    }
};


// ******************************** //
// *** Test for reference count *** //
// ******************************** //
template<typename T>
void shared_ptr_test(const std::string& test_name)
{
    T pA(new derived0(false)); 
    T pB(pA); 
    assert(pA.ref_count() == 2);
    assert(pB.ref_count() == 2);
    {
        // copy semantics
        T p0(pA);
        T p1;
        p1 = pA;
        assert(pA.ref_count() == 4);
        assert(pB.ref_count() == 4);
        assert(p0.ref_count() == 4);
        assert(p1.ref_count() == 4);
        
        {
            // move semantics
            T p2(std::move(pA));
            T p3;
            p3 = std::move(pB);
            assert(pA.ref_count() == 0);
            assert(pB.ref_count() == 0);
            assert(p0.ref_count() == 4);
            assert(p1.ref_count() == 4);
            assert(p2.ref_count() == 4);
            assert(p3.ref_count() == 4);

            // assignment within same set
            p0 = p1;
            assert(p0.ref_count() == 4);
            assert(p1.ref_count() == 4);
            assert(p2.ref_count() == 4);
            assert(p3.ref_count() == 4);

            p1 = std::move(p2);
            assert(p0.ref_count() == 3);
            assert(p1.ref_count() == 3);
            assert(p2.ref_count() == 0);
            assert(p3.ref_count() == 3);

            p2 = std::move(p3);
            assert(p0.ref_count() == 3);
            assert(p1.ref_count() == 3);
            assert(p2.ref_count() == 3);
            assert(p3.ref_count() == 0);
        }
        // going out of scope 
        assert(p0.ref_count() == 2);
        assert(p1.ref_count() == 2);

        // container of shared_ptr
        pA = T(new derived1(false));
        pB = pA;
        assert(pA.ref_count() == 2);
        assert(pB.ref_count() == 2);

        std::vector<T> vec;
        for(int n=0; n!=10; ++n) 
        {
            vec.push_back(pA);
        }
        assert(pA.ref_count() == 12);
        assert(pB.ref_count() == 12);
    }
    assert(pA.ref_count() == 2);
    assert(pB.ref_count() == 2);

    // factory of base (not for derived)
    if constexpr (std::is_same_v<T, alg::simple_shared_ptr<base>>)
    {
        pA = alg::make_simple_shared<base>(false);
    }
    else
    {
        pA = alg::make_shared<base>(false);
    }
    assert(pA.ref_count() == 1);
    assert(pB.ref_count() == 1);
    print_summary(test_name, "succeeded");
}




// ************************ //
// *** Test for deleter *** //
// ************************ //
void test_simple_shared_ptr_constructor_deleter_sequence()
{
    std::cout << "\n\n### using alg::simple_shared_ptr for base ###";
    {
        alg::simple_shared_ptr<base> p0(new base(true));
        alg::simple_shared_ptr<base> p1(new derived0(true));
        alg::simple_shared_ptr<base> p2(new derived1(true));
        std::cout << "\n----- test virtual member -----";
        p0->memfct();
        p1->memfct();
        p2->memfct();
        std::cout << "\n----- out of scope -----";
    }
    std::cout << "\n\n### using alg::simple_shared_ptr for no_virtual_base ###";
    {
        alg::simple_shared_ptr<no_virtual_base> p0(new no_virtual_base(true));    
        alg::simple_shared_ptr<no_virtual_base> p1(new no_virtual_derived0(true));
        alg::simple_shared_ptr<no_virtual_base> p2(new no_virtual_derived1(true));
        std::cout << "\n----- test virtual member -----";
        p0->memfct();
        p1->memfct();
        p2->memfct();
        std::cout << "\n----- out of scope ----- (incorrect)";
    }
    std::cout << "\n\n### using alg::simple_shared_ptr for base with custom_deleter ###";
    {
        custom_deleter<base>     deleter;
        custom_deleter<derived0> deleter0; 
        custom_deleter<derived1> deleter1;
        alg::simple_shared_ptr<base, custom_deleter<base>>     p0(new base(true),     deleter);
        alg::simple_shared_ptr<base, custom_deleter<derived0>> p1(new derived0(true), deleter0);
        alg::simple_shared_ptr<base, custom_deleter<derived1>> p2(new derived1(true), deleter1);
        std::cout << "\n----- out of scope ----- (incorrect)";
    }
    std::cout << "\n\n### using alg::simple_shared_ptr for no_virtual_base with custom_deleter ###";
    {
        custom_deleter<no_virtual_base>     deleter;
        custom_deleter<no_virtual_derived0> deleter0; 
        custom_deleter<no_virtual_derived1> deleter1;
        alg::simple_shared_ptr<no_virtual_base, custom_deleter<no_virtual_base>>     p0(new no_virtual_base(true),     deleter);    
        alg::simple_shared_ptr<no_virtual_base, custom_deleter<no_virtual_derived0>> p1(new no_virtual_derived0(true), deleter0);
        alg::simple_shared_ptr<no_virtual_base, custom_deleter<no_virtual_derived1>> p2(new no_virtual_derived1(true), deleter1);
        std::cout << "\n----- out of scope ----- (incorrect)";
    }
    std::cout << "\n";
}


void test_shared_ptr_constructor_deleter_sequence()
{
    std::cout << "\n\n### using alg::shared_ptr for base ###";
    {
        alg::shared_ptr<base> p0(new base(true));
        alg::shared_ptr<base> p1(new derived0(true));
        alg::shared_ptr<base> p2(new derived1(true));
        std::cout << "\n----- test virtual member -----";
        p0->memfct();
        p1->memfct();
        p2->memfct();
        std::cout << "\n----- out of scope -----";
    }
    std::cout << "\n\n### using alg::shared_ptr for no_virtual_base with custom_deleter ###";
    {
        alg::shared_ptr<no_virtual_base> p0(new no_virtual_base(true));
        alg::shared_ptr<no_virtual_base> p1(new no_virtual_derived0(true));
        alg::shared_ptr<no_virtual_base> p2(new no_virtual_derived1(true));
        std::cout << "\n----- test virtual member -----";
        p0->memfct();
        p1->memfct();
        p2->memfct();
        std::cout << "\n----- out of scope -----";
    }
    std::cout << "\n\n### using alg::shared_ptr for base ###";
    {
        custom_deleter<base>     deleter;
        custom_deleter<derived0> deleter0; 
        custom_deleter<derived1> deleter1;
        alg::shared_ptr<base> p0(new base(true),     deleter);
        alg::shared_ptr<base> p1(new derived0(true), deleter0);
        alg::shared_ptr<base> p2(new derived1(true), deleter1);
        std::cout << "\n----- out of scope -----";
    }
    std::cout << "\n\n### using alg::shared_ptr for no_virtual_base with custom_deleter ###";
    {
        custom_deleter<no_virtual_base>     deleter;
        custom_deleter<no_virtual_derived0> deleter0; 
        custom_deleter<no_virtual_derived1> deleter1;
        alg::shared_ptr<no_virtual_base> p0(new no_virtual_base(true),     deleter);
        alg::shared_ptr<no_virtual_base> p1(new no_virtual_derived0(true), deleter0);
        alg::shared_ptr<no_virtual_base> p2(new no_virtual_derived1(true), deleter1);
        std::cout << "\n----- out of scope -----";
    }
    std::cout << "\n";
}


void test_shared_ptr()
{
    // Test reference counting
    shared_ptr_test<alg::simple_shared_ptr<base>>("alg::simple_shared_ptr --- reference counting");
    shared_ptr_test<alg::       shared_ptr<base>>("alg::shared_ptr ---------- reference counting");

    // Test constructor & deleter sequence <--- turn it ON if need
    if (false) 
    {
        test_simple_shared_ptr_constructor_deleter_sequence();
        test_shared_ptr_constructor_deleter_sequence();
    }
}
