/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef SEARCHTREE_H
#define SEARCHTREE_H

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
key, and the node is the value */
typedef std::map<wxChar, nSearchTreeNode, std::less<wxChar> > SearchTreeLinkMap;

typedef std::vector<SearchTreeLinkMap::iterator> SearchTreeStack;

/** SearchTreeNodesArray contains all the nodes for a search tree */
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

class SearchTreePoint
{
public:
    nSearchTreeNode n; /// Which node are we pointing to?
    size_t depth; /// At what depth is the string's end located?
    SearchTreePoint(): n(0), depth(0) {}
    SearchTreePoint(nSearchTreeNode nn, size_t dd) { n = nn; depth = dd; }
};

class SearchTreeNode
{
    friend class BasicSearchTree;
    friend class BasicSearchTreeIterator;
public:
    SearchTreeNode();
    SearchTreeNode(unsigned int depth, nSearchTreeNode parent, nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);
    virtual ~SearchTreeNode();
    nSearchTreeNode GetParent() const { return m_Parent; }
    void SetParent(nSearchTreeNode newparent) { m_Parent = newparent; }
    nSearchTreeNode GetChild(wxChar ch);
    size_t GetItemNo(size_t depth);
    size_t AddItemNo(size_t depth, size_t itemno);
    SearchTreeNode* GetParent(const BasicSearchTree* tree) const;
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
        0 for 0 characters in the tree matched, 1 for 1 character matched, etc.
        */
    unsigned int GetDeepestMatchingPosition(BasicSearchTree* tree, const wxString& s,unsigned int StringStartDepth);
    wxString Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren = false);
    void Dump(BasicSearchTree* tree,nSearchTreeNode node_id,const wxString& prefix,wxString& result);

    static wxString SerializeString(const wxString& s);
    static wxString U2S(unsigned int u);
    static wxString I2S(int i);
    static bool UnSerializeString(const wxString& s,wxString& result);
    static bool S2U(const wxString& s,unsigned int& u);
    static bool S2I(const wxString& s,int& i);

protected:
    unsigned int       m_Depth;
    nSearchTreeNode    m_Parent;
    nSearchTreeLabel   m_Label;
    unsigned int       m_LabelStart;
    unsigned int       m_LabelLen;
    SearchTreeLinkMap  m_Children;
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
        and customized, or to improve the memory management. */
    virtual SearchTreeNode* CreateNode(unsigned int depth, nSearchTreeNode parent, nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);

    /** Gets the string corresponding to the tree point 'nn'.
        If 'top' is specified, it gets the string that goes from node 'top' to point 'nn'. */
    wxString GetString(const SearchTreePoint& nn, nSearchTreeNode top = 0) const;

    /** Obtains the node with number n,NULL if n is invalid.
        If NullOnZero == true, returns NULL if n is 0. */
    SearchTreeNode* GetNode(nSearchTreeNode n, bool NullOnZero = false);
    /// Finds the node that starts from node 'parent', and has the suffix s.
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

template <class T>
class SearchTree: public BasicSearchTree
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
    virtual wxString SerializeItem(size_t idx) { return wxString(_T("")); }
    /// Unserializes the items to be stored
    virtual void* UnserializeItem(const wxString& s) { return NULL; }

protected:
    std::vector<T> m_Items;   /// The actual stored items

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
bool SearchTree<T>::SaveCacheTo(const wxString& filename)
{
    return true;
}

template <class T>
bool SearchTree<T>::LoadCacheFrom(const wxString& filename)
{
    return true;
}

template <class T>
T SearchTree<T>::GetItem(const wxChar* s)
{
    wxString tmps(s);
    return GetItem(tmps);
}

template <class T>
T SearchTree<T>::GetItem(const wxString& s)
{
    size_t itemno = GetItemNo(s);
    if(!itemno && !s.empty())
        return T();
    return GetItemAtPos(itemno);
}

template <class T>
size_t SearchTree<T>::AddItem(const wxString& s, T item, bool replaceexisting)
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
T& SearchTree<T>::operator[](const wxString& s)
{
    size_t curpos = GetItemNo(s);
    if(!curpos)
    {
        T newitem;
        curpos = AddItem(s, newitem);
    }

    return m_Items[curpos];
}

template <class T>
wxString SearchTree<T>::Serialize()
{
    wxString result;
    size_t i;
    result << _T("<SearchTree>\n");
    result << SerializeLabels();
    result << _T("<nodes>\n");
    for (i = 0; i < m_Nodes.size(); ++i)
        result << m_Nodes[i]->Serialize(this, i, false);
    result << _T("</nodes>\n");
    result << _T(" <items>\n");
    for (i = 1; i < m_Items.size(); ++i)
    {
        result << SerializeItem(i);

    }
    result << _T(" </items>\n");
    result << _T("</SearchTree>\n");
    return result;
}

#endif
