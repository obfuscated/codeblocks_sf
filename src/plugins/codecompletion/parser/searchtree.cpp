/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "searchtree.h"

// *** SearchTreeIterator ***

BasicSearchTreeIterator::BasicSearchTreeIterator() :
    m_CurNode(0),
    m_Eof(false),
    m_Tree(0),
    m_LastTreeSize(0),
    m_LastAddedNode(0)
{
}

BasicSearchTreeIterator::BasicSearchTreeIterator(BasicSearchTree* tree) :
    m_CurNode(0),
    m_Eof(false),
    m_Tree(tree),
    m_LastTreeSize(0),
    m_LastAddedNode(0)
{
    if (m_Tree)
    {
        m_LastTreeSize = m_Tree->m_Nodes.size();
        if (m_LastTreeSize)
            m_LastAddedNode = m_Tree->GetNode(m_LastTreeSize - 1);
    }
}

bool BasicSearchTreeIterator::IsValid()
{
    if (!this || !m_Tree || m_LastTreeSize!= m_Tree->m_Nodes.size() || m_LastAddedNode != m_Tree->m_Nodes[m_LastTreeSize - 1])
        return false;
    return true;
}

bool BasicSearchTreeIterator::FindPrev(bool includechildren)
{
    bool result;
    result = false;

    SearchTreeLinkMap::const_iterator it;
    do
    {
        if (!IsValid())
            break;
        SearchTreeNode* curnode = m_Tree->GetNode(m_CurNode);
        if (!curnode)
            break;

        result = true;
        while (m_CurNode)
        {
            m_Eof = false;
            result = FindPrevSibling();
            if (!result)
                return false;
            if (!m_Eof)
                break;
            m_CurNode = curnode->m_Parent;
            curnode = m_Tree->GetNode(m_CurNode);
            if (!curnode)
                return false;
        }

        if (includechildren)
        {
            while (curnode->m_Children.size())
            {
                it = curnode->m_Children.end();
                --it;
                m_CurNode = it->second;
                curnode = m_Tree->GetNode(m_CurNode,true);
                if (!curnode)
                    return false;
            }
        }
        m_Eof = false;
        break;
    }while (true);
    return result;
}

bool BasicSearchTreeIterator::FindNext(bool includechildren)
{
    bool result;
    result = false;

    SearchTreeLinkMap::const_iterator it;
    do
    {
        if (!IsValid())
            break;
        SearchTreeNode* curnode = m_Tree->GetNode(m_CurNode);
        if (!curnode)
            break;

        result = true;
        if (includechildren)
        {
            it = curnode->m_Children.begin();
            if (it != curnode->m_Children.end())
            {
                m_CurNode = it->second;
                curnode = m_Tree->GetNode(m_CurNode);
                if (!curnode)
                {
                    return false;
                }
                break;
            }
        }
        m_Eof = true;
        while (m_CurNode)
        {
            m_Eof = false;
            result = FindNextSibling();
            if (!m_Eof)
                break;
            m_CurNode = curnode->m_Parent;
            curnode = m_Tree->GetNode(m_CurNode);
            if (!curnode)
                return false;
        }
        break;
    }while (true);
    return result;
}

bool BasicSearchTreeIterator::FindNextSibling()
{
    if (!IsValid())
        return false;
    if (!m_CurNode /* || !m_Stack.size() */)
        m_Eof = true;

    SearchTreeNode* node = m_Tree->GetNode(m_CurNode);
    if (!node)
        return false;
    wxChar ch = node->GetChar(m_Tree);
    node = node->GetParent(m_Tree);
    if (!node)
        return false;
    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::const_iterator it = the_map->find(ch);
    if (it == the_map->end())
        m_Eof = true;
    else
    {
        ++it;
        if (it == the_map->end())
            m_Eof = true;
        else
            m_CurNode = it->second;
    }
    return true;
}

bool BasicSearchTreeIterator::FindPrevSibling()
{
    if (!IsValid())
        return false;
    if (!m_CurNode /* || !m_Stack.size() */)
        m_Eof = true;

    SearchTreeNode* node = m_Tree->GetNode(m_CurNode);
    if (!node)
        return false;
    wxChar ch = node->GetChar(m_Tree);
    node = node->GetParent(m_Tree);
    if (!node)
        return false;
    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::const_iterator it = the_map->find(ch);
    if (it == the_map->end())
        m_Eof = true;
    else
    {
        if (it == the_map->begin())
            m_Eof = true;
        else
        {
            --it;
            m_CurNode = it->second;
        }
    }
    return true;
}

