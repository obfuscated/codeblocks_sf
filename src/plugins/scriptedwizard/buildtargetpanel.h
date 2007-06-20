#ifndef BUILDTARGETPANEL_H
#define BUILDTARGETPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(BuildTargetPanel)
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
//*)

class BuildTargetPanel: public wxPanel
{
    public:

        BuildTargetPanel(wxWindow* parent,wxWindowID id = -1);
        virtual ~BuildTargetPanel();

        void ShowCompiler(bool show)
        {
            lblCompiler->Show(show);
            cmbCompiler->Show(show);
        }
        void SetTargetName(const wxString& name)
        {
            txtName->SetValue(name);
        }
        void SetEnableDebug(bool debug)
        {
            chkEnableDebug->SetValue(debug);
        }

        wxComboBox* GetCompilerCombo(){ return cmbCompiler; }
        bool GetEnableDebug() const { return chkEnableDebug->IsChecked(); }
        wxString GetTargetName() const { return txtName->GetValue(); }
        wxString GetOutputDir() const { return txtOut->GetValue(); }
        wxString GetObjectOutputDir() const { return txtObjOut->GetValue(); }


        //(*Identifiers(BuildTargetPanel)
        static const long ID_STATICTEXT1;
        static const long ID_STATICTEXT3;
        static const long ID_TEXTCTRL1;
        static const long ID_STATICTEXT2;
        static const long ID_COMBOBOX1;
        static const long ID_STATICTEXT4;
        static const long ID_TEXTCTRL2;
        static const long ID_STATICTEXT5;
        static const long ID_TEXTCTRL3;
        static const long ID_CHECKBOX1;
        //*)

    private:

        //(*Handlers(BuildTargetPanel)
        void OntxtNameText(wxCommandEvent& event);
        //*)

        //(*Declarations(BuildTargetPanel)
        wxBoxSizer* BoxSizer1;
        wxStaticText* StaticText3;
        wxTextCtrl* txtName;
        wxStaticText* lblCompiler;
        wxComboBox* cmbCompiler;
        wxTextCtrl* txtOut;
        wxTextCtrl* txtObjOut;
        wxCheckBox* chkEnableDebug;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
