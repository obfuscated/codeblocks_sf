#ifndef IMPEXPCONFIG_H
#define IMPEXPCONFIG_H

#include <wx/dialog.h>

class ImpExpConfig : public wxDialog
{
	public:
		ImpExpConfig(wxWindow* parent);
		virtual ~ImpExpConfig();
	protected:
        void OnImport(wxCommandEvent& event);
        void OnExport(wxCommandEvent& event);
        void OnToggle(wxCommandEvent& event);
        void OnUpdateUI(wxUpdateUIEvent& event);
	private:
        wxString AskForFile(const wxString& msg, int flags);
        DECLARE_EVENT_TABLE();
};

#endif // IMPEXPCONFIG_H
