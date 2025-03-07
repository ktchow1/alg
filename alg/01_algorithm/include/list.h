#pragma once
#include<iostream>
#include<cstdint>
#include<vector>
// ******************************************* //
// singly_list            doubly_list
// --------------------+----------------------
//                     |  insert_first_node
// insert_before_head  |  insert_before_head
//                     |  insert_after_tail
// insert_before       |  insert_before
// insert_after        |  insert_after
//                     |  erase_only_node
// erase_head          |  erase_head
//                     |  erase_tail
// erase               |  erase
// get_prev_node       |
// ******************************************* //
// For push and pop, please :
// 1. new or delete node
// 2. update links involved
// 3. update head or tail involved 
// ******************************************* //


namespace alg { namespace list
{
    template<typename T>
    struct node
    {
        template<typename...ARGS>
        node(ARGS...args) : m_value{std::forward<ARGS>(args)...}, m_prev(nullptr), m_next(nullptr)
        {
        }

        T        m_value;
        node<T>* m_prev;
        node<T>* m_next;
    };

    template<template<typename> class LIST, typename T>
    bool compare(const LIST<T>& list, const std::vector<T>& vec) // vec is golden answer
    {
        node<T>* this_node = list.begin();
        for(std::uint32_t n=0; n!=vec.size(); ++n)
        {
            if (this_node == nullptr)         return false;
            if (this_node->m_value != vec[n]) return false;
            this_node = this_node->m_next;
        }
        return true;
    }
}}


namespace alg { namespace list
{
    template<typename T>
    class singly_list
    {
    public:
        using value_type = T;

        template<typename...ARGS>
        auto insert_before_head(ARGS&&...args)
        {
            node<T>* new_node = new node<T>{std::forward<ARGS>(args)...};
            new_node->m_next  = m_head;
            m_head = new_node;
            return new_node;
        }

        template<typename...ARGS>
        auto insert_before(node<T>* this_node, ARGS&&...args)
        {
            if (this_node == m_head) return insert_before_head(std::forward<ARGS>(args)...);

            node<T>*  new_node = new node<T>{std::forward<ARGS>(args)...};
            node<T>* prev_node = get_prev_node(this_node);
             new_node->m_next  = prev_node->m_next;
            prev_node->m_next  =  new_node;
            return new_node;
        }

        template<typename...ARGS>
        auto insert_after(node<T>* this_node, ARGS&&...args)
        {
            node<T>* new_node =  new node<T>{std::forward<ARGS>(args)...};
             new_node->m_next = this_node->m_next;
            this_node->m_next =  new_node;
            return new_node;
        }
        
        void erase_head()
        {
            if (m_head == nullptr) return;

            node<T>* this_node = m_head;
            m_head = m_head->m_next;
            delete this_node;
        }

        void erase(node<T>* this_node)
        {
            if (this_node == m_head) return erase_head();

            node<T>* prev_node = get_prev_node(this_node);
            prev_node->m_next = this_node->m_next;
            delete this_node;
        }

        void reverse()
        {
            if (m_head == nullptr) return;

            node<T>* prev_node = nullptr;   // BUG : if initialize prev_node = m_head
            node<T>* this_node = m_head;    //                     this_node = m_head->m_prev 
            while(this_node != nullptr)     //       the output list will NOT end with nullptr !!!
            {
                node<T>* next_node = this_node->m_next;
                this_node->m_next = prev_node;
                prev_node = this_node;
                this_node = next_node;
            }
            m_head = prev_node;
        }

        node<T>* begin() const noexcept
        {
            return m_head;
        }

        node<T>* end() const noexcept
        {
            return nullptr;
        }    

        bool empty() const noexcept
        {
            return (m_head == nullptr);
        }

    private:
        // Caller is responsible for checking :begin
        // * head node is NOT nullptr
        // * this_node is NOT m_head
        node<T>* get_prev_node(node<T>* this_node) const noexcept
        {
            node<T>* prev_node = m_head;
            while(prev_node->m_next != this_node)
            {
                prev_node = prev_node->m_next;
            }
            return prev_node;
        }

    private:
        node<T>* m_head = nullptr;
    };
}}


namespace alg { namespace list
{
    template<typename T>
    class doubly_list
    {
    public:
        using value_type = T;

        template<typename...ARGS>
        auto insert_first_node(ARGS&&...args)
        {
            node<T>* new_node = new node<T>{std::forward<ARGS>(args)...};
            m_head = new_node;
            m_tail = new_node;
            return new_node;
        }

        template<typename...ARGS>
        auto insert_before_head(ARGS&&...args)
        {
            if (m_head == nullptr) return insert_first_node(std::forward<ARGS>(args)...);

            node<T>* new_node = new node<T>{std::forward<ARGS>(args)...};
            new_node->m_next = m_head;
            m_head->m_prev = new_node;
            m_head = new_node;
            return new_node;
        }

        template<typename...ARGS>
        auto insert_after_tail(ARGS&&...args)
        {
            if (m_head == nullptr) return insert_first_node(std::forward<ARGS>(args)...);

            node<T>* new_node = new node<T>{std::forward<ARGS>(args)...};
            new_node->m_prev = m_tail;
            m_tail->m_next = new_node;
            m_tail = new_node;
            return new_node;
        }

