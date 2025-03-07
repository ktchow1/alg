#pragma once
#include<cstdint>
#include<functional>
#include<limits>
#include<vector>
#include<queue>
#include<stack>
#include<algorithm>
// *************************************************************************** //
// 1. Tree definitions and properties
// -  what is a tree
// -  what is a binary tree
// -  what is a binary complete tree
// -  what is a binary complete sorted tree
// 2. Tree functions 
// -  insert 
// -  find 
// -  depth
// -  balance factor
// -  balance & rotate
// -  traversal         |     applications 
//    ------------------+-----------------------------------------------------
//    BFS               | 
//    DFS_pre_order     |     traversal in prefix tree
//    DFS_in_order      |     traversal in avl tree for sorting
//    DFS_post_order    |     destruction / calculate depth & balance factor
// *************************************************************************** //
// Null check 
// * check this_node           <--- less check, more recursion, simpler code
// * check this_node->m_lhs &
//         this_node->m_rhs    <--- more check, less recursion, faster runtime
// *************************************************************************** //
  
namespace alg
{
    template<typename T>
    struct traversal_cache
    {
        void operator()(const T& x)
        {
            m_values.push_back(x);
        }

        std::vector<T> m_values;
    };

    template<typename T>
    bool is_equal(const std::vector<T>& v0, 
                  const std::vector<T>& v1)
    {
        if (v0.size() != v1.size()) return false;
        for(std::uint32_t n=0; n!=v0.size(); ++n)
        {
            if (v0[n]!=v1[n]) return false;
        }
        return true;
    }

    template<typename T>
    bool is_sorted(const std::vector<T>& v) 
    {
        if (v.size() <= 1) return true;
        for(std::uint32_t n=0; n!=v.size()-1; ++n)
        {
            if (v[n]>v[n+1]) return false;
        }
        return true;
    }
}

namespace alg { namespace avl
{
    template<typename T>
    struct node
    {
        node() : m_value{}, m_lhs{nullptr}, m_rhs{nullptr}
        {
        }

        explicit node(const T& x) : m_value{x}, m_lhs{nullptr}, m_rhs{nullptr}
        {
        }

        T        m_value;
        node<T>* m_lhs;
        node<T>* m_rhs;
    };

    enum class mode : std::uint8_t
    {
        dfs_in_order_recursive,    
        dfs_pre_order_recursive,   
        dfs_post_order_recursive,  
        dfs_in_order_iterative,    
        dfs_pre_order_iterative,   
        bfs_iterative              
    };

    template<typename T, typename F>
    void traverse_rhs_only(const node<T>* this_node, F& fct) // for using avl::node as doubly_list only
    {
        while(this_node!=nullptr)
        {
            fct(this_node->m_value);
            this_node = this_node->m_rhs;
        }
    }

    template<typename T>
    class tree
    {
    public:
        tree() : m_root(nullptr), m_memfree(true) // BUG1 : forget to init m_root
        {
        }

        explicit tree(node<T>* root_created_by_user) : m_root(root_created_by_user), m_memfree(true)
        {
        }

        ~tree() 
        { 
            if (m_memfree) destruct(m_root); // BUG5 : dont forget destructor (test with Valgrind)
        }

    public:
              node<T>* insert(const T& x)                { return insert(&m_root, x);      } 
        const node<T>* find  (const T& x) const noexcept { return find(m_root, x);         }
        std::uint32_t  depth()            const noexcept { return depth(m_root);           }
        std:: int32_t  balance_factor()   const noexcept { return balance_factor(m_root);  }
        const node<T>* root()             const noexcept { return m_root;                  }
              node<T>* root()                            { return m_root;                  }
        void  no_memfree_on_destruction()                { m_memfree = false;              } // caller will modify topology, responsible for memory free

        template<typename F> requires std::invocable<F,T>
        void traverse(F& fct, const mode& m) const noexcept 
        {
            if      (m == mode::dfs_in_order_recursive)    dfs_in_order_recursive(m_root, fct); 
            else if (m == mode::dfs_pre_order_recursive)   dfs_pre_order_recursive(m_root, fct);
            else if (m == mode::dfs_post_order_recursive)  dfs_post_order_recursive(m_root, fct);
            else if (m == mode::dfs_pre_order_iterative)   dfs_pre_order_iterative(m_root, fct);
            else if (m == mode::dfs_in_order_iterative)    dfs_in_order_iterative(m_root, fct);
            else                                           bfs_iterative(m_root, fct);
        }

