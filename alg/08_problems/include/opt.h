#include<iostream>
#include<sstream>
#include<cstdint>
#include<string>
#include<array>
#include<queue>
#include<unordered_set>
#include<unordered_map>


// ******************************************************************************************************* //
// Root is the ultimate parent of all nodes.
// ******************************************************************************************************* //
// Error codes in order of priority :
// E1 = incorrect string format
// E2 = duplicated edge
// E3 = a node having more than two children
// E4 = a tree not covering all nodes (i.e. multi-trees or multi-roots)
// E5 = a tree having cycles or 
//      a node having multi-parents
// ******************************************************************************************************* //
// Hint E4
// 1. if the node-set consists of multi-separated  -trees, then there is E4 only (no E5d)
// 2. if the node-set consists of multi-overlapping-trees, then there are both E4,E5d
// ******************************************************************************************************* //
// Hint for E5 
// 1. cycle != multi-parents, although they are overlapping sets
// 2. cycle means a node having its parent / grandparent as child
//
// Lets classify E5 into 4 cases ... 
// E5a : an edge from a node to the root                 : forming cycle, with no multi-parents
// E5b : an edge from a node to its parent / grandparent : forming cycle, with    multi-parents
// E5c : an edge from a node to another branch           :      no cycle, with    multi-parents
// E5d : an edge from a node to another tree             :      no cycle, eith    multi-parents, return E4
// ...   hence E5d always comes together with E4.
// ******************************************************************************************************* //
namespace opt
{
    class tree_checker
    {
    public:
        tree_checker() : root(empty)
        {
            for(auto& x:errors) x = false;
        }

        void debug() const
        {
            std::cout << "\n[tree]";
            for(const auto& x:tree)
            {
                std::cout << "\n" << x.first << " : ";
                std::cout << " parents=[";
                for(const auto& y:x.second.parents) std::cout << y;
                std::cout << "] children=[";
                for(const auto& y:x.second.children) std::cout << y;
                std::cout << "] label = " << x.second.label;
                std::cout << ", lhs = " << x.second.lhs_child;
                std::cout << ", rhs = " << x.second.rhs_child;
            }

            std::cout << "\nErrors : ";
            for(const auto& x:errors)
            {
                std::cout << std::boolalpha << x << " ";
            }
        }
        
        bool load(const std::string& str)
        {
            // pass through input string
            std::uint32_t n=0; 
            while(n<str.size())
            {
                char x; 
                char y;

                // *** E1 check *** //
                if (!extract_one_edge(str, n, x, y))
                {
                    errors[0] = true;
                    return false; 
                }
                
                // *** E2 check (fill tree-struct) *** //
                auto iter0 = tree.find(x);
                if (iter0 == tree.end())
                {
                    tree[x] = info{empty,y,empty,{},{y}};
                }
                else
                {
                    auto i  = iter0->second.children.find(y);
                    if  (i == iter0->second.children.end())     iter0->second.children.insert(y);
                    else                                        errors[1] = true;

                    // For final output
                    if      (iter0->second.lhs_child == empty)  iter0->second.lhs_child = y;
                    else if (iter0->second.rhs_child == empty)  iter0->second.rhs_child = y;
                }

                auto iter1 = tree.find(y);
                if (iter1 == tree.end())
                {
                    tree[y] = info{empty,empty,empty,{x},{}};
                }
                else
                {
                    auto i  = iter1->second.parents.find(x);
                    if  (i == iter1->second.parents.end())      iter1->second.parents.insert(x);
                    else                                        errors[1] = true; // redundant
                }
            }

            for(const auto& x:tree)
            {
                // *** E3 check *** //
                if (x.second.children.size()>2)
                {
                    errors[2] = true;
                }

                // *** E4 1st-check *** //
                if (x.second.parents.empty())
                {
                    if (root == empty) root = x.first;
                    else errors[3] = true; // multiple roots
                }

                // *** E5 1st-check *** //
                if (x.second.parents.size()>1)
                {
                    errors[4] = true;
                }
            }

            // *** E4 2nd-check *** //
            if (!errors[3] && tree.size() > 0)
            {
                errors[3] = multiple_blobs(tree.begin()->first);
            }
            
            // *** E5 2nd-check *** //
            if (root == empty)
            {
                errors[4] = true;
            } 
            return true;
        }
        
        std::string get_output() const
        {
            if (errors[0]) return "E1"; 
            if (errors[1]) return "E2";
            if (errors[2]) return "E3";
            if (errors[3]) return "E4";
            if (errors[4]) return "E5";
            
            std::stringstream ss;
            ss << "(";
            print_to_ss(root, ss);
            ss << ")";
            return ss.str();
        }
     
    private:   
        bool extract_one_edge(const std::string& str, std::uint32_t& n, char& x, char& y)
        {
            // read a space char
            if (n>0)
            {
                if (str[n]!=' ') return false;
                ++n;
            }
            if (str[n]!='(') return false;
            ++n;
            if (str[n]<'A' || str[n]>'Z') return false;
            x = str[n];
            ++n;
            if (str[n]!=',') return false;
            ++n;
            if (str[n]<'A' || str[n]>'Z') return false;
            y = str[n];
            ++n;
            if (str[n]!=')') return false;
            ++n;
            return true;
        }

        // Iterative implementation 
        bool multiple_blobs(char c)
        {
            std::queue<char> q;
            q.push(c);

            while(!q.empty())
            {
                auto iter = tree.find(q.front());
                q.pop();

                if (iter != tree.end() && iter->second.label == empty)
                {
                    iter->second.label = labelled;
                    for(auto& x:iter->second.parents)  q.push(x);
                    for(auto& x:iter->second.children) q.push(x);
                }
            }

            for(const auto& x:tree)
            {
                if (x.second.label != labelled) return true;
            }
            return false;
        }

        
        // Recursive implementation
        void print_to_ss(char c, std::stringstream& ss) const
        {
            ss << c;    
            auto iter = tree.find(c);
            if (iter != tree.end())
            {
                if (iter->second.lhs_child != empty)
                {
                    ss << "(";
                    print_to_ss(iter->second.lhs_child, ss);
                    ss << ")";

                    if (iter->second.rhs_child != empty)
                    {
                        ss << "(";
                        print_to_ss(iter->second.rhs_child, ss);
                        ss << ")";
                    }
                }
            }
        }

    private:
        static const char empty = '*';
        static const char labelled = 'x';
        struct info
        {
            char label;
            char lhs_child;
            char rhs_child;
            std::unordered_set<char> parents;
            std::unordered_set<char> children;
        };    
        
    private:
        char root;
        std::unordered_map<char,info> tree;
        std::array<bool,5> errors;
    };
}

