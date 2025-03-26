#pragma once
#include<cstdint>
#include<vector>
#include<list>
#include<map>
#include<optional>


// ******************************************************************* //
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
// * approach 1 is implemented in the below
//
// If lru_map supports delete, then lru_list will be more complicated,
// as it needs to store the whole history as well.
// ******************************************************************* //

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

    public: // For testing, peek without changing lru list
        std::list<K> peek() const 
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

    public: // For testing, peek without changing lru list
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
        std::map<K, std::pair<V, typename std::list<K>::iterator>> m_map;
        mutable lru_list<K,N> m_list;
    };
}
