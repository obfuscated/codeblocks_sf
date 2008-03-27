/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
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
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#ifndef PROJECTCONFIGURATIONPANEL_H
#define PROJECTCONFIGURATIONPANEL_H

//(*Headers(ProjectConfigurationPanel)
#include <wx/panel.h>
#include <wx/timer.h>
class wxCheckBox;
class wxTextCtrl;
class wxTreeCtrl;
class wxTreeEvent;
class wxListBox;
class wxStaticText;
class wxBoxSizer;
class wxButton;
class wxStaticBoxSizer;
//*)

#include <configurationpanel.h>
#include <wx/treectrl.h>
#include "projectconfiguration.h"
#include "resultmap.h"


class ProjectConfigurationPanel: public cbConfigurationPanel
{
	public:

		ProjectConfigurationPanel(wxWindow* parent,ProjectConfiguration* Config,cbProject* Proj,TypedResults& KnownLibs);
		virtual ~ProjectConfigurationPanel();

	private:

        WX_DECLARE_STRING_HASH_MAP(wxTreeItemId,IdsMap);

        virtual wxString GetTitle() const;
        virtual wxString GetBitmapBaseName() const;
        virtual void OnApply();
        virtual void OnCancel();

        void LoadData();
        void StoreData();

        void FillKnownLibraries();

        void BuildEntry(const wxTreeItemId& Id,ResultArray& Array);
        wxTreeItemId OtherCategoryId();
        wxTreeItemId PkgConfigId();
        wxTreeItemId CategoryId(const wxString& Category);

        ProjectConfiguration* m_Configuration;
        ProjectConfiguration  m_ConfCopy;
        cbProject* m_Project;
        TypedResults& m_KnownLibs;
        IdsMap m_CategoryMap;
        bool m_IsOtherCategory;
        bool m_IsPkgConfig;

		//(*Declarations(ProjectConfigurationPanel)
		wxButton* m_AddUnknown;
		wxCheckBox* m_Tree;
		wxButton* Button1;
		wxStaticText* StaticText1;
		wxTreeCtrl* m_KnownLibrariesTree;
		wxTextCtrl* m_UnknownLibrary;
		wxTextCtrl* m_Filter;
		wxButton* m_AddScript;
		wxButton* m_Add;
		wxCheckBox* m_NoAuto;
		wxListBox* m_UsedLibraries;
		wxStaticText* m_EventText;
		wxTimer Timer1;
		wxButton* m_Remove;
		//*)

		//(*Identifiers(ProjectConfigurationPanel)
		static const long ID_LISTBOX1;
		static const long ID_CHECKBOX2;
		static const long ID_BUTTON4;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_TREECTRL1;
		static const long ID_STATICTEXT1;
		static const long ID_TEXTCTRL2;
		static const long ID_CHECKBOX1;
		static const long ID_BUTTON5;
		static const long ID_TEXTCTRL1;
		static const long ID_BUTTON3;
		static const long ID_STATICTEXT2;
		static const long ID_TIMER1;
		//*)

		//(*Handlers(ProjectConfigurationPanel)
		void Onm_TreeClick(wxCommandEvent& event);
		void OnTimer1Trigger(wxTimerEvent& event);
		void Onm_FilterText(wxCommandEvent& event);
		void Onm_FilterTextEnter(wxCommandEvent& event);
		void Onm_KnownLibrariesTreeSelectionChanged(wxTreeEvent& event);
		void Onm_UsedLibrariesSelect(wxCommandEvent& event);
		void Onm_RemoveClick(wxCommandEvent& event);
		void Onm_AddClick(wxCommandEvent& event);
		void Onm_UnknownLibraryText(wxCommandEvent& event);
		void Onm_AddUnknownClick(wxCommandEvent& event);
		void Onm_AddScriptClick(wxCommandEvent& event);
		//*)

		wxString GetUserListName(const wxString &VarName);

		DECLARE_EVENT_TABLE()
};

#endif
