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

#ifndef WXSSETTINGS_H
#define WXSSETTINGS_H

#include <wx/intl.h>
#include <configurationpanel.h>

//(*Headers(wxsSettings)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>
//*)

class wxsSettings: public cbConfigurationPanel
{
public:
    wxsSettings(wxWindow* parent, wxWindowID id = -1);
    virtual ~wxsSettings();

protected:
    wxString GetTitle() const { return _("wxSmith settings"); }
    wxString GetBitmapBaseName() const { return _T("wxsmith"); }

    //(*Handlers(wxsSettings)
    void OnDragTargetColClick(wxCommandEvent& event);
    void OnDragParentColClick(wxCommandEvent& event);
    void OnUseGridClick(wxCommandEvent& event);
    //*)

    void OnApply();
    void OnCancel() {}

private:
    //(*Identifiers(wxsSettings)
    static const long ID_CHECKBOX11;
    static const long ID_CHOICE2;
    static const long ID_COMBOBOX1;
    static const long ID_BUTTON1;
    static const long ID_BUTTON2;
    static const long ID_CHECKBOX7;
    static const long ID_SPINCTRL1;
    static const long ID_CHECKBOX9;
    static const long ID_RADIOBUTTON1;
    static const long ID_RADIOBUTTON2;
    static const long ID_RADIOBUTTON3;
    static const long ID_RADIOBUTTON4;
    static const long ID_SPINCTRL2;
    static const long ID_CHECKBOX1;
    static const long ID_CHECKBOX2;
    static const long ID_CHECKBOX3;
    static const long ID_CHECKBOX4;
    static const long ID_CHECKBOX5;
    static const long ID_CHECKBOX6;
    static const long ID_CHOICE1;
    static const long ID_SPINCTRL3;
    static const long ID_CHECKBOX8;
    static const long ID_CHECKBOX10;
    static const long ID_CHECKBOX12;
    static const long ID_RADIOBUTTON5;
    static const long ID_RADIOBUTTON6;
    static const long ID_RADIOBUTTON7;
    static const long ID_RADIOBUTTON8;
    //*)

    //(*Declarations(wxsSettings)
    wxBoxSizer* BoxSizer2;
    wxButton* m_DragParentCol;
    wxButton* m_DragTargetCol;
    wxCheckBox* m_BorderBottom;
    wxCheckBox* m_BorderDU;
    wxCheckBox* m_BorderLeft;
    wxCheckBox* m_BorderRight;
    wxCheckBox* m_BorderTop;
    wxCheckBox* m_Continous;
    wxCheckBox* m_RemovePrefix;
    wxCheckBox* m_SizeExpand;
    wxCheckBox* m_SizeShaped;
    wxCheckBox* m_UniqueIDsOnly;
    wxCheckBox* m_UseGrid;
    wxCheckBox* m_UseI18N;
    wxChoice* m_BrowserPlacements;
    wxChoice* m_Placement;
    wxComboBox* m_DragAssistType;
    wxFlexGridSizer* FlexGridSizer6;
    wxRadioButton* m_Icons16;
    wxRadioButton* m_Icons32;
    wxRadioButton* m_NoneI18N;
    wxRadioButton* m_NoneI18N_T;
    wxRadioButton* m_NoneI18NwxS;
    wxRadioButton* m_NoneI18NwxT;
    wxRadioButton* m_TIcons16;
    wxRadioButton* m_TIcons32;
    wxSpinCtrl* m_Border;
    wxSpinCtrl* m_GridSize;
    wxSpinCtrl* m_Proportion;
    //*)

    int m_InitialPlacement;

    DECLARE_EVENT_TABLE()
};

#endif
