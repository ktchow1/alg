#include<cassert>
#include<memory>
#include<utility.h>


// **************************************************** //
// *** 0. Forward declaration of concrete component *** //
// **************************************************** //
class componentA;
class componentB;
class componentC;


// ***************************************** //
// *** 1. Definition of abstract visitor *** //
// ***************************************** //
class component_visitor
{
public:
    virtual void visit(const componentA&) = 0;
    virtual void visit(const componentB&) = 0;
    virtual void visit(const componentC&) = 0;
};


// ******************************************* //
// *** 2. Definition of abstract component *** //
// ******************************************* //
class component
{
public:
    virtual ~component() {}
    virtual void accept(component_visitor&) const = 0; // This function invokes visiting straight away.
};


// ******************************************* //
// *** 3. Definition of concrete component *** //
// ******************************************* //
class componentA : public component
{
public:
    void accept(component_visitor& v) const override { v.visit(*this); }
};

class componentB : public component
{
public:
    void accept(component_visitor& v) const override { v.visit(*this); }
};

class componentC : public component
{
public:
    void accept(component_visitor& v) const override { v.visit(*this); }
};


// ***************************************** //
// *** 4. Definition of concrete visitor *** //
// ***************************************** //
class component_printer : public component_visitor
{
public:
    void visit(const componentA&) override { ++m_A_visit_count; }
    void visit(const componentB&) override { ++m_B_visit_count; }
    void visit(const componentC&) override { ++m_C_visit_count; }
    
    int m_A_visit_count = 0;
    int m_B_visit_count = 0;
    int m_C_visit_count = 0;
};

class component_editor : public component_visitor
{
public:
    void visit(const componentA&) override { ++m_A_visit_count; }
    void visit(const componentB&) override { ++m_B_visit_count; }
    void visit(const componentC&) override { ++m_C_visit_count; }
    
    int m_A_visit_count = 0;
    int m_B_visit_count = 0;
    int m_C_visit_count = 0;
};


// ****************************************************** //
// *** 5. Definition of visitor for variant-component *** //
// ****************************************************** //
// Depend on std::visit, need to define operator()
//
class component_printer2
{
public:
    void operator()(const componentA&) { ++m_A_visit_count; }
    void operator()(const componentB&) { ++m_B_visit_count; }
    void operator()(const componentC&) { ++m_C_visit_count; }
    
    int m_A_visit_count = 0;
    int m_B_visit_count = 0;
    int m_C_visit_count = 0;
};

class component_editor2
{
public:
    void operator()(const componentA&) { ++m_A_visit_count; }
    void operator()(const componentB&) { ++m_B_visit_count; }
    void operator()(const componentC&) { ++m_C_visit_count; }
    
    int m_A_visit_count = 0;
    int m_B_visit_count = 0;
    int m_C_visit_count = 0;
};
  

// ********************** //
// *** Test functions *** //
// ********************** //
void test_inheritance_visitor()
{
    std::vector<std::shared_ptr<component>> vec;
    vec.push_back(std::make_shared<componentA>());
    vec.push_back(std::make_shared<componentB>());
    vec.push_back(std::make_shared<componentC>());

    component_printer v0;
    component_editor v1;
    
    vec[0]->accept(v0);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 0);
    assert(v0.m_C_visit_count == 0);
    vec[0]->accept(v1);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 0);
    assert(v1.m_C_visit_count == 0);
    vec[1]->accept(v0);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 0);
    vec[1]->accept(v1);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 0);
    vec[2]->accept(v0);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 1);
    vec[2]->accept(v1);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 1);
    vec[0]->accept(v0);
    assert(v0.m_A_visit_count == 2);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 1);
    vec[0]->accept(v1);
    assert(v1.m_A_visit_count == 2);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 1);
    print_summary("variant - inheritance visitor", "succeeded");
}


void test_variant_visitor()
{
    std::vector<std::variant<componentA, componentB, componentC>> vec;
    vec.push_back(componentA{});
    vec.push_back(componentB{});
    vec.push_back(componentC{});

    component_printer2 v0;
    component_editor2 v1;
    
    std::visit(v0, vec[0]);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 0);
    assert(v0.m_C_visit_count == 0);
    std::visit(v1, vec[0]);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 0);
    assert(v1.m_C_visit_count == 0);
    std::visit(v0, vec[1]);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 0);
    std::visit(v1, vec[1]);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 0);
    std::visit(v0, vec[2]);
    assert(v0.m_A_visit_count == 1);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 1);
    std::visit(v1, vec[2]);
    assert(v1.m_A_visit_count == 1);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 1);
    std::visit(v0, vec[0]);
    assert(v0.m_A_visit_count == 2);
    assert(v0.m_B_visit_count == 1);
    assert(v0.m_C_visit_count == 1);
    std::visit(v1, vec[0]);
    assert(v1.m_A_visit_count == 2);
    assert(v1.m_B_visit_count == 1);
    assert(v1.m_C_visit_count == 1);
    print_summary("variant - variant visitor", "succeeded");
}


void test_variant_visit()
{
    test_inheritance_visitor();
    test_variant_visitor();
}
