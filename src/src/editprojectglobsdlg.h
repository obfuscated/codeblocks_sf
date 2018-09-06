#ifndef DLGEDITPROJECTGLOB_H
#define DLGEDITPROJECTGLOB_H

//(*Headers(EditProjectGlobsDlg)
#include <wx/bmpbuttn.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/gbsizer.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

#include "cbproject.h"

class EditProjectGlobsDlg: public wxDialog
{
    public:

        EditProjectGlobsDlg(cbProject::Glob &glob, wxWindow* parent, wxWindowID id = wxID_ANY,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize);
        ~EditProjectGlobsDlg() override;

        cbProject::Glob GetGlob();
    private:
        //(*Declarations(EditProjectGlobsDlg)
        wxCheckBox* m_CheckRecursive;
        wxTextCtrl* m_TextPath;
        wxTextCtrl* m_TextWildcart;
        //*)

    private:

        //(*Identifiers(EditProjectGlobsDlg)
        static const long ID_TEXTCTRL_PATH;
        static const long ID_BTN_BROWSE;
        static const long ID_BTN_OTHER;
        static const long ID_TEXTCTRL_WILDCART;
        static const long ID_CHECK_RECURSIVE;
        //*)

    private:

        //(*Handlers(EditProjectGlobsDlg)
        void OnBrowseClick(wxCommandEvent& event);
        void OnOtherClick(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
