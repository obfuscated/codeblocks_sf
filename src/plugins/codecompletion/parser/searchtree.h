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

/**  node index, we hold the node address in a vector and reference it by index */
typedef size_t nSearchTreeNode;
/** label index, we put the labels (strings) in a vector, and access them by index */
typedef size_t nSearchTreeLabel;

class SearchTreeNode;
class BasicSearchTree;
class SearchTreePoint;

/** SearchTreeLinkMap is the list of the edges towards child nodes. The character is the
 *  key, and the child node index is the value, \see SearchTreeNode for a reference.
 */
typedef std::map<wxChar, nSearchTreeNode, std::less<wxChar> > SearchTreeLinkMap;


/** SearchTreeNodesArray contains all the nodes for a search tree, currently the vector should
 *  have all the slots filled with a valid node address, so remove a node from the vector is not
 *  allowed.
 */
typedef std::vector<SearchTreeNode*> SearchTreeNodesArray;

/** SearchTreePointsArray contains a list of tree points defining strings (keys), \see SearchTreePoint
 *  for details about how unique strings(key) are referenced by SearchTreePoint.
 */
typedef std::vector<SearchTreePoint> SearchTreePointsArray;

/** SearchTreeItemsMap contains all the items belonging to an node, the key is the depth of the point,
 * which is the string length from the root node to the current point, the value is an index to point
 * array, \see SearchTreeNode as a reference.
 */
typedef std::map<size_t,size_t, std::less<size_t> > SearchTreeItemsMap;

/** SearchTreeLabelsArray contains the labels used by the nodes, each node contains an incoming edge
 *  string, to identify the edge string, we need three member variables:
 *  1, a string index in the vector, \see SearchTreeNode::nSearchTreeLabel
 *  2, the edge's start index in the string, \see SearchTreeNode::m_LabelStart
 *  3, the edge's length, \see SearchTreeNode::m_LabelLen
 */
typedef std::vector<wxString> SearchTreeLabelsArray;

/** SearchTreeIterator lets us iterate through the nodes of a BasicSearchTree */
class BasicSearchTreeIterator
{
public:
    /** default constructor */
    BasicSearchTreeIterator();

    /** constructor */
    BasicSearchTreeIterator(BasicSearchTree* tree);

    /** destructor */
    virtual ~BasicSearchTreeIterator() {}

    /** check to see whether the last newest added node is the last element of the node array */
    bool IsValid();

    bool FindPrev(bool includechildren = true);

    bool FindNext(bool includechildren = true);

    /** overload the *() operator, get the node index pointed by the iterator */
    const nSearchTreeNode& operator* () const { return m_CurNode; }

    /** go to next node */
    const BasicSearchTreeIterator& operator++() { FindNext(); return *this; }

    /** go to previous node */
    const BasicSearchTreeIterator& operator--() { FindPrev(); return *this; }

    /** go to the next node under the same parent's link map */
    bool FindNextSibling();

    /** go to the previous node under the same parent's link map */
    bool FindPrevSibling();

    /** check to see a sibling node with the first character 'ch' exists */
    bool FindSibling(wxChar ch);

    /** reach the end of the tree */
    bool Eof() { return (!IsValid() || m_Eof); }

protected:
    nSearchTreeNode  m_CurNode;        /// current pointed node index
    bool             m_Eof;            /// whether the iterator reaches end of tree
    BasicSearchTree* m_Tree;           /// pointer to tree instance
    size_t           m_LastTreeSize;   /// For checking validity
    SearchTreeNode*  m_LastAddedNode;  /// For checking validity
};

/** This class is used to access items of the tree, each node may contains a lot of items, E.g.
 * @code
 *   - "" (0)
 *         \- "p" (4)
 *                 +- "hysi" (2)
 *                 |          +- "cs" (1)
 *                 |          \- "ology" (3)
 *                 \- "sychic" (5)
 * @endcode
 *  In the above tree, we have totally 6 nodes, each node can have an items map
 *  such as the node (2), we may have a item map with up to 4 elements
 * @code
 *  depth -> string
 *  2     -> "ph"
 *  3     -> "phy"
 *  4     -> "phys"
 *  5     -> "physi"
 * @endcode
 *  the value of the item map is not the string, but an size_t value (item number)
 *  To access one item, we can use a SearchTreePoint, \see SearchTreePoint
 *  Now, if you have a SearchTreePoint(n=2, depth=2), you can get the string "ph" and its item number
 */
