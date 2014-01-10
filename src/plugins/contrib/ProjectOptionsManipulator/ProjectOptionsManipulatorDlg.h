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
	  enum EProjectScanOption  { eSearch,   eSearchNot, eRemove,       eAdd        };
	  enum EProjectOption      { eCompiler, eLinker,    eCompilerPath, eLinkerPath };
	  enum EProjectLevelOption { eProject,  eTarget                                };

		ProjectOptionsManipulatorDlg(wxWindow* parent,wxWindowID id=wxID_ANY);
		virtual ~ProjectOptionsManipulatorDlg();

    bool               GetScanForWorkspace();
    bool               GetScanForProject();
    size_t             GetProjectIdx();
    EProjectScanOption GetScanOption();
    wxString           GetOption();
    bool               GetOptionActive(EProjectOption opt);
    bool               GetOptionActive(EProjectLevelOption opt);

		//(*Declarations(ProjectOptionsManipulatorDlg)
		wxRadioBox* m_RboOperation;
		wxCheckBox* m_ChkOptionsLinkerPath;
		wxChoice* m_ChoOptionsLevel;
		wxCheckBox* m_ChkOptionsLinker;
		wxCheckBox* m_ChkOptionsCompiler;
		wxChoice* m_ChoScanProjects;
		wxTextCtrl* m_TxtOptions;
		wxChoice* m_ChoScan;
		wxCheckBox* m_ChkOptionsCompilerPath;
		//*)

	protected:

		//(*Identifiers(ProjectOptionsManipulatorDlg)
		static const long ID_CHO_SCAN;
		static const long ID_CHO_SCAN_PROJECTS;
		static const long ID_RBO_OPERATION;
		static const long ID_TXT_OPTIONS;
		static const long ID_CHK_OPTIONS_COMPILER;
		static const long ID_CHK_OPTIONS_LINKER;
		static const long ID_CHK_OPTIONS_COMPILER_PATH;
		static const long ID_CHK_OPTIONS_LINKER_PATH;
		static const long ID_CHO_OPTIONS_LEVEL;
		//*)

	private:

		//(*Handlers(ProjectOptionsManipulatorDlg)
		void OnScanSelect(wxCommandEvent& event);
		//*)

		void OnOk(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

#endif
