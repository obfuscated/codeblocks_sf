/////////////////////////////////////////////////////////////////////////////
// Name:        searchtree.h
// Purpose:     N-ary Search Tree
//              The classes used here implement a N-Ary Search Tree for
//              associative arrays.
//              Search time for a word of length k is O(k)
//              Addition of N words of average size k, takes O(N*k) time.
//
// Author:      Ricardo Garcia
// Modified by:
// Created:     04/12/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Ricardo Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
#ifndef SEARCHTREE_H
#define SEARCHTREE_H

#include <vector>
#include <map>
#include <set>

#ifdef __WXWINDOWS__

#  include <wx/string.h>
#  define string wxString
#  define char wxChar

#else

#  include <string>

#endif

using namespace std;

typedef size_t nSearchTreeNode;
typedef size_t nSearchTreeLabel;

class SearchTreeNode;
class BasicSearchTree;
class SearchTreePoint;

/** SearchTreeLinkMap is the list of the edges towards other nodes. The character is the
key, and the node is the value */
typedef map<char,nSearchTreeNode,less<char> > SearchTreeLinkMap;

typedef vector<SearchTreeLinkMap::iterator> SearchTreeStack;

/** SearchTreeNodesArray contains all the nodes for a search tree */
typedef vector<SearchTreeNode*> SearchTreeNodesArray;

/** SearchTreePointsArray contains a list of tree points defining strings */
typedef vector<SearchTreePoint> SearchTreePointsArray;

/** SearchTreeItemsMap contains all the items belonging to an edge */
typedef map<size_t,size_t,less<size_t> > SearchTreeItemsMap;

/** SearchTreeLabelsArray contains the labels used by the nodes */
typedef vector<string> SearchTreeLabelsArray;

/** SearchTreeIterator lets us iterate through the nodes of a BasicSearchTree */
class BasicSearchTreeIterator
{
    public:
        BasicSearchTreeIterator();
        BasicSearchTreeIterator(BasicSearchTree* tree);
        virtual ~BasicSearchTreeIterator() {}
        bool IsValid();
        bool FindPrev(bool includechildren = true);
        bool FindNext(bool includechildren = true);
        const nSearchTreeNode& operator* () const { return m_CurNode; }
        const BasicSearchTreeIterator& operator++() { FindNext(); return *this; }
        const BasicSearchTreeIterator& operator--() { FindPrev(); return *this; }
        bool FindNextSibling();
        bool FindPrevSibling();
        bool FindSibling(char ch);
        bool eof() { return (!IsValid() || m_eof); }
        nSearchTreeNode m_CurNode;
        bool m_eof; // Reached end of tree
    protected:
        BasicSearchTree* m_pTree;
        size_t m_LastTreeSize; // For checking validity
        SearchTreeNode* m_LastAddedNode; // For checking validity
        SearchTreeStack m_Stack;
        vector<SearchTreeLinkMap*> m_Stack2;
};

class SearchTreePoint
{
    public:
        nSearchTreeNode n; /// Which node are we pointing to?
        size_t depth; /// At what depth is the string's end located?
        SearchTreePoint ():n(0),depth(0) {}
        SearchTreePoint (nSearchTreeNode nn, size_t dd) { n = nn; depth = dd; }
};

