#ifndef PLUGINWIZARDDLG_H
#define PLUGINWIZARDDLG_H

#include <wx/dialog.h>
#include <settings.h> // SDK
#include <cbplugin.h>

class PluginWizardDlg : public wxDialog
{
    public:
        PluginWizardDlg();
        ~PluginWizardDlg();
		const wxString& GetHeaderFilename(){ return m_Header; }
		const wxString& GetImplementationFilename(){ return m_Implementation; }
	private:
    	void OnUpdateUI(wxUpdateUIEvent& event);
		void OnEditInfoClick(wxCommandEvent& event);
		void OnOKClick(wxCommandEvent& event);
		void OnNameChange(wxCommandEvent& event);
		void DoGuardBlock();
		void DoAddHeaderOption(wxString& buffer, bool has, const wxString& retVal = " -1");
		void DoAddHeaderTool(wxString& buffer);
		void DoAddHeaderCompiler(wxString& buffer);
		void DoAddHeaderDebugger(wxString& buffer);
		void DoAddHeaderCodeCompletion(wxString& buffer);
		void DoAddSourceTool(const wxString& classname, wxString& buffer);
		void DoAddSourceCompiler(const wxString& classname, wxString& buffer);
		void DoAddSourceDebugger(const wxString& classname, wxString& buffer);
		void DoAddSourceCodeCompletion(const wxString& classname, wxString& buffer);

		PluginInfo m_Info;
		wxString m_Header;
		wxString m_Implementation;
		
        DECLARE_EVENT_TABLE()
};

#endif // PLUGINWIZARDDLG_H
