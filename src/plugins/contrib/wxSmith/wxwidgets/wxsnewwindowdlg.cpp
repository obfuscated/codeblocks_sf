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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsnewwindowdlg.h"
#include "wxwidgetsres.h"
#include "wxsdialogres.h"
#include "wxsframeres.h"
#include "wxspanelres.h"
#include "wxwidgetsgui.h"
#include "wxsitemresdata.h"
#include "../wxsmith.h"
#include "../wxsproject.h"

#include <logmanager.h>
#include <projectmanager.h>
#include <cbproject.h>
#include <projectfile.h>

//(*InternalHeaders(wxsNewWindowDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

namespace
{
    // Don't want to use wxFileName since it breaks slashes/backslashes stuff

    wxString GetPathPart(const wxString& FileName)
    {
        for ( size_t i=FileName.Length(); i-->0; )
        {
            switch ( FileName[i] )
            {
                case _T('/'):
                case _T('\\'):
                    return FileName.Mid(0,i+1);
            }
        }

        return wxEmptyString;
    }

    wxString GetNoExt(const wxString& FileName)
    {
        for ( size_t i=FileName.Length(); i-->0; )
        {
            switch ( FileName[i] )
            {
                case _T('/'):
                case _T('\\'):
                    return FileName;

                case _T('.'):
                    return FileName.Mid(0,i);
            }
        }
        return FileName;
    }
}

//(*IdInit(wxsNewWindowDlg)
const long wxsNewWindowDlg::ID_STATICTEXT1 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL1 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT2 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL2 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT3 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL3 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX1 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL4 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX3 = wxNewId();
const long wxsNewWindowDlg::ID_BUTTON1 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX2 = wxNewId();
const long wxsNewWindowDlg::ID_COMBOBOX1 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT11 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL8 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX4 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL5 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT4 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL6 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT7 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT8 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT9 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT10 = wxNewId();
const long wxsNewWindowDlg::ID_BUTTON2 = wxNewId();
const long wxsNewWindowDlg::ID_BUTTON3 = wxNewId();
const long wxsNewWindowDlg::ID_BUTTON4 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT5 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX5 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX9 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX6 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX10 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX7 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX11 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX8 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX12 = wxNewId();
const long wxsNewWindowDlg::ID_STATICTEXT6 = wxNewId();
const long wxsNewWindowDlg::ID_TEXTCTRL7 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX14 = wxNewId();
const long wxsNewWindowDlg::ID_CHECKBOX13 = wxNewId();
//*)

BEGIN_EVENT_TABLE(wxsNewWindowDlg,wxDialog)
    //(*EventTable(wxsNewWindowDlg)
    //*)
    EVT_BUTTON(wxID_OK,wxsNewWindowDlg::OnCreate)
    EVT_BUTTON(wxID_CANCEL,wxsNewWindowDlg::OnCancel)
END_EVENT_TABLE()

