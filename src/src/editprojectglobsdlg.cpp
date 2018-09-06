

#include <sdk.h>

#ifndef CB_PRECOMP
    #include "macrosmanager.h"
    #include "uservarmanager.h"
#endif

#include "editprojectglobsdlg.h"

//(*InternalHeaders(EditProjectGlobsDlg)
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(EditProjectGlobsDlg)
const long EditProjectGlobsDlg::ID_TEXTCTRL_PATH = wxNewId();
const long EditProjectGlobsDlg::ID_BTN_BROWSE = wxNewId();
const long EditProjectGlobsDlg::ID_BTN_OTHER = wxNewId();
const long EditProjectGlobsDlg::ID_TEXTCTRL_WILDCART = wxNewId();
const long EditProjectGlobsDlg::ID_CHECK_RECURSIVE = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditProjectGlobsDlg,wxDialog)
	//(*EventTable(EditProjectGlobsDlg)
	//*)
END_EVENT_TABLE()

EditProjectGlobsDlg::EditProjectGlobsDlg(const cbProject::Glob &glob, wxWindow* parent)
{
	//(*Initialize(EditProjectGlobsDlg)
	wxBitmapButton* btnBrowse;
	wxBitmapButton* btnOther;
	wxBoxSizer* BoxSizer1;
	wxGridBagSizer* GridBagSizer1;
	wxStaticText* StaticText1;
	wxStaticText* StaticText2;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("wxID_ANY"));
	BoxSizer1 = new wxBoxSizer(wxVERTICAL);
	GridBagSizer1 = new wxGridBagSizer(5, 5);
	StaticText1 = new wxStaticText(this, wxID_ANY, _("Path:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	GridBagSizer1->Add(StaticText1, wxGBPosition(0, 0), wxDefaultSpan, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	m_TextPath = new wxTextCtrl(this, ID_TEXTCTRL_PATH, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_PATH"));
	m_TextPath->SetMinSize(wxSize(400,-1));
	GridBagSizer1->Add(m_TextPath, wxGBPosition(0, 1), wxDefaultSpan, wxEXPAND, 5);
	btnBrowse = new wxBitmapButton(this, ID_BTN_BROWSE, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BTN_BROWSE"));
	GridBagSizer1->Add(btnBrowse, wxGBPosition(0, 2), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnOther = new wxBitmapButton(this, ID_BTN_OTHER, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BTN_OTHER"));
	GridBagSizer1->Add(btnOther, wxGBPosition(0, 3), wxDefaultSpan, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	m_TextWildcart = new wxTextCtrl(this, ID_TEXTCTRL_WILDCART, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL_WILDCART"));
	GridBagSizer1->Add(m_TextWildcart, wxGBPosition(1, 1), wxDefaultSpan, wxEXPAND, 5);
	m_CheckRecursive = new wxCheckBox(this, ID_CHECK_RECURSIVE, _("Recursive"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECK_RECURSIVE"));
	m_CheckRecursive->SetValue(false);
	GridBagSizer1->Add(m_CheckRecursive, wxGBPosition(2, 1), wxGBSpan(1, 3), wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, wxID_ANY, _("Wildcart:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	GridBagSizer1->Add(StaticText2, wxGBPosition(1, 0), wxDefaultSpan, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);
	GridBagSizer1->AddGrowableCol(1);
	BoxSizer1->Add(GridBagSizer1, 0, wxALL|wxEXPAND, 5);
	BoxSizer1->Add(0,0,1, wxALL|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	BoxSizer1->Add(StdDialogButtonSizer1, 0, wxALL|wxEXPAND, 5);
	SetSizer(BoxSizer1);
	BoxSizer1->Fit(this);
	BoxSizer1->SetSizeHints(this);

	Connect(ID_BTN_BROWSE,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EditProjectGlobsDlg::OnBrowseClick);
	Connect(ID_BTN_OTHER,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EditProjectGlobsDlg::OnOtherClick);
	//*)

	m_TextPath->SetValue(glob.m_Path);
	m_TextWildcart->SetValue(glob.m_WildCard);
	m_CheckRecursive->SetValue(glob.m_Recursive);
}

EditProjectGlobsDlg::~EditProjectGlobsDlg()
{
	//(*Destroy(EditProjectGlobsDlg)
	//*)
}

cbProject::Glob EditProjectGlobsDlg::GetGlob()
{
    return cbProject::Glob(m_TextPath->GetValue(), m_TextWildcart->GetValue(),
                           m_CheckRecursive->GetValue());
}


void EditProjectGlobsDlg::OnBrowseClick(cb_unused wxCommandEvent& event)
{
    wxFileName path;
    wxString basePath = Manager::Get()->GetProjectManager()->GetActiveProject()->GetBasePath();

    wxString value = m_TextPath->GetValue();
    const int idx = value.Find(DEFAULT_ARRAY_SEP);
    if (idx != -1)
        value.Remove(idx);


    // Try to "decode" custom var.
    const wxString initialValue = value;
    // Try to resolve the current path for macros
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(value);
    // Make the resolved path absolute.
    wxFileName fname(value);
    fname.MakeAbsolute(basePath);
    const wxString fullPath = fname.GetFullPath();

    // Get the new path from user.
    path = ChooseDirectory(this, _("Get project glob path"), fullPath, basePath, false, true);

    if (path.GetFullPath().empty())
        return;

    // If it was a custom var, see if we can re-insert it.
    if (initialValue != value)
    {
        wxString tmp = path.GetFullPath();
        if (tmp.Replace(value, initialValue) != 0)
        {
            // Replace the part we expressed with a custom variable again with the custom variable
            // name.
            m_TextPath->SetValue(tmp);
            return;
        }
    }

    wxString result;
    if (!basePath.empty())
    {
        // Ask the user if he wants it to be kept as relative.
        const int answer = cbMessageBox(_("Keep this as a relative path?"), _("Question"),
                                        wxICON_QUESTION | wxYES_NO, this);
        if (answer == wxID_YES)
            path.MakeRelativeTo(basePath);
        result = path.GetFullPath();
    }

    m_TextPath->SetValue(result);
}

void EditProjectGlobsDlg::OnOtherClick(cb_unused wxCommandEvent& event)
{
    UserVariableManager *userMgr = Manager::Get()->GetUserVariableManager();

    const wxString &userVar = userMgr->GetVariable(this, m_TextPath->GetValue());
    if (!userVar.empty())
        m_TextPath->SetValue(userVar);
}
