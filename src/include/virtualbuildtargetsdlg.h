/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef VIRTUALBUILDTARGETSDLG_H
#define VIRTUALBUILDTARGETSDLG_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(VirtualBuildTargetsDlg)
#include <wx/checklst.h>
#include <wx/listbox.h>
#include <wx/button.h>
#include "scrollingdialog.h"
//*)

class cbProject;

class VirtualBuildTargetsDlg: public wxScrollingDialog
{
	public:

		VirtualBuildTargetsDlg(wxWindow* parent,wxWindowID id, cbProject* project);
		virtual ~VirtualBuildTargetsDlg();

		//(*Identifiers(VirtualBuildTargetsDlg)
		//*)

	protected:

		//(*Handlers(VirtualBuildTargetsDlg)
		void OnAddClick(wxCommandEvent& event);
		void OnEditClick(wxCommandEvent& event);
		void OnRemoveClick(wxCommandEvent& event);
		void OnAliasesSelect(wxCommandEvent& event);
		void OnTargetsToggled(wxCommandEvent& event);
		//*)

		void OnUpdateUI(wxUpdateUIEvent& event);

		//(*Declarations(VirtualBuildTargetsDlg)
		wxListBox* lstAliases;
		wxButton* btnAdd;
		wxButton* btnRemove;
		wxButton* btnEdit;
		wxCheckListBox* lstTargets;
		//*)

        void SetVirtualTarget(const wxString& targetName);
        void CheckTargets();
		cbProject* m_pProject;

	private:

		DECLARE_EVENT_TABLE()
};

#endif