class SearchTreeNode
{
    friend class BasicSearchTree;
    friend class BasicSearchTreeIterator;
    public:
        SearchTreeNode();
        SearchTreeNode(unsigned int depth,nSearchTreeNode parent,nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);
        virtual ~SearchTreeNode();
        nSearchTreeNode GetParent() const { return m_parent; }
        void SetParent(nSearchTreeNode newparent) { m_parent = newparent; }
        nSearchTreeNode GetChild(char ch);
        size_t GetItemNo(size_t depth);
        size_t AddItemNo(size_t depth,size_t itemno);
        SearchTreeNode* GetParent(const BasicSearchTree* tree) const;
        SearchTreeNode* GetChild(BasicSearchTree* tree,char ch);
        string GetLabel(const BasicSearchTree* tree) const;
        char GetChar(const BasicSearchTree* tree) const;
        const string& GetActualLabel(const BasicSearchTree* tree) const;
        nSearchTreeLabel GetLabelNo() const { return m_label; }
        unsigned int GetLabelStart() const { return m_labelstart; }
        unsigned int GetLabelLen() const { return m_labellen; }
        void SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);
        unsigned int GetDepth() const { return m_depth; }
        void RecalcDepth(BasicSearchTree* tree); /// Updates the depth
        void UpdateItems(BasicSearchTree* tree); /// Updates items with parent
        /** Returns the depth of the start of the node's incoming label
            In other words, returns the (calculated) parent's depth */
        unsigned int GetLabelStartDepth() const;
        /// The label's depth is 0-based.
        bool IsLeaf() const { return m_Children.empty() && (m_depth != 0); }

        /** Gets the deepest position where the string matches the node's edge's label.
            0 for 0 characters in the tree matched, 1 for 1 character matched, etc.
            */
        unsigned int GetDeepestMatchingPosition(BasicSearchTree* tree, const string& s,unsigned int StringStartDepth);
        string Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren = false);
        void dump(BasicSearchTree* tree,nSearchTreeNode node_id,const string& prefix,string& result);


        static string SerializeString(const string& s);
        static string u2s(unsigned int u);
        static string i2s(int i);
        static bool UnSerializeString(const string& s,string& result);
        static bool s2u(const string& s,unsigned int& u);
        static bool s2i(const string& s,int& i);
    protected:
        unsigned int m_depth;
        nSearchTreeNode m_parent;
        nSearchTreeLabel m_label;
        unsigned int m_labelstart, m_labellen;
        SearchTreeLinkMap m_Children;
        SearchTreeItemsMap m_Items;
};

class BasicSearchTree
{
    friend class SearchTreeNode;
    friend class BasicSearchTreeIterator;
    public:
        BasicSearchTree();
        virtual ~BasicSearchTree();
        virtual size_t size() const { return m_Points.size(); }
        virtual size_t GetCount() const { return m_Points.size(); } /// Gets the number of items stored
        virtual void clear(); /// Clears items and tree

        /** Adds an item number to position defined by s.
            If the string already exists, returns the correspoinding item no. */
        size_t insert(const string& s);

        /// Tells if there is an item for string s
        bool HasItem(const string& s);

        /// std::map compatibility for the above
        size_t count(const string& s) { return HasItem(s) ? 1 : 0; }

        /// Gets the array position defined by s
        size_t GetItemNo(const string& s);

        /// Gets the key string for item n
        const string GetString(size_t n) const;

        /** Finds items that match a given string.
            if is_prefix==true, it finds items that start with the string.
            returns the number of matches.
        */
        size_t FindMatches(const string& s,set<size_t> &result,bool caseSensitive,bool is_prefix);

        /// Serializes the labels into an XML-compatible string
        string SerializeLabels();
        /// Dumps a graphical version of the tree
        string dump();
    protected:

        /** Creates a new node. Function is virtual so the nodes can be extended
            and customized, or to improve the memory management. */
        virtual SearchTreeNode* CreateNode(unsigned int depth,nSearchTreeNode parent,nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);

        /** Gets the string corresponding to the tree point 'nn'.
            If 'top' is specified, it gets the string that goes from node 'top' to point 'nn'. */
        string GetString(const SearchTreePoint &nn,nSearchTreeNode top = 0) const;

        /** Obtains the node with number n,NULL if n is invalid.
            If NullOnZero == true, returns NULL if n is 0. */
        SearchTreeNode* GetNode(nSearchTreeNode n,bool NullOnZero = false);
        /// Finds the node that starts from node 'parent', and has the suffix s.
        bool FindNode(const string& s, nSearchTreeNode nparent, SearchTreePoint* result);
        /// Adds Suffix s starting from node nparent.
        SearchTreePoint AddNode(const string& s, nSearchTreeNode nparent = 0);

        /// Serializes given label into an XML-escaped string.
        string SerializeLabel(nSearchTreeLabel labelno);

        /// Labels used by the nodes' edges
        SearchTreeLabelsArray m_Labels;
        /// Nodes and their edges
        SearchTreeNodesArray m_pNodes;

        /// Points defining the items' strings
        SearchTreePointsArray m_Points;

    private:
        /// Creates the tree's root node.
        void CreateRootNode();

        /** Splits the Branch that leads to node n, at the given depth.
            Used by AddNode.
            @return the newly created node
            if the given position is exactly the length of n's vertex,
            just return n.
         */
        nSearchTreeNode SplitBranch(nSearchTreeNode n,size_t depth);
};

