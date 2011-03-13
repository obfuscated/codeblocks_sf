/*
* This file is part of wxSmithAui plugin for Code::Blocks Studio
* Copyright (C) 2008-2009  César Fernández Domínguez
*
* wxSmithAui is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmithAui is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmithAui. If not, see <http://www.gnu.org/licenses/>.
*/

#include "wxsAuiToolBar.h"

#if wxCHECK_VERSION(2,8,9)

#include <wxsadvqppchild.h>
#include <wxwidgets/wxsitemresdata.h>
#include <wx/menu.h>
#include <wx/textdlg.h>
#include <wxwidgets/wxsflags.h>

#include "../wxAuiManager/wxsAuiManager.h"
#include "wxSmithAuiToolBar.h"
#include "wxsAuiToolBarItem.h"
#include "wxsAuiToolBarSeparator.h"
#include "wxsAuiToolBarLabel.h"
#include "wxsAuiToolBarSpacer.h"

using namespace wxsFlags;

//(*Headers(wxsAuiToolBarParentQP)
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
//*)

//(*InternalHeaders(wxsAuiToolBarParentQP)
#include <wx/string.h>
#include <wx/intl.h>
//*)

namespace
{
    #include "../images/wxsAuiToolBar16.xpm"
    #include "../images/wxsAuiToolBar32.xpm"

    wxsRegisterItem<wxsAuiToolBar> Reg(
        _T("wxAuiToolBar"),                 // Class name
        wxsTContainer,                      // Item type
        _T("wxWindows"),                    // License
        _T("Benjamin I. Williams"),         // Author
        _T(""),                             // Author's email
        _T(""),                             // Item's homepage
        _T("Aui"),                          // Category in palette
        40,                                 // Priority in palette
        _T("AuiToolBar"),                  // Base part of names for new items
        wxsCPP,                             // List of coding languages supported by this item
        2, 8,                               // Version
        wxBitmap(wxsAuiToolBar32_xpm),      // 32x32 bitmap
        wxBitmap(wxsAuiToolBar16_xpm),      // 16x16 bitmap
        false);                             // Allow in XRC

    /** \brief Extra parameters for aui notebook's children */
    class wxsAuiToolBarExtra: public wxsPropertyContainer
    {
        public:

            wxsAuiToolBarExtra():
                m_Label(_("Item label")),
                m_Enabled(true)
            {}

            wxString m_Label;
            bool     m_Enabled;

        protected:

            virtual void OnEnumProperties(long Flags)
            {
                WXS_SHORT_STRING_P(wxsAuiToolBarExtra,m_Label,_("Aui ToolBar label"),_T("label"),_T(""),false,500);
                WXS_BOOL_P(wxsAuiToolBarExtra,m_Enabled,_("Aui ToolBar Enabled"),_T("enabled"),true,499);
            }
    };

    WXS_ST_BEGIN(wxsAuiToolBarStyles,_T("wxAUI_TB_DEFAULT_STYLE"))
        WXS_ST_CATEGORY("wxAuiToolBar")
        WXS_ST(wxAUI_TB_TEXT)
        WXS_ST(wxAUI_TB_NO_TOOLTIPS)
        WXS_ST(wxAUI_TB_NO_AUTORESIZE)
        WXS_ST(wxAUI_TB_OVERFLOW)
        WXS_ST(wxAUI_TB_HORZ_LAYOUT)
        WXS_ST(wxAUI_TB_HORZ_TEXT)
        WXS_ST(wxAUI_TB_DEFAULT_STYLE)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsAuiToolBarEvents)
        WXS_EVI(EVT_AUITOOLBAR_TOOL_DROPDOWN,wxEVT_COMMAND_AUITOOLBAR_TOOL_DROPDOWN,wxAuiToolBarEvent,Dropdown)
        WXS_EVI(EVT_AUITOOLBAR_OVERFLOW_CLICK,wxEVT_COMMAND_AUITOOLBAR_OVERFLOW_CLICK,wxAuiToolBarEvent,OverflowClick)
        WXS_EVI(EVT_AUITOOLBAR_RIGHT_CLICK,wxEVT_COMMAND_AUITOOLBAR_RIGHT_CLICK,wxAuiToolBarEvent,RightClick)
        WXS_EVI(EVT_AUITOOLBAR_MIDDLE_CLICK,wxEVT_COMMAND_AUITOOLBAR_MIDDLE_CLICK,wxAuiToolBarEvent,MiddleClick)
        WXS_EVI(EVT_AUITOOLBAR_BEGIN_DRAG,wxEVT_COMMAND_AUITOOLBAR_BEGIN_DRAG,wxAuiToolBarEvent,BeginDrag)
    WXS_EV_END()
}

