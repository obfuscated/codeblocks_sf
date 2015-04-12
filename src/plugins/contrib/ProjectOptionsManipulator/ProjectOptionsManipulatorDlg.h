/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef PROJECTOPTIONSMANIPULATORDLG_H
#define PROJECTOPTIONSMANIPULATORDLG_H

//(*Headers(ProjectOptionsManipulatorDlg)
#include <wx/dialog.h>
class wxStdDialogButtonSizer;
class wxCheckBox;
class wxTextCtrl;
class wxRadioBox;
class wxStaticText;
class wxFlexGridSizer;
class wxBoxSizer;
class wxChoice;
//*)

class ProjectOptionsManipulatorDlg: public wxDialog
{
	public:
	  enum EProjectScanOption   { eSearch, eSearchNot, eRemove, eAdd, eFiles };
	  enum EProjectSearchOption { eEquals, eContains };
	  enum EProjectOption       { eCompiler,      eLinker,      eResCompiler,
	                              eCompilerPaths, eLinkerPaths, eResCompPaths,
	                              eLinkerLibs,    eCustomVars };
	  enum EProjectLevelOption  { eProject, eTarget };

		ProjectOptionsManipulatorDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~ProjectOptionsManipulatorDlg();

    bool                 GetScanForWorkspace();
    bool                 GetScanForProject();
    size_t               GetProjectIdx();
    EProjectScanOption   GetScanOption();
    EProjectSearchOption GetSearchOption();
    wxString             GetOption();
    wxString             GetValue();
    bool                 GetOptionActive(EProjectOption opt);
    bool                 GetOptionActive(EProjectLevelOption opt);

		//(*Declarations(ProjectOptionsManipulatorDlg)
		wxRadioBox* m_RboOperation;
		wxTextCtrl* m_TxtCustomVar;
		wxCheckBox* m_ChkOptionsCustomVar;
		wxCheckBox* m_ChkOptionsLinkerPath;
		wxChoice* m_ChoOptionsLevel;
		wxRadioBox* m_RboSearch;
		wxCheckBox* m_ChkOptionsResCompiler;
		wxCheckBox* m_ChkOptionsLinker;
		wxCheckBox* m_ChkOptionsLinkerLibs;
		wxCheckBox* m_ChkOptionsCompiler;
		wxChoice* m_ChoScanProjects;
		wxTextCtrl* m_TxtOptions;
		wxChoice* m_ChoScan;
		wxCheckBox* m_ChkOptionsResCompPath;
		wxCheckBox* m_ChkOptionsCompilerPath;
		//*)

	protected:

		//(*Identifiers(ProjectOptionsManipulatorDlg)
		static const long ID_CHO_SCAN;
		static const long ID_CHO_SCAN_PROJECTS;
		static const long ID_RBO_OPERATION;
		static const long ID_RBO_SEARCH;
		static const long ID_TXT_OPTIONS;
		static const long ID_CHK_OPTIONS_COMPILER;
		static const long ID_CHK_OPTIONS_LINKER;
		static const long ID_CHK_OPTIONS_RES_COMPILER;
		static const long ID_CHK_OPTIONS_COMPILER_PATH;
		static const long ID_CHK_OPTIONS_LINKER_PATH;
		static const long ID_CHK_OPTIONS_RES_COMP_PATH;
		static const long ID_CHK_OPTIONS_LINKER_LIBS;
		static const long ID_CHK_OPTIONS_CUSTOM_VAR;
		static const long ID_TXT_CUSTOM_VAR;
		static const long ID_CHO_OPTIONS_LEVEL;
		//*)

	private:

		//(*Handlers(ProjectOptionsManipulatorDlg)
		void OnScanSelect(wxCommandEvent& event);
		void OnOperationSelect(wxCommandEvent& event);
		//*)

		void OnOk(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
