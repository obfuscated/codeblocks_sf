/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef SEARCHTREE_H
#define SEARCHTREE_H

#include "prep.h"
#include <wx/string.h>

#include <vector>
#include <map>
#include <set>

typedef size_t nSearchTreeNode;
typedef size_t nSearchTreeLabel;

class SearchTreeNode;
class BasicSearchTree;
class SearchTreePoint;

/** SearchTreeLinkMap is the list of the edges towards other nodes. The character is the
 *  key, and the node index is the value
 */
typedef std::map<wxChar, nSearchTreeNode, std::less<wxChar> > SearchTreeLinkMap;

//typedef std::vector<SearchTreeLinkMap::iterator> SearchTreeStack;

/** SearchTreeNodesArray contains all the nodes for a search tree */
// FIXME (ollydbg#1#): what about if an old node is deleted, then new node added, in this case, we
// should still keep the node index continuous in the tree.
typedef std::vector<SearchTreeNode*> SearchTreeNodesArray;

/** SearchTreePointsArray contains a list of tree points defining strings */
typedef std::vector<SearchTreePoint> SearchTreePointsArray;

/** SearchTreeItemsMap contains all the items belonging to an edge */
typedef std::map<size_t,size_t, std::less<size_t> > SearchTreeItemsMap;

/** SearchTreeLabelsArray contains the labels used by the nodes */
typedef std::vector<wxString> SearchTreeLabelsArray;

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
    bool FindSibling(wxChar ch);
    bool Eof() { return (!IsValid() || m_Eof); }

protected:
    nSearchTreeNode  m_CurNode;
    bool             m_Eof; // Reached end of tree
    BasicSearchTree* m_Tree;
    size_t           m_LastTreeSize; // For checking validity
    SearchTreeNode*  m_LastAddedNode; // For checking validity
};
/** This class is used to access items of the tree, each node may contains a lot of items, E.g.
 *   - "" (0)
 *         \- "p" (4)
 *                 +- "hysi" (2)
 *                 |          +- "cs" (1)
 *                 |          \- "ology" (3)
 *                 \- "sychic" (5)
 *  In the above tree, we have totally 6 nodes, each node can have an items map
 *  such as the node number 2, we have have mostly item map like
 *  depth -> string
 *  5     -> "ph"
 *  6     -> "phy"
 *  7     -> "phys"
 *  8     -> "physi"
 *  Now, if you have a SearchTreePoint(n=2, depth=5), you can get the string "ph"
 */
class SearchTreePoint
{
public:
    nSearchTreeNode n; /// Which node are we pointing to?
    size_t depth; /// At what depth is the string's end located?
    SearchTreePoint(): n(0), depth(0) {}
    SearchTreePoint(nSearchTreeNode nn, size_t dd) { n = nn; depth = dd; }
};

/** This class represents a node of the tree, we still take an example E.g.
 *
 *   - "" (0)
 *         \- "p" (4)
 *                 +- "hysi" (2)
 *                 |          +- "cs" (1)
 *                 |          \- "ology" (3)
 *                 \- "sychic" (5)
 *
 *  Here, a tree of 6 nodes. Let's look at node (2), its label is "hysi", it have two children nodes
 *  "cs" (1) and "ology" (3). To access children nodes from parent node, we need a link map, here is
 *  an example of link map for node (2), note linkmap's key is always single character.
 *  char -> node index
 *   'c' -> 1
 *   'o' -> 3
 *  The parent node of the node (2) is the node (4), note that the left most node in the above tree
 *  is the root node, the root node has always index number 0, because it was the first node created
 *  when the tree constructed.
 *
 */
class SearchTreeNode
{
    friend class BasicSearchTree;
    friend class BasicSearchTreeIterator;
public:
    SearchTreeNode();
    SearchTreeNode(unsigned int depth,
                   nSearchTreeNode parent,
                   nSearchTreeLabel label,
                   unsigned int labelstart,
                   unsigned int labellen);
    virtual ~SearchTreeNode();
    nSearchTreeNode GetParent() const { return m_Parent; }
    void SetParent(nSearchTreeNode newparent) { m_Parent = newparent; }

    /** This will loop of the link map of the node, and find the child node with its edge beginning
     *  with the single character, return the valid node index then, if no such child node exists,
     *  it simply return 0
     */
    nSearchTreeNode GetChild(wxChar ch);

    size_t GetItemNo(size_t depth);
    size_t AddItemNo(size_t depth, size_t itemno);
    SearchTreeNode* GetParent(const BasicSearchTree* tree) const;

