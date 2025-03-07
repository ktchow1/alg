#pragma once
#include<cstdint>
#include<limits>
#include<stack>
#include<queue>


namespace alg
{
    template<typename C0, typename C1>
    requires std::is_same_v<typename C0::value_type, typename C1::value_type>
    class container_comparator
    {
    public:
        // containers are constructed internally
        container_comparator()
            : m_container_ptr0{new C0}, 
              m_container_ptr1{new C1},
              m_allocate(true)
        {
        }

        // containers are provided by user
        container_comparator(C0& container0, C1& container1)
            : m_container_ptr0{&container0},
              m_container_ptr1{&container1},
              m_allocate(false)
        {
        }

        ~container_comparator()
        {
            if (m_allocate)
            {
                delete m_container_ptr0;
                delete m_container_ptr1;
            }
        }

        // ******************************************** //
        // *** Pass member pointer as concrete type *** //
        // ******************************************** //
        void apply0(void (C0::* fp0)(), 
                    void (C1::* fp1)())
        {
            (m_container_ptr0->*fp0)();
            (m_container_ptr1->*fp1)();
        } 

        template<typename T>
        void apply0(void (C0::* fp0)(const T&), 
                    void (C1::* fp1)(const T&),
                    const T& x)
        {
            (m_container_ptr0->*fp0)(x); // BUG : Dont forget the bracket (mem.*fp)(x)
            (m_container_ptr1->*fp1)(x);
        }

        // ******************************************** //
        // *** Pass member pointer as template type *** //
        // ******************************************** //
        template<typename FP0, typename FP1>
        void apply1(FP0 fp0, FP1 fp1)
        {
            (m_container_ptr0->*fp0)();
            (m_container_ptr1->*fp1)();
        } 
        
        template<typename FP0, typename FP1, typename T>
        void apply1(FP0 fp0, FP1 fp1, const T& x)
        {
            (m_container_ptr0->*fp0)(x);
            (m_container_ptr1->*fp1)(x);
        } 

        template<typename FP0, typename FP1>
        bool compare(FP0 fp0, FP1 fp1)
        {
            if (m_container_ptr0->size() != m_container_ptr1->size()) return false;
            while (!m_container_ptr0->empty())
            {
                if (m_container_ptr1->empty()) return false;
                if((m_container_ptr0->*fp0)() !=
                   (m_container_ptr1->*fp1)())
                {
                    return false;
                }
                m_container_ptr0->pop();
                m_container_ptr1->pop();
            }
            if (!m_container_ptr1->empty()) return false;
            return true;
        }
        
        bool empty() const noexcept
        {
            return m_container_ptr0->empty() ||
                   m_container_ptr1->empty();
        }

    private:
        C0* m_container_ptr0;
        C1* m_container_ptr1;
        bool m_allocate;
    };

    template<typename C> 
    requires std::is_same_v<typename C::value_type, std::int32_t>
    class stack_with_min_comparator
    {
    public:
        void push(std::int32_t x)
        {
            m_stack0_value.push(x);
            if (m_stack0_min.empty()) 
            {
                m_stack0_min.push(x);
            }
            else
            {
                m_stack0_min.push(std::min(m_stack0_min.top(), x));
            }
            m_stack1.push(x);
        }

        void pop()
        {
            m_stack0_value.pop();
            m_stack0_min.pop();
            m_stack1.pop();
        }

        bool compare() const noexcept
        {
            bool flag = true;
            if (m_stack0_value.size() != m_stack1.size()) flag = false;
            if (m_stack0_value.size() > 0)
            {
                if (m_stack0_value.top() != m_stack1.top()) flag = false;
                if (m_stack0_min  .top() != m_stack1.min()) flag = false;
            }
  
        //  print(flag);
            ++m_count; 
            return flag;
        }

        bool empty() const noexcept
        {
            return m_stack0_value.empty() ||
                   m_stack0_min.empty() ||
                   m_stack1.empty();
        }

        void clear()
        {
            while(!m_stack0_value.empty()) m_stack0_value.pop();
            while(!m_stack0_min  .empty()) m_stack0_min  .pop();
            m_stack1.clear();
        }

    private:
        void print(bool flag) const noexcept
        {
            if (!m_stack0_value.empty()) 
            {
                std::cout << "\ntest " << m_count 
                          << ", size=" << m_stack0_value.size() << "/" << m_stack1.size()
                          << ", top="  << m_stack0_value.top()  << "/" << m_stack1.top()
                          << ", min="  << m_stack0_min  .top()  << "/" << m_stack1.min()
                          << (flag? " [OK]":" [ERROR]") << std::flush;
            }
            else
            {
                std::cout << "\ntest " << m_count 
                          << ", size=" << m_stack0_value.size() << "/" << m_stack1.size()
                          << (flag? " [OK]":" [ERROR]") << std::flush;
            }
        }

    private:
        mutable std::uint32_t    m_count = 0;
        std::stack<std::int32_t> m_stack0_value;
        std::stack<std::int32_t> m_stack0_min;
        C                        m_stack1;
    };
}

namespace alg
{
    template<typename T>
    class queue
    {
    public:
        using value_type = T;

        void push(const T& x)
        {
            m_stack0.push(x);
        }

        void pop()
        {
            migrate();
            m_stack1.pop();
        }

        const T& front() const noexcept
        {
            migrate();
            return m_stack1.top();
        }

        std::uint32_t size() const noexcept
        {
            return m_stack0.size() + m_stack1.size();
        }