wxsAuiToolBar::wxsAuiToolBar(wxsItemResData* Data):
    wxsContainer(
        Data,
        &Reg.Info,
        wxsAuiToolBarEvents,
        wxsAuiToolBarStyles),
    m_CurrentSelection(0)
{
    //ctor
}

wxsAuiToolBar::~wxsAuiToolBar()
{
    //dtor
}

bool wxsAuiToolBar::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    bool IsControl = wxDynamicCast(Item->BuildPreview(new wxFrame(0,-1,wxEmptyString),0),wxControl);
    bool IsAuiToolBarItem = Item->GetClassName().Contains(_T("wxAuiToolBar"));
    if ( !IsControl && !IsAuiToolBarItem )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("Only Controls and Aui ToolBar Items can be added to wxAuiToolBar."));
        }
        return false;
    }

	return wxsContainer::OnCanAddChild(Item,ShowMessage);
}

bool wxsAuiToolBar::OnCanAddToParent(wxsParent* Parent,bool ShowMessage)
{
    if ( Parent->GetClassName() != _T("wxAuiManager") )
    {
        if ( ShowMessage )
        {
            wxMessageBox(_("wxAuiToolBar can only be added to wxAuiManager."));
        }
        return false;
    }

	return wxsContainer::OnCanAddToParent(Parent,ShowMessage);
}

wxsPropertyContainer* wxsAuiToolBar::OnBuildExtra()
{
    return new wxsAuiToolBarExtra();
}

wxString wxsAuiToolBar::OnXmlGetExtraObjectClass()
{
    return _T("AuiToolBarItem");
}

