/***************************************************************
 * Name:      codestatconfig.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
 **************************************************************/

#include "codestatconfig.h"

BEGIN_EVENT_TABLE (CodeStatConfigDlg, wxPanel)
EVT_COMBOBOX(XRCID("combo_Names"), CodeStatConfigDlg::ComboBoxEvent)
END_EVENT_TABLE ()

CodeStatConfigDlg::CodeStatConfigDlg(wxWindow* parent, LanguageDef lang[NB_FILETYPES])
{
    wxXmlResource::Get()->LoadPanel(this, parent, _("dlgCodeStatConfig"));
    wxComboBox* combo_Names = XRCCTRL(*this, "combo_Names", wxComboBox);

    // Writing languages names in the combo-box and saving the language classes in a local variable
    for (int i=0; i<NB_FILETYPES; i++)
    {
       languages[i] = lang[i];
       combo_Names->Append(languages[i].name);
    }

    combo_Names->SetSelection(0);
    PrintLanguageInfo(0);
}

CodeStatConfigDlg::~CodeStatConfigDlg()
{
}

void CodeStatConfigDlg::OnApply()
{
    // user pressed OK; save settings
    //SaveSettings();
}

// Print the caracteristics for the language selected
void CodeStatConfigDlg::ComboBoxEvent(wxCommandEvent & event)
{
	PrintLanguageInfo(event.GetSelection());
}

// Print the caracteristics for the language number "id"
void CodeStatConfigDlg::PrintLanguageInfo(int id)
{
	wxStaticText* txt_Name = XRCCTRL(*this, "txt_Name", wxStaticText);
	txt_Name->SetLabel(languages[id].name);
	wxStaticText* txt_FileTypes = XRCCTRL(*this, "txt_FileTypes", wxStaticText);
	wxString ext_string = _T("");
	for (unsigned int i=0; i<languages[id].ext.GetCount(); i++)
	   ext_string = ext_string + _T(" ") + languages[id].ext[i];
	txt_FileTypes->SetLabel(ext_string);
	wxStaticText* txt_SingleComment = XRCCTRL(*this, "txt_SingleComment", wxStaticText);
	txt_SingleComment->SetLabel(languages[id].single_line_comment);
	wxStaticText* txt_MultiLineCommentBegin = XRCCTRL(*this, "txt_MultiLineCommentBegin", wxStaticText);
	txt_MultiLineCommentBegin->SetLabel(languages[id].multiple_line_comment[0]);
	wxStaticText* txt_MultiLineCommentEnd = XRCCTRL(*this, "txt_MultiLineCommentEnd", wxStaticText);
	txt_MultiLineCommentEnd->SetLabel(languages[id].multiple_line_comment[1]);
}