bool BasicSearchTreeIterator::FindSibling(wxChar ch)
{
    if (!IsValid())
        return false;
    if (!m_CurNode /* || !m_Stack.size() */)
        m_Eof = true;

    SearchTreeNode* node = m_Tree->GetNode(m_CurNode);
    if (!node)
        return false;
    node = node->GetParent(m_Tree);
    if (!node)
        return false;

    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::const_iterator it = the_map->find(ch);
    if (it == the_map->end())
        m_Eof = true;
    else
        m_CurNode = it->second;

    return true;
}

// *** SearchTreeNode ***

SearchTreeNode::SearchTreeNode() :
    m_Depth(0),
    m_Parent(0),
    m_Label(0),
    m_LabelStart(0),
    m_LabelLen(0)
{
}

SearchTreeNode::SearchTreeNode(unsigned int depth, nSearchTreeNode parent, nSearchTreeLabel label,
                               unsigned int labelstart, unsigned int labellen) :
    m_Depth(depth),
    m_Parent(parent),
    m_Label(label),
    m_LabelStart(labelstart),
    m_LabelLen(labellen)
{
}

SearchTreeNode::~SearchTreeNode()
{
}

inline nSearchTreeNode SearchTreeNode::GetChild(wxChar ch)
{
    SearchTreeLinkMap::const_iterator found = m_Children.find(ch);
    if (found == m_Children.end())
        return 0;
    return found->second;
}

inline size_t SearchTreeNode::GetItemNo(size_t depth)
{
    SearchTreeItemsMap::const_iterator found = m_Items.find(depth);
    if (found == m_Items.end())
        return 0;
    return found->second;
}

size_t SearchTreeNode::AddItemNo(size_t depth, size_t itemno)
{
    SearchTreeItemsMap::const_iterator found = m_Items.find(depth);
    if (found == m_Items.end())
        m_Items[depth]=itemno;
    else if (found->second==0)
        m_Items[depth]=itemno;
    else
        itemno = found->second;
    return itemno;
}

inline SearchTreeNode* SearchTreeNode::GetParent(const BasicSearchTree* tree) const
{
    if (!m_Depth)
        return NULL;
    return tree->m_Nodes[m_Parent];
}

inline SearchTreeNode* SearchTreeNode::GetChild(BasicSearchTree* tree,wxChar ch)
{
    nSearchTreeNode child = GetChild(ch);
    return tree->GetNode(child,true);
}

wxString SearchTreeNode::GetLabel(const BasicSearchTree* tree) const
{
    if (!m_Depth || m_Label >= tree->m_Labels.size())
        return wxString(_T(""));
    return tree->m_Labels[m_Label].substr(m_LabelStart,m_LabelLen);
}

inline wxChar SearchTreeNode::GetChar(const BasicSearchTree* tree) const
{
    if (!m_Depth)
        return 0;
    const wxString& the_label = GetActualLabel(tree);
    return the_label[m_LabelStart];
}

inline const wxString& SearchTreeNode::GetActualLabel(const BasicSearchTree* tree) const
{
    return tree->m_Labels[m_Label];
}

inline void SearchTreeNode::SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen)
{
    m_Label = label;
    m_LabelStart = labelstart;
    m_LabelLen = labellen;
}

inline unsigned int SearchTreeNode::GetLabelStartDepth() const
{
    if (!m_Depth || m_LabelLen >= m_Depth )
        return 0;
    return (m_Depth - m_LabelLen);
}