wxObject* wxsAuiToolBar::OnBuildPreview(wxWindow* Parent,long PreviewFlags)
{
    UpdateCurrentSelection();
    wxsAuiManager* AuiManager = (wxsAuiManager*) GetParent();
    if (!AuiManager)
        return nullptr;
    wxsAuiPaneInfoExtra* PaneInfo = (wxsAuiPaneInfoExtra*) AuiManager->GetChildExtra(AuiManager->GetChildIndex(this));
    wxSmithAuiToolBar* AuiToolBar;

    long Gripper = 0;
    if ( PaneInfo->m_Gripper == wxTOP)
    {
            Gripper |= wxAUI_TB_VERTICAL;
    }
    AuiToolBar = new wxSmithAuiToolBar(Parent,-1,Pos(Parent),Size(Parent),Style()|Gripper);

	if ( !GetChildCount() && !(PreviewFlags&pfExact) )
	{
	    // Adding additional label to prevent from having zero-sized AuiToolBar
	    AuiToolBar->AddControl(new wxStaticText(AuiToolBar,-1,_("No Items")));
	}

	for ( int i=0; i<GetChildCount(); i++ )
	{
	    wxsItem* Child = GetChild(i);
	    wxsAuiToolBarExtra* Extra = (wxsAuiToolBarExtra*)GetChildExtra(i);
	    wxString ClassName = Child->GetClassName();

        if ( ClassName == _T("wxAuiToolBarItem") )
        {
            wxsAuiToolBarItem* Item = (wxsAuiToolBarItem*) Child;
            Item->m_ItemId = wxNewId();
            Item->m_HasGripper = PaneInfo->m_Gripper;
            Item->m_GripperSize = m_GripperSize;
            AuiToolBar->AddTool(
                Item->m_ItemId,
                Extra->m_Label,
                Item->m_Bitmap.GetPreview(wxDefaultSize,wxART_TOOLBAR),
                Item->m_DisabledBitmap.GetPreview(wxDefaultSize,wxART_TOOLBAR),
                Item->m_ItemKind,
                Item->m_ShortHelp,
                Item->m_LongHelp,
                NULL);
            if ( !Extra->m_Enabled ) AuiToolBar->EnableTool(Item->m_ItemId, false);
            if ( Item->m_DropDown && (Item->m_ItemKind == wxITEM_NORMAL) ) AuiToolBar->SetToolDropDown(Item->m_ItemId, true);
            AuiToolBar->Realize();
            Item->BuildPreview(AuiToolBar,PreviewFlags);
        }
        else if ( ClassName == _T("wxAuiToolBarSeparator") )
        {
            wxsAuiToolBarSeparator* Separator = (wxsAuiToolBarSeparator*) Child;
            Separator->m_ItemId = wxNewId();
            Separator->m_HasGripper = PaneInfo->m_Gripper;
            Separator->m_GripperSize = m_GripperSize;
            AuiToolBar->AddTool(Separator->m_ItemId,wxEmptyString,wxNullBitmap,wxNullBitmap,wxITEM_SEPARATOR,wxEmptyString,wxEmptyString,NULL);
            if ( !Extra->m_Enabled ) AuiToolBar->EnableTool(Separator->m_ItemId, false);
            AuiToolBar->Realize();
            Separator->BuildPreview(AuiToolBar,PreviewFlags);
        }
        else if ( ClassName == _T("wxAuiToolBarLabel") )
        {
            wxsAuiToolBarLabel* Label = (wxsAuiToolBarLabel*) Child;
            Label->m_ItemId = wxNewId();
            Label->m_HasGripper = PaneInfo->m_Gripper;
            Label->m_GripperSize = m_GripperSize;
            AuiToolBar->AddLabel(Label->m_ItemId,Extra->m_Label,Label->m_IsDefault ? -1 : Label->m_Width);
            if ( !Extra->m_Enabled ) AuiToolBar->EnableTool(Label->m_ItemId, false);
            AuiToolBar->Realize();
            Label->BuildPreview(AuiToolBar,PreviewFlags);
        }
        else if ( ClassName == _T("wxAuiToolBarSpacer") )
        {
            wxsAuiToolBarSpacer* Spacer = (wxsAuiToolBarSpacer*) Child;
            Spacer->m_ItemId = wxNewId();
            Spacer->m_HasGripper = PaneInfo->m_Gripper;
            Spacer->m_GripperSize = m_GripperSize;
            if ( Spacer->m_Stretch ) AuiToolBar->AddStretchSpacer(Spacer->m_Proportion,Spacer->m_ItemId);
            else                     AuiToolBar->AddSpacer(Spacer->m_Pixels,Spacer->m_ItemId);
            if ( !Extra->m_Enabled ) AuiToolBar->EnableTool(Spacer->m_ItemId, false);
            AuiToolBar->Realize();
            Spacer->BuildPreview(AuiToolBar,PreviewFlags);
        }
        else    // If child is not any of wxAuiToolBarItems, it is a wxControl
        {
            wxControl* ChildAsControl = wxDynamicCast(Child->BuildPreview(AuiToolBar,PreviewFlags),wxControl);
            AuiToolBar->AddControl(ChildAsControl,Extra->m_Label);
            if ( !Extra->m_Enabled ) ChildAsControl->Enable(false);
        }
	}

    AuiToolBar->Realize();
	return AuiToolBar;
}

