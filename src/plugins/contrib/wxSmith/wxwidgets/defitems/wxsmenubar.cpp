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

#include "wxsmenubar.h"
#include "wxsmenueditor.h"
#include "../wxsitemresdata.h"
#include <wx/menu.h>

namespace
{
    wxsRegisterItem<wxsMenuBar> Reg(_T("MenuBar"),wxsTTool,_T("Tools"),90);

    class MenuEditorDialog: public wxDialog
    {
        public:

            wxsMenuEditor* Editor;

            MenuEditorDialog(wxsMenuBar* MenuBar):
                wxDialog(NULL,-1,_("MenuBar editor"),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)
            {
                wxBoxSizer* Sizer = new wxBoxSizer(wxVERTICAL);
                Sizer->Add(Editor = new wxsMenuEditor(this,MenuBar),1,wxEXPAND,0);
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

    BEGIN_EVENT_TABLE(MenuEditorDialog,wxDialog)
        EVT_BUTTON(wxID_OK,MenuEditorDialog::OnOK)
    END_EVENT_TABLE()
}

wxsMenuBar::wxsMenuBar(wxsItemResData* Data):
    wxsTool(
        Data,
        &Reg.Info,
        NULL,
        NULL,
        flVariable|flSubclass|flExtraCode)
{
}

void wxsMenuBar::OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP:
            Code << Codef(Language,_T("%C();\n"));
            for ( int i=0; i<GetChildCount(); i++ )
            {
                GetChild(i)->BuildCreatingCode(Code,WindowParent,Language);
            }
            Code << Codef(Language,_T("SetMenuBar(%v);\n"),GetVarName().c_str());
            BuildSetupWindowCode(Code, WindowParent, Language);
            break;

        default:
            wxsCodeMarks::Unknown(_T("wxsMenuBar::OnBuildCreatingCode"),Language);
    }
}

void wxsMenuBar::OnEnumToolProperties(long Flags)
{
}

void wxsMenuBar::OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language)
{
    switch ( Language )
    {
        case wxsCPP: Decl.Add(_T("<wx/menu.h>")); break;
        default: wxsCodeMarks::Unknown(_T("wxsMenuBar::OnEnumDeclFiles"),Language);
    }
}

bool wxsMenuBar::OnCanAddToResource(wxsItemResData* Data,bool ShowMessage)
{
    if ( Data->GetClassType() != _T("wxFrame") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("wxMenuBar can be added to wxFrame only"));
        }
        return false;
    }

    for ( int i=0; i<Data->GetToolsCount(); i++ )
    {
        if ( Data->GetTool(i)->GetClassName() == _T("wxMenuBar") )
        {
            if ( ShowMessage )
            {
                cbMessageBox(_("Can not add two or more wxMenuBar classes\ninto one wxFrame"));
            }
            return false;
        }
    }

    return true;
}

bool wxsMenuBar::OnCanAddChild(wxsItem* Item,bool ShowMessage)
{
    if ( Item->GetClassName() != _T("wxMenu") )
    {
        if ( ShowMessage )
        {
            cbMessageBox(_("Only wxMenu items can be added into wxMenuBar"));
        }
        return false;
    }
    return true;
}

bool wxsMenuBar::OnMouseDClick(wxWindow* Preview,int PosX,int PosY)
{
    MenuEditorDialog Dlg(this);
    Dlg.ShowModal();
    return false;
}