template <class T>
class SearchTree: public BasicSearchTree
{
    public:
        SearchTree();
        virtual ~SearchTree();
        virtual void clear(); /// Clears the tree
        size_t GetCount() const; /// Gets the number of items stored
        virtual size_t size() const; /// Same as GetCount
        bool SaveCacheTo(const string& filename); /// Stores the Tree and items into a file
        bool LoadCacheFrom(const string& filename); /// Loads the Tree and items from a file
        string Serialize();
        T GetItem(const string& s); /// Gets the item at position defined by s
        T GetItem(const char* s);
        size_t AddItem(const string& s,T item,bool replaceexisting = false); /// Adds an item to position defined by s
        T& GetItemAtPos(size_t i); /// Gets the item found at position i
        void SetItemAtPos(size_t i,T item); /// Replaces the item found at position i

        /** Gets the item found at position s. Inserts new empty one if not found. */
        T& operator[](const string& s);
        /// Serializes the stored items
        virtual string SerializeItem(size_t idx) { return string(_T("")); }
        /// Unserializes the items to be stored
        virtual void* UnserializeItem(const string& s) { return NULL; }
    protected:
        vector<T> m_Items;   /// The actual stored items

        /// Releases the stored items from memory. Called by Clear();
        virtual void ClearItems();

        /// Adds a null item to position 0.
        virtual bool AddFirstNullItem();

};

template <class T>
SearchTree<T>::SearchTree():BasicSearchTree()
{
    m_Items.clear();
    AddFirstNullItem();
}

template <class T>
SearchTree<T>::~SearchTree()
{
    ClearItems();
}

template <class T>
void SearchTree<T>::clear()
{
    ClearItems();
    BasicSearchTree::clear();
    AddFirstNullItem();
}

template <class T>
size_t SearchTree<T>::GetCount() const
{
    size_t result = m_Items.size() -1;
    return result;
}

template <class T>
size_t SearchTree<T>::size() const
{
    size_t result = m_Items.size() -1;
    return result;
}


template <class T>
bool SearchTree<T>::SaveCacheTo(const string& filename)
{
    return true;
}

template <class T>
bool SearchTree<T>::LoadCacheFrom(const string& filename)
{
    return true;
}

template <class T>
T SearchTree<T>::GetItem(const char* s)
{
    string tmps(s);
    return GetItem(tmps);
}

template <class T>
T SearchTree<T>::GetItem(const string& s)
{
    size_t itemno = GetItemNo(s);
    if(!itemno && !s.empty())
        return T();
    return GetItemAtPos(itemno);
}

template <class T>
size_t SearchTree<T>::AddItem(const string& s,T item,bool replaceexisting)
{
    size_t itemno = insert(s);
    if(itemno > m_Items.size())
        m_Items.resize(itemno);
    else if(itemno == m_Items.size())
        m_Items.push_back(item);
    else if(replaceexisting)
        m_Items[itemno] = item;
    return itemno;
}

template <class T>
T& SearchTree<T>::GetItemAtPos(size_t i)
{
    if(i>=m_Items.size() || i < 1)
        i = 0;
    return m_Items[i];
}

template <class T>
void SearchTree<T>::SetItemAtPos(size_t i,T item)
{
    m_Items[i]=item;
}

template <class T>
void SearchTree<T>::ClearItems()
/// Called by BasicSearchTree::BasicSearchTree() and BasicSearchTree::Clear()
{
    m_Items.clear();
}

template <class T>
bool SearchTree<T>::AddFirstNullItem()
{
    T newvalue;
    m_Items.push_back(newvalue);
    return true;
}

template <class T>
T& SearchTree<T>::operator[](const string& s)
{
    size_t curpos = GetItemNo(s);
    if(!curpos)
    {
        T newitem;
        curpos = AddItem(s,newitem);
    }
    return m_Items[curpos];
}

template <class T>
string SearchTree<T>::Serialize()
{
    string result;
    size_t i;
    result << _T("<SearchTree>\n");
    result << SerializeLabels();
    result << _T("<nodes>\n");
    for(i=0;i<m_pNodes.size();i++)
        result << m_pNodes[i]->Serialize(this,i,false);
    result << _T("</nodes>\n");
    result << _T(" <items>\n");
    for(i=1;i < m_Items.size();i++)
    {
        result << SerializeItem(i);

    }
    result << _T(" </items>\n");
    result << _T("</SearchTree>\n");
    return result;
}

#ifdef __WXWINDOWS__

#undef string
#undef char

#endif

#endif
