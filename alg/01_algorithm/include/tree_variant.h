#pragma once
#include<optional>
#include<tree.h>


namespace alg
{
    template<typename T, typename CMP = std::less<T>>
    class heap
    {
    public:
        using value_type = T;

        template<typename...ARGS>
        void push(ARGS&&...args)
        {
            m_impl.emplace_back(std::forward<ARGS>(args)...);
            ascend(size()-1);
        }

        void pop()
        {
            m_impl[0] = m_impl[size()-1];
            m_impl.pop_back();
            descend();
        }

        const T& top() const noexcept
        {
            return m_impl[0];
        }

        std::uint32_t size() const noexcept
        {
            return m_impl.size();
        }

        bool empty() const noexcept
        {
            return m_impl.empty();
        }

        void clear()
        {
            m_impl.clear();
        }

    private:
        void ascend(std::uint32_t n) // ascend element n
        {
            while(n > 0)
            {
                std::uint32_t m = (n-1)/2;
                if (CMP{}(m_impl[n], m_impl[m]))
                {
                    std::swap(m_impl[n], m_impl[m]);
                    n = m;
                }
                else return;
            }
        }

        void descend() // descend element 0 
        {
            std::uint32_t n = 0;
            while(n < size())
            {
                std::uint32_t m0 = 2*n+1;
                std::uint32_t m1 = 2*n+2;
                if (m1 < size())
                {
                    if (CMP{}(m_impl[m0], m_impl[m1]))
                    {
                        if (CMP{}(m_impl[m0], m_impl[n])) { std::swap(m_impl[n], m_impl[m0]); n = m0; }
                        else return;
                    }
                    else
                    {
                        if (CMP{}(m_impl[m1], m_impl[n])) { std::swap(m_impl[n], m_impl[m1]); n = m1; }
                        else return;
                    }
                }
                else if (m0 < size())
                {
                    if (CMP{}(m_impl[m0], m_impl[n])) { std::swap(m_impl[n], m_impl[m0]); n = m0; }
                    else return;
                }
                else return;
            }
        }

    private:
        std::vector<T> m_impl;
    };
}


// ****************************** //
// *** Inplace implementation *** //
// ****************************** //
namespace alg
{
    // when CMP = std::less,    sort sequence with std::less,    build heap with std::greater
    // when CMP = std::greater, sort sequence with std::greater, build heap with std::less
    namespace details
    {
        template<typename ITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
        void ascend_with_inversed_comparator(ITER begin, ITER back) // support size >= 1
        {
            ITER iter_n = back;
            while(iter_n > begin)
            {
                ITER iter_m = begin;
                std::advance(iter_m, (std::distance(begin, iter_n)-1) / 2);

                if (CMP{}(*iter_m, *iter_n)) 
                {
                    std::swap(*iter_n, *iter_m);
                    iter_n = iter_m;
                }
                else return;
            }
        }

        template<typename ITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
        void descend_with_inversed_comparator(ITER begin, ITER end) // support size >= 0
        {
            ITER iter_n = begin;
            while(iter_n < end)
            {
                ITER iter_m0 = begin; 
                ITER iter_m1 = begin; 
                std::advance(iter_m0, 2 * std::distance(begin, iter_n) + 1);
                std::advance(iter_m1, 2 * std::distance(begin, iter_n) + 2);

                if (iter_m1 < end)
                {
                    if (CMP{}(*iter_m1, *iter_m0))
                    {
                        if (CMP{}(*iter_n, *iter_m0)) { std::swap(*iter_n, *iter_m0); iter_n = iter_m0; }
                        else return;
                    }
                    else
                    {
                        if (CMP{}(*iter_n, *iter_m1)) { std::swap(*iter_n, *iter_m1); iter_n = iter_m1; }
                        else return;
                    }
                }
                else if (iter_m0 < end)
                {
                    if (CMP{}(*iter_n, *iter_m0)) { std::swap(*iter_n, *iter_m0); iter_n = iter_m0; }
                    else return;
                }
                else return;
            }
        }
    }

    template<typename ITER, typename CMP = std::less<typename std::iterator_traits<ITER>::value_type>>
    void heap_sort(ITER begin, ITER end) // random access iterator
    {
        if (begin == end) return; // BUG : Dont miss this.

        // *** step 1 : push into heap *** //
        ITER iter = begin; 
        for(++iter; iter!=end; ++iter)
        {
            details::ascend_with_inversed_comparator<ITER,CMP>(begin, iter);
        }

        // *** step 2 : pop from heap *** //
        iter = end; 
        for(--iter; iter!=begin; --iter)
        {
            std::swap(*begin, *iter);
            details::descend_with_inversed_comparator<ITER,CMP>(begin, iter);
        }
    }
}


