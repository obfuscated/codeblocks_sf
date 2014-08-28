#include <sdk.h>
#include <prep.h>
#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
    #include <wx/combobox.h>
#endif
#include <wx/tglbtn.h>

#include "compilerflagdlg.h"
#include "compileroptions.h"

CompilerFlagDlg::CompilerFlagDlg(wxWindow* parent, CompOption* opt, wxArrayString& categ,
                                 const wxString &selectedCategory)
{
    wxXmlResource::Get()->LoadObject(this, parent, wxT("CompilerFlagDlg"), wxT("wxDialog"));
    NameText = (wxTextCtrl*)FindWindow(XRCID("ID_Name"));
    CompilerText = (wxTextCtrl*)FindWindow(XRCID("ID_Compiler"));
    LinkerText = (wxTextCtrl*)FindWindow(XRCID("ID_Linker"));
    CategoryCombo = (wxComboBox*)FindWindow(XRCID("ID_Category"));
    AgainstText = (wxTextCtrl*)FindWindow(XRCID("ID_Against"));
    MessageText = (wxTextCtrl*)FindWindow(XRCID("ID_Message"));
    SupersedeText = (wxTextCtrl*)FindWindow(XRCID("ID_Supersede"));
    ExclusiveToggle = (wxToggleButton*)FindWindow(XRCID("ID_Exclusive"));

    Connect(XRCID("ID_Against"), wxEVT_COMMAND_TEXT_UPDATED, (wxObjectEventFunction)&CompilerFlagDlg::OnAdvancedOption);
    Connect(XRCID("ID_Exclusive"), wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, (wxObjectEventFunction)&CompilerFlagDlg::OnAdvancedOption);

    copt = opt;
    if (!copt->name.IsEmpty())
    {
        SetTitle(_("Modify flag"));
        wxString oName;
        copt->name.EndsWith(wxT("[") + copt->option + wxT("]"), &oName);
        if (oName == wxEmptyString)
            copt->name.EndsWith(wxT("[") + copt->additionalLibs + wxT("]"), &oName);
        if (oName == wxEmptyString)
            oName = copt->name;
        NameText->SetValue(oName.Trim());
        CompilerText->SetValue(copt->option);
        LinkerText->SetValue(copt->additionalLibs);
        CategoryCombo->SetValue(copt->category);
        AgainstText->SetValue(copt->checkAgainst);
        MessageText->SetValue(copt->checkMessage);
        SupersedeText->SetValue(copt->supersedes);
        if (copt->exclusive)
        {
            ExclusiveToggle->SetValue(true);
            ExclusiveToggle->SetLabel(_("True"));
        }
    }
    CategoryCombo->Append(categ);
    if (!selectedCategory.empty())
        CategoryCombo->SetStringSelection(selectedCategory);
    MessageText->Enable(!AgainstText->GetValue().Trim().Trim(false).IsEmpty());

    SetSize(GetPosition().x - 57, wxDefaultCoord, GetMinWidth() + 114, GetMinHeight());
    SetMaxSize(wxSize(-1, GetMinHeight()));
}

CompilerFlagDlg::~CompilerFlagDlg()
{
    // dtor
}

void CompilerFlagDlg::OnAdvancedOption(cb_unused wxCommandEvent& event)
{
    if (ExclusiveToggle->GetValue())
        ExclusiveToggle->SetLabel(_("True"));
    else
        ExclusiveToggle->SetLabel(_("False"));
    MessageText->Enable(!AgainstText->GetValue().Trim().Trim(false).IsEmpty());
}

void CompilerFlagDlg::EndModal(int retCode)
{
    bool same = copt->name.BeforeLast(wxT('[')).Trim() == NameText->GetValue() &&
                copt->option         == CompilerText->GetValue()  &&
                copt->additionalLibs == LinkerText->GetValue()    &&
                copt->category       == CategoryCombo->GetValue() &&
                copt->checkAgainst   == AgainstText->GetValue()   &&
                copt->checkMessage   == MessageText->GetValue()   &&
                copt->supersedes     == SupersedeText->GetValue() &&
                (copt->exclusive     == ExclusiveToggle->GetValue());
    copt->name           = NameText->GetValue().Trim().Trim(false);
    copt->option         = CompilerText->GetValue().Trim().Trim(false);
    copt->additionalLibs = LinkerText->GetValue().Trim().Trim(false);
    copt->category       = CategoryCombo->GetValue().Trim().Trim(false);
    if (copt->category.IsEmpty())
        copt->category = wxT("General");
    copt->checkAgainst = AgainstText->GetValue().Trim().Trim(false);
    if (!copt->checkAgainst.IsEmpty())
        copt->checkMessage = MessageText->GetValue().Trim().Trim(false);
    copt->supersedes = SupersedeText->GetValue().Trim().Trim(false);
    copt->exclusive  = ExclusiveToggle->GetValue();
    if (same || copt->name.IsEmpty() || (copt->option.IsEmpty() && copt->additionalLibs.IsEmpty()))
        wxDialog::EndModal(wxID_CANCEL);
    wxDialog::EndModal(retCode);
}