        //      Y                    X
        //     / \    lhs rotate    / \
        //    X   C   <---------   A   Y
        //   / \      --------->      / \
        //  A   B     rhs rotate     B   C

        void rotate_lhs() { m_root = rotate_lhs(m_root); }
        void rotate_rhs() { m_root = rotate_rhs(m_root); }

        node<T>* rotate_lhs(node<T>* this_node) 
        {
            if (this_node->m_rhs == nullptr) return this_node; // no rotation

            node<T>* new_root = this_node->m_rhs;
            this_node->m_rhs = new_root->m_lhs;
            new_root->m_lhs = this_node;
            return new_root;
        }

        node<T>* rotate_rhs(node<T>* this_node)
        {
            if (this_node->m_lhs == nullptr) return this_node; // no rotation

            node<T>* new_root = this_node->m_lhs;
            this_node->m_lhs = new_root->m_rhs;
            new_root->m_rhs = this_node;
            return new_root;
        }

    private:
        void destruct(node<T>* this_node)
        {
            // Important : destruction is done with post_order 
            if (this_node)
            {   
                destruct(this_node->m_lhs);
                destruct(this_node->m_rhs);
                delete this_node;
            }
        }

        node<T>* insert(node<T>** this_node_ptr, const T& x) // BUG2 : need to use node<T>** for this_node_ptr
        {
            if      (*this_node_ptr == nullptr)          { *this_node_ptr = new node<T>(x); return *this_node_ptr; }
            else if (x < (*this_node_ptr)->m_value)      { return insert(&(*this_node_ptr)->m_lhs, x); }
            else if (x > (*this_node_ptr)->m_value)      { return insert(&(*this_node_ptr)->m_rhs, x); }
            else return *this_node_ptr;
        }

        const node<T>* find(const node<T>* this_node, const T& x) const noexcept
        {
            if      (this_node == nullptr)                 return nullptr;
            else if (x < this_node->m_value)               return find(this_node->m_lhs, x);
            else if (x > this_node->m_value)               return find(this_node->m_rhs, x);
            else                                           return this_node;
        }

        std::uint32_t depth(const node<T>* this_node) const noexcept
        {
            if (this_node) return 1 + std::max(depth(this_node->m_lhs), depth(this_node->m_rhs));
            else           return 0;
        }

        std::int32_t balance_factor(const node<T>* this_node) const noexcept
        {
            if (this_node) return depth(this_node->m_lhs) - depth(this_node->m_rhs);
            else           return 0;
        }

    private:
        template<typename F> requires std::invocable<F,T>
        void dfs_in_order_recursive(const node<T>* this_node, F& fct) const noexcept
        {
            if (this_node == nullptr) return;
            
            dfs_in_order_recursive(this_node->m_lhs, fct);
            fct(this_node->m_value); 
            dfs_in_order_recursive(this_node->m_rhs, fct);
        }

        template<typename F> requires std::invocable<F,T>
        void dfs_pre_order_recursive(const node<T>* this_node, F& fct) const noexcept
        {
            if (this_node == nullptr) return;
           
            fct(this_node->m_value); 
            dfs_pre_order_recursive(this_node->m_lhs, fct);
            dfs_pre_order_recursive(this_node->m_rhs, fct);
        }

        template<typename F> requires std::invocable<F,T>
        void dfs_post_order_recursive(const node<T>* this_node, F& fct) const noexcept
        {
            if (this_node == nullptr) return;
            
            dfs_post_order_recursive(this_node->m_lhs, fct);
            dfs_post_order_recursive(this_node->m_rhs, fct);
            fct(this_node->m_value); 
        }

        template<typename F> requires std::invocable<F,T>
        void dfs_pre_order_iterative(const node<T>* this_node, F& fct) const noexcept
        {
            std::stack<const node<T>*> s; // there exists nullptr in s
            s.push(this_node);

            while(!s.empty())
            {
                this_node = s.top();
                s.pop(); // BUG3 : pop immediately after top
                if (this_node) 
                {
                    fct(this_node->m_value);
                    s.push(this_node->m_rhs); // BUG4 : push rhs first
                    s.push(this_node->m_lhs); // BUG4 : push lhs later, we process lhs first
                }
            }
        }
 
