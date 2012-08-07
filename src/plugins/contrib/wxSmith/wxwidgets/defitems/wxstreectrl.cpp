/** \file wxstreectrl.cpp
*
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
* This code is based in part on wxsimagecombobox from the wxSmithImage plug-in,
* copyright Ron Collins and released under the GPL.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxstreectrl.h"
#include "wxsimagelist.h"
#include "../properties/wxsimagelisteditordlg.h"
#include "../properties/wxsimagetreeeditordlg.h"

namespace
{
    wxsRegisterItem<wxsTreeCtrl> Reg(_T("TreeCtrl"), wxsTWidget, _T("Standard"), 10);

    WXS_ST_BEGIN(wxsTreeCtrlStyles, _T("wxTR_DEFAULT_STYLE"))
    WXS_ST_CATEGORY("wxTreeCtrl")
    WXS_ST(wxTR_EDIT_LABELS)
    WXS_ST(wxTR_NO_BUTTONS)
    WXS_ST(wxTR_HAS_BUTTONS)
    WXS_ST(wxTR_TWIST_BUTTONS)
    WXS_ST(wxTR_NO_LINES)
    WXS_ST(wxTR_FULL_ROW_HIGHLIGHT)
    WXS_ST(wxTR_LINES_AT_ROOT)
    WXS_ST(wxTR_HIDE_ROOT)
    WXS_ST(wxTR_ROW_LINES)
    WXS_ST(wxTR_HAS_VARIABLE_ROW_HEIGHT)
    WXS_ST(wxTR_SINGLE)
    WXS_ST(wxTR_MULTIPLE)
    WXS_ST(wxTR_EXTENDED)
    WXS_ST(wxTR_DEFAULT_STYLE)
    WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsTreeCtrlEvents)
    WXS_EVI(EVT_TREE_BEGIN_DRAG, wxEVT_COMMAND_TREE_BEGIN_DRAG, wxTreeEvent, BeginDrag)
    WXS_EVI(EVT_TREE_BEGIN_RDRAG, wxEVT_COMMAND_TREE_BEGIN_RDRAG, wxTreeEvent, BeginRDrag)
    WXS_EVI(EVT_TREE_END_DRAG, wxEVT_COMMAND_TREE_END_DRAG, wxTreeEvent, EndDrag)
    WXS_EVI(EVT_TREE_BEGIN_LABEL_EDIT, wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEvent, BeginLabelEdit)
    WXS_EVI(EVT_TREE_END_LABEL_EDIT, wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEvent, EndLabelEdit)
    WXS_EVI(EVT_TREE_DELETE_ITEM, wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEvent, DeleteItem)
    WXS_EVI(EVT_TREE_GET_INFO, wxEVT_COMMAND_TREE_GET_INFO, wxTreeEvent, GetInfo)
    WXS_EVI(EVT_TREE_SET_INFO, wxEVT_COMMAND_TREE_SET_INFO, wxTreeEvent, SetInfo)
    WXS_EVI(EVT_TREE_ITEM_ACTIVATED, wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEvent, ItemActivated)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSED, wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEvent, ItemCollapsed)
    WXS_EVI(EVT_TREE_ITEM_COLLAPSING, wxEVT_COMMAND_TREE_ITEM_COLLAPSING, wxTreeEvent, ItemCollapsing)
    WXS_EVI(EVT_TREE_ITEM_EXPANDED, wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEvent, Item_bExpanded)
    WXS_EVI(EVT_TREE_ITEM_EXPANDING, wxEVT_COMMAND_TREE_ITEM_EXPANDING, wxTreeEvent, Item_bExpanding)
    WXS_EVI(EVT_TREE_ITEM_RIGHT_CLICK, wxEVT_COMMAND_TREE_ITEM_RIGHT_CLICK, wxTreeEvent, ItemRightClick)
    WXS_EVI(EVT_TREE_ITEM_MIDDLE_CLICK, wxEVT_COMMAND_TREE_ITEM_MIDDLE_CLICK, wxTreeEvent, ItemMiddleClick)
    WXS_EVI(EVT_TREE_SEL_CHANGED, wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEvent, SelectionChanged)
    WXS_EVI(EVT_TREE_SEL_CHANGING, wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEvent, SelectionChanging)
    WXS_EVI(EVT_TREE_KEY_DOWN, wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEvent, KeyDown)
    WXS_EVI(EVT_TREE_ITEM_GETTOOLTIP, wxEVT_COMMAND_TREE_ITEM_GETTOOLTIP, wxTreeEvent, ItemGetToolTip)
    WXS_EVI(EVT_TREE_ITEM_MENU, wxEVT_COMMAND_TREE_ITEM_MENU, wxTreeEvent, ItemMenu)
    WXS_EV_END()
}

wxsTreeCtrl::wxsTreeCtrl(wxsItemResData *Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsTreeCtrlEvents,
        wxsTreeCtrlStyles),
    m_sImageList(_("<none>")),
    m_bExpand(false)
{
    m_arrItems.Clear();
}

/*! \brief Create the initial control.
 *
 * \return void
 *
 */
