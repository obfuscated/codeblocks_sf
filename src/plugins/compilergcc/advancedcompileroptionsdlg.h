#ifndef ADVANCEDCOMPILEROPTIONSDLG_H
#define ADVANCEDCOMPILEROPTIONSDLG_H

#include <wx/dialog.h>
#include <compiler.h>

class AdvancedCompilerOptionsDlg : public wxDialog
{
	public:
		AdvancedCompilerOptionsDlg(wxWindow* parent, int compilerIdx);
		virtual ~AdvancedCompilerOptionsDlg();
	protected:
        void OnCommandsChange(wxCommandEvent& event);
        void OnRegexChange(wxCommandEvent& event);
        void OnRegexTest(wxCommandEvent& event);
        void OnRegexAdd(wxCommandEvent& event);
        void OnRegexDelete(wxCommandEvent& event);
        void OnRegexUp(wxCommandEvent& event);
        void OnRegexDown(wxCommandEvent& event);
        
        void EndModal(int retCode);
        void ReadCompilerOptions();
        void WriteCompilerOptions();
        void DisplayCommand(int nr);
        
        void FillRegexes();
        void FillRegexDetails(int index);
        void SaveRegexDetails(int index);
        
        int m_CompilerIdx;
        int m_LastCmdIndex;
        RegExArray m_Regexes;
        int m_SelectedRegex;
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
	private:
        wxString ControlCharsToString(const wxString& src);
        wxString StringToControlChars(const wxString& src);
        DECLARE_EVENT_TABLE();
};

#endif // ADVANCEDCOMPILEROPTIONSDLG_H

