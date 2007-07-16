/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxstoolbar.h"
#include "wxstoolbaritem.h"
#include "wxstoolbareditor.h"
#include "../wxsitemresdata.h"
#include <wx/toolbar.h>

namespace
{
    wxsRegisterItem<wxsToolBar> Reg(_T("ToolBar"),wxsTTool,_T("Tools"),80);

    class ToolBarEditorDialog: public wxDialog
    {
        public:

            wxsToolBarEditor* Editor;

            ToolBarEditorDialog(wxsToolBar* ToolBar):
                wxDialog(0,-1,_("ToolBar editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
            {
                wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
                Sizer->Add(Editor = new wxsToolBarEditor(this,ToolBar),1,wxEXPAND,0);
                Sizer->Add(CreateButtonSizer(wxOK|wxCANCEL),0,wxEXPAND,15);
                SetSizer(Sizer);
                Sizer->SetSizeHints(this);
                PlaceWindow(this,pdlCentre,true);
            }

            void OnOK(wxCommandEvent& event)
            {
                Editor->ApplyChanges();
                EndModal(wxID_OK);
            }

            DECLARE_EVENT_TABLE()
    };

    BEGIN_EVENT_TABLE(ToolBarEditorDialog,wxDialog)
        EVT_BUTTON(wxID_OK,ToolBarEditorDialog::OnOK)
    END_EVENT_TABLE()


    WXS_ST_BEGIN(wxsToolBarStyles,_T("wxTB_HORIZONTAL|wxNO_BORDER"))
        WXS_ST(wxTB_FLAT)
        WXS_ST(wxTB_DOCKABLE)
        WXS_ST(wxTB_VERTICAL)
        WXS_ST(wxTB_HORIZONTAL)
        WXS_ST(wxTB_3DBUTTONS)
        WXS_ST(wxTB_TEXT)
        WXS_ST(wxTB_NOICONS)
        WXS_ST(wxTB_NODIVIDER)
        WXS_ST(wxTB_NOALIGN)
        WXS_ST(wxTB_HORZ_LAYOUT)
        WXS_ST(wxTB_HORZ_TEXT)
        // TODO: Get rid of this guard after switching to strict 2.8
        #if wxCHECK_VERSION(2,8,0)
            WXS_ST(wxTB_TOP)
            WXS_ST(wxTB_LEFT)
            WXS_ST(wxTB_RIGHT)
            WXS_ST(wxTB_BOTTOM)
        #endif
        WXS_ST_DEFAULTS()
    WXS_ST_END()
}

wxsToolBar::wxsToolBar(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        0,
        wxsToolBarStyles,
        flVariable|flId|flSubclass|flExtraCode),
    m_Packing(-1),
    m_Separation(-1)
{
    m_BitmapSize.IsDefault = true;
    m_BitmapSize.X = 16;
    m_BitmapSize.Y = 16;
    m_BitmapSize.DialogUnits = false;
    m_Margins.IsDefault = true;
    m_Margins.X = 16;
    m_Margins.Y = 16;
    m_Margins.DialogUnits = false;
}

void wxsToolBar::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            // NOTE: This code assumes that parent is wxToolBar
            if ( GetChildCount() )
            {
                /*
                if ( !GetParent() && GetResourceData()->GetClassType()==_T("wxFrame") )
                {
                    Codef(_T("%v = CreateToolBar(%T, %I, %N);\n"),GetVarName().c_str());
                }
                else
                */
                {
                    Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
                }

                if ( !m_BitmapSize.IsDefault )
                {
                    Codef(_T("%ASetToolBitmapSize(%s);\n"),m_BitmapSize.GetSizeCode(WindowParent,Language).c_str());
                }
                if ( !m_Margins.IsDefault )
                {
                    Codef(_T("%ASetMargins(%s);\n"),m_Margins.GetSizeCode(WindowParent,Language).c_str());
                }
                if ( m_Packing >= 0 )
                {
                    Codef(_T("%ASetToolPacking(%d);\n"),m_Packing);
                }
                if ( m_Separation >= 0 )
                {
                    Codef(_T("%ASetToolSeparation(%d);\n"),m_Separation);
                }
                for ( int i=0; i<GetChildCount(); i++ )
                {
                    wxsItem* Child = GetChild(i);
                    Child->BuildCreatingCode(Code,GetVarName(),Language);
                    if ( Child->GetClassName() != _T("wxToolBarToolBase") )
                    {
                        Codef(_T("%AAddControl(%v);\n"),Child->GetVarName().c_str());
                    }
                }
                Codef(_T("%ARealize();\n"));
                if ( !GetParent() && GetResourceData()->GetClassType()==_T("wxFrame") )
                {
                    Codef(_T("SetToolBar(%v);\n"),GetVarName().c_str());
                }
                BuildSetupWindowCode(Code, WindowParent, Language);
            }
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsToolBar::OnBuildCreatingCode"),Language);
    }
}