void wxsTreeCtrl::OnBuildCreatingCode()
{
    int             i, n;
    int             iRed, iGreen, iBlue;
    wxsImageList   *ilist;
    int             iLevel, iImg1, iImg2, iImg3, iImg4;
    wxColor         colour;
    bool            bBold;
    wxString        sText;
    wxString        arrItems[32];
    wxString        sSource;
    wxString        sTop;
    wxString        sVarName;
    wxString        sItem;
    wxString        sPrevItem;


    switch(GetLanguage())
    {
        case wxsCPP:
            {
                AddHeader(_T("<wx/treectrl.h>"), GetInfo().ClassName, 0);
                AddHeader(_T("<wx/treectrl.h>"), _T("wxTreeEvent"), 0);
                Codef(_T("%C(%W, %I, %P, %S, %T, %V, %N);\n"));

                // assign the image-list -- AFTER the image list has been built
                sVarName = GetVarName();
                ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(this, m_sImageList);
                if(ilist != NULL)
                {
                    // Locator comment.
                    AddEventCode(wxString::Format(_("// Set the images for %s.\n"), sVarName.wx_str()));

                    #if wxCHECK_VERSION(2, 9, 0)
                    sSource.Printf(_T("%s->SetImageList(%s);\n"), sVarName.wx_str(), m_sImageList.wx_str());
                    #else
                    sSource.Printf(_T("%s->SetImageList(%s);\n"), sVarName.c_str(), m_sImageList.c_str());
                    #endif
                    AddEventCode(sSource);
                }

                // and now each item in the tree data list
                sTop = wxEmptyString;
                arrItems[0] = wxEmptyString;
                n = 0;
                for(i = 2;i < (int)m_arrItems.GetCount();i++)
                {
                    // the string, broken into pieces
                    sSource = m_arrItems.Item(i);
                    wxsImageTreeEditorDlg::ParseTreeItem(sSource, iLevel, colour, bBold, iImg1, iImg2, iImg3, iImg4, sText);

                    // make a name for the new item
                    n += 1;
                    sItem.Printf(_("_Item%d"), n);
                    sItem = sVarName + sItem;

                    arrItems[iLevel] = sItem;

                    // there is a problem with wxTR_HIDE_ROOT and ScrollTo(root), so make sure that
                    // we only scroll to first shown item
                    if(i == 3)
                    {
                        sTop = sItem;
                    }

                    // ID of parent item
                    if(iLevel <= 0)
                    {
                        sPrevItem = wxEmptyString;
                    }
                    else
                    {
                        sPrevItem = arrItems[iLevel - 1];
                    }

                    // make the new item -- level 0 is the root item
                    if(iLevel <= 0)
                    {
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("wxTreeItemId %s = %s->AddRoot(%n);\n"), sItem.wx_str(), sVarName.wx_str(), sText.wx_str());
                        #else
                        Codef(_T("wxTreeItemId %s = %s->AddRoot(%n);\n"), sItem.c_str(), sVarName.c_str(), sText.c_str());
                        #endif
                    }
                    else
                    {
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("wxTreeItemId %s = %s->AppendItem(%s, %n);\n"), sItem.wx_str(), sVarName.wx_str(), sPrevItem.wx_str(), sText.wx_str());
                        #else
                        Codef(_T("wxTreeItemId %s = %s->AppendItem(%s, %n);\n"), sItem.c_str(), sVarName.c_str(), sPrevItem.c_str(), sText.c_str());
                        #endif
                    }

                    // set text color of current item if not the default color of black
                    iRed = colour.Red();
                    iGreen = colour.Green();
                    iBlue = colour.Blue();
                    if((colour.IsOk()) && ((iRed + iGreen + iBlue) != 0))
                    {
                        sSource.Printf(_("%d,%d,%d"), iRed, iGreen, iBlue);
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("%ASetItemTextColour(%s, wxColour(%s));\n"), sItem.wx_str(), sSource.wx_str());
                        #else
                        Codef(_T("%ASetItemTextColour(%s, wxColour(%s));\n"), sItem.c_str(), sSource.c_str());
                        #endif
                    }

                    if(bBold)
                    {
                        Codef(_T("%ASetItemBold(%s, true);\n"), sItem.wx_str());
                    }

                    // and the image-list indices
                    if(ilist != NULL)
                    {
                        if(iImg1 >= 0)
                        {
                            #if wxCHECK_VERSION(2, 9, 0)
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Normal);\n"), sVarName.wx_str(), sItem.wx_str(), iImg1);
                            #else
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Normal);\n"), sVarName.c_str(), sItem.c_str(), iImg1);
                            #endif
                            AddEventCode(sSource);
                        }
                        if(iImg2 >= 0)
                        {
                            #if wxCHECK_VERSION(2, 9, 0)
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Selected);\n"), sVarName.wx_str(), sItem.wx_str(), iImg2);
                            #else
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Selected);\n"), sVarName.c_str(), sItem.c_str(), iImg2);
                            #endif
                            AddEventCode(sSource);
                        }
                        if(iImg3 >= 0)
                        {
                            #if wxCHECK_VERSION(2, 9, 0)
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Expanded);\n"), sVarName.wx_str(), sItem.wx_str(), iImg3);
                            #else
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_Expanded);\n"), sVarName.c_str(), sItem.c_str(), iImg3);
                            #endif
                            AddEventCode(sSource);
                        }
                        if(iImg4 >= 0)
                        {
                            #if wxCHECK_VERSION(2, 9, 0)
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_SelectedExpanded);\n"), sVarName.wx_str(), sItem.wx_str(), iImg4);
                            #else
                            sSource.Printf(_T("%s->SetItemImage(%s, %d, wxTreeItemIcon_SelectedExpanded);\n"), sVarName.c_str(), sItem.c_str(), iImg4);
                            #endif
                            AddEventCode(sSource);
                        }
                    }
                }

                // show everything
                if(m_bExpand)
                {
                    Codef(_T("%AExpandAll();\n"));
                }
                if(sTop.Length() > 0)
                {
                    Codef(_T("%AScrollTo(%s);\n"), sTop.wx_str());
                }

                BuildSetupWindowCode();
                return;
            }

        default:
            {
                wxsCodeMarks::Unknown(_T("wxsTreeCtrl::OnBuildCreatingCode"), GetLanguage());
            }
    }
}

