/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "searchtree.h"

// *** SearchTreeIterator ***

BasicSearchTreeIterator::BasicSearchTreeIterator() :
m_CurNode(0),
m_eof(false),
m_pTree(0),
m_LastTreeSize(0),
m_LastAddedNode(0)
{
    m_Stack.clear();
    m_Stack2.clear();
}

BasicSearchTreeIterator::BasicSearchTreeIterator(BasicSearchTree* tree) :
m_CurNode(0),
m_eof(false),
m_pTree(tree),
m_LastTreeSize(0),
m_LastAddedNode(0)
{
    if(m_pTree)
    {
        m_LastTreeSize = m_pTree->m_pNodes.size();
        if(m_LastTreeSize)
            m_LastAddedNode = m_pTree->GetNode(m_LastTreeSize - 1);
    }
    m_Stack.clear();
    m_Stack2.clear();
}

bool BasicSearchTreeIterator::IsValid()
{
    if(!this || !m_pTree || m_LastTreeSize!= m_pTree->m_pNodes.size() || m_LastAddedNode != m_pTree->m_pNodes[m_LastTreeSize - 1])
        return false;
    return true;
}

bool BasicSearchTreeIterator::FindPrev(bool includechildren)
{
    bool result;
    result = false;

    SearchTreeLinkMap::iterator it;
    do
    {
        if(!IsValid())
            break;
        SearchTreeNode* curnode = m_pTree->GetNode(m_CurNode);
        if(!curnode)
            break;

        result = true;
        while(m_CurNode)
        {
            m_eof = false;
            result = FindPrevSibling();
            if(!result)
                return false;
            if(!m_eof)
                break;
            m_CurNode = curnode->m_parent;
            curnode = m_pTree->GetNode(m_CurNode);
            if(!curnode)
                return false;
        }

        if(includechildren)
        {
            while(curnode->m_Children.size())
            {
                it = curnode->m_Children.end();
                --it;
                m_CurNode = it->second;
                curnode = m_pTree->GetNode(m_CurNode,true);
                if(!curnode)
                    return false;
            }
        }
        m_eof = false;
        break;
    }while(true);
    return result;
}

bool BasicSearchTreeIterator::FindNext(bool includechildren)
{
    bool result;
    result = false;

    SearchTreeLinkMap::iterator it;
    do
    {
        if(!IsValid())
            break;
        SearchTreeNode* curnode = m_pTree->GetNode(m_CurNode);
        if(!curnode)
            break;

        result = true;
        if(includechildren)
        {
            it = curnode->m_Children.begin();
            if(it != curnode->m_Children.end())
            {
                m_CurNode = it->second;
                curnode = m_pTree->GetNode(m_CurNode);
                if(!curnode)
                {
                    return false;
                }
                break;
            }
        }
        m_eof = true;
        while(m_CurNode)
        {
            m_eof = false;
            result = FindNextSibling();
            if(!m_eof)
                break;
            m_CurNode = curnode->m_parent;
            curnode = m_pTree->GetNode(m_CurNode);
            if(!curnode)
                return false;
        }
        break;
    }while(true);
    return result;
}

bool BasicSearchTreeIterator::FindNextSibling()
{
    if(!IsValid())
        return false;
    if(!m_CurNode /* || !m_Stack.size() */)
        m_eof = true;

    SearchTreeNode* node = m_pTree->GetNode(m_CurNode);
    if(!node)
        return false;
    wxChar ch = node->GetChar(m_pTree);
    node = node->GetParent(m_pTree);
    if(!node)
        return false;
    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::iterator it = the_map->find(ch);
    if(it == the_map->end())
        m_eof = true;
    else
    {
        ++it;
        if(it == the_map->end())
            m_eof = true;
        else
            m_CurNode = it->second;
    }
    return true;
}

