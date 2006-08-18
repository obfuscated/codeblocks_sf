#include <sdk.h>
#include "classbrowserbuilderthread.h"
#include <globals.h>

ClassBrowserBuilderThread::ClassBrowserBuilderThread(Parser* parser,
                                                    wxTreeCtrl& tree,
                                                    const wxString& active_filename,
                                                    BrowserOptions options,
                                                    TokensTree* pTokens,
                                                    ClassBrowserBuilderThread** threadVar)
    : wxThread(wxTHREAD_DETACHED),
    m_pParser(parser),
    m_Tree(tree),
    m_ActiveFilename(active_filename),
    m_Options(options),
    m_pTokens(pTokens),
    m_ppThreadVar(threadVar)
{
    //ctor
}

ClassBrowserBuilderThread::~ClassBrowserBuilderThread()
{
    //dtor
}

void* ClassBrowserBuilderThread::Entry()
{
    BuildTree();
    if (m_ppThreadVar)
        *m_ppThreadVar = 0;
    return 0;
}

void ClassBrowserBuilderThread::BuildTree()
{
    wxCriticalSectionLocker lock(s_MutexProtection);

    if (TestDestroy())
        return;

    wxArrayString treeState;
    wxTreeItemId root = m_Tree.GetRootItem();
    if (root.IsOk())
        ::SaveTreeState(&m_Tree, root, treeState);

	m_Tree.Freeze();
    m_Tree.DeleteAllItems();
    TokenFilesSet currset;
    currset.clear();
    Token* token = 0;

    if (TestDestroy())
    {
        m_Tree.Thaw();
        return;
    }

    // "mark" tokens based on scope
    bool fnameEmpty = m_ActiveFilename.IsEmpty();
    m_ActiveFilename.Append(_T('.'));
    if(!fnameEmpty && !m_Options.showAllSymbols)
    {
        for(size_t i = 1; i < m_pTokens->m_FilenamesMap.size(); ++i)
        {
            if(m_pTokens->m_FilenamesMap.GetString(i).StartsWith(m_ActiveFilename))
                currset.insert(i);
        }
    }

    if (TestDestroy())
    {
        m_Tree.Thaw();
        return;
    }

	root = m_Tree.AddRoot(_("Symbols"), PARSER_IMG_SYMBOLS_FOLDER);
	if (m_Options.viewFlat)
	{
        TokenIdxSet::iterator it,it_end;
        it = m_pTokens->m_GlobalNameSpace.begin();
        it_end = m_pTokens->m_GlobalNameSpace.end();

        for(;it != it_end;++it)
        {
            token = m_pTokens->at(*it);
            if(!token || !token->m_IsLocal || token->m_ParentIndex!=-1 || !token->MatchesFiles(currset))
                continue;
            AddTreeNode(root, token);

            if (TestDestroy())
            {
                m_Tree.Thaw();
                return;
            }
        }
		m_Tree.SortChildren(root);
	}
	else
	{
        wxTreeItemId globalNS = m_Tree.AppendItem(root, _("Global namespace"), PARSER_IMG_NAMESPACE);
        AddTreeNamespace(globalNS, 0,currset);
        if (TestDestroy())
        {
            m_Tree.Thaw();
            return;
        }
        BuildTreeNamespace(root, 0,currset);
        if (TestDestroy())
        {
            m_Tree.Thaw();
            return;
        }
	}

    if (root.IsOk())
    {
        ::RestoreTreeState(&m_Tree, root, treeState);
        if (!m_Tree.IsExpanded(root))
            m_Tree.Expand(root);
    }
	m_Tree.Thaw();
	// wxString memdump = m_pTokens->m_Tree.Serialize();
	// Manager::Get()->GetMessageManager()->DebugLog(memdump);
}

void ClassBrowserBuilderThread::BuildTreeNamespace(const wxTreeItemId& parentNode, Token* parent, const TokenFilesSet& currset)
{
	TokenIdxSet::iterator it,it_end;
	int parentidx;
	if(!parent)
	{
        it = m_pTokens->m_TopNameSpaces.begin();
        it_end = m_pTokens->m_TopNameSpaces.end();
        parentidx = -1;
	}
    else
    {
        it = parent->m_Children.begin();
        it_end = parent->m_Children.end();
        parentidx = parent->GetSelf();
    }

    bool hasCurrset = currset.size() != 0;
	for(;it != it_end; it++)
	{
	    Token* token = m_pTokens->at(*it);
	    if(!token || /* !token->m_Bool || */ (!hasCurrset && !token->m_IsLocal) || token->m_TokenKind != tkNamespace)
            continue;
//        if(hasCurrset && !token->MatchesFiles(currset))
//            continue;
//        Manager::Get()->GetMessageManager()->DebugLog(_T("  + Matching namespace: ") + token->m_Name);
        ClassTreeData* ctd = new ClassTreeData(token);
        wxTreeItemId newNS = m_Tree.AppendItem(parentNode, token->m_Name, PARSER_IMG_NAMESPACE, -1, ctd);
        BuildTreeNamespace(newNS, token, currset);
        AddTreeNamespace(newNS, token, currset);
        // remove branch if empty
        if (!m_Tree.ItemHasChildren(newNS))
            m_Tree.Delete(newNS);
        if (TestDestroy())
            return;
	}
    m_Tree.SortChildren(parentNode);
}

