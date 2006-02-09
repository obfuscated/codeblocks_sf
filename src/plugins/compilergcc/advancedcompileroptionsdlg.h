#ifndef ADVANCEDCOMPILEROPTIONSDLG_H
#define ADVANCEDCOMPILEROPTIONSDLG_H

#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <compiler.h>

class AdvancedCompilerOptionsDlg : public wxDialog
{
	public:
		AdvancedCompilerOptionsDlg(wxWindow* parent, const wxString& compilerId);
		virtual ~AdvancedCompilerOptionsDlg();
	protected:
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
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
	private:
        wxString ControlCharsToString(const wxString& src);
        wxString StringToControlChars(const wxString& src);
        DECLARE_EVENT_TABLE();
};

#endif // ADVANCEDCOMPILEROPTIONSDLG_H

