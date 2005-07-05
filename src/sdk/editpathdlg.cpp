#include "editpathdlg.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stattext.h>


BEGIN_EVENT_TABLE(EditPathDlg, wxDialog)
    EVT_UPDATE_UI(-1, EditPathDlg::OnUpdateUI)
    EVT_BUTTON(XRCID("btnBrowse"), EditPathDlg::OnBrowse)
END_EVENT_TABLE()

EditPathDlg::EditPathDlg(wxWindow* parent,
        const wxString& path,
        const wxString& basepath,
        const wxString& title,
        const wxString& message,
        const bool wantDir,
        const wxString& filter)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgEditPath"));

	XRCCTRL(*this, "txtPath", wxTextCtrl)->SetValue(path);
	XRCCTRL(*this, "dlgEditPath", wxDialog)->SetTitle(title);
	
	if (!wantDir) {
        XRCCTRL(*this, "lblText", wxStaticText)->SetLabel(_("File:"));
	}
	
	m_Path = path;
	m_WantDir = wantDir;
	m_Message = message;
	m_Basepath = basepath;
	m_Filter = filter;
	m_AskMakeRelative = true;
	m_ShowCreateDirButton = false;
}

EditPathDlg::~EditPathDlg()
{
	//dtor
}

void EditPathDlg::OnBrowse(wxCommandEvent& event)
{
    wxFileName fname(XRCCTRL(*this, "txtPath", wxTextCtrl)->GetValue());
        
    if (m_WantDir)
    {
        wxFileName path(ChooseDirectory(this, m_Message, m_Path,
                m_Basepath, m_AskMakeRelative, m_ShowCreateDirButton));
        
        if (path.GetFullPath().IsEmpty())
            return;
            
        if (m_AskMakeRelative && !m_Basepath.IsEmpty())
        {
            // ask the user if he wants it to be kept as relative
            if (wxMessageBox(_("Keep this as a relative path?"),
                            _("Question"),
                            wxICON_QUESTION | wxYES_NO) == wxYES)
            {
                path.MakeRelativeTo(m_Basepath);
            }
        }
        
        XRCCTRL(*this, "txtPath", wxTextCtrl)->SetValue(path.GetFullPath());
    } else {
        wxFileDialog dlg(this, m_Message, fname.GetPath(), fname.GetFullName(),
                 m_Filter, wxCHANGE_DIR);
        if (dlg.ShowModal() == wxID_OK) {
            wxFileName path(dlg.GetPath());
            if (m_AskMakeRelative && !m_Basepath.IsEmpty())
            {
                // ask the user if he wants it to be kept as relative
                if (wxMessageBox(_("Keep this as a relative path?"),
                                _("Question"),
                                wxICON_QUESTION | wxYES_NO) == wxYES)
                {
                    path.MakeRelativeTo(m_Basepath);
                }
            }
            XRCCTRL(*this, "txtPath", wxTextCtrl)->SetValue(path.GetFullPath());
        }
    }
        
}

void EditPathDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
    wxButton* btn = (wxButton*)FindWindow(wxID_OK);
    if (btn)
        btn->Enable(!XRCCTRL(*this, "txtPath", wxTextCtrl)->GetValue().IsEmpty());
}

void EditPathDlg::EndModal(int retCode)
{
    // update m_Lib
    m_Path = XRCCTRL(*this, "txtPath", wxTextCtrl)->GetValue();
    wxDialog::EndModal(retCode);
}