    /** return a Node pointer, this is simply a wrapper function of GetChild(wxChar ch), besides that
     *  it use the valid node index to reference a valid node instance address.
     */
    SearchTreeNode* GetChild(BasicSearchTree* tree,wxChar ch);
    wxString GetLabel(const BasicSearchTree* tree) const;
    wxChar GetChar(const BasicSearchTree* tree) const;
    const wxString& GetActualLabel(const BasicSearchTree* tree) const;
    nSearchTreeLabel GetLabelNo() const { return m_Label; }
    unsigned int GetLabelStart() const { return m_LabelStart; }
    unsigned int GetLabelLen() const { return m_LabelLen; }
    void SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);
    unsigned int GetDepth() const { return m_Depth; }
    void RecalcDepth(BasicSearchTree* tree); /// Updates the depth
    void UpdateItems(BasicSearchTree* tree); /// Updates items with parent
    /** Returns the depth of the start of the node's incoming label
     *  In other words, returns the (calculated) parent's depth
     */
    unsigned int GetLabelStartDepth() const;
    /// The label's depth is 0-based.
    bool IsLeaf() const { return m_Children.empty() && (m_Depth != 0); }

    /** Gets the deepest position where the string matches the node's edge's label.
     *  0 for 0 characters in the tree matched, 1 for 1 character matched, etc.
     */
    unsigned int GetDeepestMatchingPosition(BasicSearchTree* tree, const wxString& s, unsigned int StringStartDepth);
    wxString Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren = false);
    void Dump(BasicSearchTree* tree,nSearchTreeNode node_id,const wxString& prefix,wxString& result);

    static wxString SerializeString(const wxString& s);
    static wxString U2S(unsigned int u);
    static wxString I2S(int i);
    static bool UnSerializeString(const wxString& s,wxString& result);
    static bool S2U(const wxString& s,unsigned int& u);
    static bool S2I(const wxString& s,int& i);

protected:

    unsigned int       m_Depth;     // the string length from the root node to the current node

    nSearchTreeNode    m_Parent;    // parent node index

    nSearchTreeLabel   m_Label;     // the string index
    unsigned int       m_LabelStart;// label start index in the string
    unsigned int       m_LabelLen;  // label length in the string

    SearchTreeLinkMap  m_Children;  // link to descent nodes

    SearchTreeItemsMap m_Items;     // depth->size_t map, for a label "abcd", we can store "a", "ab" different depths
};

class BasicSearchTree
{
    friend class SearchTreeNode;
    friend class BasicSearchTreeIterator;
public:
    BasicSearchTree();
    virtual ~BasicSearchTree();
    virtual size_t size() const { return m_Points.size(); }     /// How many string keys are stored
    virtual size_t GetCount() const { return m_Points.size(); } /// Gets the number of items stored
    virtual void clear(); /// Clears items and tree

    /** Adds an item number to position defined by s.
        If the string already exists, returns the corresponding item no. */
    size_t insert(const wxString& s);

    /// Tells if there is an item for string s
    bool HasItem(const wxString& s);

    /// std::map compatibility for the above
    size_t count(const wxString& s) { return HasItem(s) ? 1 : 0; }

    /// Gets the array position defined by s
    size_t GetItemNo(const wxString& s);

    /// Gets the key string for item n
    const wxString GetString(size_t n) const;

    /** Finds items that match a given string.
        if is_prefix==true, it finds items that start with the string.
        returns the number of matches.
    */
    size_t FindMatches(const wxString& s, std::set<size_t>& result, bool caseSensitive, bool is_prefix);

    /// Serializes the labels into an XML-compatible string
    wxString SerializeLabels();

    /// Dumps a graphical version of the tree
    wxString dump();

protected:
    /** Creates a new node. Function is virtual so the nodes can be extended
     *  and customized, or to improve the memory management.
     */
    virtual SearchTreeNode* CreateNode(unsigned int depth,
                                       nSearchTreeNode parent,
                                       nSearchTreeLabel label,
                                       unsigned int labelstart,
                                       unsigned int labellen);

    /** Gets the string corresponding to the tree point 'nn'.
     *  If 'top' is specified, it gets the string that goes from node 'top' to point 'nn'.
     *  the default top value is 0, this means we want to get a string from the root to the point
     */
    wxString GetString(const SearchTreePoint& nn, nSearchTreeNode top = 0) const;

    /** Obtains the node with number n,NULL if n is invalid.
     *  If NullOnZero == true, returns NULL if n is 0, this forbid returning the root node.
     */
    SearchTreeNode* GetNode(nSearchTreeNode n, bool NullOnZero = false);

    /** Finds the node that starts from node 'parent', and has the suffix s.
     *  Note that even FindNode return false, the result still be filled with a valid value
     *  For example, we have a Node with Label "abc", then we are search the string "abcd" in this
     *  Node, Find Node will get false, but the result has pointing to the Node of "abc"
     */
    bool FindNode(const wxString& s, nSearchTreeNode nparent, SearchTreePoint* result);

    /// Adds Suffix s starting from node nparent.
    SearchTreePoint AddNode(const wxString& s, nSearchTreeNode nparent = 0);

    /// Serializes given label into an XML-escaped string.
    wxString SerializeLabel(nSearchTreeLabel labelno);

private:
    /// Creates the tree's root node.
    void CreateRootNode();