class SearchTreePoint
{
public:
    nSearchTreeNode n;  /// Which node are we pointing to?
    size_t depth;       /// At what depth is the string's end located?
    SearchTreePoint(): n(0), depth(0) {}
    SearchTreePoint(nSearchTreeNode nn, size_t dd) { n = nn; depth = dd; }
};

/** This class represents a node of the tree, we still take an example E.g.
 * @code
 *   - "" (0)
 *         \- "p" (4)
 *                 +- "hysi" (2)
 *                 |          +- "cs" (1)
 *                 |          \- "ology" (3)
 *                 \- "sychic" (5)
 * @endcode
 *  Here, a tree of 6 nodes. Let's look at node (2), its incoming edge is "hysi", it has two child nodes
 *  "cs" (1) and "ology" (3). To access child nodes from parent node, we need a link map, here is
 *  an example of link map for node (2), note that link map's key is always a single character.
 * @code
 *  char -> node index
 *   'c' -> 1
 *   'o' -> 3
 * @endcode
 *  The parent node of the node (2) is the node (4), note that the left most node in the above tree
 *  is the root node, the root node has index number 0, because it was the first node created
 *  when the tree constructed.
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
    /** return the parent node index */
    nSearchTreeNode GetParent() const { return m_Parent; }

    /** set the parent node index */
    void SetParent(nSearchTreeNode newparent) { m_Parent = newparent; }

    /** This will loop of the link map of the node, and find the child node with its edge beginning
     *  with the single character, return the valid node index then, if no such child node exists,
     *  it simply return 0
     */
    nSearchTreeNode GetChild(wxChar ch);

    /** the element stored in the item map is currently size_t item number */
    size_t GetItemNo(size_t depth);

    /** set the item value in the item map */
    size_t AddItemNo(size_t depth, size_t itemno);

    /** get the parent node pointer, this is simply a wrapper function of nSearchTreeNode GetParent() */
    SearchTreeNode* GetParent(const BasicSearchTree* tree) const;

    /** return a child node pointer, this is simply a wrapper function of GetChild(wxChar ch), besides that
     *  it use the valid node index to reference a valid node instance address.
     */
    SearchTreeNode* GetChild(BasicSearchTree* tree,wxChar ch);

    /** get the incoming edge string of the node, in the above example, it is "hysi" for node(2) */
    wxString GetLabel(const BasicSearchTree* tree) const;

    /** get first character of the incoming edge, in the above example, it is 'h' for node(2) */
    wxChar GetChar(const BasicSearchTree* tree) const;

    /** get the full label string, note the incoming edge is only a sub string of the full label */
    const wxString& GetActualLabel(const BasicSearchTree* tree) const;

    /** the index of the full label in tree->m_Labels array */
    nSearchTreeLabel GetLabelNo() const { return m_Label; }

    /** the first character index in the full label */
    unsigned int GetLabelStart() const { return m_LabelStart; }

    /** the length of the incoming label, in the above example, it is 4 for node"hysi"(2) */
    unsigned int GetLabelLen() const { return m_LabelLen; }

    /** specify the incoming edge of the current node */
    void SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen);

    /** the depth of node is the string length from root node to the last character of the incoming edge */
    unsigned int GetDepth() const { return m_Depth; }

    /** Updates the depth by recalculate the m_Depth of the node, this happens the parent node has changed */
    void RecalcDepth(BasicSearchTree* tree);

    /**  Updates items with parent, move some items upward to its parent node */
    void UpdateItems(BasicSearchTree* tree);

    /** Returns the depth of the start of the node's incoming label
     *  In other words, returns the (calculated) parent's depth
     */
    unsigned int GetLabelStartDepth() const;

    /** check to see this node is a leaf node. Note the label's depth is 0-based */
    bool IsLeaf() const { return m_Children.empty() && (m_Depth != 0); }

    /** Gets the deepest position where the string matches the edge's label.
     *  0 for 0 characters in the tree matched, 1 for 1 character matched, etc.
     * \prarm StringStartDepth is the start label position  of the root node
     */
    unsigned int GetDeepestMatchingPosition(BasicSearchTree* tree, const wxString& s, unsigned int StringStartDepth);

    /** serialize the node information to a string */
    wxString Serialize(BasicSearchTree* tree, nSearchTreeNode node_id, bool withchildren = false);
    /** pretty print the node to a string, used for debugging */
    void Dump(BasicSearchTree* tree, nSearchTreeNode node_id, const wxString& prefix, wxString& result);

    static wxString SerializeString(const wxString& s);
    static wxString U2S(unsigned int u);
    static wxString I2S(int i);
    static bool UnSerializeString(const wxString& s,wxString& result);
    static bool S2U(const wxString& s,unsigned int& u);
    static bool S2I(const wxString& s,int& i);

