#ifndef ADVANCEDCOMPILEROPTIONSDLG_H
#define ADVANCEDCOMPILEROPTIONSDLG_H

#include <wx/dialog.h>
#include <wx/string.h>
#include "compiler.h"	// RegExArray, COMPILER_COMMAND_TYPES_COUNT

class wxCommandEvent;
class wxSpinEvent;

class AdvancedCompilerOptionsDlg : public wxDialog
{
	public:
		AdvancedCompilerOptionsDlg(wxWindow* parent, const wxString& compilerId);
		virtual ~AdvancedCompilerOptionsDlg();
		bool IsDirty() const {return m_bDirty;}
	private:
        void OnCommandsChange(wxCommandEvent& event);
        void OnRegexChange(wxCommandEvent& event);
        void OnRegexTest(wxCommandEvent& event);
        void OnRegexAdd(wxCommandEvent& event);
        void OnRegexDelete(wxCommandEvent& event);
        void OnRegexDefaults(wxCommandEvent& event);
        void OnRegexUp(wxSpinEvent& event);
        void OnRegexDown(wxSpinEvent& event);

        void EndModal(int retCode);
        void ReadCompilerOptions();
        void WriteCompilerOptions();
        void DisplayCommand(int nr);

        void FillRegexes();
        void FillRegexDetails(int index);
        void SaveRegexDetails(int index);

        wxString m_CompilerId;
        int m_LastCmdIndex;
        RegExArray m_Regexes;
        int m_SelectedRegex;
        bool m_bDirty;		//!< Have changes being made to settings in the dialog
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
        DECLARE_EVENT_TABLE();
};

#endif // ADVANCEDCOMPILEROPTIONSDLG_H