bool BasicSearchTreeIterator::FindPrevSibling()
{
    if(!IsValid())
        return false;
    if(!m_CurNode /* || !m_Stack.size() */)
        m_eof = true;

    SearchTreeNode* node = m_pTree->GetNode(m_CurNode);
    if(!node)
        return false;
    wxChar ch = node->GetChar(m_pTree);
    node = node->GetParent(m_pTree);
    if(!node)
        return false;
    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::iterator it = the_map->find(ch);
    if(it == the_map->end())
        m_eof = true;
    else
    {
        if(it == the_map->begin())
            m_eof = true;
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
    if(!IsValid())
        return false;
    if(!m_CurNode /* || !m_Stack.size() */)
        m_eof = true;

    SearchTreeNode* node = m_pTree->GetNode(m_CurNode);
    if(!node)
        return false;
    node = node->GetParent(m_pTree);
    if(!node)
        return false;

    SearchTreeLinkMap* the_map = &node->m_Children;
    SearchTreeLinkMap::iterator it = the_map->find(ch);
    if(it == the_map->end())
        m_eof = true;
    else
    {
        m_CurNode = it->second;
    }
    return true;
}

// *** SearchTreeNode ***

SearchTreeNode::SearchTreeNode():
m_depth(0),
m_parent(0),
m_label(0),
m_labelstart(0),
m_labellen(0)
{
    m_Children.clear();
    m_Items.clear();
}

SearchTreeNode::SearchTreeNode(unsigned int depth,nSearchTreeNode parent,nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen):
m_depth(depth),
m_parent(parent),
m_label(label),
m_labelstart(labelstart),
m_labellen(labellen)
{
    m_Children.clear();
    m_Items.clear();
}

SearchTreeNode::~SearchTreeNode()
{
}

inline nSearchTreeNode SearchTreeNode::GetChild(wxChar ch)
{
    SearchTreeLinkMap::iterator found = m_Children.find(ch);
    if(found == m_Children.end())
        return 0;
    return found->second;
}

inline size_t SearchTreeNode::GetItemNo(size_t depth)
{
    SearchTreeItemsMap::iterator found = m_Items.find(depth);
    if(found == m_Items.end())
        return 0;
    return found->second;
}

size_t SearchTreeNode::AddItemNo(size_t depth,size_t itemno)
{
    SearchTreeItemsMap::iterator found = m_Items.find(depth);
    if(found == m_Items.end())
        m_Items[depth]=itemno;
    else if(found->second==0)
        m_Items[depth]=itemno;
    else
        itemno = found->second;
    return itemno;
}

inline SearchTreeNode* SearchTreeNode::GetParent(const BasicSearchTree* tree) const
{
    if(!m_depth)
        return NULL;
    return tree->m_pNodes[m_parent];
}

inline SearchTreeNode* SearchTreeNode::GetChild(BasicSearchTree* tree,wxChar ch)
{
    nSearchTreeNode child = GetChild(ch);
    return tree->GetNode(child,true);
}

wxString SearchTreeNode::GetLabel(const BasicSearchTree* tree) const
{
    if(!m_depth || m_label >= tree->m_Labels.size())
        return wxString(_T(""));
    return tree->m_Labels[m_label].substr(m_labelstart,m_labellen);
}

inline wxChar SearchTreeNode::GetChar(const BasicSearchTree* tree) const
{
    if(!m_depth)
        return 0;
    const wxString& the_label = GetActualLabel(tree);
    return the_label[m_labelstart];
}

inline const wxString& SearchTreeNode::GetActualLabel(const BasicSearchTree* tree) const
{
    return tree->m_Labels[m_label];
}

inline void SearchTreeNode::SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen)
{
    m_label = label;
    m_labelstart = labelstart;
    m_labellen = labellen;
}

inline unsigned int SearchTreeNode::GetLabelStartDepth() const
{
    if(!m_depth || m_labellen >= m_depth )
        return 0;
    return (m_depth - m_labellen);
}

inline unsigned int SearchTreeNode::GetDeepestMatchingPosition(BasicSearchTree* tree, const wxString& s,unsigned int StringStartDepth)
{
    if(StringStartDepth >= GetDepth())
        return GetDepth();

    if(StringStartDepth + s.length() <= GetLabelStartDepth())
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
    if(i_limit > m_labellen)
        i_limit = m_labellen;

    for(i = 0; i < i_limit; i++)
    {
        if(the_label[m_labelstart+i]!=s[startpos+i])
            break;
    }

    return GetLabelStartDepth() + i;
}

inline void SearchTreeNode::RecalcDepth(BasicSearchTree* tree)
{
    unsigned int curdepth = 0;
    SearchTreeNode *parent = GetParent(tree);
    if(parent)
        curdepth = parent->GetDepth();
    m_depth = curdepth + m_labellen;
}