/*! \brief    Build the control preview.
 *
 * \param parent wxWindow*    The parent window.
 * \param flags long                    The control flags.
 * \return wxObject*                     The constructed control.
 *
 */
wxObject *wxsTreeCtrl::OnBuildPreview(wxWindow *Parent, long Flags)
{
    int             i, n;
    wxsImageList   *ilist;
    int             iLevel, iImg1, iImg2, iImg3, iImg4;
    wxColor         colour;
    bool            bBold;
    wxString        sText;
    wxTreeItemId    idItems[32];
    wxString        sSource;
    wxTreeItemId    item;
    wxTreeItemId    top;

    wxTreeCtrl *preview = new wxTreeCtrl(Parent, GetId(), Pos(Parent), Size(Parent), Style());
    preview->DeleteAllItems();

    // make an image list
    m_imageList.RemoveAll();
    ilist = (wxsImageList *) wxsImageListEditorDlg::FindTool(this, m_sImageList);
    if(ilist != NULL)
    {
        ilist->GetImageList(m_imageList);
    }

    preview->SetImageList(&m_imageList);

    // and all of the items
    n = m_arrItems.GetCount();
    for(i=2;i < n;i++)
    {
        sSource = m_arrItems.Item(i);
        wxsImageTreeEditorDlg::ParseTreeItem(sSource, iLevel, colour, bBold, iImg1, iImg2, iImg3, iImg4, sText);
        if(iLevel == 0)
        {
            item = preview->AddRoot(sText);
        }
        else
        {
            item = preview->AppendItem(idItems[iLevel-1], sText);
        }
        idItems[iLevel] = item;

        // there is a problem with wxTR_HIDE_ROOT and ScrollTo(root), so make sure that
        // we only scroll to first shown item
        if(i == 3)
        {
            top = item;
        }

        // set colors and images
        if(colour.IsOk())
        {
            preview->SetItemTextColour(item, colour);
        }
        preview->SetItemBold(item, bBold);
        preview->SetItemImage(item, iImg1, wxTreeItemIcon_Normal);
        preview->SetItemImage(item, iImg2, wxTreeItemIcon_Selected);
        preview->SetItemImage(item, iImg3, wxTreeItemIcon_Expanded);
        preview->SetItemImage(item, iImg4, wxTreeItemIcon_SelectedExpanded);
    };

    // show it
    if(m_bExpand)
    {
        preview->ExpandAll();
    }
    if(top.IsOk())
    {
        preview->ScrollTo(top);
    }

    // save the hide-root flag
    top.Unset();

    return SetupWindow(preview, Flags);
}

