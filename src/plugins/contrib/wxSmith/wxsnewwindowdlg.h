#ifndef WXSNEWWINDOWDLG_H
#define WXSNEWWINDOWDLG_H

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include "resources/wxswindowres.h"

class wxsNewWindowDlg : public wxDialog
{
	public:
		wxsNewWindowDlg(wxWindow* parent,wxsWindowRes::WindowResType Type);
		virtual ~wxsNewWindowDlg();
		
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
        wxsWindowRes::WindowResType Type;
        
        DECLARE_EVENT_TABLE()
};

#endif // WXSNEWDIALOGDLG_H
