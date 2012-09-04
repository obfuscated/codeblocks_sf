/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include    <iostream>
#include    "wxsTreeListCtrl.h"

using namespace std;


//-----------------------------------------------------------------------------
// max levels we can reasonably expect a tree to reach
// this is not the max number of items, but max depth of the hierarchy

#define     kLMAX           24

//------------------------------------------------------------------------------

namespace
{
#include "images/TreeList16.xpm"
#include "images/TreeList32.xpm"

wxsRegisterItem<wxsTreeListCtrl> Reg (
    _T ("wxTreeListCtrl"),          // Class name
    wxsTWidget,                     // Item type
    _T ("wxWindows"),               // License
    _T ("Ron Collins"),             // Author
    _T ("rcoll@theriver.com"),      // Author's email
    _T (""),                        // Item's homepage
    _T ("Contrib"),                // Category in palette
    30,                             // Priority in palette
    _T ("TreeListCtrl"),            // Base part of names for new items
    wxsCPP,                         // List of coding languages supported by this item
    1, 0,                           // Version
    wxBitmap (TreeList32_xpm),      // 32x32 bitmap
    wxBitmap (TreeList16_xpm),      // 16x16 bitmap
    false);                         // We do not allow this item inside XRC files



WXS_ST_BEGIN (wxsTreeListCtrlStyles,_T ("wxTR_DEFAULT_STYLE") )
WXS_ST_CATEGORY ("wxTreeListCtrl")
WXS_ST (wxTR_EDIT_LABELS)
WXS_ST (wxTR_NO_BUTTONS)
WXS_ST (wxTR_HAS_BUTTONS)
WXS_ST (wxTR_TWIST_BUTTONS)
WXS_ST (wxTR_NO_LINES)
WXS_ST (wxTR_FULL_ROW_HIGHLIGHT)
WXS_ST (wxTR_LINES_AT_ROOT)
WXS_ST (wxTR_HIDE_ROOT)
WXS_ST (wxTR_ROW_LINES)
WXS_ST (wxTR_HAS_VARIABLE_ROW_HEIGHT)
WXS_ST (wxTR_SINGLE)
WXS_ST (wxTR_MULTIPLE)
#if !wxCHECK_VERSION(2, 9, 0)
WXS_ST (wxTR_EXTENDED)
#endif
WXS_ST (wxTR_DEFAULT_STYLE)
WXS_ST (wxTR_VIRTUAL)
WXS_ST_DEFAULTS()
WXS_ST_END()

WXS_EV_BEGIN (wxsTreeListCtrlEvents)
WXS_EVI (EVT_TREE_BEGIN_DRAG,wxEVT_COMMAND_TREE_BEGIN_DRAG,wxTreeEvent,BeginDrag)
WXS_EVI (EVT_TREE_BEGIN_RDRAG,wxEVT_COMMAND_TREE_BEGIN_RDRAG,wxTreeEvent,BeginRDrag)
WXS_EVI (EVT_TREE_END_DRAG,wxEVT_COMMAND_TREE_END_DRAG,wxTreeEvent,EndDrag)
WXS_EVI (EVT_TREE_BEGIN_LABEL_EDIT,wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT,wxTreeEvent,BeginLabelEdit)
WXS_EVI (EVT_TREE_END_LABEL_EDIT,wxEVT_COMMAND_TREE_END_LABEL_EDIT,wxTreeEvent,EndLabelEdit)
WXS_EVI (EVT_TREE_DELETE_ITEM,wxEVT_COMMAND_TREE_DELETE_ITEM,wxTreeEvent,DeleteItem)
WXS_EVI (EVT_TREE_GET_INFO,wxEVT_COMMAND_TREE_GET_INFO,wxTreeEvent,GetInfo)
WXS_EVI (EVT_TREE_SET_INFO,wxEVT_COMMAND_TREE_SET_INFO,wxTreeEvent,SetInfo)
WXS_EVI (EVT_TREE_ITEM_ACTIVATED,wxEVT_COMMAND_TREE_ITEM_ACTIVATED,wxTreeEvent,ItemActivated)
WXS_EVI (EVT_TREE_ITEM_COLLAPSED,wxEVT_COMMAND_TREE_ITEM_COLLAPSED,wxTreeEvent,ItemCollapsed)
WXS_EVI (EVT_TREE_ITEM_COLLAPSING,wxEVT_COMMAND_TREE_ITEM_COLLAPSING,wxTreeEvent,ItemCollapsing)
WXS_EVI (EVT_TREE_ITEM_EXPANDED,wxEVT_COMMAND_TREE_ITEM_EXPANDED,wxTreeEvent,ItemExpanded)
WXS_EVI (EVT_TREE_ITEM_EXPANDING,wxEVT_COMMAND_TREE_ITEM_EXPANDING,wxTreeEvent,ItemExpanding)
WXS_EVI (EVT_TREE_ITEM_RIGHT_CLICK,wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK,wxTreeEvent,ItemRightClick)
WXS_EVI (EVT_TREE_ITEM_MIDDLE_CLICK,wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK,wxTreeEvent,ItemMiddleClick)
WXS_EVI (EVT_TREE_SEL_CHANGED,wxEVT_COMMAND_TREE_SEL_CHANGED,wxTreeEvent,SelectionChanged)
WXS_EVI (EVT_TREE_SEL_CHANGING,wxEVT_COMMAND_TREE_SEL_CHANGING,wxTreeEvent,SelectionChanging)
WXS_EVI (EVT_TREE_KEY_DOWN,wxEVT_COMMAND_TREE_KEY_DOWN,wxTreeEvent,KeyDown)
WXS_EVI (EVT_TREE_ITEM_GETTOOLTIP,wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP,wxTreeEvent,ItemGetToolTip)
WXS_EVI (EVT_TREE_ITEM_MENU,wxEVT_COMMAND_TREE_ITEM_MENU,wxTreeEvent,ItemMenu)
WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsTreeListCtrl::wxsTreeListCtrl (wxsItemResData* Data) :
    wxsWidget (
        Data,
        &Reg.Info,
        wxsTreeListCtrlEvents,
        wxsTreeListCtrlStyles)
{

    m_ColCount = 5;
    m_ColWidth = 64;
    m_ColTitles.Clear();
    m_ReadOnly = false;

    m_ColTitles.Add (_ ("!Define Column Titles") );
    m_ColTitles.Add (_ ("!Text lines that begin with \"!\" are ignored, but blank") );
    m_ColTitles.Add (_ ("!lines are significant (a blank line is a blank title for") );
    m_ColTitles.Add (_ ("!a column).") );
    m_ColTitles.Add (_ ("First") );
    m_ColTitles.Add (_ ("Second") );
    m_ColTitles.Add (_ ("Third") );
    m_ColTitles.Add (_ ("Fourth") );
    m_ColTitles.Add (_ ("Fifth") );

    m_TreeData.Add (_ ("! Define node data for a wxTreeCtrl.") );
    m_TreeData.Add (_ ("! Note that all lines that start with a \"!\" are comment lines and ignored.") );
    m_TreeData.Add (_ ("! Also, all empty lines are ignored (a line of spaces is not an empty line).") );
    m_TreeData.Add (_ ("!") );
    m_TreeData.Add (_ ("! The node data is organized similar to a Tabbed Hierarchical Values (THV) file,") );
    m_TreeData.Add (_ ("! such that each line of text represents one label for a single tree node.") );
    m_TreeData.Add (_ ("! The number of leading TAB characters indicate the indentation level of the") );
    m_TreeData.Add (_ ("! tree node.") );
    m_TreeData.Add (_ ("!") );
    m_TreeData.Add (_ ("! Space (or blank) characters are not used the same as TAB characters; if your") );
    m_TreeData.Add (_ ("! editor substitutes spaces for TABs, use the \"TAB Char\" attribute to change") );
    m_TreeData.Add (_ ("! the character used to indicate the tree node level.") );
    m_TreeData.Add (_ ("!") );
    m_TreeData.Add (_ ("! Following is a sample tree data definition:") );
    m_TreeData.Add (_ ("") );
    m_TreeData.Add (_ ("First level, no tabs\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("\tSecond level, 1 tab\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("\t\tThird level, 2 tabs\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("First level again, no tabs\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("\tSecond level again, 1 tab\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("\tAnother second level, 1 tab\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("\t\tThird level again, 2 tabs\tSecond Column\tThird Column\tFourth Column\tFifth Column") );
    m_TreeData.Add (_ ("") );



}

//------------------------------------------------------------------------------

void wxsTreeListCtrl::OnBuildCreatingCode()
{

    if (GetLanguage() == wxsCPP)
    {
        AddHeader (_T ("<wx/treelistctrl.h>"),GetInfo().ClassName,0);
        Codef (_T ("%C(%W, %I, %P, %S, %T, %V, %N);\n") );
        BuildItemCode();
        BuildSetupWindowCode();
    }
    else
    {
        wxsCodeMarks::Unknown (_T ("wxsTreeCtrl::OnBuildCreatingCode"),GetLanguage() );
    };
}

//------------------------------------------------------------------------------

wxObject* wxsTreeListCtrl::OnBuildPreview (wxWindow* Parent,long Flags)
{
    wxTreeListCtrl*  Preview;

    Preview = new wxTreeListCtrl (Parent, GetId(), Pos (Parent), Size (Parent), Style() );
    PreviewItemCode (Preview);
    SetupWindow (Preview, Flags);

    return Preview;
}

//------------------------------------------------------------------------------

void wxsTreeListCtrl::OnEnumWidgetProperties (long Flags)
{
    static const long    TabValues[] = {        0,       1,       2,  0};
    static const wxChar* TabNames[]  = {_T ("TAB"), _T ("+"), _T (">"),  0};


// how many columns?

    WXS_LONG (wxsTreeListCtrl, m_ColCount, _ ("Column Count"), _ ("colcount"), 1);

// default size of each column

    WXS_LONG (wxsTreeListCtrl, m_ColWidth, _ ("Column Width"), _ ("colwidth"), 64);

// select the char to delimit the node levels

    WXS_ENUM (wxsTreeListCtrl, m_TabChar, _ ("TAB Char"), _ ("tabchar"), TabValues, TabNames, 0);

// pre-load column titles

    WXS_ARRAYSTRING (wxsTreeListCtrl, m_ColTitles, _ ("Column Titles"), _ ("coltitles"), _ ("item") );

// allow user to pre-load tree node data

    WXS_ARRAYSTRING (wxsTreeListCtrl, m_TreeData, _ ("Tree Items"), _ ("treedata"), _ ("node") );

// is this tree read-only?

    WXS_BOOL (wxsTreeListCtrl, m_ReadOnly, _ ("Read Only"), _ ("readonly"), false);

}

//------------------------------------------------------------------------------

void wxsTreeListCtrl::BuildItemCode (void)
{
    int             i,n;
    int             j,k;
    wxString        idname;
    wxString        ss, tt, pp;
    int             id[kLMAX];
    wxArrayString   items;


// enuough columns?

    if (m_ColCount < 1) { m_ColCount = 1; }

// add all the col info

    n = 0;
    for (i=0; i < (int)m_ColTitles.GetCount(); i++)
    {
        if (n >= m_ColCount) { break; }

        ss = m_ColTitles.Item (i);
        ss.Trim (true);
        ss.Trim (false);
        if (ss[0] == '!') { continue; }

        Codef (_ ("%AAddColumn(%t, %ld);\n"), ss.wx_str(), m_ColWidth);
        n += 1;
    };

// count number of nodes, add 1 for the root

    n = 0;
    for (i=0; i < (int)m_TreeData.GetCount(); i++)
    {
        ss = m_TreeData.Item (i);
        if (ss.Len() == 0) { continue; }
        if (ss[0] == '!') { continue; }

        n += 1;
    };

    n += 1;

// name of the array of node IDs

    idname = GetVarName() + _ ("ID");

// add declaration for that many IDs

    ss.Printf (_ ("wxTreeItemId    %s[%d];"), idname.wx_str(), n);
    AddDeclaration (ss);

// make sure there is no false readings

    for (i=0; i < kLMAX; i++) { id[i] = 0; }

// make our own root item

    Codef (_ ("%s[0] = %AAddRoot(_(\"(root)\"));\n"), idname.wx_str() );
    id[0] = 0;

// and now each item in the tree data list

    k = 0;
    for (i=0; i < (int)m_TreeData.GetCount(); i++)
    {

// the string

        ss = m_TreeData.Item (i);

// some lines are skipped over

        if (ss.Len() == 0) { continue; }
        if (ss[0] == '!') { continue; }

// get the level and the label texts

        ParseItems (ss, n, items);
        if (n <= 0) { n = 1; }

// ID of the new item

        k += 1;
        id[n] = k;
        ss.Printf (_ ("%s[%d]"), idname.c_str(), k);

// the parent item

        j = id[n-1];
        pp.Printf (_ ("%s[%d]"), idname.c_str(), id[n-1]);

// base text of the new item

        if (items.GetCount() == 0) { tt = _ (""); }
        else { tt = items.Item (0); }

// make the base item

        Codef (_ ("%s = %AAppendItem(%s, _(\"%s\"));\n"), ss.wx_str(), pp.wx_str(), tt.wx_str() );

// now each of the sub-items

        for (j=1; j < (int)items.GetCount(); j++)
        {
            if (j >= m_ColCount) { break; }
            tt = items.Item (j);
            Codef (_ ("%ASetItemText(%s, %d, %t);\n"), ss.wx_str(), j, tt.wx_str() );
        };
    };

// show everything

    Codef (_ ("%AExpandAll(%s[0]);\n"), idname.wx_str() );

// read-only or editable?

    for (i=0; i<m_ColCount; i++)
    {
        Codef (_ ("%ASetColumnEditable(%d, %b);\n"), i, m_ReadOnly);
    };


}

//------------------------------------------------------------------------------

void wxsTreeListCtrl::PreviewItemCode (wxTreeListCtrl* inPreview)
{
    int             i,n;
    int             j;
    wxString        ss, tt;
    wxTreeItemId    id[kLMAX];
    wxArrayString   items;

// enuough columns?

    if (m_ColCount < 1) { m_ColCount = 1; }

// add all the col info

    n = 0;
    for (i=0; i < (int)m_ColTitles.GetCount(); i++)
    {
        if (n >= m_ColCount) { break; }

        ss = m_ColTitles.Item (i);
        ss.Trim (true);
        ss.Trim (false);
        if (ss[0] == '!') { continue; }

        inPreview->AddColumn (ss, m_ColWidth);
        n += 1;
    };

// make sure there is no false readings

    for (i=0; i < kLMAX; i++) { id[i].Unset(); }

// make our own root item

    id[0] = inPreview->AddRoot (_ ("(root)") );

// and now each item in the tree data list

    for (i=0; i < (int)m_TreeData.GetCount(); i++)
    {

// the string

        ss = m_TreeData.Item (i);

// some lines are skipped over

        if (ss.Len() == 0) { continue; }
        if (ss[0] == '!') { continue; }

// get the level and the label texts

        ParseItems (ss, n, items);
        if (n <= 0) { n = 1; }

// make a new item, checking for valid parent

        if (! id[n-1].IsOk() ) { return; }

        if (items.GetCount() == 0) { tt = _ (""); }
        else { tt = items.Item (0); }

        id[n] = inPreview->AppendItem (id[n-1], tt);

// then all the sub-items

        for (j=1; j < (int)items.GetCount(); j++)
        {
            if (j >= m_ColCount) { break; }
            tt = items.Item (j);
            inPreview->SetItemText (id[n], j, tt);
        };
    };

// show everything

    inPreview->ExpandAll (id[0]);

// read only?

    for (i=0; i<m_ColCount; i++)
    {
        inPreview->SetColumnEditable (i, m_ReadOnly);
    };

}

//------------------------------------------------------------------------------

void wxsTreeListCtrl::ParseItems (wxString inSource, int& outLevel, wxArrayString& outItems)
{
    int         i,n;
    int         tab;
    wxString    ss, tt;

// the correct tab char

    if      (m_TabChar == 0) { tab = '\t'; }
    else if (m_TabChar == 1) { tab = '+'; }
    else if (m_TabChar == 2) { tab = '>'; }
    else { tab = '\t'; }

// working copy

    ss = inSource;

// delete old junk

    outItems.Clear();

// count the level depth; level is always 1 higher than tab count

    n = 0;
    while ( (ss.Len() > 0) && (ss[0] == tab) )
    {
        n += 1;
        ss.erase (0, 1);
    };
    outLevel = n + 1;

// copy over labels

    i = 0;
    while (i < (int)ss.Len() )
    {
        tt = _ ("");
        while ( (i < (int)ss.Len() ) && (ss[i] != tab) ) { tt += ss[i++]; }
        outItems.Add (tt);
        if (i < (int)ss.Len() ) { i += 1; }
    };

// and the last item

    tt = ss;
    tt.Trim (true);
    tt.Trim (false);
    outItems.Add (tt);
}







