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
    template<typename K, typename V, std::uint32_t N>
    class lru_list
    {
    public:
        // **************************************************************** //
        // Create node whenever we set,   return insert-node by iterator
        // Delete node whenever its full, return delete-node by optional<K>
        // **************************************************************** //
        std::pair<typename std::list<std::pair<K,V>>::iterator, std::optional<K>> set(const K& key, const V& value) 
        {
            auto add_iter = m_list.emplace(m_list.begin(), key, value);

            std::optional<K> del_key;
            if (m_list.size() > N)
            {
                auto del_iter = --m_list.end();
                del_key = del_iter->first;
                m_list.erase(del_iter);
            }
            return std::make_pair(add_iter, del_key);
        }

        void update(const typename std::list<std::pair<K,V>>::iterator& iter)
        {
            m_list.splice(m_list.begin(), m_list, iter); // move "iter" to the front
        }

    public: 
        const auto& peek() const // for testing only
        {
            return m_list;
        }

    private:
        std::list<std::pair<K,V>> m_list;
    };


    template<typename K, typename V, std::uint32_t N>
    class lru_map
    {
    public:
        void set(const K& key, const V& value)
        {
            auto map_iter = m_map.find(key);
            if (map_iter != m_map.end())
            {
                m_list.update(map_iter->second);
                map_iter->second->second = value;
            }
            else
            {
                auto [add_iter, del_key] = m_list.set(key, value);
                m_map[key] = add_iter;
                
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
                m_list.update(map_iter->second);
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
            return m_list.peek();
        }

    private:
        std::map<K, typename std::list<std::pair<K,V>>::iterator> m_map;
        mutable lru_list<K,V,N> m_list;
    };
}



// *************************************************************************************** //
// Value indexed map 
//
// * this is a map<K,V> that allows
//   O(logN) search of key
//   O(logN) search of value
//   iteratible in key   (sorted in increasing order)
//   iteratible in value (sorted in decreasing order)
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
        void set(const K& key, const V& value)
        {
            auto map0_iter = m_map0.find(key);
            if (map0_iter != m_map0.end())
            {
            //  map0_iter->second->first = value; // BUG : KEY cannot be modified, need to erase and re-insert
                
                m_map1.erase(map0_iter->second);
                map0_iter->second = m_map1.emplace(value, key);
            }
            else
            {
                m_map0[key] = m_map1.emplace(value, key);
            } 
        }
  
        std::optional<V> get_with_key(const K& key) const
        {
            auto map0_iter = m_map0.find(key);
            if (map0_iter != m_map0.end())
            {
                return std::make_optional(map0_iter->second.first);
            }
            else
            {
                return std::nullopt;
            }
        }

        // Todo : return all keys with same value
        std::optional<K> get_with_value(const V& value) const
        {
            auto map1_iter = m_map1.find(value);
            if (map1_iter != m_map1.end())
            {
                return std::make_optional(map1_iter->second);
            }
            else
            {
                return std::nullopt;
            }
        }
        
        /*
    public:
        // Todo
        std::optional<std::pair<K.V>> get_top() const
        {
        }

        // Apply range-lib here
        std::vector<std::pair<K.V>> get_top_N(std::uint32_t N) const
        {
        } */
  
    private:
        std::multimap<V,K,std::greater<>> m_map1;                // secondary map : V to K (note : use comparator)
        std::map<K, typename decltype(m_map1)::iterator> m_map0; //   primary map : K to V (note : use decltype to reduce code)
    }; 
}