/*! \brief Enumerate the control's properties.
 *
 * \param flags long    The control flags.
 * \return void
 *
 */
void wxsTreeCtrl::OnEnumWidgetProperties(long Flags)
{
    static wxString     sImageNames[128];
    static const wxChar *pImageNames[128];

    int                 i, n;
    wxString            ss, tt;
    wxArrayString       aa;

    // find available image lists and store them in our local static arrays
    FindAllImageLists(aa);
    n = aa.GetCount();
    if(n > 127)
    {
        n = 127;
    }

    for(i = 0; i < n; i++)
    {
        ss = aa.Item(i);
        sImageNames[i] = ss;
        pImageNames[i] = (const wxChar *) sImageNames[i];
    }
    pImageNames[n] = NULL;

    WXS_EDITENUM(wxsTreeCtrl, m_sImageList, _("Image List"), _T("image_list"), pImageNames, _("<none>"))

    // The list of items to appear in the tree.
    UpdateTreeItemList();

    WXS_IMAGETREE(wxsTreeCtrl, m_arrItems, _("Tree Items"), wxT("tree_items"));
    WXS_ARRAYSTRING(wxsTreeCtrl, m_arrItems, _("Items as Text"), wxT("items_text"), _("item2"));
    WXS_BOOL(wxsTreeCtrl, m_bExpand, _("Expand All"), _("expand_all"), false);
}

/*! \brief Find all tools that are image lists and return their names.
 *
 * \param aNames wxArrayString&
 * \return void
 *
 */
void wxsTreeCtrl::FindAllImageLists(wxArrayString &aNames)
{
    int             i, n;
    wxsItemResData  *res;
    wxsTool         *tool;
    wxString        ss, tt;

    // start the list with a chance to de-select any old list
    aNames.Clear();
    aNames.Add(_("<none>"));

    // find all tools that are "wxImageList"
    res = GetResourceData();
    n   = res->GetToolsCount();
    for (i = 0;i < n;i++)
    {
        tool = res->GetTool(i);
        ss   = tool->GetUserClass();

        if ((ss == wxT("wxImageList")) && (n < 127))
        {
            ss = tool->GetVarName();
            aNames.Add(ss);
        }
    }
}

/*! \brief .Update the list of combo items to send to the wsxImageTreeEditorDlg.
 *
 * \return void
 *
 */
void wxsTreeCtrl::UpdateTreeItemList()
{
    int              i, n;
    wxString         ss, tt;
    wxArrayString    aa;

    // first 2 items are always our var name and the name of the image list
    aa.Clear();
    ss = GetVarName();
    aa.Add(ss);
    ss = m_sImageList;
    aa.Add(ss);

    // then copy over everything else the user entered last time
    n = m_arrItems.GetCount();
    for(i = 2;i < n;i++)
    {
        ss = m_arrItems.Item(i);
        aa.Add(ss);
    }

    // then put back in original list
    m_arrItems.Clear();
    n = aa.GetCount();
    for(i = 0;i < n;i++)
    {
        ss = aa.Item(i);
        m_arrItems.Add(ss);
    }

    // make sure that FindTool has a valid wxsItem* to work from in the dialog
    wxsImageListEditorDlg::FindTool(this, m_sImageList);
}