        bool empty() const noexcept
        {
            return m_stack0.empty() && m_stack1.empty();
        }

        void clear()
        {
            m_stack0.clear();
            m_stack1.clear();
        }

    private:
        void migrate() const noexcept
        {
            if (m_stack1.empty())
            {
                while(!m_stack0.empty())
                {
                    m_stack1.push(m_stack0.top());
                    m_stack0.pop();
                }
            }
        }

    private:
        mutable std::stack<T> m_stack0;  // always for pushing
        mutable std::stack<T> m_stack1;  // always for popping
    };
}

namespace alg
{
    template<typename T>
    class stack
    {
    public:
        using value_type = T;

        void push(const T& x)
        {
            if (m_active_queue0)
            {
                m_queue0.push(x);
            }
            else
            {
                m_queue1.push(x);
            }
        }

        void pop()
        {
            migrate();
            if (m_active_queue0) 
            {
                m_queue0.pop();
                m_active_queue0 = false;
            }
            else 
            {
                m_queue1.pop();
                m_active_queue0 = true;
            }
        }

        const T& top() const noexcept
        {
            migrate();
            if (m_active_queue0)
            {
                return m_queue0.front();
            }
            else 
            {
                return m_queue1.front();
            }
        }

        std::uint32_t size() const noexcept
        {
            return m_queue0.size() + m_queue1.size();
        }

        bool empty() const noexcept
        {
            return m_queue0.empty() && m_queue1.empty();
        }

        void clear()
        {
            m_active_queue0 = true;
            m_queue0.clear();
            m_queue1.clear();
        }

    private:
        void migrate() const noexcept
        {
            if (m_active_queue0)
            {
                while(m_queue0.size() > 1)
                {
                    m_queue1.push(m_queue0.front());
                    m_queue0.pop();
                }
            }
            else
            {
                while(m_queue1.size() > 1)
                {
                    m_queue0.push(m_queue1.front());
                    m_queue1.pop();
                }
            }
        }

    private:
        mutable bool m_active_queue0 = true;
        mutable std::queue<T> m_queue0; // should contains >= 1 item after migrating to queue 1
        mutable std::queue<T> m_queue1; // should contains >= 1 item after migrating to queue 0
    };
}

namespace alg { namespace obj_pool
{
    template<typename T>
    struct node
    {
        T        m_value;
        node<T>* m_next = nullptr;
    }; 

    template<typename T>
    class obj_pool
    {
    public:
        obj_pool(const std::uint32_t size) : m_impl(size)
        {
            m_unused_head = &m_impl[0];
            for(std::uint32_t n=0; n!=size-1; ++n)
            {
                m_impl[n].m_next = &m_impl[n+1];
            } 
            m_impl[size-1].m_next = nullptr;
        }

        template<typename...ARGS>
        node<T>* request(ARGS&&...args)
        {
            if (m_unused_head == nullptr) return nullptr;

            // update link
            node<T>* new_node = m_unused_head;
            m_unused_head = m_unused_head->m_next;

            // update node
            new (&(new_node->m_value)) T {std::forward<ARGS>(args)...};
            new_node->m_next = nullptr;
            return new_node; 
        }

        void release(node<T>* del_node) 
        {
            if (del_node == nullptr) return;

            // update node
            del_node->m_value.~T();

            // update link
            del_node->m_next = m_unused_head;
            m_unused_head = del_node;
        }

    private:
        std::vector<node<T>> m_impl;
        node<T>* m_unused_head;
    };

    template<typename T>
    class stack
    {
    public:
        using value_type = T;

        stack(obj_pool<T>& pool) : m_pool(pool), m_head(nullptr), m_size(0)
        {
        }

        template<typename...ARGS>
        void push(ARGS&&...args)
        {
            auto new_node = m_pool.request(std::forward<ARGS>(args)...);
            if (new_node)
            {
                new_node->m_next = m_head;
                m_head = new_node;
                ++m_size;
            }
        }

        void pop()
        {
            if (m_head)
            {
                auto del_node = m_head;
                m_head = m_head->m_next;
                --m_size;

                m_pool.release(del_node);
            }
        }

        const T& top() const noexcept
        {
            return m_head->m_value;
        }

        std::uint32_t size() const noexcept
        {
            return m_size;
        }

        bool empty() const noexcept
        {
            return (m_head == nullptr);
        }

        void clear()
        {
            while(!empty()) pop();
        }

    private:
        obj_pool<T>&  m_pool;
        node<T>*      m_head;
        std::uint32_t m_size;
    };
}}

namespace alg
{
    class stack_with_min // for signed int only
    {
    public:
        using value_type = std::int32_t;

        void push(std::int32_t x)
        {
            if (m_impl.empty())
            {
                m_impl.push(x);
                m_min = x;
            }
            else if (x < m_min)
            {
                m_impl.push(x - (m_min-x));
                m_min = x;
            }
            else
            {
                m_impl.push(x);
            }
        }

        void pop()
        {
            if (m_impl.top() < m_min)
            {
                m_min = m_min + (m_min-m_impl.top());
                m_impl.pop();
            }
            else
            {
                m_impl.pop();
            }
        }

        std::int32_t top() const noexcept
        {
            if (m_impl.top() < m_min)
            {
                return m_min;
            }
            else
            {
                return m_impl.top();
            }
        }
        
        std::int32_t min() const noexcept
        {
            return m_min;
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
            while(!empty()) pop();
        }

    private:
        std::stack<std::int32_t> m_impl;
        std::int32_t m_min;
    };
}
