#include<iostream>
#include<iomanip>
#include<cassert>
#include<tree.h>
#include<utility.h>


template<typename T>
class root_value_overwriter
{
public:
    explicit root_value_overwriter(alg::avl::node<T>* root) : m_root(root)
    {
    }

    void overwrite()
    {
        if (m_root)
        {
            m_old_value = m_root->m_value;
            if      (m_root->m_lhs)  m_root->m_value = m_root->m_lhs->m_value-1;
            else if (m_root->m_rhs)  m_root->m_value = m_root->m_rhs->m_value+1;
        }
    }

    void restore()
    {
        m_root->m_value = m_old_value;
    }

private:
    alg::avl::node<T>* m_root;
    T m_old_value;
};


void test_avl_tree()
{
    std::uint32_t trial = 10000;
    std::uint32_t error0 = 0;
    std::uint32_t error1 = 0;
    std::uint32_t error2 = 0;
    std::uint32_t error3 = 0;
    std::uint32_t error4 = 0;
    std::uint32_t error5 = 0;
    std::uint32_t error6 = 0;

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        alg::avl::tree<std::uint32_t> tree;

        std::uint32_t N = 50 + rand()%500;
        for(std::uint32_t n=0; n!=N; ++n)
        {
            std::uint32_t x = rand()%500;
            auto* node = tree.insert(x);
        }

        alg::traversal_cache<std::uint32_t> f0;
        alg::traversal_cache<std::uint32_t> f1;
        alg::traversal_cache<std::uint32_t> f2;
        alg::traversal_cache<std::uint32_t> f3;
        alg::traversal_cache<std::uint32_t> f4;
        alg::traversal_cache<std::uint32_t> f5;
        tree.traverse(f0, alg::avl::mode::dfs_in_order_recursive);  
        tree.traverse(f1, alg::avl::mode::dfs_pre_order_recursive); 
        tree.traverse(f2, alg::avl::mode::dfs_post_order_recursive);  
        tree.traverse(f3, alg::avl::mode::dfs_pre_order_iterative); 
        tree.traverse(f4, alg::avl::mode::dfs_in_order_iterative);  
        tree.traverse(f5, alg::avl::mode::bfs_iterative);  

        if (!alg::is_sorted(f0.m_values))                                           ++error0;
        if (!alg::is_equal(f0.m_values, f4.m_values))                               ++error1;
        if (!alg::is_equal(f1.m_values, f3.m_values))                               ++error2;
        if ( alg::is_equal(f0.m_values, f1.m_values))                               ++error3;
        if (!alg::avl::is_vec_post_ordered(f2.m_values.begin(), f2.m_values.end())) ++error4; // *** Algo 0 *** //

        // **************** //
        // *** Rotation *** //
        // **************** //
        auto root0 = tree.root();
        auto rhs0  = root0->m_rhs;
        if  (rhs0 != nullptr)
        {
            auto rhslhs = rhs0->m_lhs;

            tree.rotate_lhs();
            auto root1  = tree.root();
            auto lhs1   = root1->m_lhs;
            auto lhsrhs =  lhs1->m_rhs;
            if (root0  == root1 || 
                root0  != lhs1  || 
                root1  != rhs0  || 
                rhslhs != lhsrhs)      ++error5;

            tree.rotate_rhs();
            auto root2 = tree.root();
            if (root0 != root2)        ++error6;
        }
    }
    print_summary("avl_tree verify  in_order",     error0, trial);
    print_summary("avl_tree compare in_order",     error1, trial);
    print_summary("avl_tree compare pre_order",    error2, trial);
    print_summary("avl_tree compare in/pre_order", error3, trial);
    print_summary("avl_tree verify  post_order",   error4, trial);
    print_summary("avl_tree rotate  lhs",          error5, trial);
    print_summary("avl_tree rotate  rhs",          error6, trial);
}


void test_avl_algo()
{
    std::uint32_t trial = 10000;
    std::uint32_t error0 = 0;
    std::uint32_t error1 = 0;
    std::uint32_t error2 = 0;
    std::uint32_t error3 = 0;
    std::uint32_t error4 = 0;
    std::uint32_t error5 = 0;
    std::uint32_t error6 = 0;

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        alg::avl::tree<std::uint32_t> tree;

        std::uint32_t N = 50 + rand()%500; 
        for(std::uint32_t n=0; n!=N; ++n)
        {
            std::uint32_t x = rand()%500;
            auto* node = tree.insert(x);
        }

        // ************** //
        // *** Algo 1 *** //
        // ************** //
        root_value_overwriter<std::uint32_t> overwriter(tree.root());
        if (!alg::avl::is_avl_tree_sorted(tree.root()))   ++error0;
        overwriter.overwrite();  
        if ( alg::avl::is_avl_tree_sorted(tree.root()))   ++error1;
        overwriter.restore();  
        if (!alg::avl::is_avl_tree_sorted(tree.root()))   ++error2;

        // ************** //
        // *** Algo 2 *** //
        // ************** //
        alg::traversal_cache<std::uint32_t> f0;
        alg::traversal_cache<std::uint32_t> f1;

        tree.traverse(f0, alg::avl::mode::dfs_in_order_recursive);  
        auto* root = alg::avl::create_avl_tree_from_sorted_vec(f0.m_values.begin(), f0.m_values.end()); 
        
                                                                          // Why construct balanced tree from root?
        alg::avl::tree bal_tree{root};                                    // 1. use tree traversal to verify output
        bal_tree.traverse(f1, alg::avl::mode::dfs_in_order_recursive);    // 2. use tree destructor to avoid memleak [verified with Valgrind]

        if (!alg::is_equal(f0.m_values, f1.m_values))                     ++error3;
        if (    bal_tree.depth()           >     tree.depth())            ++error4; 
        if (abs(bal_tree.balance_factor()) > abs(tree.balance_factor()))  ++error5; 
        
        // ************** //
        // *** Algo 3 *** //
        // ************** //
        alg::avl::node<std::uint32_t>* list_head;
        alg::avl::node<std::uint32_t>* list_tail;
        std::tie(list_head, list_tail) = alg::avl::create_doubly_list_from_avl_tree(tree.root()); 
        tree.no_memfree_on_destruction(); // Todo : solve memleak

        alg::traversal_cache<std::uint32_t> f2;
        alg::avl::traverse_rhs_only(list_head, f2);
        if (!alg::is_equal(f0.m_values, f2.m_values))                      ++error6;
    }
    print_summary("avl_tree verified to be sorted",                        error0, trial);
    print_summary("avl_tree verified to be unsorted",                      error1, trial);
    print_summary("avl_tree verified to be resorted",                      error2, trial);
    print_summary("avl_tree balanced by resort, compare vec",              error3, trial);
    print_summary("avl_tree balanced by resort, compare depth",            error4, trial);
    print_summary("avl_tree balanced by resort, compare BF (known error)", error5, trial);
    print_summary("avl_tree create doubly list",                           error6, trial);
}


// ***************************************************************************************************************** //
// Valgrind test is done :
// valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./build/debug/Test 
//
// * with    avl::tree destructor, no memleak
// * without avl::tree destructor,    memleak for test involving alg::avl::tree
//
// Destructor for avl::tree is implemented, it can avoid memleak.                         <--- like test_avl_tree()
// Destructor for node<T>* is not implemented, memleak occurs when using raw ptr as tree. <--- like test_avl_algo()
// **************************************************************************************************************** //
void test_tree()
{
    test_avl_tree(); 
    test_avl_algo();
}

