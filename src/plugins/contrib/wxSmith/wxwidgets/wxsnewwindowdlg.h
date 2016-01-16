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

#ifndef WXSNEWWINDOWDLG_H
#define WXSNEWWINDOWDLG_H

//(*Headers(wxsNewWindowDlg)
#include <wx/combobox.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "scrollingdialog.h"
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "wxsitemres.h"

class wxsItemResData;
class wxsProject;

class wxsNewWindowDlg : public wxScrollingDialog
{
public:
    wxsNewWindowDlg(wxWindow* parent, const wxString& ResType, wxsProject* Project);
    virtual ~wxsNewWindowDlg();

    //(*Identifiers(wxsNewWindowDlg)
    static const long ID_TEXTCTRL1;
    static const long ID_TEXTCTRL2;
    static const long ID_TEXTCTRL3;
    static const long ID_CHECKBOX1;
    static const long ID_TEXTCTRL4;
    static const long ID_CHECKBOX3;
    static const long ID_BUTTON1;
    static const long ID_CHECKBOX2;
    static const long ID_COMBOBOX1;
    static const long ID_TEXTCTRL8;
    static const long ID_CHECKBOX4;
    static const long ID_TEXTCTRL5;
    static const long ID_TEXTCTRL6;
    static const long ID_BUTTON2;
    static const long ID_BUTTON3;
    static const long ID_BUTTON4;
    static const long ID_CHECKBOX5;
    static const long ID_CHECKBOX9;
    static const long ID_CHECKBOX6;
    static const long ID_CHECKBOX10;
    static const long ID_CHECKBOX7;
    static const long ID_CHECKBOX11;
    static const long ID_CHECKBOX8;
    static const long ID_CHECKBOX12;
    static const long ID_TEXTCTRL7;
    static const long ID_CHECKBOX14;
    static const long ID_CHECKBOX15;
    static const long ID_CHECKBOX13;
    //*)

protected:
    //(*Handlers(wxsNewWindowDlg)
    void OnCreate(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnClassChanged(wxCommandEvent& event);
    void OnSourceChanged(wxCommandEvent& event);
    void OnHeaderChanged(wxCommandEvent& evend);
    void OnUseXrcChange(wxCommandEvent& event);
    void OnXrcChanged(wxCommandEvent& event);
    void OnUsePCHClick(wxCommandEvent& event);
    void OnCtorParentClick(wxCommandEvent& event);
    void OnCtorIdClick(wxCommandEvent& event);
    void OnCtorPosClick(wxCommandEvent& event);
    void OnCtorSizeClick(wxCommandEvent& event);
    void OnCtorParentDefClick(wxCommandEvent& event);
    void OnCtorIdDefClick(wxCommandEvent& event);
    void OnCtorPosDefClick(wxCommandEvent& event);
    void OnCtorSizeDefClick(wxCommandEvent& event);
    void OnUseInitFuncClick(wxCommandEvent& event);
    void OnAdvOpsClick(wxCommandEvent& event);
    void OnScopeIdsClick(wxCommandEvent& event);
    void OnScopeMembersClick(wxCommandEvent& event);
    void OnScopeHandlersClick(wxCommandEvent& event);
    //*)

    //(*Declarations(wxsNewWindowDlg)
    wxCheckBox* m_UseInitFunc;
    wxTextCtrl* m_InitFunc;
    wxCheckBox* m_CtorParentDef;
    wxCheckBox* m_CtorSize;
    wxTextCtrl* m_Xrc;
    wxCheckBox* m_CtorIdDef;
    wxCheckBox* m_UseI18n;
    wxButton* m_ScopeHandlers;
    wxCheckBox* m_AddWxs;
    wxButton* m_ScopeMembers;
    wxCheckBox* m_UseFwdDecl;
    wxCheckBox* m_CtorPos;
    wxTextCtrl* m_CtorCustom;
    wxBoxSizer* m_RootSizer;
    wxCheckBox* m_CtorSizeDef;
    wxComboBox* m_Pch;
    wxStaticBoxSizer* m_AdvancedOptionsSizer;
    wxCheckBox* m_CtorParent;
    wxTextCtrl* m_PchGuard;
    wxBoxSizer* BoxSizer1;
    wxTextCtrl* m_Source;
    wxTextCtrl* m_BaseClass;
    wxCheckBox* m_CtorPosDef;
    wxTextCtrl* m_Class;
    wxCheckBox* m_XRCAutoload;
    wxCheckBox* m_CtorId;
    wxButton* m_AdvOps;
    wxButton* m_ScopeIds;
    wxCheckBox* m_UseXrc;
    wxTextCtrl* m_Header;
    wxCheckBox* m_UsePCH;
    //*)

    virtual bool PrepareResource(wxsItemRes* Res, wxsItemResData* Data);

    wxString DetectPchFile();

private:

    bool m_SourceNotTouched;
    bool m_HeaderNotTouched;
    bool m_XrcNotTouched;
    bool m_BlockText;
    bool m_AdvOpsShown;
    bool m_AppManaged;
    wxString m_Type;
    wxsProject* m_Project;
    wxString m_SourceDirectory;

    wxsItemRes::NewResourceParams::Scope m_ScopeIdsVal;
    wxsItemRes::NewResourceParams::Scope m_ScopeMembersVal;
    wxsItemRes::NewResourceParams::Scope m_ScopeHandlersVal;

    void UpdateScopeButtons();

    DECLARE_EVENT_TABLE()
};

#endif