inline unsigned int SearchTreeNode::GetDeepestMatchingPosition(BasicSearchTree* tree, const wxString& s,unsigned int StringStartDepth)
{
    if (StringStartDepth >= GetDepth())
        return GetDepth();

    if (StringStartDepth + s.length() <= GetLabelStartDepth())
        return StringStartDepth + s.length();
    // StringStartDepth + s.length() = string's depth. It must be greater
    //   than the label's start depth, otherwise there's an error.
    // Example: If StringStartDepth = 0, s.length() = 1, then string's depth = 1.
    // If the parent node's depth = 1, it means the comparison should belong
    // to the parent node's edge (the first character in the  tree), not this one.

    unsigned int startpos = GetLabelStartDepth() - StringStartDepth;
    // startpos determines the starting position of the string, to compare with
    // the label.
    // if StringStartDepth = 0, and the Label's Start Depth = 0
    // (it means we're comparing an edge that goes from the root node to
    // the currentnode). So we should start comparison at string's position 0-0 = 0.


    // Now let's compare the strings and find the first difference.
    const wxString& the_label = GetActualLabel(tree);
    size_t i,i_limit;
    i_limit = s.length() - startpos;
    if (i_limit > m_LabelLen)
        i_limit = m_LabelLen;

    for (i = 0; i < i_limit; i++)
    {
        if (the_label[m_LabelStart+i]!=s[startpos+i])
            break;
    }

    return GetLabelStartDepth() + i;
}

inline void SearchTreeNode::RecalcDepth(BasicSearchTree* tree)
{
    unsigned int curdepth = 0;
    SearchTreeNode *parent = GetParent(tree);
    if (parent)
        curdepth = parent->GetDepth();
    m_Depth = curdepth + m_LabelLen;
}

void SearchTreeNode::UpdateItems(BasicSearchTree* tree)
{
    SearchTreeNode* parentnode = tree->GetNode(m_Parent,true);
    if (!parentnode)
       return;
    SearchTreeItemsMap newmap;
    size_t mindepth = parentnode->GetDepth();
    SearchTreeItemsMap::const_iterator i;
    newmap.clear();
    for (i = m_Items.begin();i!=m_Items.end();i++)
    {
        if (i->first <= mindepth)
            parentnode->m_Items[i->first]=i->second;
        else
            newmap[i->first]=i->second;
    }
    m_Items.clear();
    for (i = newmap.begin();i!=newmap.end();i++)
        m_Items[i->first]=i->second;
}

wxString SearchTreeNode::U2S(unsigned int u)
{
    if (!u)
        return _T("0");
    wxString result(_T("")),revresult(_T(""));
    int i = 0;
    while (u>0)
    {
        revresult <<  (wxChar)(_T('0') + (u % 10));
        u/=10;
        i++;
    }
    while (i>0)
    {
        i--;
        result << revresult[i];
    }
    return result;
}

wxString SearchTreeNode::I2S(int i)
{
    wxString result(_T(""));
    if (i<0)
        result << _T('-');
    result << U2S(abs(i));
    return result;
}

wxString SearchTreeNode::Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren)
{
    wxString result,children,sparent,sdepth,slabelno,slabelstart,slabellen;
    SearchTreeLinkMap::const_iterator link;
    SearchTreeItemsMap::const_iterator item;
    sparent = U2S(m_Parent);
    sdepth = U2S(m_Depth);
    slabelno = U2S(m_Label);
    slabelstart = U2S(m_LabelStart);
    slabellen = U2S(m_LabelLen);

    result << _T(" <node id=\"") << node_id << _T("\" parent=\"") << sparent << _T("\"");
    result << _T(" depth=\"") << sdepth << _T("\" label=\"");
    result << slabelno << _T(',') << slabelstart << _T(',') << slabellen;
    result << _T("\">\n");
    result << _T("  <items>\n");
    for (item = m_Items.begin();item != m_Items.end();item++)
    {
        if (item->second)
        {
            result << _T("    <item depth=\"") << U2S(item->first)
                   << _T("\" itemid=\"")       << U2S(item->second)
                   <<  _T("\"") << _T(" />\n");
        }
    }
    result << _T("  </items>\n");
    result << _T("  <children>\n");
    for (link = m_Children.begin();link != m_Children.end();link++)
    {
        if (link->second)
        {
            result << _T("    <child char=\"") << SerializeString(wxString(link->first))
                   << _T("\" nodeid=\"") << U2S(link->second) <<  _T("\"") << _T(" />\n");
        }
    }

    result << _T("  </children>\n");
    result << _T(" </node>\n");
    if (withchildren)
    {
        for (link = m_Children.begin();link != m_Children.end();link++)
        {
            if (link->second)
            {
                result << tree->GetNode(link->second,false)->Serialize(tree,link->second,true);
            }
        }
    }
    return result;
}

