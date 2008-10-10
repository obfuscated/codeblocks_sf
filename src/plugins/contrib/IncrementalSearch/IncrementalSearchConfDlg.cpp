#include "sdk.h"
#ifndef CB_PRECOMP
  #include <wx/button.h>
  #include <wx/checkbox>
  #include <wx/colordlg.h>
  #include <wx/event.h>
  #include "configmanager.h"
  #include "manager.h"
#endif
#include <wx/colour.h>
#include <wx/xrc/xmlres.h>

#include "IncrementalSearchConfDlg.h"

BEGIN_EVENT_TABLE(IncrementalSearchConfDlg,wxPanel)
    EVT_BUTTON(XRCID("btnIncSearchConfColourFound"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfColourHighlight"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfNotFoundBG"),IncrementalSearchConfDlg::OnChooseColour)
    EVT_BUTTON(XRCID("btnIncSearchConfWrappedBG"),IncrementalSearchConfDlg::OnChooseColour)
END_EVENT_TABLE()

IncrementalSearchConfDlg::IncrementalSearchConfDlg(wxWindow* parent)
{
	wxXmlResource::Get()->LoadObject(this,parent,_T("IncrementalSearchConfDlg"),_("wxPanel"));

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

    long red     = cfg->ReadInt(_("/incremental_search/text_found_colour_red_value"),    0xa0);
    long green   = cfg->ReadInt(_("/incremental_search/text_found_colour_green_value"),  0x20);
    long blue    = cfg->ReadInt(_("/incremental_search/text_found_colour_blue_value"),   0xf0);
    XRCCTRL(*this, "btnIncSearchConfColourFound", wxButton)->SetBackgroundColour(wxColour(red, green, blue));
    red     = cfg->ReadInt(_("/incremental_search/highlight_colour_red_value"),    0xff);
    green   = cfg->ReadInt(_("/incremental_search/highlight_colour_green_value"),  0xa5);
    blue    = cfg->ReadInt(_("/incremental_search/highlight_colour_blue_value"),   0x00);
    XRCCTRL(*this, "btnIncSearchConfColourHighlight", wxButton)->SetBackgroundColour(wxColour(red, green, blue));
    red     = cfg->ReadInt(_("/incremental_search/text_not_found_colour_red_value"),    0xff);
    green   = cfg->ReadInt(_("/incremental_search/text_not_found_colour_green_value"),  0x7f);
    blue    = cfg->ReadInt(_("/incremental_search/text_not_found_colour_blue_value"),   0x7f);
    XRCCTRL(*this, "btnIncSearchConfNotFoundBG", wxButton)->SetBackgroundColour(wxColour(red, green, blue));
    red     = cfg->ReadInt(_("/incremental_search/wrapped_colour_red_value"),    0x7f);
    green   = cfg->ReadInt(_("/incremental_search/wrapped_colour_green_value"),  0x7f);
    blue    = cfg->ReadInt(_("/incremental_search/wrapped_colour_blue_value"),   0xff);
    XRCCTRL(*this, "btnIncSearchConfWrappedBG", wxButton)->SetBackgroundColour(wxColour(red, green, blue));
    XRCCTRL(*this, "chkIncSearchConfCenterText", wxCheckBox)->SetValue(cfg->ReadBool(_("/incremental_search/center_found_text_on_screen"),true));
}

IncrementalSearchConfDlg::~IncrementalSearchConfDlg()
{
}

void IncrementalSearchConfDlg::OnChooseColour(wxCommandEvent& event)
{
    wxColourDialog tempColourDlg(this);
    tempColourDlg.Centre();
    wxString strEntry = _("");
    wxButton* pButton = 0L;
    if(event.GetId() == XRCID("btnIncSearchConfColourFound"))
    {
        strEntry = _("text_found");
        pButton=XRCCTRL(*this, "btnIncSearchConfColourFound", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfColourHighlight"))
    {
        strEntry = _("highlight");
        pButton=XRCCTRL(*this, "btnIncSearchConfColourHighlight", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfNotFoundBG"))
    {
        strEntry = _("text_not_found");
        pButton=XRCCTRL(*this, "btnIncSearchConfNotFoundBG", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfWrappedBG"))
    {
        strEntry = _("wrapped");
        pButton=XRCCTRL(*this, "btnIncSearchConfWrappedBG", wxButton);
    }


    if(pButton && !strEntry.empty())
    {
        const int result = tempColourDlg.ShowModal();

        if (result == wxID_OK)
        {
            wxColour tempColour = tempColourDlg.GetColourData().GetColour();
            ConfigManager* cfg = Manager::Get()->GetConfigManager(_("editor"));
            cfg->Write(_("/incremental_search/")+strEntry+_("_colour_red_value"),    static_cast<int>(tempColour.Red())    );
            cfg->Write(_("/incremental_search/")+strEntry+_("_colour_green_value"),  static_cast<int>(tempColour.Green())  );
            cfg->Write(_("/incremental_search/")+strEntry+_("_colour_blue_value"),   static_cast<int>(tempColour.Blue())   );
            pButton->SetBackgroundColour(tempColour);
        }
    }
}

void IncrementalSearchConfDlg::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_("editor"));
    cfg->Write(_("/incremental_search/center_found_text_on_screen"), (bool) XRCCTRL(*this, "chkIncSearchConfCenterText", wxCheckBox)->IsChecked());
}

