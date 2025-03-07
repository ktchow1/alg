#include<iostream>
#include<cassert>
#include<unique_ptr.h> 
#include<utility.h> 


namespace unique_ptr
{
    std::vector<std::string> invoked;


    // ************************ //
    // *** Base and derived *** //
    // ************************ //
    class base
    {
    public:
        base(bool debug, int id) : _debug(debug), _id(id) { if (_debug) invoked.push_back("base constructor");  }
        virtual ~base()                                   { if (_debug) invoked.push_back("base destructor");   }
        int get_id() const noexcept { return _id; }

    protected:
        bool _debug;
        int _id;
    };

    class derived0 : public base
    {
    public:
        derived0(bool debug, int id) : base(debug,id) { if (_debug) invoked.push_back("derived0 constructor");  }
       ~derived0() override                           { if (_debug) invoked.push_back("derived0 destructor");   }
    };

    class derived1 : public base
    {
    public:
        derived1(bool debug, int id) : base(debug,id) { if (_debug) invoked.push_back("derived1 constructor");  }
       ~derived1() override                           { if (_debug) invoked.push_back("derived1 destructor");   }
    };


    // *************** //
    // *** Deleter *** //
    // *************** //
    class custom_deleter
    {
    public:
        void operator()(base* ptr) const
        {
            invoked.push_back("custom_deleter for id " + std::to_string(ptr->get_id()));

            // when this is called, ptr must not be nullptr, hence no checking needed
            delete ptr;
        }
    };
}


void test_unique_ptr()
{
    using namespace unique_ptr;

    alg::unique_ptr<base> p0(new base(false,10));
    assert(static_cast<bool>(p0));
    assert(p0->get_id()==10);

    // moving to new unique_ptr
    alg::unique_ptr<base> p1(std::move(p0));
    assert(!static_cast<bool>(p0));
    assert( static_cast<bool>(p1));
    assert(p1->get_id()==10);

    alg::unique_ptr<base> p2;
    p2 = std::move(p1);
    assert(!static_cast<bool>(p0));
    assert(!static_cast<bool>(p1));
    assert( static_cast<bool>(p2));
    assert(p2->get_id()==10);

    // moving to old unique_ptr
    p2.reset(new derived0(false,11));
    assert(!static_cast<bool>(p0));
    assert(!static_cast<bool>(p1));
    assert( static_cast<bool>(p2));
    assert(p2->get_id()==11);

    // factory
    p0 = alg::make_unique<base>(false,12);
    p1 = alg::make_unique<base>(false,13);
    assert(static_cast<bool>(p0));
    assert(static_cast<bool>(p1));
    assert(static_cast<bool>(p2));
    assert(p0->get_id()==12);
    assert(p1->get_id()==13);
    assert(p2->get_id()==11);

    // custom deleter
    { 
        alg::unique_ptr<base,custom_deleter> p3(new base    (true,20), custom_deleter{});
        alg::unique_ptr<base,custom_deleter> p4(new derived0(true,21), custom_deleter{});
        alg::unique_ptr<base,custom_deleter> p5(new derived1(true,22), custom_deleter{});
    }
    assert(invoked[ 0] == "base constructor");
    assert(invoked[ 1] == "base constructor");
    assert(invoked[ 2] == "derived0 constructor");
    assert(invoked[ 3] == "base constructor");
    assert(invoked[ 4] == "derived1 constructor");
    assert(invoked[ 5] == "custom_deleter for id 22");
    assert(invoked[ 6] == "derived1 destructor");
    assert(invoked[ 7] == "base destructor");
    assert(invoked[ 8] == "custom_deleter for id 21");
    assert(invoked[ 9] == "derived0 destructor");
    assert(invoked[10] == "base destructor");
    assert(invoked[11] == "custom_deleter for id 20");
    assert(invoked[12] == "base destructor");
    
    auto p6 = alg::make_unique<base>(false,30);
    assert(static_cast<bool>(p6));

    print_summary("unique_ptr", "succeeded");
}

