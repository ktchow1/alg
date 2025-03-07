#pragma once
#include<cstdint>
#include<functional>
#include<variant>


namespace alg
{
    // ******************************************************** //
    // *** Wrappers supporting CTAD without explicity guide *** //
    // ******************************************************** //
    template<typename FCT> 
    struct filter    
    { 
        FCT value; 
    };

    template<typename FCT> 
    struct transform 
    { 
        FCT value; 
    };
    
    struct take      
    { 
        std::uint32_t value; 
    };


    // ***************** //
    // *** lazy_view *** //
    // ***************** //
    template<typename C> // C = container
    struct lazy_view
    {
    public:
        using              T = typename C::value_type;
        using    filter_type = std::function<bool(T)>;
        using transform_type = std::function<T(T)>;

        struct take_type
        { 
            // Todo : putting m_count here is not good, since :
            //
            // * m_count is made mutable because of remark 1A
            // * contruct std::vector from alg::lazy_view once
            // * contruct std::vector from alg::lazy_view again
            // * the 2 vectors will be different, which does not make sense
            // * the 2nd vector is empty, as m_count == m_limit 
            //
            // solution : lazy_view_iterator owns a copy of lazy_view, see remark 1B 
            //
            mutable std::uint32_t m_count;
            const   std::uint32_t m_limit;  
        }; 

        using logic = std::variant<filter_type, transform_type, take_type>;
        friend class lazy_view_iterator;


        // ************************** //
        // *** lazy_view_iterator *** //
        // ************************** //
        class lazy_view_iterator
        {
        public:
            lazy_view_iterator(const lazy_view& view, const C::iterator& iter) : m_view(view), m_iter(iter)
            {
                if (!is_valid()) next_valid(); 
            }

        public: 
            void operator++()
            {
                next_valid();
            }

            bool operator==(const lazy_view_iterator& rhs) const
            {
                return m_iter == rhs.m_iter;
            }

            const T& operator*() const
            {
                return m_value;
            }

        private:
            bool is_valid() 
            {
                if (m_iter == m_view.m_end) return true;

                m_value = *m_iter;
                return apply_logic();
            }

            void next_valid()
            {
                while(true)
                {
                    if (m_iter == m_view.m_end) return;

                    ++m_iter;
                    m_value = *m_iter;
                    if (apply_logic()) return;
                }
            }

            // ******************************************************//
            // return  true : caller can stop iterating
            // return false : caller needs to iterate to next element 
            // ******************************************************//
            bool apply_logic()
            {
                for(auto& logic:m_view.m_logics)
                { 
                    if (logic.index() == 0)
                    {
                        if (!std::get<0>(logic)(m_value)) 
                        {
                            return false;
                        }
                    }
                    else if (logic.index() == 1)
                    {
                        m_value = std::get<1>(logic)(m_value);
                    }
                    else if (logic.index() == 2)
                    {
                        if (std::get<2>(logic).m_count >= 
                            std::get<2>(logic).m_limit)
                        {
                            m_iter = m_view.m_end;
                            return true; 
                        }
                        ++std::get<2>(logic).m_count; // Remark 1A
                    }
                } 
                return true;
            }

        private:
        //  const lazy_view& m_view; // Remark 1B
            lazy_view        m_view;
            C::iterator      m_iter;
            T                m_value;
        };

    public:
        lazy_view(C& container) : m_begin(container.begin()), m_end(container.end()), m_logics{}
        {
        }

        // *********************************************** //
        // *** Casting operator - with lazy evaluation *** //
        // *********************************************** //
        operator C()
        {
            C output;
            for(auto iter=begin(); iter!=end(); ++iter)
            {
                output.push_back(*iter);
            }
            return output;
        }

        auto begin() const 
        {
            return lazy_view_iterator(*this, m_begin);
        }

        auto end() const 
        {
            return lazy_view_iterator(*this, m_end);
        }

        void add_logic(const logic& x)
        {
            m_logics.push_back(x);
        }

    private:
        C::iterator        m_begin;
        C::iterator        m_end;    
        std::vector<logic> m_logics;
    };


    // *********************************** //
    // *** Pipe for creating lazy_view *** //
    // *********************************** //
    template<typename C, typename FCT>
    lazy_view<C> operator | (C& lhs, const filter<FCT>& rhs)
    {
        lazy_view<C> output(lhs);
        output.add_logic(typename lazy_view<C>::filter_type{rhs.value}); 
        return output;
    }

    template<typename C, typename FCT>
    lazy_view<C> operator | (C& lhs, const transform<FCT>& rhs)
    {
        lazy_view<C> output(lhs);
        output.add_logic(typename lazy_view<C>::transform_type{rhs.value}); 
        return output;
    }

    template<typename C>
    lazy_view<C> operator | (C& lhs, const take& rhs)
    {
        lazy_view<C> output(lhs);
        output.add_logic(typename lazy_view<C>::take_type{0, rhs.value}); 
        return output;
    }


    // ************************** //
    // *** Pipe for cascading *** //
    // ************************** //
    template<typename C, typename FCT>
    lazy_view<C> operator | (lazy_view<C>&& lhs, const filter<FCT>& rhs) // Is "lhs" universal reference or rvalue reference?
    {
        lhs.add_logic(typename lazy_view<C>::filter_type{rhs.value}); 
        return lhs;
    }

    template<typename C, typename FCT>
    lazy_view<C> operator | (lazy_view<C>&& lhs, const transform<FCT>& rhs)
    {
        lhs.add_logic(typename lazy_view<C>::transform_type{rhs.value}); 
        return lhs;
    }

    template<typename C>
    lazy_view<C> operator | (lazy_view<C>&& lhs, const take& rhs)
    {
        lhs.add_logic(typename lazy_view<C>::take_type{0, rhs.value});
        return lhs;
    }
}