void wxsAuiToolBar::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            wxsAuiManager* AuiManager = (wxsAuiManager*) GetParent();
            wxsAuiPaneInfoExtra* PaneInfo = (wxsAuiPaneInfoExtra*) AuiManager->GetChildExtra(AuiManager->GetChildIndex(this));

            AddHeader(_T("<wx/aui/aui.h>"),GetInfo().ClassName,0);
            AddHeader(_T("<wx/aui/aui.h>"),_T("wxAuiToolBarEvent"),0);

            if ( PaneInfo->m_Gripper == wxTOP )
            {
                Codef(_T("%C(%W, %I, %P, %S, %T|wxAUI_TB_VERTICAL);\n"));
            }
            else
            {
                Codef(_T("%C(%W, %I, %P, %S, %T);\n"));
            }

            AddChildrenCode();

            for ( int i=0; i<GetChildCount(); i++ )
            {
                wxsItem* Child = GetChild(i);
                wxsAuiToolBarExtra* Extra = (wxsAuiToolBarExtra*)GetChildExtra(i);
                wxString ClassName = Child->GetClassName();

                if ( ClassName == _T("wxAuiToolBarItem") )
                {
                    wxsAuiToolBarItem* Item = (wxsAuiToolBarItem*) Child;
                    wxsBitmapData Bitmap = Item->m_Bitmap;
                    wxsBitmapData DisabledBitmap = Item->m_DisabledBitmap;
                    wxString ItemKind;
                    switch ( Item->m_ItemKind )
                    {
                        case wxITEM_NORMAL:
                        {
                            ItemKind = _T("wxITEM_NORMAL");
                            break;
                        }
                        case wxITEM_CHECK:
                        {
                            ItemKind = _T("wxITEM_CHECK");
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    Codef(_T("%AAddTool(%s, %t, %i, %i, %s, %t, %t, NULL);\n"),
                        #if wxCHECK_VERSION(2, 9, 0)
                        Child->GetIdName().wx_str(),
                        Extra->m_Label.wx_str(),
                        &Bitmap,_T("wxART_TOOLBAR"),
                        &DisabledBitmap,_T("wxART_TOOLBAR"),
                        ItemKind.wx_str(),
                        Item->m_ShortHelp.wx_str(),
                        Item->m_LongHelp.wx_str());
                        #else
                        Child->GetIdName().c_str(),
                        Extra->m_Label.c_str(),
                        &Bitmap,_T("wxART_TOOLBAR"),
                        &DisabledBitmap,_T("wxART_TOOLBAR"),
                        ItemKind.c_str(),
                        Item->m_ShortHelp.c_str(),
                        Item->m_LongHelp.c_str());
                        #endif
                    if ( Item->m_DropDown && (Item->m_ItemKind == wxITEM_NORMAL) )
                    {
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("%ASetToolDropDown(%s, true);\n"),Item->GetIdName().wx_str());
                        #else
                        Codef(_T("%ASetToolDropDown(%s, true);\n"),Item->GetIdName().c_str());
                        #endif

                    }
                }
                else if ( ClassName == _T("wxAuiToolBarSeparator") )
                {
                    Codef(_T("%AAddSeparator();\n"));
                }
                else if ( ClassName == _T("wxAuiToolBarLabel") )
                {
                    wxsAuiToolBarLabel* Label = (wxsAuiToolBarLabel*) Child;
                    if ( Label->m_IsDefault )
                    {
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("%AAddLabel(%s, %t);\n"),Child->GetIdName().wx_str(),Extra->m_Label.wx_str());
                        #else
                        Codef(_T("%AAddLabel(%s, %t);\n"),Child->GetIdName().c_str(),Extra->m_Label.c_str());
                        #endif

                    }
                    else
                    {
                        #if wxCHECK_VERSION(2, 9, 0)
                        Codef(_T("%AAddLabel(%s, %t, %d);\n"),Child->GetIdName().wx_str(),Extra->m_Label.wx_str(),Label->m_Width);
                        #else
                        Codef(_T("%AAddLabel(%s, %t, %d);\n"),Child->GetIdName().c_str(),Extra->m_Label.c_str(),Label->m_Width);
                        #endif
                    }
                }
                else if ( ClassName == _T("wxAuiToolBarSpacer") )
                {
                    wxsAuiToolBarSpacer* Spacer = (wxsAuiToolBarSpacer*) Child;
                    if ( Spacer->m_Stretch ) Codef(_T("%AAddStretchSpacer(%d);\n"),Spacer->m_Proportion);
                    else                     Codef(_T("%AAddSpacer(%d);\n"),Spacer->m_Pixels);
                }
                else
                {
                    // If child is not any of wxAuiToolBarItems, it is a wxControl
                    #if wxCHECK_VERSION(2, 9, 0)
                    Codef(_T("%AAddControl(%o, %t);\n"),i,Extra->m_Label.wx_str());
                    if ( !Extra->m_Enabled && Child->GetBaseProps()->m_Enabled ) Codef(_T("%s->Enable(false);\n"),Child->GetVarName().wx_str());
                    #else
                    Codef(_T("%AAddControl(%o, %t);\n"),i,Extra->m_Label.c_str());
                    if ( !Extra->m_Enabled && Child->GetBaseProps()->m_Enabled ) Codef(_T("%s->Enable(false);\n"),Child->GetVarName().c_str());
                    #endif
                    continue;
                }

                #if wxCHECK_VERSION(2, 9, 0)
                if ( !Extra->m_Enabled ) Codef(_T("%AEnableTool(%s, false);\n"),Child->GetIdName().wx_str());
                #else
                if ( !Extra->m_Enabled ) Codef(_T("%AEnableTool(%s, false);\n"),Child->GetIdName().c_str());
                #endif
            }

            Codef(_T("%ARealize();\n"));

            break;
        }

        default:
        {
            wxsCodeMarks::Unknown(_T("wxsAuiToolBar::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

bool wxsAuiToolBar::OnMouseClick(wxWindow* Preview,int PosX,int PosY)
{
    UpdateCurrentSelection();
    wxSmithAuiToolBar* AuiToolBar = (wxSmithAuiToolBar*)Preview;
    int Hit = AuiToolBar->HitTest(wxPoint(PosX,PosY));
    if ( (Hit != wxNOT_FOUND) && (Hit < GetChildCount()) )
    {
        wxsItem* OldSel = m_CurrentSelection;
        m_CurrentSelection = GetChild(Hit);
        GetResourceData()->SelectItem(m_CurrentSelection,true);
        return OldSel != m_CurrentSelection;
    }
    return false;
}

bool wxsAuiToolBar::OnIsChildPreviewVisible(wxsItem* Child)
{
    UpdateCurrentSelection();
    return Child == m_CurrentSelection;
}

bool wxsAuiToolBar::OnEnsureChildPreviewVisible(wxsItem* Child)
{
    if ( IsChildPreviewVisible(Child) ) return false;
    m_CurrentSelection = Child;
    UpdateCurrentSelection();
    return true;
}

void wxsAuiToolBar::UpdateCurrentSelection()
{
    wxsItem* NewCurrentSelection = 0;
    for ( int i=0; i<GetChildCount(); i++ )
    {
        if ( m_CurrentSelection == GetChild(i) ) return;
        // Extra below is currently un-used
        // wxsAuiToolBarExtra* Extra = (wxsAuiToolBarExtra*)GetChildExtra(i);
        if ( i==0 )
        {
            NewCurrentSelection = GetChild(i);
        }
    }
    m_CurrentSelection = NewCurrentSelection;
}

#endif // wxCHECK_VERSION(2,8,9)
