#pragma once
#include<cstdint>
#include<vector>
#include<list>
#include<map>
#include<optional>
#include<ranges>


// ********************************************************************************* //
// LRU map
// ********************************************************************************* //
// Approach 1 
// * std::unordered_map<K, std::pair<V, std::list<K>::iterator> 
// * std::list<K>
//
//
// Approach 2
// * std::unordered_map<K, std::list<std::pair<K,V>>::iterator> 
// * std::list<std::pair<K,V>>
//
//
// That is, we can put V in either container. Both approaches are ok :
// * given an iter in  map, we can get its list position instantly
// * given an iter in list, we can get its  map position in O(1) 
// * approach 2 is implemented in the below, to be consistent with value_indexed_map
// ********************************************************************************* //
namespace alg
{
    template<typename K, typename V, std::uint32_t N>
    class lru_map
    {
    public:
        void set(const K& key, const V& value)
        {
            // *************************** //
            // *** Exist - update time *** //
            // *************************** //
            if (auto map_iter = m_map.find(key); map_iter != m_map.end())
            {
                m_list.splice(m_list.begin(), m_list, map_iter->second); // move list iterator to the front
                map_iter->second->second = value;
            }
            // ********************************* //
            // *** Not exist - add and erase *** //
            // ********************************* //
            else
            {
                if (m_list.size() == N)
                {
                    m_map.erase(m_list.back().first);
                    m_list.pop_back();
                }
                m_map.emplace(key, m_list.emplace(m_list.begin(), key, value));
            }
        }

        std::optional<V> get(const K& key) const
        {
            // *************************** //
            // *** Exist - update time *** //
            // *************************** //
            if (auto map_iter = m_map.find(key); map_iter != m_map.end())
            {
                m_list.splice(m_list.begin(), m_list, map_iter->second); // move list iterator to the front
                return std::make_optional(map_iter->second->second);
            }
            // ******************************* //
            // *** Not exist - return null *** //
            // ******************************* //
            else
            {
                return std::nullopt;
            }
        }

    public: 
        auto peek() const // for testing only
        {
            return m_list;
        }

    private:
        mutable std::list<std::pair<K,V>> m_list; // new items in the front, old items at the back
        std::map<K, typename decltype(m_list)::iterator> m_map;
    };
}



// ********************************************************************** //
// Value indexed map 
// ********************************************************************** //
// This is inspired by Ilia's question "Highest volumn stock problem" :
// * map that can accept (symbol, price)
// * map that can be queried with symbol
// * map that can be queried with top N price 
//
//
// This is dual map implemenatation, that allows :
// * O(logN) search of key
// * O(logN) search of value
// * iteratible in key   (sorted in increasing order)
// * iteratible in value (sorted in decreasing order)
// * K to V is one to many mapping
// * V to K is many to one mapping
// 
//
// Therefore :
// * least recent used map = map + list
// * value indexed map     = map + multimap
// ********************************************************************** //
namespace alg
{
    template<typename K, typename V> 
    class value_indexed_map
    {
    public:
        void set(const K& key, const V& value) // key = symbol, value = price
        {
            if (auto map_iter = m_map.find(key); map_iter != m_map.end())
            {
            //  map_iter->second->first = value; // BUG : KEY of map1 cannot be modified, need to erase and re-insert
                
                m_sort.erase(map_iter->second); // avoid redundant map-search here, we store iterator in m_map
                map_iter->second = m_sort.emplace(value, key);
            }
            else
            {
                m_map.emplace(key, m_sort.emplace(value, key));
            } 
        }
  
    public:
        std::optional<V> get_by_key(const K& key) const
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                return std::make_optional(map_iter->second->first);
            }
            else
            {
                return std::nullopt;
            }
        }

        auto get_by_value(const V& value) const
        {
            std::vector<K> ans;
            auto range = m_sort.equal_range(value);
            for(auto iter=range.first; iter!=range.second; ++iter)
            {
                ans.push_back(iter->second);
            }
            return ans;
        }
        
        auto get_top(std::uint32_t N) const
        {
            std::vector<std::pair<K,V>> ans;
            for(const auto& [value, key] : m_sort | std::views::take(N))
            {
                ans.emplace_back(key, value);
            }
            return ans;
        } 
  
    private:
        std::multimap<V,K,std::greater<V>> m_sort;  
        std::map<K,typename decltype(m_sort)::iterator> m_map; 
    }; 
}


// ******************************************************************************************************* //
// About STL container :
//
// std::map
// * cannot modify key
// * erase(key)                <--- does     involve binary search
// * erase(iterator)           <--- does not involve binary search
// * emplace(key, value)       <--- construct of K-V pair and copy of V (if K exists, support V overwrite)
// * map[key] = value          <--- construct of K-V pair               (if K exists,      no V overwrite)
//
//
// std::list
// * use of emplace(iterator)  <--- does     return new iterator
// * use of emplace_back       <--- does not return new iterator
// * use of splice
//
//
// std::priority_queue
// * cannot use priority_queue instead of list in LRU, because : 
//   cannot modify value in the middle and keep it sorted
//
//
// std::multimap
// * use of equal_range
// * use of lower_bound (in other tests)
// * use of upper_bound (in other tests)
// * use of std::views::take(N)
// ******************************************************************************************************* //
