#include "linklibdlg.h"
#include <cbproject.h>
#include <compiler.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/filename.h>

BEGIN_EVENT_TABLE(LinkLibDlg, wxDialog)
    EVT_UPDATE_UI(-1, LinkLibDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnBrowse"), LinkLibDlg::OnBrowse)
END_EVENT_TABLE()

LinkLibDlg::LinkLibDlg(wxWindow* parent, cbProject* project, ProjectBuildTarget* target, Compiler* compiler, const wxString& lib)
    : m_pProject(project),
    m_pTarget(target),
    m_pCompiler(compiler),
    m_Lib(lib)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgLinkLib"));

	XRCCTRL(*this, "txtLib", wxTextCtrl)->SetValue(lib);
}

LinkLibDlg::~LinkLibDlg()
{
	//dtor
}

void LinkLibDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileName fname(XRCCTRL(*this, "txtLib", wxTextCtrl)->GetValue());
    wxFileDialog dlg(this, _("Choose library"), fname.GetPath(), fname.GetFullName());
    if (dlg.ShowModal() == wxID_OK)
        XRCCTRL(*this, "txtLib", wxTextCtrl)->SetValue(dlg.GetPath());
        
}

void LinkLibDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxButton* btn = (wxButton*)FindWindow(wxID_OK);
    if (btn)
        btn->Enable(!XRCCTRL(*this, "txtLib", wxTextCtrl)->GetValue().IsEmpty());
}

void LinkLibDlg::EndModal(int retCode)
{
    // update m_Lib
    m_Lib = XRCCTRL(*this, "txtLib", wxTextCtrl)->GetValue();
    wxDialog::EndModal(retCode);
}