wxsNewWindowDlg::wxsNewWindowDlg(wxWindow* parent,const wxString& ResType,wxsProject* Project):
    m_SourceNotTouched(true),
    m_HeaderNotTouched(true),
    m_XrcNotTouched(true),
    m_BlockText(false),
    m_AdvOpsShown(true),
    m_Type(ResType),
    m_Project(Project)
{
    //(*Initialize(wxsNewWindowDlg)
    wxStaticText* StaticText2;
    wxStaticText* StaticText1;
    wxStaticText* StaticText3;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    m_RootSizer = new wxBoxSizer(wxVERTICAL);
    StaticBoxSizer3 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Options"));
    FlexGridSizer1 = new wxFlexGridSizer(0, 2, 5, 5);
    FlexGridSizer1->AddGrowableCol(1);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Class Name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
    FlexGridSizer1->Add(StaticText1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Class = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    FlexGridSizer1->Add(m_Class, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 5);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Header file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT2"));
    FlexGridSizer1->Add(StaticText2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Header = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    FlexGridSizer1->Add(m_Header, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Source file:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT3"));
    FlexGridSizer1->Add(StaticText3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Source = new wxTextCtrl(this, ID_TEXTCTRL3, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    FlexGridSizer1->Add(m_Source, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_UseXrc = new wxCheckBox(this, ID_CHECKBOX1, _("Xrc File:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_UseXrc->SetValue(false);
    FlexGridSizer1->Add(m_UseXrc, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Xrc = new wxTextCtrl(this, ID_TEXTCTRL4, wxEmptyString, wxDefaultPosition, wxSize(80,-1), 0, wxDefaultValidator, _T("ID_TEXTCTRL4"));
    FlexGridSizer1->Add(m_Xrc, 0, wxEXPAND|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer1->Add(10,6,1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_XRCAutoload = new wxCheckBox(this, ID_CHECKBOX3, _("Add XRC file to autoload list"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_XRCAutoload->SetValue(false);
    m_XRCAutoload->Disable();
    FlexGridSizer1->Add(m_XRCAutoload, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticBoxSizer3->Add(FlexGridSizer1, 1, wxALL|wxEXPAND|wxALIGN_LEFT|wxALIGN_TOP, 0);
    m_RootSizer->Add(StaticBoxSizer3, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    m_AdvOps = new wxButton(this, ID_BUTTON1, _("+ Advanced options"), wxDefaultPosition, wxDefaultSize, wxBU_LEFT|wxNO_BORDER, wxDefaultValidator, _T("ID_BUTTON1"));
    m_RootSizer->Add(m_AdvOps, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    m_AdvancedOptionsSizer = new wxStaticBoxSizer(wxVERTICAL, this, wxEmptyString);
    FlexGridSizer2 = new wxFlexGridSizer(0, 2, 5, 5);
    FlexGridSizer2->AddGrowableCol(1);
    m_UsePCH = new wxCheckBox(this, ID_CHECKBOX2, _("Use PCH:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_UsePCH->SetValue(false);
    FlexGridSizer2->Add(m_UsePCH, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_Pch = new wxComboBox(this, ID_COMBOBOX1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_COMBOBOX1"));
    FlexGridSizer2->Add(m_Pch, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer1->Add(21,16,0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText11 = new wxStaticText(this, ID_STATICTEXT11, _("PCH guard define:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT11"));
    BoxSizer1->Add(StaticText11, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer2->Add(BoxSizer1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    m_PchGuard = new wxTextCtrl(this, ID_TEXTCTRL8, _("WX_PRECOMP"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL8"));
    FlexGridSizer2->Add(m_PchGuard, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    m_UseInitFunc = new wxCheckBox(this, ID_CHECKBOX4, _("Init code in function:"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX4"));
    m_UseInitFunc->SetValue(false);
    FlexGridSizer2->Add(m_UseInitFunc, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_InitFunc = new wxTextCtrl(this, ID_TEXTCTRL5, _("BuildContent"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL5"));
    m_InitFunc->Disable();
    FlexGridSizer2->Add(m_InitFunc, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Base class name:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    FlexGridSizer2->Add(StaticText4, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_BaseClass = new wxTextCtrl(this, ID_TEXTCTRL6, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL6"));
    FlexGridSizer2->Add(m_BaseClass, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText7 = new wxStaticText(this, ID_STATICTEXT7, _("Scopes:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT7"));
    FlexGridSizer2->Add(StaticText7, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 0);
    FlexGridSizer4 = new wxFlexGridSizer(0, 3, 0, 5);
    FlexGridSizer4->AddGrowableCol(0);
    FlexGridSizer4->AddGrowableCol(1);
    FlexGridSizer4->AddGrowableCol(2);
    StaticText8 = new wxStaticText(this, ID_STATICTEXT8, _("IDs:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT8"));
    FlexGridSizer4->Add(StaticText8, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText9 = new wxStaticText(this, ID_STATICTEXT9, _("Members:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT9"));
    FlexGridSizer4->Add(StaticText9, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText10 = new wxStaticText(this, ID_STATICTEXT10, _("Handlers:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT10"));
    FlexGridSizer4->Add(StaticText10, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ScopeIds = new wxButton(this, ID_BUTTON2, _("Public"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    FlexGridSizer4->Add(m_ScopeIds, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ScopeMembers = new wxButton(this, ID_BUTTON3, _("Public"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON3"));
    FlexGridSizer4->Add(m_ScopeMembers, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ScopeHandlers = new wxButton(this, ID_BUTTON4, _("Public"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON4"));
    FlexGridSizer4->Add(m_ScopeHandlers, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(FlexGridSizer4, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
    StaticText5 = new wxStaticText(this, ID_STATICTEXT5, _("Constructor arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT5"));
    FlexGridSizer2->Add(StaticText5, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 10);
    FlexGridSizer3->AddGrowableCol(0);
    m_CtorParent = new wxCheckBox(this, ID_CHECKBOX5, _("Parent"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX5"));
    m_CtorParent->SetValue(true);
    FlexGridSizer3->Add(m_CtorParent, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorParentDef = new wxCheckBox(this, ID_CHECKBOX9, _("Def. value"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX9"));
    m_CtorParentDef->SetValue(false);
    FlexGridSizer3->Add(m_CtorParentDef, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorId = new wxCheckBox(this, ID_CHECKBOX6, _("Id"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX6"));
    m_CtorId->SetValue(true);
    FlexGridSizer3->Add(m_CtorId, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorIdDef = new wxCheckBox(this, ID_CHECKBOX10, _("Def. value"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX10"));
    m_CtorIdDef->SetValue(true);
    FlexGridSizer3->Add(m_CtorIdDef, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorPos = new wxCheckBox(this, ID_CHECKBOX7, _("Position"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX7"));
    m_CtorPos->SetValue(false);
    FlexGridSizer3->Add(m_CtorPos, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorPosDef = new wxCheckBox(this, ID_CHECKBOX11, _("Def. value"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX11"));
    m_CtorPosDef->SetValue(true);
    m_CtorPosDef->Disable();
    FlexGridSizer3->Add(m_CtorPosDef, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorSize = new wxCheckBox(this, ID_CHECKBOX8, _("Size"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX8"));
    m_CtorSize->SetValue(false);
    FlexGridSizer3->Add(m_CtorSize, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorSizeDef = new wxCheckBox(this, ID_CHECKBOX12, _("Def. value"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX12"));
    m_CtorSizeDef->SetValue(true);
    m_CtorSizeDef->Disable();
    FlexGridSizer3->Add(m_CtorSizeDef, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    FlexGridSizer2->Add(FlexGridSizer3, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StaticText6 = new wxStaticText(this, ID_STATICTEXT6, _("Custom arguments:"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT6"));
    FlexGridSizer2->Add(StaticText6, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
    m_CtorCustom = new wxTextCtrl(this, ID_TEXTCTRL7, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL7"));
    FlexGridSizer2->Add(m_CtorCustom, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_AdvancedOptionsSizer->Add(FlexGridSizer2, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_UseFwdDecl = new wxCheckBox(this, ID_CHECKBOX14, _("Use forward declarations"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX14"));
    m_UseFwdDecl->SetValue(false);
    m_AdvancedOptionsSizer->Add(m_UseFwdDecl, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_AddWxs = new wxCheckBox(this, ID_CHECKBOX13, _("Add wxs file to project"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX13"));
    m_AddWxs->SetValue(false);
    m_AdvancedOptionsSizer->Add(m_AddWxs, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_RootSizer->Add(m_AdvancedOptionsSizer, 0, wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_RootSizer->Add(300,5,0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
    StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
    StdDialogButtonSizer1->Realize();
    m_RootSizer->Add(StdDialogButtonSizer1, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(m_RootSizer);
    m_RootSizer->Fit(this);
    m_RootSizer->SetSizeHints(this);

    Connect(ID_TEXTCTRL1,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsNewWindowDlg::OnClassChanged);
    Connect(ID_TEXTCTRL2,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsNewWindowDlg::OnHeaderChanged);
    Connect(ID_TEXTCTRL3,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsNewWindowDlg::OnSourceChanged);
    Connect(ID_CHECKBOX1,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnUseXrcChange);
    Connect(ID_TEXTCTRL4,wxEVT_COMMAND_TEXT_UPDATED,(wxObjectEventFunction)&wxsNewWindowDlg::OnXrcChanged);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnAdvOpsClick);
    Connect(ID_CHECKBOX2,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnUsePCHClick);
    Connect(ID_CHECKBOX4,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnUseInitFuncClick);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnScopeIdsClick);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnScopeMembersClick);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnScopeHandlersClick);
    Connect(ID_CHECKBOX5,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorParentClick);
    Connect(ID_CHECKBOX9,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorParentDefClick);
    Connect(ID_CHECKBOX6,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorIdClick);
    Connect(ID_CHECKBOX10,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorIdDefClick);
    Connect(ID_CHECKBOX7,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorPosClick);
    Connect(ID_CHECKBOX11,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorPosDefClick);
    Connect(ID_CHECKBOX8,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorSizeClick);
    Connect(ID_CHECKBOX12,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&wxsNewWindowDlg::OnCtorSizeDefClick);
    //*)

    ConfigManager* Cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));

    m_BlockText = true;
    m_SourceDirectory = Cfg->Read(_T("/newresource/sourcedirectory"),_T(""));

    wxString StrippedName = ResType.Mid(2);
    wxString ResName = wxString::Format(_("New%s"),StrippedName.c_str());
    m_Class->SetValue(ResName);
    m_Source->SetValue(m_SourceDirectory + ResName+_T(".cpp"));
    m_Header->SetValue(m_SourceDirectory + ResName+_T(".h"));
    m_Xrc   ->SetValue(m_SourceDirectory + ResName+_T(".xrc"));
    m_Xrc->Disable();
    SetTitle(wxString::Format(_("New %s resource"),ResType.c_str()));
    DetectPchFile();
    m_BaseClass->SetValue(ResType);
    wxCommandEvent event;
    OnAdvOpsClick(event);
    Center();
    m_AppManaged =
        m_Project->GetGUI() &&
        m_Project->GetGUI()->GetName()==_T("wxWidgets") &&
        m_Project->GetGUI()->CheckIfApplicationManaged();
    m_XRCAutoload->SetValue(m_AppManaged);

    m_UseInitFunc->SetValue(Cfg->ReadBool(_T("/newresource/useinitfunc"),m_UseInitFunc->GetValue()));
    m_InitFunc->Enable(m_UseInitFunc->GetValue());
    m_InitFunc->SetValue(Cfg->Read(_T("/newresource/initfunc"),m_InitFunc->GetValue()));
    m_CtorParent->SetValue(Cfg->ReadBool(_T("/newresource/ctorparent"),m_CtorParent->GetValue()));
    m_CtorParentDef->Enable(m_CtorParent->GetValue());
    m_CtorParentDef->SetValue(Cfg->ReadBool(_T("/newresource/ctorparentdef"),m_CtorParentDef->GetValue()));
    m_CtorId->SetValue(Cfg->ReadBool(_T("/newresource/ctorid"),m_CtorId->GetValue()));
    m_CtorIdDef->Enable(m_CtorId->GetValue());
    m_CtorIdDef->SetValue(Cfg->ReadBool(_T("/newresource/ctorid"),m_CtorIdDef->GetValue()));
    m_CtorPos->SetValue(Cfg->ReadBool(_T("/newresource/ctorpos"),m_CtorPos->GetValue()));
    m_CtorPosDef->Enable(m_CtorPos->GetValue());
    m_CtorPosDef->SetValue(Cfg->ReadBool(_T("/newresource/ctorpos"),m_CtorPosDef->GetValue()));
    m_CtorSize->SetValue(Cfg->ReadBool(_T("/newresource/ctorsize"),m_CtorSize->GetValue()));
    m_CtorSizeDef->Enable(m_CtorSize->GetValue());
    m_CtorSizeDef->SetValue(Cfg->ReadBool(_T("/newresource/ctorsize"),m_CtorSizeDef->GetValue()));
    m_UseXrc->SetValue(Cfg->ReadBool(_T("/newresource/usexrc"),m_UseXrc->GetValue()));
    m_Xrc->Enable(m_UseXrc->GetValue());
    m_XRCAutoload->SetValue(Cfg->ReadBool(_T("/newresource/xrcautoloag"),m_XRCAutoload->GetValue()));
    m_AddWxs->SetValue(Cfg->ReadBool(_T("/newresource/addwxs"),m_AddWxs->GetValue()));
    m_ScopeIdsVal = (wxsItemRes::NewResourceParams::Scope)Cfg->ReadInt(_T("/newresource/scopeids"),(int)wxsItemRes::NewResourceParams::Protected);
    m_ScopeMembersVal = (wxsItemRes::NewResourceParams::Scope)Cfg->ReadInt(_T("/newresource/scopemembers"),(int)wxsItemRes::NewResourceParams::Public);
    m_ScopeHandlersVal = (wxsItemRes::NewResourceParams::Scope)Cfg->ReadInt(_T("/newresource/scopehandlers"),(int)wxsItemRes::NewResourceParams::Private);
    m_UseFwdDecl->SetValue(Cfg->ReadBool(_T("/newresource/usefwddecl"),m_UseFwdDecl->GetValue()));
    m_PchGuard->SetValue(Cfg->Read(_T("/newresource/pchguard"),m_PchGuard->GetValue()));
    UpdateScopeButtons();
    OnUseXrcChange(event);

    m_BlockText = false;

}

wxsNewWindowDlg::~wxsNewWindowDlg()
{
    //(*Destroy(wxsNewWindowDlg)
    //*)
}

void wxsNewWindowDlg::OnCancel(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void wxsNewWindowDlg::OnCreate(wxCommandEvent& event)
{
	bool CreateXrc = m_UseXrc->GetValue();
    cbProject* cbProj = m_Project->GetCBProject();

    wxsItemRes::NewResourceParams Params;
    Params.Class          = m_Class->GetValue();
    Params.Src            = m_Source->GetValue();
    Params.Hdr            = m_Header->GetValue();
    Params.Xrc            = CreateXrc ? m_Xrc->GetValue() : _T("");
    Params.Pch            = m_Pch->GetValue();
    Params.Wxs            = _T("");
    Params.InitFunc       = m_InitFunc->GetValue();
    Params.BaseClass      = m_BaseClass->GetValue();
    Params.CustomCtorArgs = m_CtorCustom->GetValue();
    Params.UsePch         = m_UsePCH->GetValue();
    Params.UseInitFunc    = m_UseInitFunc->GetValue();
    Params.CtorParent     = m_CtorParent->GetValue();
    Params.CtorParentDef  = m_CtorParentDef->GetValue();
    Params.CtorId         = m_CtorId->GetValue();
    Params.CtorIdDef      = m_CtorIdDef->GetValue();
    Params.CtorPos        = m_CtorPos->GetValue();
    Params.CtorPosDef     = m_CtorPosDef->GetValue();
    Params.CtorSize       = m_CtorSize->GetValue();
    Params.CtorSizeDef    = m_CtorSizeDef->GetValue();
    Params.ScopeIds       = m_ScopeIdsVal;
    Params.ScopeMembers   = m_ScopeMembersVal;
    Params.ScopeHandlers  = m_ScopeHandlersVal;
    Params.UseFwdDecl     = m_UseFwdDecl->GetValue();
    Params.PchGuard       = m_PchGuard->GetValue();

    // Need to do some checks
    // Validating name
    // TODO: Do not use fixed language
    if ( !wxsCodeMarks::ValidateIdentifier(wxsCPP,Params.Class) )
    {
        wxMessageBox(_("Invalid class name"));
        return;
    }

    // Validating base class
    if ( !wxsCodeMarks::ValidateIdentifier(wxsCPP,Params.BaseClass) )
    {
        wxMessageBox(_("Invalid name of base class"));
        return;
    }

    // Validating init function name
    if ( Params.UseInitFunc && !wxsCodeMarks::ValidateIdentifier(wxsCPP,Params.InitFunc) )
    {
        wxMessageBox(_("Invalid name of init function"));
        return;
    }

    // Checking if there's given resoure in current project
    if ( m_Project->FindResource(Params.Class) )
    {
        wxMessageBox(wxString::Format(_("Resource '%s' already exists"),Params.Class.c_str()));
        return;
    }

    // Validating PCH guard if needed
    if ( Params.UsePch && (Params.PchGuard.IsEmpty() || !wxsCodeMarks::ValidateIdentifier(wxsCPP,Params.PchGuard)) )
    {
        wxMessageBox(_("Invalid name of pch guard"));
        return;
    }

    // Checking if files already exist
    wxString ProjectPrefix = m_Project->GetProjectPath();
    Params.GenHdr = true;
    if ( wxFileName::FileExists(ProjectPrefix+Params.Hdr) )
    {
        switch ( wxMessageBox(wxString::Format(
            _("File '%s' already exists. Overwrite it ?"),Params.Hdr.c_str()),
            _("File exists"),
            wxYES_NO|wxCANCEL|wxICON_ERROR) )
        {
            case wxCANCEL: return;
            case wxNO: Params.GenHdr = false; break;
        }
    }

    Params.GenSrc = true;
    if ( wxFileName::FileExists(ProjectPrefix+Params.Src) )
    {
        switch ( wxMessageBox(wxString::Format(
            _("File '%s' already exists. Overwrite it ?"),Params.Src.c_str()),
            _("File exists"),wxYES_NO|wxCANCEL|wxICON_ERROR) )
        {
            case wxCANCEL: return;
            case wxNO: Params.GenSrc = false; break;
        }
    }

    Params.GenXrc = CreateXrc;
    if ( wxFileName::FileExists(ProjectPrefix+Params.Xrc) )
    {
        // We will add new resource to existing one creating multi-resource xrc
        Params.GenXrc = false;
    }

    // Disable button to prevent more clicks while resource is created
    wxWindow* OkBtn = FindWindowById(wxID_OK);
    if ( OkBtn )
    {
        OkBtn->Disable();
    }

    // Creating new resource
    wxsItemRes* NewResource = 0;

    if ( m_Type == _T("wxDialog") )
    {
        NewResource = new wxsDialogRes(m_Project);
    }
    else if ( m_Type == _T("wxFrame") )
    {
        NewResource = new wxsFrameRes(m_Project);
    }
    else if ( m_Type == _T("wxPanel") )
    {
        NewResource = new wxsPanelRes(m_Project);
    }
    else
    {
        Manager::Get()->GetLogManager()->DebugLog(_T("wxSmith: Internal error: unknown type when creating resource"));
        EndModal(wxID_CANCEL);
        return;
    }

    // Building new data
    if ( !NewResource->CreateNewResource(Params) )
    {
        delete NewResource;
        Manager::Get()->GetLogManager()->DebugLog(_T("wxSmith: Couldn't generate new resource"));
        EndModal(wxID_CANCEL);
        return;
    }

    wxsItemResData* Data = NewResource->BuildResData(0);
    Data->BeginChange();

    // Updating content of resource
    // This is done to allow XRC loader load proper data
    if ( !PrepareResource(NewResource,Data) )
    {
        wxMessageBox(_("Error while initializing resource"));
        Data->EndChange();
        delete Data;
        delete NewResource;
        EndModal(wxID_CANCEL);
        return;
    }

    if ( !m_Project->AddResource(NewResource) )
    {
        wxMessageBox(_("Error while adding new resource into project"));
        Data->EndChange();
        delete Data;
        delete NewResource;
        EndModal(wxID_CANCEL);
        return;
    }

    Data->EndChange();
    Data->Save();
    delete Data;

    // Register xrc file in autoload list if needed
    if ( m_AppManaged && CreateXrc && m_XRCAutoload->GetValue() && !Params.Xrc.IsEmpty() )
    {
        wxWidgetsGUI* GUI = wxDynamicCast(m_Project->GetGUI(),wxWidgetsGUI);
        if ( GUI )
        {
            wxArrayString& AutoList = GUI->GetLoadedResources();
            if ( AutoList.Index(Params.Xrc) == wxNOT_FOUND )
            {
                AutoList.Add(Params.Xrc);
                GUI->RebuildApplicationCode();
            }
        }
    }

    // Adding new files to project
    wxArrayInt Targets;
    Manager::Get()->GetProjectManager()->AddFileToProject(Params.Hdr,cbProj,Targets);
    if (Targets.GetCount() != 0)
    {
        Manager::Get()->GetProjectManager()->AddFileToProject(Params.Src,cbProj,Targets);
        if ( !Params.Wxs.IsEmpty() && m_AddWxs->GetValue() )
        {
            Manager::Get()->GetProjectManager()->AddFileToProject(Params.Wxs,cbProj,Targets);
        }
    }
    Manager::Get()->GetProjectManager()->RebuildTree();

    // Opening editor for this resource
    NewResource->EditOpen();

    // Saving configuration

    ConfigManager* Cfg = Manager::Get()->GetConfigManager(_T("wxsmith"));
    Cfg->Write(_T("/newresource/useinitfunc"),m_UseInitFunc->GetValue());
    Cfg->Write(_T("/newresource/initfunc"),m_InitFunc->GetValue());
    Cfg->Write(_T("/newresource/ctorparent"),m_CtorParent->GetValue());
    Cfg->Write(_T("/newresource/ctorparentdef"),m_CtorParentDef->GetValue());
    Cfg->Write(_T("/newresource/ctorid"),m_CtorId->GetValue());
    Cfg->Write(_T("/newresource/ctorid"),m_CtorIdDef->GetValue());
    Cfg->Write(_T("/newresource/ctorpos"),m_CtorPos->GetValue());
    Cfg->Write(_T("/newresource/ctorpos"),m_CtorPosDef->GetValue());
    Cfg->Write(_T("/newresource/ctorsize"),m_CtorSize->GetValue());
    Cfg->Write(_T("/newresource/ctorsize"),m_CtorSizeDef->GetValue());
    Cfg->Write(_T("/newresource/usexrc"),m_UseXrc->GetValue());
    Cfg->Write(_T("/newresource/xrcautoloag"),m_XRCAutoload->GetValue());
    Cfg->Write(_T("/newresource/addwxs"),m_AddWxs->GetValue());
    Cfg->Write(_T("/newresource/scopeids"),(int)m_ScopeIdsVal);
    Cfg->Write(_T("/newresource/scopemembers"),(int)m_ScopeMembersVal);
    Cfg->Write(_T("/newresource/scopehandlers"),(int)m_ScopeHandlersVal);
    Cfg->Write(_T("/newresource/sourcedirectory"),m_SourceDirectory);
    Cfg->Write(_T("/newresource/usefwddecl"),m_UseFwdDecl->GetValue());
    Cfg->Write(_T("/newresource/pchguard"),m_PchGuard->GetValue());

    EndModal(wxID_OK);
}

void wxsNewWindowDlg::OnClassChanged(wxCommandEvent& event)
{
    if ( m_BlockText ) return;
    m_BlockText = true;
    if ( m_HeaderNotTouched ) m_Header->SetValue(m_SourceDirectory + m_Class->GetValue() + _T(".h"));
    if ( m_SourceNotTouched ) m_Source->SetValue(m_SourceDirectory + m_Class->GetValue() + _T(".cpp"));
    if ( m_XrcNotTouched    ) m_Xrc->   SetValue(m_SourceDirectory + m_Class->GetValue() + _T(".xrc"));
    m_BlockText = false;
}

void wxsNewWindowDlg::OnSourceChanged(wxCommandEvent& event)
{
    if ( m_BlockText ) return;
    m_BlockText = true;
    m_SourceNotTouched = false;
    m_BlockText = false;
}

void wxsNewWindowDlg::OnHeaderChanged(wxCommandEvent& event)
{
    if ( m_BlockText ) return;
    m_BlockText = true;
    m_SourceDirectory = GetPathPart(m_Header->GetValue());
    wxString Base = GetNoExt(m_Header->GetValue());
    if ( m_SourceNotTouched )
    {
        m_Source->SetValue(Base+_T(".cpp"));
    }
    if ( m_XrcNotTouched )
    {
        m_Xrc->SetValue(Base+_T(".h"));
    }
    m_HeaderNotTouched = false;
    m_BlockText = false;
}

void wxsNewWindowDlg::OnUseXrcChange(wxCommandEvent& event)
{
    m_Xrc->Enable(m_UseXrc->GetValue());
    m_XRCAutoload->Enable(m_UseXrc->GetValue() && m_AppManaged);

    if ( m_UseXrc->GetValue() )
    {
        m_CtorParent->SetValue(true);
        m_CtorId->SetValue(false);
        m_CtorPos->SetValue(false);
        m_CtorSize->SetValue(false);
        m_CtorParent->Disable();
        m_CtorId->Disable();
        m_CtorPos->Disable();
        m_CtorSize->Disable();
        m_CtorParentDef->Enable();
        m_CtorIdDef->Disable();
        m_CtorPosDef->Disable();
        m_CtorSizeDef->Disable();
    }
    else
    {
        m_CtorParent->Enable();
        m_CtorId->Enable();
        m_CtorPos->Enable();
        m_CtorSize->Enable();
    }
}

void wxsNewWindowDlg::OnXrcChanged(wxCommandEvent& event)
{
    if ( m_BlockText ) return;
    m_BlockText = true;
    m_XrcNotTouched = false;
    m_BlockText = false;
}

wxString wxsNewWindowDlg::DetectPchFile()
{
    // Searching for files that are good candidates for pch files
    cbProject* Proj = m_Project->GetCBProject();
    for ( int i=0; i<Proj->GetFilesCount(); i++ )
    {
        ProjectFile* File = Proj->GetFile(i);
        if ( File && File->file.GetExt()==_T("h") && File->compile )
        {
            int Index = m_Pch->Append(File->relativeFilename);
            if ( File->file.GetFullName() == _T("wx_pch.h") )
            {
                // Since wx_pch.h is default pch file generated
                // by wizard, it's preffered as pch choice
                m_Pch->SetSelection(Index);
            }
        }
    }

    // Updating the rest of stuff
    if ( m_Pch->GetCount() == 0 )
    {
        m_UsePCH->SetValue(false);
        m_Pch->Disable();
    }
    else
    {
        m_UsePCH->SetValue(true);
        if ( m_Pch->GetSelection() == wxNOT_FOUND )
        {
            m_Pch->SetSelection(0);
        }
    }

    return m_Pch->GetStringSelection();
}

void wxsNewWindowDlg::OnUsePCHClick(wxCommandEvent& event)
{
    m_Pch->Enable(m_UsePCH->GetValue());
    m_PchGuard->Enable(m_UsePCH->GetValue());
}

void wxsNewWindowDlg::OnCtorParentClick(wxCommandEvent& event)
{
    m_CtorParentDef->Enable(m_CtorParent->GetValue());
}

void wxsNewWindowDlg::OnCtorIdClick(wxCommandEvent& event)
{
    m_CtorIdDef->Enable(m_CtorId->GetValue());
}

void wxsNewWindowDlg::OnCtorPosClick(wxCommandEvent& event)
{
    m_CtorPosDef->Enable(m_CtorPos->GetValue());
}

void wxsNewWindowDlg::OnCtorSizeClick(wxCommandEvent& event)
{
    m_CtorSizeDef->Enable(m_CtorSize->GetValue());
}

void wxsNewWindowDlg::OnCtorParentDefClick(wxCommandEvent& event)
{
    if ( m_CtorParentDef->GetValue() )
    {
        m_CtorIdDef->SetValue(true);
        m_CtorPosDef->SetValue(true);
        m_CtorSizeDef->SetValue(true);
    }
    else
    {
    }
}

void wxsNewWindowDlg::OnCtorIdDefClick(wxCommandEvent& event)
{
    if ( m_CtorIdDef->GetValue() )
    {
        m_CtorPosDef->SetValue(true);
        m_CtorSizeDef->SetValue(true);
    }
    else
    {
        m_CtorParentDef->SetValue(false);
    }
}

void wxsNewWindowDlg::OnCtorPosDefClick(wxCommandEvent& event)
{
    if ( m_CtorPosDef->GetValue() )
    {
        m_CtorSizeDef->SetValue(true);
    }
    else
    {
        m_CtorIdDef->SetValue(false);
        m_CtorParentDef->SetValue(false);
    }
}

void wxsNewWindowDlg::OnCtorSizeDefClick(wxCommandEvent& event)
{
    if ( m_CtorSizeDef->GetValue() )
    {
    }
    else
    {
        m_CtorIdDef->SetValue(false);
        m_CtorParentDef->SetValue(false);
        m_CtorPosDef->SetValue(false);
    }
}

void wxsNewWindowDlg::OnUseInitFuncClick(wxCommandEvent& event)
{
    m_InitFunc->Enable(m_UseInitFunc->GetValue());
}

void wxsNewWindowDlg::OnAdvOpsClick(wxCommandEvent& event)
{
    Freeze();
    m_AdvOpsShown = !m_AdvOpsShown;
    wxString BaseLabel = _("Advanced options");
    if ( m_AdvOpsShown )
    {
        m_RootSizer->Show(m_AdvancedOptionsSizer);
        m_AdvOps->SetLabel(_T("- ") + BaseLabel);
    }
    else
    {
        m_RootSizer->Hide(m_AdvancedOptionsSizer);
        m_AdvOps->SetLabel(_T("+ ") + BaseLabel);
    }
    SetMinSize(wxSize(10,10));
    SetSize(wxSize(10,10));
    Layout();
    m_RootSizer->Fit(this);
    m_RootSizer->SetSizeHints(this);
    Thaw();
}

bool wxsNewWindowDlg::PrepareResource(wxsItemRes* Res,wxsItemResData* Data)
{
    wxsBaseProperties* Props = Data->GetRootItem()->GetBaseProps();

    Props->m_ParentFromArg   = m_CtorParent->GetValue();
    Props->m_IdFromArg       = m_CtorId->GetValue();
    Props->m_PositionFromArg = m_CtorPos->GetValue();
    Props->m_SizeFromArg     = m_CtorSize->GetValue();

    return true;
}

void wxsNewWindowDlg::OnScopeIdsClick(wxCommandEvent& event)
{
    switch ( m_ScopeIdsVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeIdsVal = wxsItemRes::NewResourceParams::Protected; break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeIdsVal = wxsItemRes::NewResourceParams::Private; break;
        default:                                       m_ScopeIdsVal = wxsItemRes::NewResourceParams::Public; break;
    }
    UpdateScopeButtons();
}

void wxsNewWindowDlg::OnScopeMembersClick(wxCommandEvent& event)
{
    switch ( m_ScopeMembersVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeMembersVal = wxsItemRes::NewResourceParams::Protected; break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeMembersVal = wxsItemRes::NewResourceParams::Private; break;
        default:                                       m_ScopeMembersVal = wxsItemRes::NewResourceParams::Public; break;
    }
    UpdateScopeButtons();
}

void wxsNewWindowDlg::OnScopeHandlersClick(wxCommandEvent& event)
{
    switch ( m_ScopeHandlersVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeHandlersVal = wxsItemRes::NewResourceParams::Protected; break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeHandlersVal = wxsItemRes::NewResourceParams::Private; break;
        default:                                       m_ScopeHandlersVal = wxsItemRes::NewResourceParams::Public; break;
    }
    UpdateScopeButtons();
}

void wxsNewWindowDlg::UpdateScopeButtons()
{
    switch ( m_ScopeIdsVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeIds->SetLabel(_T("Public")); break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeIds->SetLabel(_T("Protected")); break;
        default:                                       m_ScopeIds->SetLabel(_T("Private")); break;
    }
    switch ( m_ScopeMembersVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeMembers->SetLabel(_T("Public")); break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeMembers->SetLabel(_T("Protected")); break;
        default:                                       m_ScopeMembers->SetLabel(_T("Private")); break;
    }
    switch ( m_ScopeHandlersVal )
    {
        case wxsItemRes::NewResourceParams::Public:    m_ScopeHandlers->SetLabel(_T("Public")); break;
        case wxsItemRes::NewResourceParams::Protected: m_ScopeHandlers->SetLabel(_T("Protected")); break;
        default:                                       m_ScopeHandlers->SetLabel(_T("Private")); break;
    }
}