        // ******************************************************************************//
        // RULE 1 : overtake this_node by lhs, cache this_node as it is next-to-process
        // RULE 2 : pop previous overtaken node, process it, visit rhs child, goto rule 1
        // ******************************************************************************//
        template<typename F> requires std::invocable<F,T>
        void dfs_in_order_iterative(const node<T>* this_node, F& fct) const noexcept
        {
            std::stack<const node<T>*> s; // there are no nullptr in s

            while(this_node || !s.empty())
            {
                // RULE 1
                if (this_node != nullptr) 
                {
                    s.push(this_node);
                    this_node = this_node->m_lhs;
                }
                // RULE 2 
                else
                {
                    this_node = s.top(); 
                    s.pop();

                    fct(this_node->m_value);
                    this_node = this_node->m_rhs;
                }
            }
        }

        template<typename F> requires std::invocable<F,T>
        void bfs_iterative(const node<T>* this_node, F& fct) const noexcept
        {
            std::queue<const node<T>*> q; // there exists nullptr in q
            q.push(this_node);

            while(!q.empty())
            {
                this_node = q.front();
                if (this_node) 
                {
                    fct(this_node->m_value);
                    q.push(this_node->m_lhs);
                    q.push(this_node->m_rhs);
                }
                q.pop();
            }
        }

    private:
        node<T>* m_root;
        bool m_memfree;
    };
}}


// *********************** //
// *** Tree algorithms *** //
// *********************** //
// For these algo, implementation is easier by 
// * assuming input node is non-nullptr
// * checking this_node->m_lhs for nullptr
// * checking this_node->m_rhs for nullptr
//
namespace alg { namespace avl
{
    template<typename ITER>
    bool is_vec_post_ordered(const ITER& begin, const ITER& end)
    {
        if (begin == end)  return true;
        ITER last =  end;
        --last;

        if (begin == last) return true;
        ITER mid = begin;

        while(mid != last)
        {
            if (*mid > *last) break;
            ++mid; 
        }
        for(ITER iter=mid; iter!=last; ++iter) // BUG : Dont forget this part
        {
            if (*iter < *last) return false;
        }
        return is_vec_post_ordered(begin,mid) && is_vec_post_ordered(mid,last);
    }

    template<typename T>
    bool is_avl_tree_sorted(const node<T>* root, 
                            const T& min = std::numeric_limits<T>::min(),
                            const T& max = std::numeric_limits<T>::max())
    {
        if (root->m_lhs == nullptr)  
        {
            if (root->m_value < min) return false;
        }
        else
        {
            if (!is_avl_tree_sorted(root->m_lhs, min, root->m_value)) return false;
        }

        if (root->m_rhs == nullptr)  
        {
            if (root->m_value > max) return false;
        }
        else
        {
            if (!is_avl_tree_sorted(root->m_rhs, root->m_value, max)) return false;
        }
        return true;
    }

    template<typename ITER>
    node<typename std::iterator_traits<ITER>::value_type>* create_avl_tree_from_sorted_vec(const ITER& begin, const ITER& end) 
    {   
        if (begin == end) return nullptr; 

        ITER mid = begin;
        std::advance(mid, std::distance(begin, end)/2);

        // BUG6 : tree created by this algo will not be destructed and hence memleak
        auto* new_node = new node<typename std::iterator_traits<ITER>::value_type> {*mid};   // can handle case when size = 1
        new_node->m_lhs = create_avl_tree_from_sorted_vec(begin, mid);                       // when size=1, mid = begin, will return nullptr
        new_node->m_rhs = create_avl_tree_from_sorted_vec(mid+1, end);                       // when size=1, mid+1 = end, will return nullptr
        return new_node;
    }

    template<typename T>
    std::pair<node<T>*, node<T>*> create_doubly_list_from_avl_tree(node<T>* root) 
    {
        node<T>* head;    
        node<T>* tail;

        // Process LHS
        if (root->m_lhs != nullptr) 
        {
            auto temp = create_doubly_list_from_avl_tree(root->m_lhs);
            root->m_lhs = temp.second;
            temp.second->m_rhs = root;
            head = temp.first;
        }
        else
        {
            root->m_lhs = nullptr;
            head = root;
        }

        // Process RHS
        if (root->m_rhs != nullptr) 
        {
            auto temp = create_doubly_list_from_avl_tree(root->m_rhs);
            root->m_rhs = temp.first;
            temp.first->m_lhs = root;
            tail = temp.second;
        }
        else
        {
            root->m_rhs = nullptr;
            tail = root;
        }
        return std::make_pair(head, tail);
    }

}}