void SearchTreeNode::UpdateItems(BasicSearchTree* tree)
{
    SearchTreeNode* parentnode = tree->GetNode(m_parent,true);
    if(!parentnode)
       return;
    SearchTreeItemsMap newmap;
    size_t mindepth = parentnode->GetDepth();
    SearchTreeItemsMap::iterator i;
    newmap.clear();
    for(i = m_Items.begin();i!=m_Items.end();i++)
    {
        if(i->first <= mindepth)
            parentnode->m_Items[i->first]=i->second;
        else
            newmap[i->first]=i->second;
    }
    m_Items.clear();
    for(i = newmap.begin();i!=newmap.end();i++)
        m_Items[i->first]=i->second;
}

wxString SearchTreeNode::u2s(unsigned int u)
{
    if(!u)
        return _T("0");
    wxString result(_T("")),revresult(_T(""));
    int i = 0;
    while(u>0)
    {
        revresult <<  (wxChar)(_T('0') + (u % 10));
        u/=10;
        i++;
    }
    while(i>0)
    {
        i--;
        result << revresult[i];
    }
    return result;
}

wxString SearchTreeNode::i2s(int i)
{
    wxString result(_T(""));
    if(i<0)
        result << _T('-');
    result << u2s(abs(i));
    return result;
}

wxString SearchTreeNode::Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren)
{
    wxString result,children,sparent,sdepth,slabelno,slabelstart,slabellen;
    SearchTreeLinkMap::iterator link;
    SearchTreeItemsMap::iterator item;
    sparent = u2s(m_parent);
    sdepth = u2s(m_depth);
    slabelno = u2s(m_label);
    slabelstart = u2s(m_labelstart);
    slabellen = u2s(m_labellen);

    result << _T(" <node id=\"") << node_id << _T("\" parent=\"") << sparent << _T("\"");
    result << _T(" depth=\"") << sdepth << _T("\" label=\"");
    result << slabelno << _T(',') << slabelstart << _T(',') << slabellen;
    result << _T("\">\n");
    result << _T("  <items>\n");
    for(item = m_Items.begin();item != m_Items.end();item++)
    {
        if(item->second)
        {

            result << _T("    <item depth=\"") << u2s(item->first) << _T("\" itemid=\"") << u2s(item->second) <<  _T("\"") << _T(" />\n");
        }
    }
    result << _T("  </items>\n");
    result << _T("  <children>\n");
    for(link = m_Children.begin();link != m_Children.end();link++)
    {
        if(link->second)
        {

            result << _T("    <child char=\"") << SerializeString(wxString(link->first)) << _T("\" nodeid=\"") << u2s(link->second) <<  _T("\"") << _T(" />\n");
        }
    }

    result << _T("  </children>\n");
    result << _T(" </node>\n");
    if(withchildren)
    {
        for(link = m_Children.begin();link != m_Children.end();link++)
        {
            if(link->second)
            {
                result << tree->GetNode(link->second,false)->Serialize(tree,link->second,true);
            }
        }
    }
    return result;
}

void SearchTreeNode::dump(BasicSearchTree* tree,nSearchTreeNode node_id,const wxString& prefix,wxString& result)
{
    wxString suffix(_T(""));
    suffix << _T("- \"") << SerializeString(GetLabel(tree)) << _T("\" (") << u2s(node_id) << _T(")");
    if(prefix.length() && prefix[prefix.length()-1]=='|')
        result << prefix.substr(0,prefix.length()-1) << _T('+') << suffix << _T('\n');
    else if(prefix.length() && prefix[prefix.length()-1]==' ')
        result << prefix.substr(0,prefix.length()-1) << _T('\\') << suffix << _T('\n');
    else
        result << prefix << suffix << _T('\n');
    wxString newprefix(prefix);
    newprefix.append(suffix.length() - 2, _T(' '));
    newprefix << _T("|");
    SearchTreeLinkMap::iterator i;
    unsigned int cnt = 0;
    for(i = m_Children.begin(); i!= m_Children.end(); i++)
    {
        if(cnt == m_Children.size() - 1)
            newprefix[newprefix.length() - 1] = _T(' ');
        tree->GetNode(i->second,false)->dump(tree,i->second,newprefix,result);
        cnt++;
    }
}

// *** BasicSearchTree ***

BasicSearchTree::BasicSearchTree()
{
    m_pNodes.clear();
    m_Labels.clear();
    m_Points.clear();
    CreateRootNode();
}

