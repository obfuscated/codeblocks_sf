

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
const long EditProjectGlobsDlg::ID_TEXTCTRL1 = wxNewId();
const long EditProjectGlobsDlg::ID_BITMAPBUTTON1 = wxNewId();
const long EditProjectGlobsDlg::ID_BITMAPBUTTON2 = wxNewId();
const long EditProjectGlobsDlg::ID_CHECKBOX1 = wxNewId();
const long EditProjectGlobsDlg::ID_TEXTCTRL2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditProjectGlobsDlg,wxDialog)
	//(*EventTable(EditProjectGlobsDlg)
	//*)
END_EVENT_TABLE()

EditProjectGlobsDlg::EditProjectGlobsDlg(cbProject::Glob &glob, wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(EditProjectGlobsDlg)
	wxBoxSizer* BoxSizer1;
	wxBoxSizer* BoxSizer2;
	wxGridBagSizer* GridBagSizer1;
	wxStaticText* StaticText1;
	wxStaticText* StaticText2;
	wxStdDialogButtonSizer* StdDialogButtonSizer1;

	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	GridBagSizer1 = new wxGridBagSizer(0, 0);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	StaticText1 = new wxStaticText(this, wxID_ANY, _("Path:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer1->Add(StaticText1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	txtPath = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	BoxSizer1->Add(txtPath, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnBrowse = new wxBitmapButton(this, ID_BITMAPBUTTON1, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_FILE_OPEN")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON1"));
	BoxSizer1->Add(btnBrowse, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	btnOther = new wxBitmapButton(this, ID_BITMAPBUTTON2, wxArtProvider::GetBitmap(wxART_MAKE_ART_ID_FROM_STR(_T("wxART_EXECUTABLE_FILE")),wxART_BUTTON), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW, wxDefaultValidator, _T("ID_BITMAPBUTTON2"));
	BoxSizer1->Add(btnOther, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	GridBagSizer1->Add(BoxSizer1, wxGBPosition(0, 0), wxDefaultSpan, wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	chkRecursive = new wxCheckBox(this, ID_CHECKBOX1, _("Recursive"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	chkRecursive->SetValue(false);
	BoxSizer2->Add(chkRecursive, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StaticText2 = new wxStaticText(this, wxID_ANY, _("Wildcart:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer2->Add(StaticText2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	txtWildcart = new wxTextCtrl(this, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL2"));
	BoxSizer2->Add(txtWildcart, 1, wxEXPAND, 5);
	GridBagSizer1->Add(BoxSizer2, wxGBPosition(1, 0), wxDefaultSpan, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	StdDialogButtonSizer1 = new wxStdDialogButtonSizer();
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	StdDialogButtonSizer1->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	StdDialogButtonSizer1->Realize();
	GridBagSizer1->Add(StdDialogButtonSizer1, wxGBPosition(2, 0), wxDefaultSpan, wxTOP|wxEXPAND, 5);
	GridBagSizer1->AddGrowableCol(0);
	SetSizer(GridBagSizer1);
	GridBagSizer1->Fit(this);
	GridBagSizer1->SetSizeHints(this);

	Connect(ID_BITMAPBUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EditProjectGlobsDlg::OnBrowseClick);
	Connect(ID_BITMAPBUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&EditProjectGlobsDlg::OnOtherClick);
	//*)

	txtPath->SetValue(glob.m_Path);
	txtWildcart->SetValue(glob.m_WildCard);
	chkRecursive->SetValue(glob.m_Recursive);
}

EditProjectGlobsDlg::~EditProjectGlobsDlg()
{
	//(*Destroy(EditProjectGlobsDlg)
	//*)
}

cbProject::Glob EditProjectGlobsDlg::GetGlob()
{
    cbProject::Glob ret(txtPath->GetValue(),
                        txtWildcart->GetValue(),
                        chkRecursive->GetValue());
    return ret;
}


void EditProjectGlobsDlg::OnBrowseClick(wxCommandEvent& event)
{
    wxFileName path;
    wxString basePath = Manager::Get()->GetProjectManager()->GetActiveProject()->GetBasePath();

    wxString val = txtPath->GetValue();
    int idx = val.Find(DEFAULT_ARRAY_SEP);
    if (idx != -1)
        val.Remove(idx);


    // try to "decode" custom var
    wxString initialVal = val;
    // try to resolve the current path for macros
    Manager::Get()->GetMacrosManager()->ReplaceEnvVars(val);
    // Make the resolved path absolute
    wxFileName fname(val);
    fname.MakeAbsolute(basePath);
    wxString fullPath = fname.GetFullPath();

    // Get the new path from user
    path = ChooseDirectory(this, _("Get project glob path"),
                           fullPath,
                           basePath, false, true);

    if (path.GetFullPath().empty())
        return;

    // if it was a custom var, see if we can re-insert it
    if (initialVal != val)
    {
        wxString tmp = path.GetFullPath();
        if (tmp.Replace(val, initialVal) != 0)
        {
            // replace the part we expressed with a custom variable again with
            // the custom variable name
            txtPath->SetValue(tmp);
            return;
        }
    }

    wxString result;
    if (!basePath.empty())
    {
        // ask the user if he wants it to be kept as relative
        if (cbMessageBox(_("Keep this as a relative path?"),
                        _("Question"),
                        wxICON_QUESTION | wxYES_NO, this) == wxID_YES)
        {
            path.MakeRelativeTo(basePath);
        }
        result = path.GetFullPath();
    }

    txtPath->SetValue(result);
}

void EditProjectGlobsDlg::OnOtherClick(wxCommandEvent& event)
{
    UserVariableManager *userMgr = Manager::Get()->GetUserVariableManager();

    const wxString &userVar = userMgr->GetVariable(this, txtPath->GetValue());
    if ( !userVar.empty() )
    {
        txtPath->SetValue(userVar);
    }
}
