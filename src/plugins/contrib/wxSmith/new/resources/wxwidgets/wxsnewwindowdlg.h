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
//*)
#include "resources/wxswindowres.h"

class wxsNewWindowDlg : public wxDialog
{
	public:
		wxsNewWindowDlg(wxWindow* parent,const wxString& ResType);
		virtual ~wxsNewWindowDlg();

        //(*Identifiers(wxsNewWindowDlg)
        enum Identifiers
        {
            ID_BUTTON1 = 0x1000,
            ID_BUTTON2,
            ID_CHECKBOX1,
            ID_STATICTEXT1,
            ID_STATICTEXT2,
            ID_STATICTEXT3,
            ID_STATICTEXT4,
            ID_TEXTCTRL1,
            ID_TEXTCTRL2,
            ID_TEXTCTRL3,
            ID_TEXTCTRL4
        };
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

        //(*Declarations(wxsNewWindowDlg)
        wxBoxSizer* BoxSizer1;
        wxStaticBoxSizer* StaticBoxSizer1;
        wxFlexGridSizer* FlexGridSizer1;
        wxTextCtrl* Class;
        wxTextCtrl* Header;
        wxTextCtrl* Source;
        wxCheckBox* UseXrc;
        wxTextCtrl* Xrc;
        wxBoxSizer* BoxSizer2;
        //*)

        virtual bool PrepareResource(wxsWindowRes* Res) { return true; }

	private:

        bool SourceNotTouched;
        bool HeaderNotTouched;
        bool XrcNotTouched;
        bool BlockText;
        wxString Type;

        DECLARE_EVENT_TABLE()
};

#endif