BasicSearchTree::~BasicSearchTree()
{
    int i;
    SearchTreeNode* curnode;
    for(i = m_pNodes.size(); i > 0;i--)
    {
        curnode = m_pNodes[i-1];
        if(curnode)
            delete curnode;
    }
    m_pNodes.clear();
    m_Labels.clear();
    m_Points.clear();
}

void BasicSearchTree::clear()
{
    int i;
    SearchTreeNode* curnode;
    for(i = m_pNodes.size(); i > 0;i--)
    {
        curnode = m_pNodes[i-1];
        if(curnode)
            delete curnode;
    }
    m_pNodes.clear();
    m_Labels.clear();
    m_Points.clear();
    CreateRootNode();
}

const wxString BasicSearchTree::GetString(size_t n) const
{
    if(n >= m_Points.size())
        return _T("");
    return GetString(m_Points[n],0);
}

wxString BasicSearchTree::GetString(const SearchTreePoint &nn,nSearchTreeNode top) const
{
    wxString result(_T(""));
    wxString tmplabel;
    if(!nn.n || nn.n==top)
        return result;
    const SearchTreeNode *curnode;
    vector<wxString> the_strings;
    the_strings.clear();
    for(curnode = m_pNodes[nn.n];curnode && curnode->GetDepth();curnode = curnode->GetParent(this))
    {
        if(nn.depth <= curnode->GetLabelStartDepth()) // Is nn.depth is above this node's edge?
            continue;
        the_strings.push_back(curnode->GetLabel(this));
        if(nn.depth < curnode->GetDepth()) // is nn.depth somewhere in the middle of this node's edge?
            the_strings[the_strings.size()-1] = the_strings[the_strings.size()-1].substr(0,nn.depth - curnode->GetLabelStartDepth());
        if(curnode->GetParent()==top)
            break;
    }
    for(size_t i = the_strings.size();i > 0;--i)
        result << the_strings[i - 1];
    return result;
}

SearchTreeNode* BasicSearchTree::GetNode(nSearchTreeNode n,bool NullOnZero)
{
    SearchTreeNode* result = NULL;
    if((n || !NullOnZero) && n < m_pNodes.size())
        result = m_pNodes[n];
    return result;
}

bool BasicSearchTree::FindNode(const wxString& s, nSearchTreeNode nparent, SearchTreePoint* result)
{
    SearchTreeNode *parentnode, *childnode;
    nSearchTreeNode nchild;
    size_t top_depth = m_pNodes[nparent]->GetDepth();
    size_t curpos = 0; /* Current position inside the string */
    bool found = false;

    if(s.empty())
    {
        if(result)
        {
            result->n = nparent;
            result->depth = m_pNodes[result->n]->GetDepth();
        }
        return true;
    }

    do
    {
        parentnode = m_pNodes[nparent];
        if(s.empty() || curpos >= s.length() ) // If string is empty, return the node and its vertex's length
        {
            if(result)
            {
                result->n = nparent;
                result->depth = top_depth + s.length();
            }
            found = true;
            break;
        }

        nchild=parentnode->GetChild(s[curpos]);
        childnode = GetNode(nchild,true);
        if(!childnode)
        {
            if(result)
            {
                result->n = nparent;
                result->depth = parentnode->GetDepth();
            }
            found = false;
            break;
        }

        unsigned int newdepth = childnode->GetDeepestMatchingPosition(this,s,top_depth);

        if(result)
        {
            result->n = nchild;
            result->depth = newdepth;
        }
        found =(newdepth == childnode->GetDepth() || newdepth == top_depth + s.length());
        curpos = newdepth - top_depth;
        if(found)
        {
            nparent = nchild;
        }
    }while(found);
    return found;
}

SearchTreeNode* BasicSearchTree::CreateNode(unsigned int depth,nSearchTreeNode parent,nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen)
{
    SearchTreeNode* result = new SearchTreeNode(depth,parent,label,labelstart,labellen);
    return result;
}

