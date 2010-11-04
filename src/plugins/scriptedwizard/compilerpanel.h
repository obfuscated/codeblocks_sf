/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef COMPILERPANEL_H
#define COMPILERPANEL_H

#include <wx/string.h>

//(*Headers(CompilerPanel)
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/combobox.h>
//*)

class CompilerPanel: public wxPanel
{
	public:

		CompilerPanel(wxWindow* parent, wxWindow* parentDialog);
		virtual ~CompilerPanel();

        wxComboBox* GetCompilerCombo(){ return cmbCompiler; }
        void EnableConfigurationTargets(bool en);

        void SetWantDebug(bool want){ chkConfDebug->SetValue(want); }
        bool GetWantDebug() const { return chkConfDebug->IsChecked() && chkConfDebug->IsShown(); }
        void SetDebugName(const wxString& name){ txtDbgName->SetValue(name); }
        wxString GetDebugName() const { return txtDbgName->GetValue(); }
        void SetDebugOutputDir(const wxString& dir){ txtDbgOut->SetValue(dir); }
        wxString GetDebugOutputDir() const { return txtDbgOut->GetValue(); }
        void SetDebugObjectOutputDir(const wxString& dir){ txtDbgObjOut->SetValue(dir); }
        wxString GetDebugObjectOutputDir() const { return txtDbgObjOut->GetValue(); }

        void SetWantRelease(bool want){ chkConfRelease->SetValue(want); }
        bool GetWantRelease() const { return chkConfRelease->IsChecked() && chkConfRelease->IsShown(); }
        void SetReleaseName(const wxString& name){ txtRelName->SetValue(name); }
        wxString GetReleaseName() const { return txtRelName->GetValue(); }
        void SetReleaseOutputDir(const wxString& dir){ txtRelOut->SetValue(dir); }
        wxString GetReleaseOutputDir() const { return txtRelOut->GetValue(); }
        void SetReleaseObjectOutputDir(const wxString& dir){ txtRelObjOut->SetValue(dir); }
        wxString GetReleaseObjectOutputDir() const { return txtRelObjOut->GetValue(); }

		//(*Identifiers(CompilerPanel)
		static const long ID_STATICTEXT1;
		static const long ID_STATICTEXT2;
		static const long ID_COMBOBOX1;
		static const long ID_CHECKBOX1;
		static const long ID_TEXTCTRL3;
		static const long ID_STATICTEXT3;
		static const long ID_TEXTCTRL1;
		static const long ID_STATICTEXT4;
		static const long ID_TEXTCTRL2;
		static const long ID_CHECKBOX3;
		static const long ID_TEXTCTRL4;
		static const long ID_STATICTEXT7;
		static const long ID_TEXTCTRL5;
		static const long ID_STATICTEXT8;
		static const long ID_TEXTCTRL6;
		//*)

	private:

		//(*Handlers(CompilerPanel)
		void OnDebugChange(wxCommandEvent& event);
		void OnReleaseChange(wxCommandEvent& event);
		//*)

		//(*Declarations(CompilerPanel)
		wxBoxSizer* BoxSizer4;
		wxStaticBoxSizer* StaticBoxSizer2;
		wxCheckBox* chkConfRelease;
		wxBoxSizer* BoxSizer5;
		wxTextCtrl* txtRelName;
		wxStaticText* StaticText1;
		wxComboBox* cmbCompiler;
		wxTextCtrl* txtRelOut;
		wxTextCtrl* txtDbgObjOut;
		wxTextCtrl* txtDbgOut;
		wxStaticBoxSizer* StaticBoxSizer1;
		wxCheckBox* chkConfDebug;
		wxTextCtrl* txtDbgName;
		wxTextCtrl* txtRelObjOut;
		//*)

        wxWindow* m_parentDialog;

		DECLARE_EVENT_TABLE()
};

#endif
