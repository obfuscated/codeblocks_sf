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
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include "scrollingdialog.h"
#include <wx/combobox.h>
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
    wxBoxSizer* m_RootSizer;
    wxFlexGridSizer* FlexGridSizer4;
    wxTextCtrl* m_Header;
    wxCheckBox* m_UseFwdDecl;
    wxTextCtrl* m_CtorCustom;
    wxButton* m_ScopeHandlers;
    wxTextCtrl* m_InitFunc;
    wxTextCtrl* m_BaseClass;
    wxCheckBox* m_CtorIdDef;
    wxTextCtrl* m_Source;
    wxCheckBox* m_CtorParent;
    wxCheckBox* m_CtorId;
    wxCheckBox* m_AddWxs;
    wxFlexGridSizer* FlexGridSizer3;
    wxTextCtrl* m_PchGuard;
    wxTextCtrl* m_Class;
    wxCheckBox* m_UsePCH;
    wxFlexGridSizer* FlexGridSizer2;
    wxStaticBoxSizer* m_AdvancedOptionsSizer;
    wxCheckBox* m_CtorParentDef;
    wxCheckBox* m_CtorSizeDef;
    wxStaticBoxSizer* StaticBoxSizer3;
    wxCheckBox* m_XRCAutoload;
    wxTextCtrl* m_Xrc;
    wxButton* m_ScopeIds;
    wxButton* m_ScopeMembers;
    wxBoxSizer* BoxSizer1;
    wxCheckBox* m_UseXrc;
    wxButton* m_AdvOps;
    wxCheckBox* m_CtorPosDef;
    wxFlexGridSizer* FlexGridSizer1;
    wxCheckBox* m_CtorPos;
    wxCheckBox* m_UseI18n;
    wxComboBox* m_Pch;
    wxStdDialogButtonSizer* StdDialogButtonSizer1;
    wxCheckBox* m_CtorSize;
    wxCheckBox* m_UseInitFunc;
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