SearchTreePoint BasicSearchTree::AddNode(const wxString& s, nSearchTreeNode nparent)
{
    SearchTreePoint result(0,0);
    nSearchTreeNode n = 0;
    bool found = this->FindNode(s,nparent,&result);
    if(!found)
    {
        // Create new node

        // If necessary, split the edge with a new node 'middle'
        // If result is exactly a node, middle will be just result.n.
        nSearchTreeNode middle = SplitBranch(result.n,result.depth);

        // Now add the node to the middle node
        SearchTreeNode* newnode;
        wxString newlabel;
        if(m_pNodes[middle]->IsLeaf())
        {
            // If it's a leaf node, just extend the label and change
            // the new node's depth to reflect the changes.
            n = middle;
            newnode = m_pNodes[n];

            // We take the part of the string that corresponds to node middle.
            // Since s starts at nparent's depth, we just get the difference and
            // it will be the position inside the string.
            newlabel = s.substr(m_pNodes[middle]->GetLabelStartDepth() - m_pNodes[nparent]->GetDepth());

            // Modify the leaf node's label to extend the point
            // Since it's a leaf node, we just concatenate to the current label the missing part.
            unsigned int oldlen = newnode->GetDepth() - newnode->GetLabelStartDepth();
            if(oldlen < newlabel.length()) // Safety check against segfaults
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
            size_t newdepth = m_pNodes[nparent]->GetDepth() + s.length();

            // start = middle's depth - nparent's depth.
            newlabel = s.substr(m_pNodes[middle]->GetDepth() - m_pNodes[nparent]->GetDepth());

            // Now we create the new label to be accessed by the leaf node "newnode".
            m_Labels.push_back(newlabel);
            nSearchTreeLabel nlabel = m_Labels.size() - 1;
            m_Labels[nlabel].Shrink();

            // Finally, we create the new node and link it to "middle".
            newnode = CreateNode(newdepth,middle,nlabel,0,newlabel.length());
            m_pNodes.push_back(newnode);
            n = m_pNodes.size()-1;
            m_pNodes[middle]->m_Children[newlabel[0u]]=n;
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
    if(!itemno && !s.empty())
        return false;
    return true;
}

size_t BasicSearchTree::GetItemNo(const wxString& s)
{
    SearchTreePoint resultpos;
    if(!FindNode(s, 0, &resultpos))
        return 0; // Invalid
    return m_pNodes[resultpos.n]->GetItemNo(resultpos.depth);
}

size_t BasicSearchTree::FindMatches(const wxString& s,set<size_t> &result,bool caseSensitive,bool is_prefix)
{

    // NOTE: Current algorithm is suboptimal, but certainly it's much better
    // than an exhaustive search.


    result.clear();
    wxString s2,curcmp,s3;
    nSearchTreeNode ncurnode;
    SearchTreeNode* curnode = 0;
    BasicSearchTreeIterator it(this);
    SearchTreeItemsMap::iterator it2;

    bool matches;

    if(!caseSensitive)
        s2 = s.Lower();
    else
        s2 = s;

    while(!it.eof())
    {
        matches = false;
        ncurnode = *it;
        curnode = m_pNodes[*it];
        if(!curnode)
            break; // Error! Found a NULL Node
        if(curnode->m_depth < s.length())
        {   // Node's string is shorter than S, therefore it CANNOT be a suffix
            // However, we can test if it does NOT match the current string.
            if(!curnode->m_depth)
                matches = true;
            else
            {
                s3 = s2.substr(curnode->GetLabelStartDepth(),curnode->GetLabelLen());
                curcmp = curnode->GetLabel(this);
                if(!caseSensitive)
                    curcmp = curcmp.Lower();
                matches = (s3 == curcmp);
            }
        }
        else
        {
            if(curnode->GetLabelStartDepth() >= s2.length())
                matches = is_prefix;
            else
            {
                s3 = s2.substr(curnode->GetLabelStartDepth());
                curcmp = curnode->GetLabel(this);
                if(!caseSensitive)
                    curcmp = curcmp.Lower();
                matches = curcmp.StartsWith(s3);
            }

            if(matches)
            {
                // Begin items addition
                if(!is_prefix)
                {
                    // Easy part: Only one length to search
                    it2 = curnode->m_Items.find(s2.length());
                    if(it2 != curnode->m_Items.end())
                        result.insert(it2->second);
                }
                else
                {
                    for(it2 = curnode->m_Items.lower_bound(s2.length()); it2 != curnode->m_Items.end(); ++it2)
                    {
                        result.insert(it2->second);
                    }
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
    result = m_pNodes[resultpos.n]->AddItemNo(resultpos.depth,itemno);
    if(m_Points.size() < result)
    {
        m_Points.resize(result,SearchTreePoint(0,0));
        m_Points[result] = resultpos;
    }
    else if(m_Points.size() == result)
    {
        m_Points.push_back(resultpos);
    }
    return result;
}

void BasicSearchTree::CreateRootNode()
{
    m_pNodes.push_back(CreateNode(0,0,0,0,0));
    m_Points.push_back(SearchTreePoint(0,0));
}

nSearchTreeNode BasicSearchTree::SplitBranch(nSearchTreeNode n,size_t depth)
{
    if(!n || !m_pNodes[n] || m_pNodes[n]->GetDepth()==depth)
        return n;
    // for !n it returns the rootnode
    // for !m_pNodes[n], it fails by returning n.
    // for m_pNodes[n]->GetDepth()==depth, it's a special case (given position is a node)
    // so we just return n.

    SearchTreeNode* child = m_pNodes[n];

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
    m_pNodes.push_back(newnode);
    nSearchTreeNode middle = m_pNodes.size() - 1;

    // Add child to middle
    child->SetParent(middle);
    child->SetLabel(labelno,child_start,child_len);
    child->RecalcDepth(this);
    newnode->m_Children[child_char]=n;
    child->UpdateItems(this);

    // Add middle to old_parent
    m_pNodes[old_parent]->m_Children[middle_char]=middle;

    return middle;
}

bool SearchTreeNode::UnSerializeString(const wxString& s,wxString& result)
{
    result.Clear();
    size_t i;
    int mode = 0;
    wxString entity(_T(""));
    unsigned int u;
    for(i = 0;mode >=0 && i<s.length();i++)
    {
        wxChar ch = s[i];
        if(ch==_T('"') || ch==_T('>') || ch==_T('<'))
        {
            mode = -1; // Error
            break;
        }
        switch(mode)
        {
            case 0: // normal
                if(ch==_T('&'))
                {
                    mode = 1;
                    entity.Clear();
                }
                else
                    result << ch;
            case 1: // escaped
                if(ch==_T('&'))
                {
                    mode = -1; // Error
                    break;
                }
                else if(ch==_T(';'))
                {
                    mode = 0;
                    if(entity==_T("quot"))
                        ch = _T('"');
                    else if(entity==_T("amp"))
                        ch = _T('&');
                    else if(entity==_T("apos"))
                        ch = _T('\'');
                    else if(entity==_T("lt"))
                        ch = _T('<');
                    else if(entity==_T("gt"))
                        ch = _T('>');
                    else if(entity[0]==_T('#') && s2u(entity.substr(1),u))
                        ch = u;
                    else
                    {
                        mode = -1; // Error: Unrecognized entity
                        break;
                    }
                    result << ch;
                }
            break;
        }
    }
    if(mode < 0)
        result.Clear();
    return (mode >= 0);
}

bool SearchTreeNode::s2u(const wxString& s,unsigned int& u)
{
    bool is_ok = true;
    u = 0;
    size_t i;
    wxChar ch;
    for(i = 0; is_ok && i < s.length();i++)
    {
        ch = s[i];
        if(ch >= _T('0') && ch <= _T('9'))
        {
            u*=10;
            u+=((unsigned int)ch) & 15;
        }
        else
            is_ok = false; // error
    }
    if(!is_ok)
        u = 0;
    return is_ok;
}

bool SearchTreeNode::s2i(const wxString& s,int& i)
{
    bool is_ok = true;
    i = 0;
    unsigned int u = 0;
    if(!s.IsEmpty())
    {
        if(s[0]==_T('-'))
        {
            if(!s2u(s.substr(1),u))
                is_ok = false;
            else
                i = 0 - u;
        }
        else
        {
            if(!s2u(s.substr(1),u))
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
    for(i=0;i<s.length();i++)
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
                if(ch >= 32 && ch <= 126)
                    result << ch;
                else
                    result << _T("&#") << SearchTreeNode::u2s((unsigned int)ch) << _T(";");
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
    unsigned int i;
    for(i=0;i<m_Labels.size();i++)
    {
        result << _T("  <label id=\"") << SearchTreeNode::u2s(i) << _T("\" data=\"") << SerializeLabel(i) << _T("\" />\n");
    }
    result << _T(" </labels>\n");
    return result;
}

wxString BasicSearchTree::dump()
{
    wxString result(_T(""));
    m_pNodes[0]->dump(this,0,_T(""),result);
    return result;
}