    /** Splits the Branch that leads to node n, at the given depth.
        Used by AddNode.
        @return the newly created node
        if the given position is exactly the length of n's vertex,
        just return n.
     */
    nSearchTreeNode SplitBranch(nSearchTreeNode n, size_t depth);

protected:
    /// Labels used by the nodes' edges
    SearchTreeLabelsArray m_Labels;
    /// Nodes and their edges
    SearchTreeNodesArray  m_Nodes;
    /// Points defining the items' strings
    SearchTreePointsArray m_Points;
};

template <class T> class SearchTree : public BasicSearchTree
{
public:
    SearchTree();
    virtual ~SearchTree();
    virtual void clear(); /// Clears the tree
    size_t GetCount() const; /// Gets the number of items stored
    virtual size_t size() const; /// Same as GetCount
    bool SaveCacheTo(const wxString& filename); /// Stores the Tree and items into a file
    bool LoadCacheFrom(const wxString& filename); /// Loads the Tree and items from a file
    wxString Serialize();
    T GetItem(const wxString& s); /// Gets the item at position defined by s
    T GetItem(const wxChar* s);
    size_t AddItem(const wxString& s, T item, bool replaceexisting = false); /// Adds an item to position defined by s
    T& GetItemAtPos(size_t i); /// Gets the item found at position i
    void SetItemAtPos(size_t i,T item); /// Replaces the item found at position i

    /** Gets the item found at position s. Inserts new empty one if not found. */
    T& operator[](const wxString& s);
    /// Serializes the stored items
    virtual wxString SerializeItem(cb_unused size_t idx) { return wxString(_T("")); }
    /// Unserializes the items to be stored
    virtual void* UnserializeItem(cb_unused const wxString& s) { return NULL; }

protected:
    std::vector<T> m_Items;   /// The actual stored items

    /// Releases the stored items from memory. Called by Clear();
    virtual void ClearItems();

    /// Adds a null item to position 0.
    virtual bool AddFirstNullItem();
};

template <class T> SearchTree<T>::SearchTree() : BasicSearchTree()
{
    m_Items.clear();
    AddFirstNullItem();
}

template <class T> SearchTree<T>::~SearchTree()
{
    ClearItems();
}

template <class T> void SearchTree<T>::clear()
{
    ClearItems();
    BasicSearchTree::clear();
    AddFirstNullItem();
}

template <class T> size_t SearchTree<T>::GetCount() const
{
    size_t result = m_Items.size() -1;
    return result;
}

template <class T> size_t SearchTree<T>::size() const
{
    size_t result = m_Items.size() -1;
    return result;
}

template <class T> bool SearchTree<T>::SaveCacheTo(const wxString& filename)
{
    return true;
}

template <class T> bool SearchTree<T>::LoadCacheFrom(const wxString& filename)
{
    return true;
}

template <class T> T SearchTree<T>::GetItem(const wxChar* s)
{
    wxString tmps(s);
    return GetItem(tmps);
}

template <class T> T SearchTree<T>::GetItem(const wxString& s)
{
    size_t itemno = GetItemNo(s);
    if (!itemno && !s.empty())
        return T();
    return GetItemAtPos(itemno);
}

template <class T> size_t SearchTree<T>::AddItem(const wxString& s, T item, bool replaceexisting)
{
    size_t itemno = insert(s);

    if (itemno > m_Items.size())
        m_Items.resize(itemno);
    else if (itemno == m_Items.size())
        m_Items.push_back(item);
    else if (replaceexisting)
        m_Items[itemno] = item;

    return itemno;
}

template <class T> T& SearchTree<T>::GetItemAtPos(size_t i)
{
    if (i>=m_Items.size() || i < 1)
        i = 0;
    return m_Items[i];
}

template <class T> void SearchTree<T>::SetItemAtPos(size_t i,T item)
{
    m_Items[i]=item;
}

/// Called by BasicSearchTree::BasicSearchTree() and BasicSearchTree::Clear()
template <class T> void SearchTree<T>::ClearItems()
{
    m_Items.clear();
}

template <class T> bool SearchTree<T>::AddFirstNullItem()
{
    T newvalue;
    m_Items.push_back(newvalue);
    return true;
}

template <class T> T& SearchTree<T>::operator[](const wxString& s)
{
    size_t curpos = GetItemNo(s);
    if (!curpos)
    {
        T newitem;
        curpos = AddItem(s, newitem);
    }

    return m_Items[curpos];
}

template <class T> wxString SearchTree<T>::Serialize()
{
    wxString result;
    result << _T("<SearchTree>\n");
    result << SerializeLabels();
    result << _T("<nodes>\n");
    for (size_t i = 0; i < m_Nodes.size(); ++i)
        result << m_Nodes[i]->Serialize(this, i, false);
    result << _T("</nodes>\n");
    result << _T(" <items>\n");
    for (size_t i = 1; i < m_Items.size(); ++i)
        result << SerializeItem(i);
    result << _T(" </items>\n");
    result << _T("</SearchTree>\n");
    return result;
}

#endif