void SearchTreeNode::Dump(BasicSearchTree* tree, nSearchTreeNode node_id, const wxString& prefix, wxString& result)
{
    wxString suffix(_T(""));
    suffix << _T("- \"") << SerializeString(GetLabel(tree)) << _T("\" (") << U2S(node_id) << _T(")");
    if (prefix.length() && prefix[prefix.length()-1]=='|')
        result << prefix.substr(0,prefix.length()-1) << _T('+') << suffix << _T('\n');
    else if (prefix.length() && prefix[prefix.length()-1]==' ')
        result << prefix.substr(0,prefix.length()-1) << _T('\\') << suffix << _T('\n');
    else
        result << prefix << suffix << _T('\n');
    wxString newprefix(prefix);
    newprefix.append(suffix.length() - 2, _T(' '));
    newprefix << _T("|");
    SearchTreeLinkMap::const_iterator i;
    unsigned int cnt = 0;
    for (i = m_Children.begin(); i!= m_Children.end(); i++)
    {
        if (cnt == m_Children.size() - 1)
            newprefix[newprefix.length() - 1] = _T(' ');
        tree->GetNode(i->second,false)->Dump(tree,i->second,newprefix,result);
        cnt++;
    }
}

// *** BasicSearchTree ***

BasicSearchTree::BasicSearchTree()
{
    m_Nodes.clear();
    m_Labels.clear();
    m_Points.clear();
    CreateRootNode();
}

BasicSearchTree::~BasicSearchTree()
{
    for (int i = m_Nodes.size() - 1; i >= 0; --i)
    {
        SearchTreeNode* curNode = m_Nodes[i];
        if (curNode)
            delete curNode;
    }
    m_Nodes.clear();
    m_Labels.clear();
    m_Points.clear();
}

void BasicSearchTree::clear()
{
    for (int i = m_Nodes.size() - 1; i >= 0; --i)
    {
        SearchTreeNode* curNode = m_Nodes[i];
        if (curNode)
            delete curNode;
    }
    m_Nodes.clear();
    m_Labels.clear();
    m_Points.clear();
    CreateRootNode();
}

const wxString BasicSearchTree::GetString(size_t n) const
{
    if (n >= m_Points.size())
        return _T("");
    return GetString(m_Points[n],0);
}

wxString BasicSearchTree::GetString(const SearchTreePoint &nn,nSearchTreeNode top) const
{
    wxString result(_T(""));
    wxString tmplabel;
    if (!nn.n || nn.n==top)
        return result;
    const SearchTreeNode *curnode;
    std::vector<wxString> the_strings;
    the_strings.clear();
    for (curnode = m_Nodes[nn.n];curnode && curnode->GetDepth();curnode = curnode->GetParent(this))
    {
        if (nn.depth <= curnode->GetLabelStartDepth()) // Is nn.depth is above this node's edge?
            continue;
        the_strings.push_back(curnode->GetLabel(this));
        if (nn.depth < curnode->GetDepth()) // is nn.depth somewhere in the middle of this node's edge?
            the_strings[the_strings.size()-1] = the_strings[the_strings.size()-1].substr(0,nn.depth - curnode->GetLabelStartDepth());
        if (curnode->GetParent()==top)
            break;
    }
    for (size_t i = the_strings.size();i > 0;--i)
        result << the_strings[i - 1];
    return result;
}

SearchTreeNode* BasicSearchTree::GetNode(nSearchTreeNode n,bool NullOnZero)
{
    SearchTreeNode* result = NULL;
    if ((n || !NullOnZero) && n < m_Nodes.size())
        result = m_Nodes[n];
    return result;
}

