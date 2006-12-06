#include <sdk.h>

#include "regexdlg.h"
#include <wx/regex.h>

#ifndef CB_PRECOMP
    #include <globals.h>
    #include <manager.h>
    #include <configmanager.h>
#endif

BEGIN_EVENT_TABLE(RegExDlg,wxDialog)
	//(*EventTable(RegExDlg)
	EVT_INIT_DIALOG(RegExDlg::OnInit)
	EVT_BUTTON(XRCID("ID_BTN_TEXT"),RegExDlg::OnTestClick)
	//*)
END_EVENT_TABLE()

RegExDlg::RegExDlg(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(RegExDlg)
	wxXmlResource::Get()->LoadDialog(this,parent,_T("RegExDlg"));
	txtRegEx = XRCCTRL(*this,"ID_TXT_REGEX",wxTextCtrl);
	txtInput = XRCCTRL(*this,"ID_TXT_INPUT",wxTextCtrl);
	txtOutput = XRCCTRL(*this,"ID_TXT_OUTPUT",wxTextCtrl);
	btnTest = XRCCTRL(*this,"ID_BTN_TEXT",wxButton);
	btnCancel = XRCCTRL(*this,"wxID_CANCEL",wxButton);
	//*)
}

RegExDlg::~RegExDlg()
{
}

void RegExDlg::OnTestClick(wxCommandEvent& event)
{
    wxString reS = txtRegEx->GetValue().Trim();
    wxString inS = txtInput->GetValue().Trim();

    if (reS.IsEmpty() || inS.IsEmpty())
    {
        cbMessageBox(_("Please enter both a regular expression and a test input..."), _("Warning"), wxICON_WARNING);
        return;
    }

    wxRegEx re;
    if (!re.Compile(reS))
    {
        cbMessageBox(_("Could not compile regular expression.\n"
                       "Check its syntax..."), _("Error"), wxICON_ERROR);
        return;
    }

    txtOutput->Clear();

    if (!re.Matches(inS))
    {
        cbMessageBox(_("No match..."), _("Sorry"), wxICON_WARNING);
        return;
    }

    wxString result;
    result << _("Match for: ") << inS << _T("\n\n");
    for (size_t i = 1; i < re.GetMatchCount(); ++i)
    {
        wxString match = wxString::Format(_("Expression %d: %s\n"), i, re.GetMatch(inS, i).c_str());
        result << match;
    }
    txtOutput->SetValue(result);
}

void RegExDlg::EndModal(int retCode)
{
    Manager::Get()->GetConfigManager(_T("regex_testbed"))->Write(_T("/regex"), txtRegEx->GetValue());
    Manager::Get()->GetConfigManager(_T("regex_testbed"))->Write(_T("/input"), txtInput->GetValue());

    wxDialog::EndModal(retCode);
}

void RegExDlg::OnInit(wxInitDialogEvent& event)
{
    txtRegEx->SetValue(Manager::Get()->GetConfigManager(_T("regex_testbed"))->Read(_T("/regex"), wxEmptyString));
    txtInput->SetValue(Manager::Get()->GetConfigManager(_T("regex_testbed"))->Read(_T("/input"), wxEmptyString));
}

void RegExDlg::OnRegExItemActivated(wxListEvent& event)
{
    //
}
