#include "impexpconfig.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/checklst.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <configmanager.h>

BEGIN_EVENT_TABLE(ImpExpConfig, wxDialog)
    EVT_BUTTON(XRCID("btnImport"), ImpExpConfig::OnImport)
    EVT_BUTTON(XRCID("btnExport"), ImpExpConfig::OnExport)
    EVT_BUTTON(XRCID("btnToggle"), ImpExpConfig::OnToggle)
END_EVENT_TABLE()

ImpExpConfig::ImpExpConfig(wxWindow* parent)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgImpExpConfig"));
	
	// fill list
	wxCheckListBox* list = XRCCTRL(*this, "lstConf", wxCheckListBox);
	list->Clear();
	const Configurations& confs = ConfigManager::GetConfigurations();
	for (size_t i = 0; i < confs.Count(); ++i)
	{
        list->Append(confs[i].desc);
        list->Check(list->GetCount() - 1, true); // checked by default
	}
}

ImpExpConfig::~ImpExpConfig()
{
	//dtor
}

wxString ImpExpConfig::AskForFile(const wxString& msg, int flags)
{
    wxFileDialog dlg(this, msg, "", "", "*.*", flags);
    if (dlg.ShowModal() == wxID_OK)
        return dlg.GetPath();
    return wxEmptyString;
}

void ImpExpConfig::OnToggle(wxCommandEvent& event)
{
    wxCheckListBox* list = XRCCTRL(*this, "lstConf", wxCheckListBox);
    for (int i = 0; i < list->GetCount(); ++i)
    {
        list->Check(i, !list->IsChecked(i));
    }
}

void ImpExpConfig::OnImport(wxCommandEvent& event)
{
    wxString filename = AskForFile(_("Choose a file to import"), wxOPEN);
    if (filename.IsEmpty())
        return;
}

void ImpExpConfig::OnExport(wxCommandEvent& event)
{
    wxString filename = AskForFile(_("Choose a file to export"), wxSAVE | wxOVERWRITE_PROMPT);
    if (filename.IsEmpty())
        return;
    
    if (wxFileExists(filename))
        wxRemoveFile(filename);

    int count = 0;
    int success_count = 0;
    // block for limiting scope of wxBusyCursor
    {
        wxBusyCursor busy;
        wxCheckListBox* list = XRCCTRL(*this, "lstConf", wxCheckListBox);
        for (int i = 0; i < list->GetCount(); ++i)
        {
            if (list->IsChecked(i))
            {
                if (ConfigManager::ExportToFile(filename, i))
                    ++success_count;
                ++count;
            }
        }
    }
    
    if (count == 0)
        wxMessageBox(_("No configuration settings selected!"), _("Error"), wxICON_ERROR);
    else
    {
        wxString msg;
        msg.Printf(_("%d of %d configuration settings were exported succesfully"), success_count, count);
        wxMessageBox(msg, _("Finished"), wxICON_INFORMATION);
    }
}