bool BasicSearchTree::FindNode(const wxString& s, nSearchTreeNode nparent, SearchTreePoint* result)
{
    SearchTreeNode *parentnode, *childnode;
    nSearchTreeNode nchild;
    size_t top_depth = m_Nodes[nparent]->GetDepth();
    size_t curpos = 0; /* Current position inside the string */
    bool found = false;

    if (s.IsEmpty())
    {
        if (result)
        {
            result->n = nparent;
            result->depth = m_Nodes[result->n]->GetDepth();
        }
        return true;
    }

    do
    {
        parentnode = m_Nodes[nparent];
        if (s.IsEmpty() || curpos >= s.length() ) // If string is empty, return the node and its vertex's length
        {
            if (result)
            {
                result->n = nparent;
                result->depth = top_depth + s.length();
            }
            found = true;
            break;
        }

        nchild = parentnode->GetChild(s[curpos]);
        childnode = GetNode(nchild,true);
        if (!childnode)
        {
            if (result)
            {
                result->n = nparent;
                result->depth = parentnode->GetDepth();
            }
            found = false;
            break;
        }

        unsigned int newdepth = childnode->GetDeepestMatchingPosition(this,s,top_depth);

        if (result)
        {
            result->n = nchild;
            result->depth = newdepth;
        }
        found =(newdepth == childnode->GetDepth() || newdepth == top_depth + s.length());
        curpos = newdepth - top_depth;

        if (found)
            nparent = nchild;
    } while (found);

    return found;
}

SearchTreeNode* BasicSearchTree::CreateNode(unsigned int depth,nSearchTreeNode parent,nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen)
{
    return new SearchTreeNode(depth,parent,label,labelstart,labellen);
}

SearchTreePoint BasicSearchTree::AddNode(const wxString& s, nSearchTreeNode nparent)
{
    SearchTreePoint result(0,0);
    nSearchTreeNode n = 0;
    bool found = this->FindNode(s,nparent,&result);
    if (!found)
    {
        // Create new node

        // If necessary, split the edge with a new node 'middle'
        // If result is exactly a node, middle will be just result.n.
        nSearchTreeNode middle = SplitBranch(result.n,result.depth);

        // Now add the node to the middle node
        SearchTreeNode* newnode;
        wxString newlabel;
        if (m_Nodes[middle]->IsLeaf())
        {
            // If it's a leaf node, just extend the label and change
            // the new node's depth to reflect the changes.
            n = middle;
            newnode = m_Nodes[n];

            // We take the part of the string that corresponds to node middle.
            // Since s starts at nparent's depth, we just get the difference and
            // it will be the position inside the string.
            newlabel = s.substr(m_Nodes[middle]->GetLabelStartDepth() - m_Nodes[nparent]->GetDepth());

            // Modify the leaf node's label to extend the point
            // Since it's a leaf node, we just concatenate to the current label the missing part.
            unsigned int oldlen = newnode->GetDepth() - newnode->GetLabelStartDepth();
            if (oldlen < newlabel.length()) // Safety check against segfaults
            {
                m_Labels[newnode->GetLabelNo()] << newlabel.substr(oldlen);
                m_Labels[newnode->GetLabelNo()].Shrink();
            }
            newnode->SetLabel(newnode->GetLabelNo(),newnode->GetLabelStart(),newlabel.length());
            newnode->RecalcDepth(this);
        }
        else
        {
            // Get the string's depth. This will be the depth of our new leaf node.
            size_t newdepth = m_Nodes[nparent]->GetDepth() + s.length();

            // start = middle's depth - nparent's depth.
            newlabel = s.substr(m_Nodes[middle]->GetDepth() - m_Nodes[nparent]->GetDepth());

            // Now we create the new label to be accessed by the leaf node "newnode".
            m_Labels.push_back(newlabel);
            nSearchTreeLabel nlabel = m_Labels.size() - 1;
            m_Labels[nlabel].Shrink();

            // Finally, we create the new node and link it to "middle".
            newnode = CreateNode(newdepth,middle,nlabel,0,newlabel.length());
            m_Nodes.push_back(newnode);
            n = m_Nodes.size()-1;
            m_Nodes[middle]->m_Children[newlabel[0u]]=n;
        }
        result.n = n;
        result.depth = newnode->GetDepth();

    }
    return result;
}

/// Tells if there is an item for string s
bool BasicSearchTree::HasItem(const wxString& s)
{
    size_t itemno = GetItemNo(s);
    if (!itemno && !s.empty())
        return false;
    return true;
}

size_t BasicSearchTree::GetItemNo(const wxString& s)
{
    SearchTreePoint resultpos;
    if ( !FindNode(s, 0, &resultpos) )
        return 0; // Invalid
    return m_Nodes[resultpos.n]->GetItemNo(resultpos.depth);
}

