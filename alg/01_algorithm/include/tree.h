#pragma once
#include<cstdint>
#include<functional>
#include<limits>
#include<vector>
#include<queue>
#include<stack>
#include<algorithm>


// **************************************************************************** //
// 1. Tree definitions and properties
// -  what is a tree
// -  what is a binary tree
// -  what is a binary complete tree
// -  what is a binary complete sorted tree
//
//
// 2. Tree functions 
// -  insert
// -  destruct
// -  find 
// -  depth
// -  balance factor
// -  balance & rotate
// -  traversal        |  applications 
//    -----------------+------------------------------------------------
//    BFS              |
//    DFS_pre_order    |  traversal in prefix tree
//    DFS_in_order     |  traversal in avl tree for sorting
//    DFS_post_order   |  destruction / calculate depth & balance factor
//
//
// 3. For each function :
// -  provide interface without node<T>* input
//    provide recursive implementation with node<T>*  for const input
//    provide recursive implementation with node<T>** for mutable input
// -  provide null checking, either :
//    check this_node       
//    check this_node->m_lhs &
//          this_node->m_rhs
// -  for vector questions that involve recursion
//    if recursion of (begin,end) into (begin,mid) & (mid,end), need to :
//    check vec.size() == 0
//    check vec.size() == 1
//    if recursion of (begin,end) into (begin,mid) & (mid+1,end), need to :
//    check vec.size() == 0 only
//
// **************************************************************************** //
  
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
        template<typename...ARGS>
        node(ARGS&&...args) : m_value{std::forward<ARGS>(args)...}, m_lhs{nullptr}, m_rhs{nullptr}
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

    template<typename T, typename CMP = std::less<T>>
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

        // Why tree::insert() needs node<T>** while
        //     list::insert() needs node<T>* only?
        // Because tree::insert() is recursive and we may modify node<T>* by new operator.
        //
        node<T>* insert(node<T>** this_node_ptr, const T& x) // BUG2 : need to use node<T>** for this_node_ptr
        {
            if      (*this_node_ptr == nullptr)           { *this_node_ptr = new node<T>(x); return *this_node_ptr; }
            else if (CMP{}(x, (*this_node_ptr)->m_value)) { return insert(&(*this_node_ptr)->m_lhs, x); }
            else if (CMP{}((*this_node_ptr)->m_value, x)) { return insert(&(*this_node_ptr)->m_rhs, x); }
            else return *this_node_ptr;
        }

        const node<T>* find(const node<T>* this_node, const T& x) const noexcept
        {
            if      (this_node == nullptr)                  return nullptr;
            else if (CMP{}(x, this_node->m_value))          return find(this_node->m_lhs, x);
            else if (CMP{}(this_node->m_value, x))          return find(this_node->m_rhs, x);
            else                                            return this_node;
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
            std::stack<const node<T>*> s; // no nullptr in s
            if (this_node != nullptr) s.push(this_node);

            while(!s.empty())
            {
                this_node = s.top();
                s.pop();                                                  // BUG3 : pop must be before push
                fct(this_node->m_value);
                if (this_node->m_rhs!= nullptr) s.push(this_node->m_rhs); // BUG4 : push rhs first
                if (this_node->m_lhs!= nullptr) s.push(this_node->m_lhs); // BUG4 : push lhs later
            }
        }
 
        // ****************************************************************************** //
        // RULE 1 : overtake this_node by lhs, cache this_node as it is next-to-process
        // RULE 2 : pop previous overtaken node, process it, visit rhs child, goto rule 1
        //
        // Rotate the tree 45 degree clockwise, we can see better
        //
        //        05  04  03  02  01  00 (root)
        //            06          11
        //    09  08  07       13 12 
        //            10 
        // ****************************************************************************** //
        template<typename F> requires std::invocable<F,T>
        void dfs_in_order_iterative(const node<T>* this_node, F& fct) const noexcept
        {
            std::stack<const node<T>*> s; // no nullptr in s

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
        void dfs_in_order_iterative_FAILED(const node<T>* this_node, F& fct) const noexcept // FAILURE IMPLEMENTATION
        {  
            std::stack<const node<T>*> s; // no nullptr in s

            if (this_node != nullptr) s.push(this_node);
            while(!s.empty())
            {
                this_node = s.top();
                if (this_node->m_lhs != nullptr) s.push(this_node->m_lhs); // <--- pushing LHS will result in infinity loop, hence this implemenation does not work
                else
                {
                    s.pop();
                    fct(this_node->m_value);
                    if (this_node->m_rhs != nullptr) s.push(this_node->m_rhs);
                }
            }
        }

        template<typename F> requires std::invocable<F,T>
        void bfs_iterative(const node<T>* this_node, F& fct) const noexcept
        {
            std::queue<const node<T>*> q; // no nullptr in q
            if (this_node != nullptr) q.push(this_node);

            while(!q.empty())
            {
                this_node = q.front();
                q.pop();
                fct(this_node->m_value);
                if (this_node->m_lhs!= nullptr) q.push(this_node->m_lhs);
                if (this_node->m_rhs!= nullptr) q.push(this_node->m_rhs);
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
    template<typename ITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
    bool is_vec_post_ordered(const ITER& begin, const ITER& end)
    {
        if (begin == end)  return true;
        ITER back =  end;
        --back;

        ITER iter = begin;
        for(; iter!=back; ++iter)
        {
            if (CMP{}(*back, *iter)) break;
        }

        ITER mid = begin;
        for(; iter!=back; ++iter) 
        {
            if (CMP{}(*iter, *back)) return false;
        }
        return is_vec_post_ordered(begin, mid) && 
               is_vec_post_ordered(mid, back);
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
        return new_node;                                                                     // why we need to check size = 0,1 in is_vec_post_ordered(), but check size = 0 in here?
    }                                                                                        // both are recursive, but in here, we reduce the size in next call every time. 

    template<typename T>
    std::pair<node<T>*, node<T>*> create_doubly_list_from_avl_tree(node<T>* this_node) 
    {
        node<T>* head;    
        node<T>* tail;

        // Process LHS
        if (this_node->m_lhs != nullptr) 
        {
            auto [lhs_head, lhs_tail] = create_doubly_list_from_avl_tree(this_node->m_lhs);
             lhs_tail->m_rhs = this_node;
            this_node->m_lhs =  lhs_tail;
            head = lhs_head;
        }
        else
        {
        //  this_node->m_lhs = nullptr; // redundant
            head = this_node;
        }

        // Process RHS
        if (this_node->m_rhs != nullptr) 
        {
            auto [rhs_head, rhs_tail] = create_doubly_list_from_avl_tree(this_node->m_rhs);
             rhs_head->m_lhs = this_node;
            this_node->m_rhs =  rhs_head;
            tail = rhs_tail;
        }
        else
        {
        //  this_node->m_rhs = nullptr; // redundant
            tail = this_node;
        }
        return std::make_pair(head, tail);
    }

}}
