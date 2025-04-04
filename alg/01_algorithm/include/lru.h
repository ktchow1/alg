#pragma once
#include<cstdint>
#include<vector>
#include<list>
#include<map>
#include<optional>


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
    template<typename K, std::uint32_t N>
    class lru_list
    {
    public:
        // *****************************************************//
        // Create node whenever we add,   return by iterator
        // Delete node whenever its full, return by optional<K>
        // *****************************************************//
        std::pair<typename std::list<K>::iterator, std::optional<K>> add(const K& key) 
        {
            auto add_iter = m_list.insert(m_list.begin(), key);

            std::optional<K> del_key;
            if (m_list.size() > N)
            {
                auto del_iter = --m_list.end();
                del_key = *del_iter;
                m_list.erase(del_iter);
            }
            return std::make_pair(add_iter, del_key);
        }

        void update(const typename std::list<K>::iterator& iter)
        {
            m_list.splice(m_list.begin(), m_list, iter); // move "iter" of "m_list" to the front
        }

    public: 
        // ************************************************ //
        // *** For testing only, peeking without change *** //
        // ************************************************ //
        const auto& peek() const 
        {
            return m_list;
        }

    private:
        std::list<K> m_list;
    };


    template<typename K, typename V, std::uint32_t N>
    class lru_map
    {
    public:
        void add(const K& key, const V& value)
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.update(map_iter->second.second);
                map_iter->second.first = value;
            }
            else
            {
                auto [add_iter, del_key] = m_list.add(key);
                m_map[key] = std::make_pair(value, add_iter);
                
                // ************************ //
                // *** Delete stale key *** //
                // ************************ //
                if (del_key)
                {
                    m_map.erase(*del_key);
                }
            }
        }

        std::optional<V> get(const K& key) const
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.update(map_iter->second.second);
                return std::make_optional(map_iter->second.first);
            }
            else
            {
                return std::nullopt;
            }
        }

    public: 
        // ************************************************ //
        // *** For testing only, peeking without change *** //
        // ************************************************ //
        auto peek() const 
        {
            std::vector<std::pair<K,V>> ans;
            for(const auto& key : m_list.peek())
            {
                auto map_iter = m_map.find(key);
                if (map_iter != m_map.end())
                {
                    ans.push_back(std::make_pair(key, map_iter->second.first));
                }
                else
                {
                    const V value{};
                    ans.push_back(std::make_pair(key, value));
                }
            }
            return ans;
        }

    private:
        std::map<K, std::pair<V, typename std::list<K>::iterator>> m_map;
        mutable lru_list<K,N> m_list;
    };
}



// *************************************************************************************** //
// Value indexed map 
//
// * this is a map<K,V> that allows
//   O(logN) search of key
//   O(logN) search of value
//   iteratible in key 
//   iteratible in value
// * K to V is one to many mapping
//   V to K is many to one mapping
// * this container is generalized from "Highest volumn stock problem" provided by Ilia
// 
// *************************************************************************************** //
// 1. Unlike LRU, which maintains sorted simply by putting latest item at the front,
//    value_indexed_map maintains sorted by std::multi_map, hence dual map implementation.
// 2. Unlike LRU, which needs to delete existing items when overflow, 
//    value_indexed_map does not delete existing items, hence simpler.
//
// *************************************************************************************** //
namespace alg
{
/*  template<typename K, typename V> 
    class value_indexed_map
    {
    public:
        void add(const K& key, const V& value)
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.update(map_iter->second.second);
                map_iter->second.first = value;
            }
            else
            {
                auto [add_iter, del_key] = m_list.add(key);
                m_map[key] = std::make_pair(value, add_iter);
                
                // ************************ //
                // *** Delete stale key *** //
                // ************************ //
                if (del_key)
                {
                    m_map.erase(*del_key);
                }
            }
        }

        std::optional<V> get_with_key(const K& key) const
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.update(map_iter->second.second);
                return std::make_optional(map_iter->second.first);
            }
            else
            {
                return std::nullopt;
            }
        }

        std::optional<K> get_with_value(const K& key) const
        {
        }

    public: 
        // ************************************************ //
        // *** For testing only, peeking without change *** //
        // ************************************************ //
        std::vector<std::pair<K,V>> peek() const 
        {
            std::vector<std::pair<K,V>> ans;
            for(const auto& key : m_list.peek())
            {
                auto map_iter = m_map.find(key);
                if (map_iter != m_map.end())
                {
                    ans.push_back(std::make_pair(key, map_iter->second.first));
                }
                else
                {
                    const V value{};
                    ans.push_back(std::make_pair(key, value));
                }
            }
            return ans;
        }

    private:
        std::multi_map<V,K> m_sec_map;                                 // secondary map : V to K
        std::map<K, typename decltyp(m_sec_map)::iterator>> m_pri_map; //   primary map : K to V
    }; */
}