size_t BasicSearchTree::FindMatches(const wxString& s, std::set<size_t>& result, bool caseSensitive, bool is_prefix)
{
    // NOTE: Current algorithm is suboptimal, but certainly it's much better
    // than an exhaustive search.
    result.clear();
    wxString s2,curcmp,s3;
    SearchTreeNode* curnode = 0;
    BasicSearchTreeIterator it(this);
    SearchTreeItemsMap::const_iterator it2;

    bool matches;

    if (!caseSensitive)
        s2 = s.Lower();
    else
        s2 = s;

    while (!it.Eof())
    {
        matches = false;
        curnode = m_Nodes[*it];
        if (!curnode)
            break; // Error! Found a NULL Node
        if (curnode->m_Depth < s.length())
        {   // Node's string is shorter than S, therefore it CANNOT be a suffix
            // However, we can test if it does NOT match the current string.
            if (!curnode->m_Depth)
                matches = true;
            else
            {
                s3 = s2.substr(curnode->GetLabelStartDepth(),curnode->GetLabelLen());
                curcmp = curnode->GetLabel(this);
                if (!caseSensitive)
                    curcmp = curcmp.Lower();
                matches = (s3 == curcmp);
            }
        }
        else
        {
            if (curnode->GetLabelStartDepth() >= s2.length())
                matches = is_prefix;
            else
            {
                s3 = s2.substr(curnode->GetLabelStartDepth());
                curcmp = curnode->GetLabel(this);
                if (!caseSensitive)
                    curcmp = curcmp.Lower();
                matches = curcmp.StartsWith(s3);
            }

            if (matches)
            {
                // Begin items addition
                if (!is_prefix)
                {
                    // Easy part: Only one length to search
                    it2 = curnode->m_Items.find(s2.length());
                    if (it2 != curnode->m_Items.end())
                        result.insert(it2->second);
                }
                else
                {
                    for (it2 = curnode->m_Items.lower_bound(s2.length()); it2 != curnode->m_Items.end(); ++it2)
                        result.insert(it2->second);
                }
                matches = is_prefix;
                // End items addition
            }
        }
        it.FindNext(matches);
    }
    return result.size();
}

size_t BasicSearchTree::insert(const wxString& s)
{
    size_t itemno = m_Points.size();
    size_t result = 0;
    SearchTreePoint resultpos;
    resultpos = AddNode(s, 0);
    result = m_Nodes[resultpos.n]->AddItemNo(resultpos.depth, itemno);
    if (m_Points.size() < result)
    {
        m_Points.resize(result,SearchTreePoint(0,0));
        m_Points[result] = resultpos;
    }
    else if (m_Points.size() == result)
        m_Points.push_back(resultpos);

    return result;
}

void BasicSearchTree::CreateRootNode()
{
    m_Nodes.push_back(CreateNode(0,0,0,0,0));
    m_Points.push_back(SearchTreePoint(0,0));
}

nSearchTreeNode BasicSearchTree::SplitBranch(nSearchTreeNode n,size_t depth)
{
    if (!n || !m_Nodes[n] || m_Nodes[n]->GetDepth()==depth)
        return n;
    // for !n it returns the rootnode
    // for !m_Nodes[n], it fails by returning n.
    // for m_Nodes[n]->GetDepth()==depth, it's a special case (given position is a node)
    // so we just return n.

    SearchTreeNode* child = m_Nodes[n];

    nSearchTreeNode old_parent = child->GetParent();

    // Create new node "middle", add it to old_parent in place of child.

    // Calculate the parent offset and the new labels' parameters.
    size_t parent_offset = depth - child->GetLabelStartDepth();
    nSearchTreeLabel labelno = child->GetLabelNo();

    unsigned int oldlabelstart = child->GetLabelStart();
    unsigned int oldlabellen = child->GetLabelLen();

    unsigned int middle_start = oldlabelstart;
    unsigned int middle_len = parent_offset;

    unsigned int child_start = middle_start + middle_len;
    unsigned int child_len = oldlabellen - middle_len;

    wxChar middle_char = m_Labels[labelno][middle_start];
    wxChar child_char = m_Labels[labelno][child_start];

    // Now we're ready to create the middle node and update accordingly

    SearchTreeNode* newnode = CreateNode(depth,old_parent,labelno,middle_start,middle_len);
    m_Nodes.push_back(newnode);
    nSearchTreeNode middle = m_Nodes.size() - 1;

    // Add child to middle
    child->SetParent(middle);
    child->SetLabel(labelno,child_start,child_len);
    child->RecalcDepth(this);
    newnode->m_Children[child_char]=n;
    child->UpdateItems(this);

    // Add middle to old_parent
    m_Nodes[old_parent]->m_Children[middle_char]=middle;

    return middle;
}

