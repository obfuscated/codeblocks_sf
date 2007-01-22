#ifndef WXSNEWWINDOWDLG_H
#define WXSNEWWINDOWDLG_H

//(*Headers(wxsNewWindowDlg)
#include <wx/checkbox.h>
#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class wxWidgetsRes;
class wxsProject;

class wxsNewWindowDlg : public wxDialog
{
	public:
		wxsNewWindowDlg(wxWindow* parent,const wxString& ResType,wxsProject* Project);
		virtual ~wxsNewWindowDlg();

        //(*Identifiers(wxsNewWindowDlg)
        static const long ID_STATICTEXT1;
        static const long ID_TEXTCTRL1;
        static const long ID_STATICTEXT2;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL3;
        static const long ID_CHECKBOX1;
        static const long ID_TEXTCTRL4;
        static const long ID_STATICTEXT4;
        static const long ID_CUSTOM1;
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
        wxTextCtrl* m_Class;
        wxTextCtrl* m_Header;
        wxTextCtrl* m_Source;
        wxCheckBox* m_UseXrc;
        wxTextCtrl* m_Xrc;
        wxStdDialogButtonSizer* Custom1;
        //*)

        virtual bool PrepareResource(wxWidgetsRes* Res) { return true; }

	private:

        bool m_SourceNotTouched;
        bool m_HeaderNotTouched;
        bool m_XrcNotTouched;
        bool m_BlockText;
        wxString m_Type;
        wxsProject* m_Project;

        DECLARE_EVENT_TABLE()
};

#endif
