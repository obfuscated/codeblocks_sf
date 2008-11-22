/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * Copyright: 2008 Jens Lody
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/choice.h>
    #include <wx/colordlg.h>
    #include <configmanager.h>
#endif

#include "IncrementalSearchConfDlg.h"

BEGIN_EVENT_TABLE(IncrementalSearchConfDlg,wxPanel)
    EVT_BUTTON(XRCID("btnIncSearchConfColourFound"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfColourHighlight"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfNotFoundBG"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfWrappedBG"),IncrementalSearchConfDlg::OnChooseColour)
END_EVENT_TABLE()

IncrementalSearchConfDlg::IncrementalSearchConfDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadObject(this,parent,_T("IncrementalSearchConfDlg"),_T("wxPanel"));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    // initialise colour-values
    long red    = 0l;
    long green  = 0l;
    long blue   = 0l;

    // get values from conf-file or predefine them with default values
    red     = cfg->ReadInt(_T("/incremental_search/text_found_colour_red_value"),    0xa0);
    green   = cfg->ReadInt(_T("/incremental_search/text_found_colour_green_value"),  0x20);
    blue    = cfg->ReadInt(_T("/incremental_search/text_found_colour_blue_value"),   0xf0);
    // set the background-colour of the button
    XRCCTRL(*this, "btnIncSearchConfColourFound", wxButton)->SetBackgroundColour(wxColour(red, green, blue));

    // get values from conf-file or predefine them with default values
    red     = cfg->ReadInt(_T("/incremental_search/highlight_colour_red_value"),    0xff);
    green   = cfg->ReadInt(_T("/incremental_search/highlight_colour_green_value"),  0xa5);
    blue    = cfg->ReadInt(_T("/incremental_search/highlight_colour_blue_value"),   0x00);
    // set the background-colour of the button
    XRCCTRL(*this, "btnIncSearchConfColourHighlight", wxButton)->SetBackgroundColour(wxColour(red, green, blue));

    // get values from conf-file or predefine them with default values
    red     = cfg->ReadInt(_T("/incremental_search/text_not_found_colour_red_value"),    0xff);
    green   = cfg->ReadInt(_T("/incremental_search/text_not_found_colour_green_value"),  0x7f);
    blue    = cfg->ReadInt(_T("/incremental_search/text_not_found_colour_blue_value"),   0x7f);
    // set the background-colour of the button
    XRCCTRL(*this, "btnIncSearchConfNotFoundBG", wxButton)->SetBackgroundColour(wxColour(red, green, blue));

    // get values from conf-file or predefine them with default values
    red     = cfg->ReadInt(_T("/incremental_search/wrapped_colour_red_value"),    0x7f);
    green   = cfg->ReadInt(_T("/incremental_search/wrapped_colour_green_value"),  0x7f);
    blue    = cfg->ReadInt(_T("/incremental_search/wrapped_colour_blue_value"),   0xff);
    // set the background-colour of the button
    XRCCTRL(*this, "btnIncSearchConfWrappedBG", wxButton)->SetBackgroundColour(wxColour(red, green, blue));

    // get value from conf-file or predefine them with default value
    XRCCTRL(*this, "chkIncSearchConfCenterText", wxCheckBox)->SetValue(cfg->ReadBool(_T("/incremental_search/center_found_text_on_screen"),true));
    XRCCTRL(*this, "idIncSearchSelectOnEscape", wxCheckBox)->SetValue(cfg->ReadBool(_T("/incremental_search/select_found_text_on_escape"),false));
    XRCCTRL(*this, "idIncSearchHighlightDefault", wxChoice)->SetSelection(cfg->ReadInt(_T("/incremental_search/highlight_default_state"),0));
    XRCCTRL(*this, "idIncSearchSelectedDefault", wxChoice)->SetSelection(cfg->ReadInt(_T("/incremental_search/selected_default_state"),0));
    XRCCTRL(*this, "idIncSearchMatchCaseDefault", wxChoice)->SetSelection(cfg->ReadInt(_T("/incremental_search/match_case_default_state"),0));
}

IncrementalSearchConfDlg::~IncrementalSearchConfDlg()
{
}

void IncrementalSearchConfDlg::OnChooseColour(wxCommandEvent& event)
{
    wxColourData data;
    wxWindow* sender = FindWindowById(event.GetId());
    data.SetColour(sender->GetBackgroundColour());

    wxColourDialog dlg(this, &data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour colour = dlg.GetColourData().GetColour();
        sender->SetBackgroundColour(colour);
    }
}

void IncrementalSearchConfDlg::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    cfg->Write(_T("/incremental_search/center_found_text_on_screen"), XRCCTRL(*this, "chkIncSearchConfCenterText", wxCheckBox)->IsChecked());
    cfg->Write(_T("/incremental_search/select_found_text_on_escape"),XRCCTRL(*this, "idIncSearchSelectOnEscape", wxCheckBox)->IsChecked());
    cfg->Write(_T("/incremental_search/highlight_default_state"),XRCCTRL(*this, "idIncSearchHighlightDefault", wxChoice)->GetSelection());
    cfg->Write(_T("/incremental_search/selected_default_state"),XRCCTRL(*this, "idIncSearchSelectedDefault", wxChoice)->GetSelection());
    cfg->Write(_T("/incremental_search/match_case_default_state"),XRCCTRL(*this, "idIncSearchMatchCaseDefault", wxChoice)->GetSelection());

    wxColor tmpColour = XRCCTRL(*this, "btnIncSearchConfColourFound", wxButton)->GetBackgroundColour();
    cfg->Write(_T("/incremental_search/text_found_colour_red_value"),    static_cast<int>(tmpColour.Red())    );
    cfg->Write(_T("/incremental_search/text_found_colour_green_value"),  static_cast<int>(tmpColour.Green())  );
    cfg->Write(_T("/incremental_search/text_found_colour_blue_value"),   static_cast<int>(tmpColour.Blue())   );

    tmpColour = XRCCTRL(*this, "btnIncSearchConfColourHighlight", wxButton)->GetBackgroundColour();
    cfg->Write(_T("/incremental_search/highlight_colour_red_value"),    static_cast<int>(tmpColour.Red())    );
    cfg->Write(_T("/incremental_search/highlight_colour_green_value"),  static_cast<int>(tmpColour.Green())  );
    cfg->Write(_T("/incremental_search/highlight_colour_blue_value"),   static_cast<int>(tmpColour.Blue())   );

    tmpColour = XRCCTRL(*this, "btnIncSearchConfNotFoundBG", wxButton)->GetBackgroundColour();
    cfg->Write(_T("/incremental_search/text_not_found_colour_red_value"),    static_cast<int>(tmpColour.Red())    );
    cfg->Write(_T("/incremental_search/text_not_found_colour_green_value"),  static_cast<int>(tmpColour.Green())  );
    cfg->Write(_T("/incremental_search/text_not_found_colour_blue_value"),   static_cast<int>(tmpColour.Blue())   );

    tmpColour = XRCCTRL(*this, "btnIncSearchConfWrappedBG", wxButton)->GetBackgroundColour();
    cfg->Write(_T("/incremental_search/wrapped_colour_red_value"),    static_cast<int>(tmpColour.Red())    );
    cfg->Write(_T("/incremental_search/wrapped_colour_green_value"),  static_cast<int>(tmpColour.Green())  );
    cfg->Write(_T("/incremental_search/wrapped_colour_blue_value"),   static_cast<int>(tmpColour.Blue())   );
}


