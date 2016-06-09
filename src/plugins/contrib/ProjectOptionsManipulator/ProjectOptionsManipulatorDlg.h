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
class wxButton;
class wxStaticBoxSizer;
class wxChoice;
//*)

class ProjectOptionsManipulatorDlg: public wxDialog
{
	public:
	  enum EProjectScanOption       { eSearch, eSearchNot, eRemove,
	                                  eAdd,    eReplace,   eFiles, eChangeCompiler };
	  enum EProjectLevelOption      { eProject, eTarget };
	  enum EProjectSearchOption     { eEquals, eContains };
	  enum EProjectOption           { eReplacePattern,
	                                  eCompiler,      eLinker,      eResCompiler,
	                                  eCompilerPaths, eLinkerPaths, eResCompPaths,
	                                  eLinkerLibs,    eCustomVars };
	  enum EProjectTargetTypeOption { eAll, eApplication, eStaticLib, eDynamicLib };

		ProjectOptionsManipulatorDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~ProjectOptionsManipulatorDlg();

    bool                     GetScanForWorkspace();
    bool                     GetScanForProject();
    size_t                   GetProjectIdx();
    EProjectScanOption       GetScanOption();
    EProjectSearchOption     GetSearchOption();
    wxString                 GetSearchFor();
    wxString                 GetReplaceWith();
    wxString                 GetCustomVarValue();
    bool                     GetOptionActive(EProjectOption opt);
    bool                     GetOptionActive(EProjectLevelOption opt);
    EProjectTargetTypeOption GetTargetTypeOption();

		//(*Declarations(ProjectOptionsManipulatorDlg)
		wxRadioBox* m_RboOperation;
		wxTextCtrl* m_TxtCustomVar;
		wxCheckBox* m_ChkOptionReplacePattern;
		wxCheckBox* m_ChkOptionsCustomVar;
		wxCheckBox* m_ChkOptionsLinkerPath;
		wxButton* m_BtnSearchCompilerDest;
		wxCheckBox* m_ChkOptionsResCompiler;
		wxTextCtrl* m_TxtOptionReplace;
		wxChoice* m_ChoOptionLevel;
		wxCheckBox* m_ChkOptionsLinker;
		wxCheckBox* m_ChkOptionsLinkerLibs;
		wxCheckBox* m_ChkOptionsCompiler;
		wxChoice* m_ChoScanProjects;
		wxButton* m_BtnSearchCompilerSrc;
		wxChoice* m_ChoTargetType;
		wxRadioBox* m_RboOptionSearch;
		wxChoice* m_ChoScan;
		wxCheckBox* m_ChkOptionsResCompPath;
		wxCheckBox* m_ChkOptionsCompilerPath;
		wxTextCtrl* m_TxtOptionSearch;
		//*)

	protected:

		//(*Identifiers(ProjectOptionsManipulatorDlg)
		static const long ID_CHO_SCAN;
		static const long ID_CHO_SCAN_PROJECTS;
		static const long ID_RBO_OPERATION;
		static const long ID_CHO_OPTION_LEVEL;
		static const long ID_TXT_OPTION_SEARCH;
		static const long ID_BTN_SEARCH_COMPILER_SRC;
		static const long TD_TXT_OPTION_REPLACE;
		static const long ID_BTN_SEARCH_COMPILER_DEST;
		static const long ID_CHK_OPTION_REPLACE_PATTERN;
		static const long ID_RBO_OPTION_SEARCH;
		static const long ID_CHK_OPTIONS_COMPILER;
		static const long ID_CHK_OPTIONS_LINKER;
		static const long ID_CHK_OPTIONS_RES_COMPILER;
		static const long ID_CHK_OPTIONS_COMPILER_PATH;
		static const long ID_CHK_OPTIONS_LINKER_PATH;
		static const long ID_CHK_OPTIONS_RES_COMP_PATH;
		static const long ID_CHK_OPTIONS_LINKER_LIBS;
		static const long ID_CHK_OPTIONS_CUSTOM_VAR;
		static const long ID_TXT_CUSTOM_VAR;
		static const long ID_CHO_TARGET_TYPE;
		//*)

	private:

		//(*Handlers(ProjectOptionsManipulatorDlg)
		void OnScanSelect(wxCommandEvent& event);
		void OnOperationSelect(wxCommandEvent& event);
		void OnTargetTypeSelect(wxCommandEvent& event);
		void OnSearchCompilerClick(wxCommandEvent& event);
		//*)

		void OnOk(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