protected:
    /** the string length from the root node to the current node (end of the edge) */
    unsigned int       m_Depth;

    /** parent node index */
    nSearchTreeNode    m_Parent;

    /** the string index, the edge is a sub-string of the label in the label array */
    nSearchTreeLabel   m_Label;

    /** label start index in the string */
    unsigned int       m_LabelStart;

    /** label length in the string */
    unsigned int       m_LabelLen;

    /** link to descent nodes, \see SearchTreeLinkMap for details */
    SearchTreeLinkMap  m_Children;
    /** depth->item number map, \see SearchTreePoint about how to access items of the node */
    SearchTreeItemsMap m_Items;
};

/** This class is generally a string -> size_t map, the tree details (graph) is already show in the
 *  declaration of \see SearchTreeNode and \see BasicSearchTreeIterator.
 */
class BasicSearchTree
{
    friend class SearchTreeNode;
    friend class BasicSearchTreeIterator;
public:
    BasicSearchTree();
    virtual ~BasicSearchTree();
    virtual size_t size() const { return m_Points.size(); }     /// How many string keys are stored
    virtual size_t GetCount() const { return m_Points.size(); } /// Gets the number of items stored
    virtual void clear(); /// Clear items and tree

    /** Adds an item number to position defined by s.
     *  If the string already exists, returns the corresponding item no.
     */
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
     *  if is_prefix==true, it finds items that start with the string.
     *  returns the number of matches.
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
     *  Used by AddNode.
     *  @return the newly created node
     *  if the given position is exactly the length of n's vertex,
     *  just return n.
     */
    nSearchTreeNode SplitBranch(nSearchTreeNode n, size_t depth);

protected:
    /// Labels used by the nodes' incoming edges
    SearchTreeLabelsArray m_Labels;
    /// Nodes array
    SearchTreeNodesArray  m_Nodes;
    /// Points array
    SearchTreePointsArray m_Points;
};
/** this is a class template derived from BasicSearchTree class, the type T was stored in a vector,
 *  and instead of returning a item number, the new class template can return an instance of T. So,
 *  basically, you can regard this class as a map of key string -> T
 */
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

    /// Gets the item at position defined by key string s
    T GetItem(const wxString& s);

    /// the key string can be specified by a wxChar array
    T GetItem(const wxChar* s);

    /// Adds an item to position defined by s and return the item number
    size_t AddItem(const wxString& s, T item, bool replaceexisting = false);

    /// Gets the item found at position i, the i is the item index
    T& GetItemAtPos(size_t i);

    /// Replaces the item found at position(index) i
    void SetItemAtPos(size_t i,T item);

    /** Gets the item found at position s. Inserts new empty one if not found. */
    T& operator[](const wxString& s);

    /// Serializes the stored items
    virtual wxString SerializeItem(cb_unused size_t idx) { return wxString(_T("")); }

    /// Unserializes the items to be stored
    virtual void* UnserializeItem(cb_unused const wxString& s) { return NULL; }

protected:
    std::vector<T> m_Items;   /// The actual stored items

    /// Releases the stored items (vector) from memory. Called by Clear();
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

#endif // SEARCHTREE_H