void wxsToolBar::OnEnumToolProperties(long Flags)
{
    WXS_SIZE(wxsToolBar,m_BitmapSize,_("Use Bitmap size"),_("  Width"),_("  Height"),_("  In Dialog Units"),_T("bitmapsize"));
    WXS_SIZE(wxsToolBar,m_Margins,_("Use Margins"),_("  Width"),_("  Height"),_("  In Dialog Units"),_T("margins"));
    WXS_LONG(wxsToolBar,m_Packing,_("Packing"),_T("packing"),-1);
    WXS_LONG(wxsToolBar,m_Separation,_("Separation"),_T("separation"),-1);
}

void wxsToolBar::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/toolbar.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsToolBar::OnEnumDeclFiles"),Language);
    }
}

bool wxsToolBar::OnCanAddToResource(wxsItemResData* Data,bool ShowMessage)
{
    if ( Data->GetClassType() != _T("wxFrame") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("wxToolBar can be added to wxFrame only"));
        }
        return false;
    }

    for ( int i=0; i<Data->GetToolsCount(); i++ )
    {
        if ( Data->GetTool(i)->GetClassName() == _T("wxToolBar") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_("Can not add two or more wxToolBar classes\ninto one wxFrame"));
            }
            return false;
        }
    }

    return true;
}

bool wxsToolBar::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    // Can add wxToolBarItem or any class derived from wxContainer

    // TODO: Remove this ugly hack !!!
    if ( Item->GetType() == wxsTContainer )
    {
        wxString ClassName = Item->GetClassName();
        if ( ClassName == _T("wxPanel") ||
             ClassName == _T("wxDialog") ||
             ClassName == _T("wxFrame") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_("Only wxControl-derived classes can be added into wxToolBar"));
            }
            return false;
        }
    }
    else if ( Item->GetType() == wxsTSizer )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("Cannot add sizers into wxToolBar"));
        }
        return false;
    }
    else if ( Item->GetType() == wxsTSpacer )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("Can not add spacer into wxToolBar"));
        }
        return false;
    }
    else if ( Item->GetType() == wxsTTool )
    {
        if ( Item->GetClassName() != _T("wxToolBarToolBase") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_("Invalid item added into wxToolBar"));
            }
            return false;
        }
    }
    return true;
}

bool wxsToolBar::OnMouseDClick(wxWindow* Preview,int PosX,int PosY)
{
    ToolBarEditorDialog Dlg(this);
    Dlg.ShowModal();
    return false;
}

bool wxsToolBar::OnXmlReadChild(TiXmlElement* Elem,bool IsXRC,bool IsExtra)
{
    if ( IsXRC )
    {
        wxString ClassName = cbC2U(Elem->Attribute("class"));
        if ( ClassName == _T("separator") )
        {
            wxsToolBarItem* Child = new wxsToolBarItem(GetResourceData(),true);
            AddChild(Child);
            return Child->XmlRead(Elem,IsXRC,IsExtra);
        }

        if ( ClassName == _T("tool") )
        {
            wxsToolBarItem* Child = new wxsToolBarItem(GetResourceData(),false);
            AddChild(Child);
            return Child->XmlRead(Elem,IsXRC,IsExtra);
        }
    }

    return wxsParent::OnXmlReadChild(Elem,IsXRC,IsExtra);
}