// ************************************* //
// 1. For disjoint_set is a tree :
// -      represented by parent link
// -  NOT represented by children link
//
// 2. Root of a set is a class-label.
// ************************************* //
namespace alg
{
    enum class find_mode : std::uint8_t
    {
        iterative,
        recursive,
        recursive_with_path_compression
    };

    template<typename T>
    class disjoint_set
    {
    public:
        explicit disjoint_set(find_mode mode) : m_mode(mode)
        {
        }

        const T& find(const T& x)
        {
            if      (m_mode == find_mode::iterative)  return find_root_iterative(x);
            else if (m_mode == find_mode::recursive)  return find_root_recursive(x);
            else                                      return find_root_recursive_with_path_compression(x);
        }

        void union_of(const T& x, const T& y)
        {
            auto root_x = find(x);
            auto root_y = find(y);
            m_parent[root_x] = root_x;
            m_parent[root_y] = root_x; // BUG : we need to update both
        }

        bool is_same_set(const T& x, const T& y) 
        {
            auto root_x = find(x);
            auto root_y = find(y);
            return root_x == root_y;
        }

    private:
        const T& find_root_iterative(const T& x) 
        {
            T temp = x;

            auto iter = m_parent.find(temp); 
            while(iter != m_parent.end())
            {
                if (iter->second == temp)   return iter->second; // return iter->second, not return "temp", which is temporary
                temp = iter->second; 
                iter = m_parent.find(temp); 
            }
            return x;
        }

        const T& find_root_recursive(const T& x)
        {
            auto iter = m_parent.find(x); 
            if (iter == m_parent.end())  return x;
            if (iter->second == x)       return x;
            return find_root_recursive(iter->second);
        }

        const T& find_root_recursive_with_path_compression(const T& x)
        {
            auto iter = m_parent.find(x); 
            if (iter == m_parent.end())  return x;
            if (iter->second == x)       return x;
            return iter->second = find_root_recursive(iter->second);
        }

    private:
        const find_mode m_mode;
        std::unordered_map<T,T> m_parent; 
    };
}


// ********************************************************************************
// AVL tree vs Prefix tree 
// * key  in avl    tree is stored in node, key == value for std::set
//   key  in prefix tree is concaternation of char of all ancesters, exclude itself
// * node in avl    tree always contains value
//   node in prefix tree may not contain value 
// * node in avl    tree has 0-2  children
//   node in prefix tree has 0-26 children
// * node are NOT pointers, they are struct
// ********************************************************************************
namespace alg
{
    template<typename V> // K = std::string
    class prefix_tree
    {
    public:
        struct node
        {
            std::optional<V> m_value;
            std::unordered_map<char, node> m_children; 
        };
        
        // compare another design (in which, node must be pointer)
    //  struct node
    //  {
    //      std::optional<V> m_value;
    //      std::array<node*, 26> m_children; 
    //  };

    public:
        template<typename...ARGS>
        const node& insert(const std::string& key, ARGS&&...args)
        {
            node* this_node = &m_root;
            for(std::uint32_t n=0; n!=key.size(); ++n)
            {
                auto iter = this_node->m_children.find(key[n]);
                if (iter != this_node->m_children.end())
                {
                    this_node = &iter->second;
                }
                else
                {
                    this_node = &this_node->m_children[key[n]];
                }
            }
            new (&*(this_node->m_value)) V {std::forward<ARGS>(args)...};
            return *this_node;
        }

        std::optional<V> find(const std::string& key) const noexcept
        {
            const node* this_node = &m_root;
            for(std::uint32_t n=0; n!=key.size(); ++n)
            {
                auto iter = this_node->m_children.find(key[n]);
                if (iter != this_node->m_children.end())
                {
                    this_node = &iter->second;
                }
                else 
                {
                    return std::nullopt;
                }
            }
            return this_node->m_value;
        }
    
    public:
        template<typename F>
        requires std::invocable<F, std::string, V>
        void traverse(F& fct) const noexcept
        {   
            dfs_pre_order_recursive(std::string{}, &m_root, fct);
        }

    private:
        template<typename F>
        requires std::invocable<F, std::string, V>
        void dfs_pre_order_recursive(const std::string& key, const node* this_node, F& fct) const noexcept
        {
            if (this_node->m_value)
            {
                fct(key, *(this_node->m_value));
            }
            for(const auto& [c, next_node]:this_node->m_children)
            {
                dfs_pre_order_recursive(key+c, &next_node, fct);
            }
        }
    
    private:
        node m_root; 
    };
}


