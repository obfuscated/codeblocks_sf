#include <wx/string.h>
#include "searchtree.h"

#ifdef __WXWINDOWS__

  #define string wxString
  #define char wxChar

#else

  #define _T(x) (x)
  #define _(x) (x)

#endif

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

inline nSearchTreeNode SearchTreeNode::GetChild(char ch)
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

inline SearchTreeNode* SearchTreeNode::GetParent(BasicSearchTree* tree)
{
    if(!m_depth)
        return NULL;
    return tree->m_pNodes[m_parent];
}

inline SearchTreeNode* SearchTreeNode::GetChild(BasicSearchTree* tree,char ch)
{
    nSearchTreeNode child = GetChild(ch);
    return tree->GetNode(child,true);
}

string SearchTreeNode::GetLabel(BasicSearchTree* tree)
{
    if(!m_depth || m_label >= tree->m_Labels.size())
        return string(_T(""));
    return tree->m_Labels[m_label].substr(m_labelstart,m_labellen);
}

inline const string& SearchTreeNode::GetActualLabel(BasicSearchTree* tree)
{
    return tree->m_Labels[m_label];
}

inline void SearchTreeNode::SetLabel(nSearchTreeLabel label, unsigned int labelstart, unsigned int labellen)
{
    m_label = label;
    m_labelstart = labelstart;
    m_labellen = labellen;
}

inline unsigned int SearchTreeNode::GetLabelStartDepth()
{
    if(!m_depth || m_labellen >= m_depth )
        return 0;
    return (m_depth - m_labellen);
}

inline unsigned int SearchTreeNode::GetDeepestMatchingPosition(BasicSearchTree* tree, const string& s,unsigned int StringStartDepth)
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
    const string& the_label = GetActualLabel(tree);
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

string SearchTreeNode::u2s(unsigned int u)
{
    if(!u)
        return _T("0");
    string result(_T("")),revresult(_T(""));
    int i = 0;
    while(u>0)
    {
        revresult <<  (char)(_T('0') + (u % 10));
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

string SearchTreeNode::Serialize(BasicSearchTree* tree,nSearchTreeNode node_id,bool withchildren)
{
    string result,children,sparent,sdepth,slabelno,slabelstart,slabellen;
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

            result << _T("    <child char=\"") << SerializeString(string(link->first)) << _T("\" nodeid=\"") << u2s(link->second) <<  _T("\"") << _T(" />\n");
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

void SearchTreeNode::dump(BasicSearchTree* tree,nSearchTreeNode node_id,const string& prefix,string& result)
{
    string suffix(_T(""));
    suffix << _T("- \"") << SerializeString(GetLabel(tree)) << _T("\" (") << u2s(node_id) << _T(")");
    if(prefix.length() && prefix[prefix.length()-1]=='|')
        result << prefix.substr(0,prefix.length()-1) << _T('+') << suffix << _T('\n');
    else if(prefix.length() && prefix[prefix.length()-1]==' ')
        result << prefix.substr(0,prefix.length()-1) << _T('\\') << suffix << _T('\n');
    else
        result << prefix << suffix << _T('\n');
    string newprefix(prefix);
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
}

void BasicSearchTree::Clear()
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
    CreateRootNode();
}

string BasicSearchTree::GetString(SearchTreePoint nn,nSearchTreeNode top)
{
    string result(_T(""));
    string tmplabel;
    if(!nn.n || nn.n==top)
        return result;
    SearchTreeNode *curnode;
    for(curnode = m_pNodes[nn.n];curnode && curnode->GetDepth();curnode = curnode->GetParent(this))
    {
        if(nn.depth <= curnode->GetLabelStartDepth()) // Is nn.depth is above this node's edge?
            continue;
        tmplabel = curnode->GetLabel(this);
        if(nn.depth < curnode->GetDepth()) // is nn.depth somewhere in the middle of this node's edge?
            tmplabel = tmplabel.substr(0,nn.depth - curnode->GetLabelStartDepth());
        result << tmplabel;
        if(curnode->GetParent()==top)
            break;
    }
    return result;
}

SearchTreeNode* BasicSearchTree::GetNode(nSearchTreeNode n,bool NullOnZero)
{
    SearchTreeNode* result = NULL;
    if((n || !NullOnZero) && n < m_pNodes.size())
        result = m_pNodes[n];
    return result;
}

bool BasicSearchTree::FindNode(const string& s, nSearchTreeNode nparent, SearchTreePoint* result)
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

SearchTreePoint BasicSearchTree::AddNode(const string& s, nSearchTreeNode nparent)
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
        string newlabel;
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
                m_Labels[newnode->GetLabelNo()] << newlabel.substr(oldlen);
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
bool BasicSearchTree::HasItem(const string& s)
{
    size_t itemno = GetItemNo(s);
    if(!itemno && !s.empty())
        return false;
    return true;
}

size_t BasicSearchTree::GetItemNo(const string& s)
{
    SearchTreePoint resultpos;
    if(!FindNode(s, 0, &resultpos))
        return 0; // Invalid
    return m_pNodes[resultpos.n]->GetItemNo(resultpos.depth);
}

size_t BasicSearchTree::AddItemNo(const string& s,size_t itemno)
{
    size_t result = 0;
    SearchTreePoint resultpos;
    resultpos = AddNode(s, 0);
    result = m_pNodes[resultpos.n]->AddItemNo(resultpos.depth,itemno);
    return result;
}

void BasicSearchTree::CreateRootNode()
{
    m_pNodes.push_back(CreateNode(0,0,0,0,0));
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

    char middle_char = m_Labels[labelno][middle_start];
    char child_char = m_Labels[labelno][child_start];

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

string SearchTreeNode::SerializeString(const string& s)
{
    string result(_T(""));
    size_t i;
    char ch;
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
string BasicSearchTree::SerializeLabel(nSearchTreeLabel labelno)
{
    string result(_T(""));
    string label = m_Labels[labelno];
    result = SearchTreeNode::SerializeString(label);
    return result;
};

string BasicSearchTree::SerializeLabels()
{
    string result;
    result << _T(" <labels>\n");
    unsigned int i;
    for(i=0;i<m_Labels.size();i++)
    {
        result << _T("  <label id=\"") << SearchTreeNode::u2s(i) << _T("\" data=\"") << SerializeLabel(i) << _T("\" />\n");
    }
    result << _T(" </labels>\n");
    return result;
}

string BasicSearchTree::dump()
{
    string result(_T(""));
    m_pNodes[0]->dump(this,0,_T(""),result);
    return result;
}
