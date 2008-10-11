#include "sdk.h"

#ifndef CB_PRECOMP
    #include <wx/xrc/xmlres.h>
    #include <wx/button.h>
    #include <wx/checkbox.h>
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
}

IncrementalSearchConfDlg::~IncrementalSearchConfDlg()
{
}

void IncrementalSearchConfDlg::OnChooseColour(wxCommandEvent& event)
{
    wxColourDialog tempColourDlg(this);
    tempColourDlg.Centre();
    wxString strEntry = _T("");
    wxButton* pButton = 0L;
    
    if(event.GetId() == XRCID("btnIncSearchConfColourFound"))
    {
        strEntry = _T("text_found");
        pButton=XRCCTRL(*this, "btnIncSearchConfColourFound", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfColourHighlight"))
    {
        strEntry = _T("highlight");
        pButton=XRCCTRL(*this, "btnIncSearchConfColourHighlight", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfNotFoundBG"))
    {
        strEntry = _T("text_not_found");
        pButton=XRCCTRL(*this, "btnIncSearchConfNotFoundBG", wxButton);
    }
    else if(event.GetId() == XRCID("btnIncSearchConfWrappedBG"))
    {
        strEntry = _T("wrapped");
        pButton=XRCCTRL(*this, "btnIncSearchConfWrappedBG", wxButton);
    }


    if(pButton && !strEntry.empty())
    {
        const int result = tempColourDlg.ShowModal();

        if (result == wxID_OK)
        {
            wxColour tempColour = tempColourDlg.GetColourData().GetColour();
            ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
            cfg->Write(_T("/incremental_search/")+strEntry+_T("_colour_red_value"),    static_cast<int>(tempColour.Red())    );
            cfg->Write(_T("/incremental_search/")+strEntry+_T("_colour_green_value"),  static_cast<int>(tempColour.Green())  );
            cfg->Write(_T("/incremental_search/")+strEntry+_T("_colour_blue_value"),   static_cast<int>(tempColour.Blue())   );
            pButton->SetBackgroundColour(tempColour);
        }
    }
}

void IncrementalSearchConfDlg::SaveSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));
    cfg->Write(_T("/incremental_search/center_found_text_on_screen"), (bool) XRCCTRL(*this, "chkIncSearchConfCenterText", wxCheckBox)->IsChecked());
}

