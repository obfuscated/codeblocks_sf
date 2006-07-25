#ifndef BUILDTARGETPANEL_H
#define BUILDTARGETPANEL_H

#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

//(*Headers(BuildTargetPanel)
#include <wx/checkbox.h>
#include <wx/combobox.h>
#include <wx/intl.h>
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
        bool GetEnableDebug(){ return chkEnableDebug->IsChecked(); }
        wxString GetTargetName(){ return txtName->GetValue(); }
        wxString GetOutputDir(){ return txtOut->GetValue(); }
        wxString GetObjectOutputDir(){ return txtObjOut->GetValue(); }


        //(*Identifiers(BuildTargetPanel)
        enum Identifiers
        {
            ID_CHECKBOX1 = 0x1000,
            ID_COMBOBOX1,
            ID_STATICTEXT1,
            ID_STATICTEXT2,
            ID_STATICTEXT3,
            ID_STATICTEXT4,
            ID_STATICTEXT5,
            ID_TEXTCTRL1,
            ID_TEXTCTRL2,
            ID_TEXTCTRL3
        };
        //*)

    protected:

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

    private:

        DECLARE_EVENT_TABLE()
};

#endif
