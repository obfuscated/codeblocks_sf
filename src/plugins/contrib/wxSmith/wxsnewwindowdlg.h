#ifndef WXSNEWWINDOWDLG_H
#define WXSNEWWINDOWDLG_H

//(*Headers(wxsNewWindowDlg)
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
//*)
#include "resources/wxswindowres.h"

class wxsNewWindowDlg : public wxDialog
{
	public:
		wxsNewWindowDlg(wxWindow* parent,const wxString& ResType);
		virtual ~wxsNewWindowDlg();

        //(*Identifiers(wxsNewWindowDlg)
        //*)

	protected:

        //(*Handlers(wxsNewWindowDlg)
        void OnCreate(wxCommandEvent& event);
        void OnCancel(wxCommandEvent& event);
        void OnClassChanged(wxCommandEvent& event);
        void OnSourceChanged(wxCommandEvent& event);
        void OnHeaderChanged(wxCommandEvent& evend);
        void OnUseXrcChange(wxCommandEvent& event);
        void OnXrcChanged(wxCommandEvent& event);
        //*)

	private:

        //(*Declarations(wxsNewWindowDlg)
        wxTextCtrl* Class;
        wxTextCtrl* Header;
        wxTextCtrl* Source;
        wxCheckBox* UseXrc;
        wxTextCtrl* Xrc;
        //*)

        bool SourceNotTouched;
        bool HeaderNotTouched;
        bool XrcNotTouched;
        bool BlockText;
        wxString Type;

        DECLARE_EVENT_TABLE()
};

#endif // WXSNEWDIALOGDLG_H
