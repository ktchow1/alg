#pragma once
#include<cstdint>
#include<vector>
#include<list>
#include<map>
#include<optional>
#include<ranges>


// ********************************************************************************* //
// Approach 1 
// * std::unordered_map<K, std::pair<V, std::list<K>::iterator> 
// * std::list<K>
//
// Approach 2
// * std::unordered_map<K, std::list<std::pair<K,V>>::iterator> 
// * std::list<std::pair<K,V>>
//
// That is, we can put V in either container. Both approaches are ok :
// * given an iter in  map, we can get its list position instantly
// * given an iter in list, we can get its  map position in O(1) 
// * approach 2 is implemented in the below, to be consistent with value_indexed_map
//
// ********************************************************************************* //
// Consideration about selecting STL container :
//
// Remark 1 
// * There is no function for modifying KEY in std::map or std::set.
// * Do not include recent_used_timestamp or version in KEY.
//
// Remark 2 
// * Can we use std::priority_queue instead of list for tracking recent-used-item? 
// * No, because :
//   (a) sorting in LRU is simple, always erase in middle and insert at the front
//   (b) we cannot change an item in the middle of priority_queue and keep it sorted 
//   (c) we cannot erase  an item in the middle of priority_queue and re-insert
//       popping is possile from top only
//        
// Remark 3 
// * Can we use std::map instead of list for tracking recent-used-item? 
// * No, because of (a) above
//
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
            // *** case 1 : amend item *** //
            // *************************** //
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.splice(m_list.begin(), m_list, map_iter->second); // move list iterator to the front
                map_iter->second->second = value;
            }
            // ************************* //
            // *** case 2 : new item *** //
            // ************************* //
            else
            {
                if (m_list.size() == N)
                {
                    m_map.erase(m_list.back().first);
                    m_list.pop_back();
                }
                m_map.emplace(key, m_list.emplace(m_list.begin(), std::make_pair(key, value)));
            }
        }

        std::optional<V> get(const K& key) const
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.splice(m_list.begin(), m_list, map_iter->second); // move list iterator to the front
                return std::make_optional(map_iter->second->second);
            }
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



// *************************************************************************************** //
// Value indexed map 
// *************************************************************************************** //
// This is inspired by Ilia's question "Highest volumn stock problem" :
// * map that can accept (symbol, price)
// * map that can be queried with symbol
// * map that can be queried wuth top N price 
//
//
// This is a map<K,V> that allows :
// * O(logN) search of key
// * O(logN) search of value
// * iteratible in key   (sorted in increasing order)
// * iteratible in value (sorted in decreasing order)
// * K to V is one to many mapping
// * V to K is many to one mapping
// 
// *************************************************************************************** //
// 1. Unlike LRU, which maintains sorted simply by putting latest item at the front,
//    value_indexed_map maintains sorted by std::multi_map, hence dual map implementation.
// 2. Unlike LRU, which needs to delete existing items when overflow, 
//    value_indexed_map does not delete existing items, hence simpler.
//
// Therefore :
// * least recent used map = map + list
// * value indexed map     = map + multimap
// *************************************************************************************** //
namespace alg
{
    template<typename K, typename V> 
    class value_indexed_map
    {
    public:
        void set(const K& key, const V& value) // key = symbol, value = price
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
            //  map_iter->second->first = value; // BUG : KEY of map1 cannot be modified, need to erase and re-insert
                
                m_sort.erase(map_iter->second); // avoid redundant map-search here, we store iterator in m_map
                map_iter->second = m_sort.emplace(value, key);
            }
            else
            {
                m_map[key] = m_sort.emplace(value, key);
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
        
        std::vector<std::pair<K,V>> get_top(std::uint32_t N) const
        {
            std::vector<std::pair<K,V>> ans;
            for(auto [value, key] : m_sort | std::views::take(N))
            {
                ans.emplace_back(key, value);
            }
            return ans;
        } 
  
    private:
        std::multimap<V,K,std::greater<>> m_sort;  
        std::map<K,typename decltype(m_sort)::iterator> m_map; 
    }; 
}
