#ifndef CLASSWIZARDDLG_H
#define CLASSWIZARDDLG_H

#include <settings.h> // SDK
#include <wx/dialog.h>

class ClassWizardDlg : public wxDialog
{
    public:
        ClassWizardDlg(wxWindow* parent);
        ~ClassWizardDlg();
		const wxString& GetHeaderFilename(){ return m_Header; }
		const wxString& GetImplementationFilename(){ return m_Implementation; }
	private:
    	void OnUpdateUI(wxUpdateUIEvent& event);
		void OnOKClick(wxCommandEvent& event);
		void OnCancelClick(wxCommandEvent& event);
		void OnNameChange(wxCommandEvent& event);
		void DoGuardBlock();
		
		wxString m_Header;
		wxString m_Implementation;
		
        DECLARE_EVENT_TABLE()
};

#endif // CLASSWIZARDDLG_H