void ClassBrowserBuilderThread::AddTreeNamespace(const wxTreeItemId& parentNode, Token* parent,const TokenFilesSet& currset)
{
	TokenIdxSet::iterator it,it_end;
	int parentidx;
	if(!parent)
	{
        it = m_pTokens->m_GlobalNameSpace.begin();
        it_end = m_pTokens->m_GlobalNameSpace.end();
        parentidx = -1;
	}
    else
    {
        it = parent->m_Children.begin();
        it_end = parent->m_Children.end();
        parentidx = parent->GetSelf();
    }

	bool has_typedefs = false,has_classes = false,has_enums = false,has_preprocessor = false,has_others = false;
	wxTreeItemId node_typedefs;
	wxTreeItemId node_classes;
	wxTreeItemId node_enums;
	wxTreeItemId node_preprocessor;
	wxTreeItemId node_others;
	wxTreeItemId* curnode = 0;

    bool hasCurrset = currset.size() != 0;
	for(;it != it_end; it++)
	{
        if (TestDestroy())
            return;

	    Token* token = m_pTokens->at(*it);
	    if(!token || /* !token->m_Bool || */ (!hasCurrset && !token->m_IsLocal))
            continue;
        if(currset.size() && !token->MatchesFiles(currset))
            continue;

        switch(token->m_TokenKind)
        {
            case tkClass:
                    if(token->m_IsTypedef)
                    {
                        if (!has_typedefs)
                        {
                            has_typedefs = true;
                            node_typedefs = m_Tree.AppendItem(parentNode, _("Typedefs"), PARSER_IMG_TYPEDEF_FOLDER);
                        }
                        curnode = &node_typedefs;
                    }
                    else
                    {
                        if(!has_classes)
                        {
                            has_classes = true;
                            node_classes = m_Tree.AppendItem(parentNode, _("Classes"), PARSER_IMG_CLASS_FOLDER);
                        }
                        curnode = &node_classes;
                    }
                    break;
            case tkEnum:
                    if(!has_enums)
                    {
                        has_enums = true;
                        node_enums = m_Tree.AppendItem(parentNode, _("Enums"), PARSER_IMG_ENUMS_FOLDER);
                    }
                    curnode = &node_enums;
                    break;
            case tkPreprocessor:
                    if(!has_preprocessor)
                    {
                        has_preprocessor = true;
                        node_preprocessor = m_Tree.AppendItem(parentNode, _("Preprocessor"), PARSER_IMG_PREPROC_FOLDER);
                    }
                    curnode = &node_preprocessor;
                    break;
            case tkEnumerator:
            case tkFunction:
            case tkVariable:
            case tkUndefined:
                    if(!has_others)
                    {
                        has_others = true;
                        node_others = m_Tree.AppendItem(parentNode, _("Others"), PARSER_IMG_OTHERS_FOLDER);
                    }
                    curnode = &node_others;
                    break;
            default:curnode = 0;
        }
        if(curnode)
            AddTreeNode(*curnode, token);
	}
    if(has_classes)
        m_Tree.SortChildren(node_classes);
    if (TestDestroy())
        return;
    if(has_enums)
        m_Tree.SortChildren(node_enums);
    if (TestDestroy())
        return;
    if(has_preprocessor)
        m_Tree.SortChildren(node_preprocessor);
    if (TestDestroy())
        return;
    if(has_others)
        m_Tree.SortChildren(node_others);
}

void ClassBrowserBuilderThread::AddTreeNode(const wxTreeItemId& parentNode, Token* token, bool childrenOnly)
{
    if (!token)
        return;
	ClassTreeData* ctd = new ClassTreeData(token);
	int image = -1;

	image = m_pParser->GetTokenKindImage(token);

	wxString str = token->m_Name;
	if (token->m_TokenKind == tkFunction || token->m_TokenKind == tkConstructor || token->m_TokenKind == tkDestructor)
        str << token->m_Args;
	if (!token->m_ActualType.IsEmpty())
		 str = str + _T(" : ") + token->m_ActualType;
	wxTreeItemId node = childrenOnly ? parentNode : m_Tree.AppendItem(parentNode, str, image, -1, ctd);

	// add children
	TokenIdxSet::iterator it;
	for(it=token->m_Children.begin();it!=token->m_Children.end();++it)
	{
	    AddTreeNode(node, m_pTokens->at(*it));
	}

	if (!m_Options.showInheritance || (token->m_TokenKind != tkClass && token->m_TokenKind != tkNamespace))
		return;
	// add ancestor's children
	for(it=token->m_Ancestors.begin();it!=token->m_Ancestors.end();++it)
	{
	    AddTreeNode(node, m_pTokens->at(*it),true);
	}

    m_Tree.SortChildren(node);
}
