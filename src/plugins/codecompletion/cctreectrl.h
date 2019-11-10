/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef CCTREECTRL
#define CCTREECTRL

#include <wx/treectrl.h>

#include <queue>

#include "parser/parser.h" // BrowserSortType
#include "parser/token.h"  // TokenKind

class wxPoint;
class wxSize;
class wxWindow;

class Token;

/** Specify which kind of node it is */
enum SpecialFolder
{
    sfToken   = 0x0001, // token node
    sfRoot    = 0x0002, // root node
    sfGFuncs  = 0x0004, // global funcs node
    sfGVars   = 0x0008, // global vars node
    sfPreproc = 0x0010, // preprocessor symbols node
    sfTypedef = 0x0020, // typedefs node
    sfBase    = 0x0040, // base classes node
    sfDerived = 0x0080, // derived classes node
    sfMacro   = 0x0100  // global macro node
};

/** Actual data stored with each node in the symbol tree */
class CCTreeCtrlData : public wxTreeItemData
{
public:
    CCTreeCtrlData(SpecialFolder sf = sfToken, Token* token = 0,
                   short int kindMask = 0xffff, int parentIdx = -1);

    /** a pointer to the associated Token instance in the TokenTree */
    Token*        m_Token;

    /** a copy of Token::m_KindMask
     * @todo this variable is not used?
     */
    short int     m_KindMask;

    /** the node's kind, it could be "root", "normal token", @sa SpecialFolder */
    SpecialFolder m_SpecialFolder;

    /** a copy of Token::m_TokenIndex */
    int           m_TokenIndex;

    /** a copy of Token::m_TokenKind */
    TokenKind     m_TokenKind;

    /** short name of the Token,
     * it is a copy of Token::m_Name
     */
    wxString      m_TokenName;

    /** the parent Token index
     * it is a copy of Token::m_ParentIndex, @sa Token::m_ParentIndex
     * @todo this variable is not used?
     */
    int           m_ParentIndex;

    /** the Token's ticket in a TokenTree
     * This is actually a copy of Token::m_Ticket, @sa Token::m_Ticket  */
    unsigned long m_Ticket;
};

class CCTreeCtrlExpandedItemData
{
public:
    CCTreeCtrlExpandedItemData(const CCTreeCtrlData* data, const int level);

    int   GetLevel() const          { return m_Level; }
    const CCTreeCtrlData& GetData() { return m_Data;  }
private:
    CCTreeCtrlData m_Data;  // copy of tree item data
    int            m_Level; // nesting level in the tree
};

typedef std::deque<CCTreeCtrlExpandedItemData> ExpandedItemVect;
typedef std::deque<CCTreeCtrlData>             SelectedItemPath;

class CCTreeCtrl : public wxTreeCtrl
{
public:
    CCTreeCtrl();
    CCTreeCtrl(wxWindow* parent, const wxWindowID id, const wxPoint& pos,
               const wxSize& size, long style);

    void SetCompareFunction(const BrowserSortType type);
    void RemoveDoubles(const wxTreeItemId& parent);

protected:
    static int CBAlphabetCompare(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBKindCompare    (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBScopeCompare   (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBLineCompare    (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);
    static int CBNoCompare      (CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);

    int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);
    int (*Compare)(CCTreeCtrlData* lhs, CCTreeCtrlData* rhs);

    DECLARE_DYNAMIC_CLASS(CCTreeCtrl)
};

#endif // CCTREECTRL
