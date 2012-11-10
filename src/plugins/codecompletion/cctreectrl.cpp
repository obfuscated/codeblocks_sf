/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #ifdef CC_BUILDTREE_MEASURING
        #include <wx/stopwatch.h>
    #endif
#endif

#include <wx/gdicmn.h> // wxPoint, wxSize

#include "cctreectrl.h"

// class CCTreeCtrlData

CCTreeCtrlData::CCTreeCtrlData(SpecialFolder sf, Token* token, short int kindMask, int parentIdx) :
    m_Token(token),
    m_KindMask(kindMask),
    m_SpecialFolder(sf),
    m_TokenIndex(token ? token->m_Index : -1),
    m_TokenKind(token ? token->m_TokenKind : tkUndefined),
    m_TokenName(token ? token->m_Name : _T("")),
    m_ParentIndex(parentIdx),
    m_Ticket(token ? token->GetTicket() : 0)
{
}

// class CCTreeCtrlExpandedItemData

CCTreeCtrlExpandedItemData::CCTreeCtrlExpandedItemData(const CCTreeCtrlData* data, const int level) :
    m_Data(*data),
    m_Level(level)
{
}

// class CCTreeCtrl

IMPLEMENT_DYNAMIC_CLASS(CCTreeCtrl, wxTreeCtrl)

CCTreeCtrl::CCTreeCtrl()
{
   Compare = &CBNoCompare;
}

CCTreeCtrl::CCTreeCtrl(wxWindow *parent, const wxWindowID id,
                       const wxPoint& pos, const wxSize& size, long style) :
    wxTreeCtrl(parent, id, pos, size, style)
{
   Compare = &CBNoCompare;
}

void CCTreeCtrl::SetCompareFunction(const BrowserSortType type)
{
    switch (type)
    {
        case bstAlphabet:
            Compare = &CBAlphabetCompare;
            break;
        case bstKind:
            Compare = &CBKindCompare;
            break;
        case bstScope:
            Compare = &CBScopeCompare;
            break;
        case bstNone:
        default:
            Compare = &CBNoCompare;
            break;
    }

}

int CCTreeCtrl::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    return Compare((CCTreeCtrlData*)GetItemData(item1), (CCTreeCtrlData*)GetItemData(item2));
}

int CCTreeCtrl::CBAlphabetCompare (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (!lhs->m_Token || !rhs->m_Token)
        return 1;
    return wxStricmp(lhs->m_Token->m_Name, rhs->m_Token->m_Name);
}

int CCTreeCtrl::CBKindCompare(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;
    if (lhs->m_TokenKind == rhs->m_TokenKind)
        return CBAlphabetCompare(lhs, rhs);

    return lhs->m_TokenKind - rhs->m_TokenKind;
}

int CCTreeCtrl::CBScopeCompare(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs)
{
    if (!lhs || !rhs)
        return 1;
    if (lhs->m_SpecialFolder != sfToken || rhs->m_SpecialFolder != sfToken)
        return -1;

    if (lhs->m_Token->m_Scope == rhs->m_Token->m_Scope)
        return CBKindCompare(lhs, rhs);

    return rhs->m_Token->m_Scope - lhs->m_Token->m_Scope;
}

int CCTreeCtrl::CBNoCompare(cb_unused CCTreeCtrlData* lhs, cb_unused CCTreeCtrlData* rhs)
{
    return 0;
}

// This does not really do what it says !
// It only removes doubles, if they are neighbours, so the tree should be sorted !!
// The last one (after sorting) remains.
void CCTreeCtrl::RemoveDoubles(const wxTreeItemId& parent)
{
    if (Manager::IsAppShuttingDown() || (!(parent.IsOk())))
        return;

#ifdef CC_BUILDTREE_MEASURING
    wxStopWatch sw;
#endif
    // we 'll loop backwards so we can delete nodes without problems
    wxTreeItemId existing = GetLastChild(parent);
    while (parent.IsOk() && existing.IsOk())
    {
        wxTreeItemId prevItem = GetPrevSibling(existing);
        if (!prevItem.IsOk())
            break;
        CCTreeCtrlData* dataExisting = (CCTreeCtrlData*)(GetItemData(existing));
        CCTreeCtrlData* dataPrev = (CCTreeCtrlData*)(GetItemData(prevItem));
        if (dataExisting &&
           dataPrev &&
           dataExisting->m_SpecialFolder == sfToken &&
           dataPrev->m_SpecialFolder == sfToken &&
           dataExisting->m_Token &&
           dataPrev->m_Token &&
           (dataExisting->m_Token->DisplayName() == dataPrev->m_Token->DisplayName()))
        {
            Delete(prevItem);
        }
        else if (existing.IsOk())
            existing = GetPrevSibling(existing);
    }
#ifdef CC_BUILDTREE_MEASURING
    CCLogger::Get()->DebugLog(F(_T("RemoveDoubles took : %ld"), sw.Time()));
#endif
}