bool SearchTreeNode::UnSerializeString(const wxString& s,wxString& result)
{
    result.Clear();
    size_t i;
    int mode = 0;
    wxString entity(_T(""));
    unsigned int u;
    for (i = 0;mode >=0 && i<s.length();i++)
    {
        wxChar ch = s[i];
        if (ch==_T('"') || ch==_T('>') || ch==_T('<'))
        {
            mode = -1; // Error
            break;
        }
        switch(mode)
        {
            case 0: // normal
                if (ch==_T('&'))
                {
                    mode = 1;
                    entity.Clear();
                }
                else
                    result << ch;
            case 1: // escaped
                if (ch==_T('&'))
                {
                    mode = -1; // Error
                    break;
                }
                else if (ch==_T(';'))
                {
                    mode = 0;
                    if      (entity==_T("quot"))
                        ch = _T('"');
                    else if (entity==_T("amp"))
                        ch = _T('&');
                    else if (entity==_T("apos"))
                        ch = _T('\'');
                    else if (entity==_T("lt"))
                        ch = _T('<');
                    else if (entity==_T("gt"))
                        ch = _T('>');
                    else if (entity[0]==_T('#') && S2U(entity.substr(1),u))
                        ch = u;
                    else
                    {
                        mode = -1; // Error: Unrecognised entity
                        break;
                    }
                    result << ch;
                }
                break;
            default:
                break;
        }
    }
    if (mode < 0)
        result.Clear();
    return (mode >= 0);
}

bool SearchTreeNode::S2U(const wxString& s,unsigned int& u)
{
    bool is_ok = true;
    u = 0;
    size_t i;
    wxChar ch;
    for (i = 0; is_ok && i < s.length();i++)
    {
        ch = s[i];
        if (ch >= _T('0') && ch <= _T('9'))
        {
            u*=10;
            u+=((unsigned int)ch) & 15;
        }
        else
            is_ok = false; // error
    }
    if (!is_ok)
        u = 0;
    return is_ok;
}

bool SearchTreeNode::S2I(const wxString& s,int& i)
{
    bool is_ok = true;
    i = 0;
    unsigned int u = 0;
    if (!s.IsEmpty())
    {
        if (s[0]==_T('-'))
        {
            if (!S2U(s.substr(1),u))
                is_ok = false;
            else
                i = 0 - u;
        }
        else
        {
            if (!S2U(s.substr(1),u))
                is_ok = false;
            else
                i = u;
        }
    }
    return is_ok;
}

wxString SearchTreeNode::SerializeString(const wxString& s)
{
    wxString result(_T(""));
    size_t i;
    wxChar ch;
    for (i=0;i<s.length();i++)
    {
        ch=s[i];
        switch(ch)
        {
            case _T('"'):
                result << _T("&quot;");break;
            case _T('\''):
                result << _T("&#39;");break;
            case _T('<'):
                result << _T("&lt;");break;
            case _T('>'):
                result << _T("&gt;");break;
            case _T('&'):
                result << _T("&amp;");break;
            default:
                if (ch >= 32 && ch <= 126)
                    result << ch;
                else
                    result << _T("&#") << SearchTreeNode::U2S((unsigned int)ch) << _T(";");
        }
    }
    return result;
}

wxString BasicSearchTree::SerializeLabel(nSearchTreeLabel labelno)
{
    wxString result(_T(""));
    wxString label = m_Labels[labelno];
    result = SearchTreeNode::SerializeString(label);
    return result;
};

wxString BasicSearchTree::SerializeLabels()
{
    wxString result;
    result << _T(" <labels>\n");
    for (unsigned int i=0;i<m_Labels.size();i++)
    {
        result << _T("  <label id=\"") << SearchTreeNode::U2S(i) << _T("\" data=\"") << SerializeLabel(i) << _T("\" />\n");
    }
    result << _T(" </labels>\n");
    return result;
}

wxString BasicSearchTree::dump()
{
    wxString result(_T(""));
    m_Nodes[0]->Dump(this, 0, _T(""), result);
    return result;
}