        template<typename...ARGS>
        auto insert_before(node<T>* this_node, ARGS&&...args)
        {
            if (this_node == m_head) return insert_before_head(std::forward<ARGS>(args)...);
            if (this_node == end())  return insert_after_tail(std::forward<ARGS>(args)...);

            node<T>*  new_node = new node<T>{std::forward<ARGS>(args)...};
            node<T>* prev_node = this_node->m_prev;
            new_node->m_prev = prev_node; 
            new_node->m_next = this_node; 
            prev_node->m_next = new_node;
            this_node->m_prev = new_node;
            return new_node;
        }
 
        template<typename...ARGS>
        auto insert_after(node<T>* this_node, ARGS&&...args)
        {
            if (this_node == m_tail) return insert_after_tail(std::forward<ARGS>(args)...);
            if (this_node == rend()) return insert_before_head(std::forward<ARGS>(args)...);

            node<T>* new_node = new node<T>{std::forward<ARGS>(args)...};
            node<T>* next_node = this_node->m_next;
            new_node->m_prev = this_node; 
            new_node->m_next = next_node;
            this_node->m_next = new_node;
            next_node->m_prev = new_node;
            return new_node;
        }

        void erase_only_node()
        {
            if (m_head == nullptr) return;
            if (m_head != m_tail)  return;

            node<T>* this_node = m_head;
            m_head = nullptr;
            m_tail = nullptr;
            delete this_node;
        }

        void erase_head() 
        {
            if (m_head == m_tail) return erase_only_node();

            node<T>* this_node = m_head;
            node<T>* next_node = m_head->m_next;
            next_node->m_prev  = nullptr;
            m_head = next_node;
            delete this_node;
        }

        void erase_tail()
        {
            if (m_head == m_tail) return erase_only_node();

            node<T>* this_node = m_tail;
            node<T>* prev_node = m_tail->m_prev;
            prev_node->m_next  = nullptr;
            m_tail = prev_node;
            delete this_node;
        }

        void erase(node<T>* this_node)
        {
            if (this_node == m_head) return erase_head();
            if (this_node == m_tail) return erase_tail();

            node<T>* prev_node = this_node->m_prev;
            node<T>* next_node = this_node->m_next;
            prev_node->m_next = next_node;
            next_node->m_prev = prev_node;
            delete this_node;
        }

        void reverse()
        {
            node<T>* this_node = m_head;
            while(this_node != nullptr)
            {
                node<T>* next_node = this_node->m_next;
                std::swap(this_node->m_prev, this_node->m_next);
                this_node = next_node;
            }
            std::swap(m_head, m_tail);
        }

        node<T>* begin() const noexcept
        {
            return m_head;
        }

        node<T>* end() const noexcept
        {
            return nullptr;
        }

        node<T>* rbegin() const noexcept
        {
            return m_tail;
        }

        node<T>* rend() const noexcept
        {
            return nullptr;
        }

        bool empty() const noexcept
        {
            return (m_head == m_tail);
        }

    private:
        node<T>* m_head = nullptr;
        node<T>* m_tail = nullptr;
    };
}}


namespace alg { namespace list
{
    template<typename T>
    node<T>* find_nth_node_before_tail(node<T>* this_node, std::uint32_t N)
    {
        node<T>* fwd_node = this_node;
        for(std::uint32_t n=0; n!=N; ++n)
        {
            if (fwd_node->m_next == nullptr) return nullptr;
            fwd_node = fwd_node->m_next;
        }

        while(fwd_node->m_next != nullptr)
        {
            this_node = this_node->m_next;
             fwd_node =  fwd_node->m_next;
        }
        return this_node;
    }
}}


namespace alg { namespace list 
{
    template<typename T>
    class loop_detection
    {
    public:
        loop_detection(const node<T>* head) : m_head(head), m_meet(nullptr)
        {
        }

        bool is_looped() 
        {
            if (m_head == nullptr) return false;
            const node<T>* slow = m_head;
            const node<T>* fast = m_head;

            while(true)
            {
                slow = slow->m_next;
                fast = fast->m_next;
                if (fast == nullptr) return false;
                fast = fast->m_next;
                
                if (slow == nullptr) return false;
                if (fast == nullptr) return false;
                if (slow == fast)  
                {
                    m_meet = slow;
                    return true;
                }
            }
        }

        std::uint32_t loop_length()
        {
            const node<T>* this_node = m_meet;
            std::uint32_t length = 0;

            while(this_node != nullptr)
            {
                this_node = this_node->m_next;
                ++length;
            
                if (this_node == m_meet) return length;
            }
            return 0;
        }

        
        // H*****E******
        //       N     *
        //       *     *
        //       *     *
        //       **M****
        //
        // H  = head
        // M  = meet
        // E  = entry
        // N1 = list.size()-1
        // 
        // (M-H) * 2 = (M-H) + loop_length
        //           = (M-H) + ((N-M) + (M-E) + 1)
        //           = (M-H) + ((N-M) + (M-H) - (E-H) + 1)
        //       E-H = N-M+1

        const node<T>* loop_entry_node()
        {
            const node<T>* node0 = m_head;
            const node<T>* node1 = m_meet;

            while(node0!=nullptr && node1!=nullptr)
            {
                if (node0 == node1) return node0;
                node0 = node0->m_next;
                node1 = node1->m_next;
            }
            return nullptr;
        }

    private:
        const node<T>* m_head;
        const node<T>* m_meet;
    };
}}
