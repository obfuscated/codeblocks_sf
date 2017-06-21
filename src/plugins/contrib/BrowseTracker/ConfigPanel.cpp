#include "ConfigPanel.h"

//(*InternalHeaders(ConfigPanel)
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ConfigPanel)
const long ConfigPanel::ID_STATICTEXT1 = wxNewId();
const long ConfigPanel::ID_CHECKBOX1 = wxNewId();
const long ConfigPanel::ID_CHECKBOX2 = wxNewId();
const long ConfigPanel::ID_RADIOBOX1 = wxNewId();
const long ConfigPanel::ID_RADIOBOX3 = wxNewId();
const long ConfigPanel::ID_SLIDER1 = wxNewId();
const long ConfigPanel::ID_RADIOBOX2 = wxNewId();
const long ConfigPanel::ID_STATICTEXT2 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ConfigPanel,wxPanel)
	//(*EventTable(ConfigPanel)
	//*)
END_EVENT_TABLE()

ConfigPanel::ConfigPanel(wxWindow* parent, wxWindowID &id)
{
	//(*Initialize(ConfigPanel)
	wxBoxSizer* BoxSizer2;
	wxFlexGridSizer* FlexGridSizer1;
	wxStaticBoxSizer* StaticBoxSizer1;

	Create(parent, id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(7, 1, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("BrowseTracker options"), wxDefaultPosition, wxSize(191,30), wxALIGN_CENTRE, _T("ID_STATICTEXT1"));
	wxFont StaticText1Font(wxDEFAULT,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	StaticText1->SetFont(StaticText1Font);
	FlexGridSizer1->Add(StaticText1, 1, wxALL|wxEXPAND, 5);
	BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
	Cfg_BrowseMarksEnabled = new wxCheckBox(this, ID_CHECKBOX1, _("Enable BrowseMarks"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
	Cfg_BrowseMarksEnabled->SetValue(false);
	BoxSizer2->Add(Cfg_BrowseMarksEnabled, 1, wxALL|wxEXPAND, 5);
	Cfg_WrapJumpEntries = new wxCheckBox(this, ID_CHECKBOX2, _("Wrap jump entries"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
	Cfg_WrapJumpEntries->SetValue(false);
	BoxSizer2->Add(Cfg_WrapJumpEntries, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(BoxSizer2, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_1[3] =
	{
		_("Browse marks"),
		_("Book marks"),
		_("Hide")
	};
	Cfg_MarkStyle = new wxRadioBox(this, ID_RADIOBOX1, _("Mark style"), wxDefaultPosition, wxDefaultSize, 3, __wxRadioBoxChoices_1, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX1"));
	FlexGridSizer1->Add(Cfg_MarkStyle, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_2[2] =
	{
		_("Left mouse "),
		_("Ctrl-Left mouse")
	};
	Cfg_ToggleKey = new wxRadioBox(this, ID_RADIOBOX3, _("Toggle BrowseMark key"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_2, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX3"));
	FlexGridSizer1->Add(Cfg_ToggleKey, 1, wxALL|wxEXPAND, 5);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Left mouse toggle delay"));
	Cfg_LeftMouseDelay = new wxSlider(this, ID_SLIDER1, 200, 0, 1000, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS, wxDefaultValidator, _T("ID_SLIDER1"));
	StaticBoxSizer1->Add(Cfg_LeftMouseDelay, 1, wxALL|wxEXPAND, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxEXPAND, 5);
	wxString __wxRadioBoxChoices_3[2] =
	{
		_("Ctrl-Left mouse"),
		_("Ctrl-Left double click")
	};
	Cfg_ClearAllKey = new wxRadioBox(this, ID_RADIOBOX2, _("Clear all browse marks"), wxDefaultPosition, wxDefaultSize, 2, __wxRadioBoxChoices_3, 1, wxRA_VERTICAL, wxDefaultValidator, _T("ID_RADIOBOX2"));
	FlexGridSizer1->Add(Cfg_ClearAllKey, 1, wxALL|wxEXPAND, 5);
	StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Note: The Ctrl-Left_Mouse key options are disabled \nwhen the editors multi-selection option is enabled at:\nSettings/Editor/Margins/Allow Multiple Selections\n\nMenu items can be used to clear BrowseMarks.\n\n"), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE, _T("ID_STATICTEXT2"));
	FlexGridSizer1->Add(StaticText2, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)
}

ConfigPanel::~ConfigPanel()
{
	//(*Destroy(ConfigPanel)
	//*)
}

