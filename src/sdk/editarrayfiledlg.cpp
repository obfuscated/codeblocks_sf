#include "sdk_precomp.h"
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/button.h>
#include <wx/listbox.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include "editarrayfiledlg.h"

BEGIN_EVENT_TABLE(EditArrayFileDlg, wxDialog)
	EVT_LISTBOX_DCLICK(XRCID("lstItems"), EditArrayFileDlg::OnEdit)
	EVT_BUTTON(XRCID("btnAdd"), EditArrayFileDlg::OnAdd)
	EVT_BUTTON(XRCID("btnEdit"), EditArrayFileDlg::OnEdit)
	EVT_BUTTON(XRCID("btnDelete"), EditArrayFileDlg::OnDelete)
	EVT_UPDATE_UI(-1, EditArrayFileDlg::OnUpdateUI)
END_EVENT_TABLE()

EditArrayFileDlg::EditArrayFileDlg(wxWindow* parent, wxArrayString& array, bool useRelativePaths, const wxString& basePath)
	: m_Array(array),
	m_UseRelativePaths(useRelativePaths),
	m_BasePath(basePath)
{
	//ctor
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgEditArrayString"));

	wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
	list->Clear();
	for (unsigned int i = 0; i < m_Array.GetCount(); ++i)
	{
        wxFileName fname;
        fname.Assign(m_Array[i]);
        if (!m_UseRelativePaths && fname.IsRelative())
            fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_BasePath);
        else if (m_UseRelativePaths && fname.IsAbsolute())
            fname.MakeRelativeTo(m_BasePath);
        m_Array[i] = fname.GetFullPath();
        list->Append(m_Array[i]);
	}
}

EditArrayFileDlg::~EditArrayFileDlg()
{
	//dtor
}

void EditArrayFileDlg::EndModal(int retCode)
{
	if (retCode == wxID_OK)
	{
		wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
		m_Array.Clear();
		for (int i = 0; i < list->GetCount(); ++i)
		{
			m_Array.Add(list->GetString(i));
		}
	}
	wxDialog::EndModal(retCode);
}

// events

void EditArrayFileDlg::OnAdd(wxCommandEvent& event)
{
    wxFileDialog dlg(this,
                    _("Select file"),
                    m_BasePath,
                    _T(""),
                    ALL_FILES_FILTER,
                    wxOPEN);

    if (dlg.ShowModal() != wxID_OK)
        return;
    wxFileName fname;
    fname.Assign(dlg.GetPath());
    if (m_UseRelativePaths)
        fname.MakeRelativeTo(m_BasePath);
    XRCCTRL(*this, "lstItems", wxListBox)->Append(fname.GetFullPath());
}

void EditArrayFileDlg::OnEdit(wxCommandEvent& event)
{
	wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
    wxFileDialog dlg(this,
                    _("Select file"),
                    m_BasePath,
                    list->GetStringSelection(),
                    ALL_FILES_FILTER,
                    wxOPEN);

    if (dlg.ShowModal() != wxID_OK)
        return;
    wxFileName fname;
    fname.Assign(dlg.GetPath());
    if (m_UseRelativePaths)
        fname.MakeRelativeTo(m_BasePath);
	list->SetString(list->GetSelection(), fname.GetFullPath());
}

void EditArrayFileDlg::OnDelete(wxCommandEvent& event)
{
	if (wxMessageBox(_("Delete this item?"), _("Confirm"), wxYES_NO) == wxYES)
	{
		wxListBox* list = XRCCTRL(*this, "lstItems", wxListBox);
		list->Delete(list->GetSelection());
	}
}

void EditArrayFileDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	bool en = XRCCTRL(*this, "lstItems", wxListBox)->GetSelection() != -1;
	XRCCTRL(*this, "btnEdit", wxButton)->Enable(en);
	XRCCTRL(*this, "btnDelete", wxButton)->Enable(en);
}
