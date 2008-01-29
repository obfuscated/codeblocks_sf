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
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/radiobut.h>
#include <wx/panel.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/combobox.h>
//*)

class wxsSettings: public cbConfigurationPanel
{
	public:

		wxsSettings(wxWindow* parent,wxWindowID id = -1);
		virtual ~wxsSettings();

	private:

		//(*Identifiers(wxsSettings)
		static const long ID_STATICTEXT10;
		static const long ID_CHOICE2;
		static const long ID_STATICTEXT11;
		static const long ID_STATICTEXT2;
		static const long ID_COMBOBOX1;
		static const long ID_STATICTEXT3;
		static const long ID_BUTTON1;
		static const long ID_STATICTEXT4;
		static const long ID_BUTTON2;
		static const long ID_CHECKBOX7;
		static const long ID_SPINCTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_CHECKBOX9;
		static const long ID_STATICTEXT5;
		static const long ID_RADIOBUTTON1;
		static const long ID_RADIOBUTTON2;
		static const long ID_STATICTEXT6;
		static const long ID_RADIOBUTTON3;
		static const long ID_RADIOBUTTON4;
		static const long ID_STATICTEXT7;
		static const long ID_SPINCTRL2;
		static const long ID_STATICTEXT8;
		static const long ID_CHECKBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_CHECKBOX3;
		static const long ID_CHECKBOX4;
		static const long ID_STATICTEXT9;
		static const long ID_CHECKBOX5;
		static const long ID_CHECKBOX6;
		static const long ID_STATICTEXT12;
		static const long ID_CHOICE1;
		static const long ID_STATICTEXT13;
		static const long ID_SPINCTRL3;
		static const long ID_CHECKBOX8;
		//*)

		//(*Handlers(wxsSettings)
		void OnDragTargetColClick(wxCommandEvent& event);
		void OnDragParentColClick(wxCommandEvent& event);
		void OnUseGridClick(wxCommandEvent& event);
		//*)

		//(*Declarations(wxsSettings)
		wxStaticText* StaticText10;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxStaticText* StaticText9;
		wxFlexGridSizer* FlexGridSizer4;
		wxComboBox* m_DragAssistType;
		wxCheckBox* chkLeft;
		wxRadioButton* m_Icons32;
		wxChoice* choicePlacement;
		wxStaticText* StaticText13;
		wxFlexGridSizer* FlexGridSizer3;
		wxCheckBox* chkTop;
		wxStaticText* StaticText6;
		wxChoice* m_BrowserPlacements;
		wxFlexGridSizer* FlexGridSizer5;
		wxStaticText* StaticText8;
		wxStaticText* StaticText11;
		wxSpinCtrl* spinProportion;
		wxCheckBox* m_Continous;
		wxFlexGridSizer* FlexGridSizer2;
		wxStaticText* StaticText1;
		wxBoxSizer* BoxSizer2;
		wxRadioButton* m_TIcons16;
		wxRadioButton* m_TIcons32;
		wxCheckBox* chkRight;
		wxCheckBox* chkExpand;
		wxStaticText* StaticText7;
		wxSpinCtrl* m_GridSize;
		wxCheckBox* chkBottom;
		wxSpinCtrl* spinBorder;
		wxRadioButton* m_Icons16;
		wxStaticText* StaticText12;
		wxFlexGridSizer* FlexGridSizer6;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxFlexGridSizer* FlexGridSizer1;
		wxButton* m_DragTargetCol;
		wxCheckBox* chkShaped;
		wxCheckBox* chkBorderDU;
		wxButton* m_DragParentCol;
		wxCheckBox* m_UseGrid;
		//*)

		int m_InitialPlacement;

        wxString GetTitle() const { return _("wxSmith settings"); }
        wxString GetBitmapBaseName() const { return _T("wxsmith"); }

        void OnApply();
        void OnCancel(){}

		DECLARE_EVENT_TABLE()
};

#endif
