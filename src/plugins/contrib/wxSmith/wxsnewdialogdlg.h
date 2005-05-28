#ifndef WXSNEWDIALOGDLG_H
#define WXSNEWDIALOGDLG_H

#include <wx/dialog.h>
#include <wx/textctrl.h>

class wxsNewDialogDlg : public wxDialog
{
	public:
		wxsNewDialogDlg(wxWindow* parent);
		virtual ~wxsNewDialogDlg();
		
	protected:
	
        void OnCreate(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnClassChanged(wxCommandEvent& event);
        void OnSourceChanged(wxCommandEvent& event);
        void OnHeaderChanged(wxCommandEvent& evend);
	
	private:
        wxTextCtrl* Class;
        wxTextCtrl* Source;
        wxTextCtrl* Header;
        bool SourceNotTouched;
        bool HeaderNotTouched;
        bool BlockText;
        
        DECLARE_EVENT_TABLE()
};

#endif // WXSNEWDIALOGDLG_H
