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
        
        void EndModal(int retCode);
        void ReadCompilerOptions();
        void WriteCompilerOptions();
        void DisplayCommand(int nr);
        
        int m_CompilerIdx;
        int m_LastCmdIndex;
        wxString m_Commands[COMPILER_COMMAND_TYPES_COUNT];
	private:
        DECLARE_EVENT_TABLE();
};

#endif // ADVANCEDCOMPILEROPTIONSDLG_H

