#include "dragscrollcfg.h"

BEGIN_EVENT_TABLE(cbDragScrollCfg,cbConfigurationPanel)
//	//(*EventTable(cbDragScrollCfg)
//	EVT_BUTTON(ID_DONEBUTTON,cbDragScrollCfg::OnDoneButtonClick)
//	//*)
END_EVENT_TABLE()

cbDragScrollCfg::cbDragScrollCfg(wxWindow* parent, cbDragScroll* pOwner, wxWindowID id)
    :pOwnerClass(pOwner)
{
    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	//(*Initialize(cbDragScrollCfg)
	//Create(parent,id,_T(""),wxDefaultPosition,wxDefaultSize,wxDEFAULT_DIALOG_STYLE);
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	//FlexGridSizer1->AddGrowableCol(0);
	//FlexGridSizer1->AddGrowableRow(0);

	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("Mouse Drag Scrolling Configuration"),wxDefaultPosition,wxDefaultSize,wxALIGN_CENTRE);
	//StaticText1->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	//StaticText1->SetFont(wxFont(10,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

	ScrollEnabled = new wxCheckBox(this,ID_ENABLEDCHECKBOX,_("Scrolling Enabled"),wxPoint(-1,-1),wxDefaultSize,0);
	ScrollEnabled->SetValue(true);
	//ScrollEnabled->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

    //Focus editor when mouse in editor window
	EditorFocusEnabled = new wxCheckBox(this,ID_EDITORENABLEDFOCUS,_("Auto Editor Focus Enabled"),wxPoint(-1,-1),wxDefaultSize,0);
	EditorFocusEnabled->SetValue(false);
	//EditorFocusEnabled->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

    //Focus follows Mouse
	MouseFocusEnabled = new wxCheckBox(this,ID_MOUSEENABLEDFOCUS,_("Focus follows Mouse"),wxPoint(-1,-1),wxDefaultSize,0);
	MouseFocusEnabled->SetValue(false);

    //Hide Right keydown events from ListCtrl windows
	MouseRightKeyCtrl = new wxCheckBox(this,ID_EDITORENABLEDFOCUS,_("Smooth Message List Scrolling"),wxPoint(-1,-1),wxDefaultSize,0);
	MouseRightKeyCtrl->SetValue(false);
	//MouseRightKeyCtrl->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));
	StaticTextMRKC = new wxStaticText(this,ID_STATICTEXTMRKC,_("(Conflicts with some Context Menus)"),wxDefaultPosition,wxDefaultSize,0);
	//StaticTextMRKC->SetFont(wxFont(7,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

	wxString wxRadioBoxChoices_ScrollDirection[2];
	wxRadioBoxChoices_ScrollDirection[0] = _("With Mouse");
	wxRadioBoxChoices_ScrollDirection[1] = _("Opposite Mouse");
	//ScrollDirection = new wxRadioBox(this,ID_RADIOBOX1,_("Scroll Direction"),wxDefaultPosition,wxSize(229,116),2,wxRadioBoxChoices_ScrollDirection,1,wxRA_VERTICAL);
	ScrollDirection = new wxRadioBox(this,ID_RADIOBOX1,_("Scroll Direction"),wxDefaultPosition,wxSize(290,70),2,wxRadioBoxChoices_ScrollDirection,1,wxRA_VERTICAL);
	//ScrollDirection->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("Mouse Key To Use"),wxDefaultPosition,wxDefaultSize,0);
	//StaticText2->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

	MouseKeyChoice = new wxChoice(this,ID_KEYCHOICE,wxDefaultPosition,wxDefaultSize,0,NULL,0);
	MouseKeyChoice->Append(_(" Right"));
	MouseKeyChoice->Append(_(" Middle"));
	MouseKeyChoice->SetSelection(0);
	//MouseKeyChoice->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));

	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("-- Adaptive Mouse Speed Sensitivity --"),wxDefaultPosition,wxDefaultSize,0);
	//StaticText3->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));
	Sensitivity = new wxSlider(this,ID_SENSITIVITY,5,1,10,wxDefaultPosition,wxDefaultSize,wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS);
	if ( 1 ) Sensitivity->SetTickFreq(1,0);
	if ( 1 ) Sensitivity->SetPageSize(1);
	if ( 0 ) Sensitivity->SetLineSize(0);
	if ( 0 ) Sensitivity->SetThumbLength(0);
	if ( 1 ) Sensitivity->SetTick(1);
	if ( 1 || 10 ) Sensitivity->SetSelection(1,10);
    //
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("-- Mouse Movement to Text Scroll Ratio --"),wxDefaultPosition,wxDefaultSize,0);
	//StaticText4->SetFont(wxFont(8,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_NORMAL,false,_("Arial")));
	MouseToLineRatio = new wxSlider(this,ID_MOUSETOLINERATIO,30,10,100,wxDefaultPosition,wxDefaultSize,wxSL_HORIZONTAL|wxSL_AUTOTICKS|wxSL_LABELS);
	if ( 1 ) MouseToLineRatio->SetTickFreq(10,0);
	if ( 1 ) MouseToLineRatio->SetPageSize(10);
	if ( 0 ) MouseToLineRatio->SetLineSize(0);
	if ( 0 ) MouseToLineRatio->SetThumbLength(0);
	if ( 1 ) MouseToLineRatio->SetTick(1);
	if ( 10 || 100 ) MouseToLineRatio->SetSelection(10,100);
    //
	FlexGridSizer1->Add(StaticText1,1,wxALL|wxALIGN_CENTER,5);
	//FlexGridSizer1->Add(-1,-1,1);
	FlexGridSizer1->Add(ScrollEnabled,1,wxALL|wxALIGN_CENTER,5);

	FlexGridSizer1->Add(EditorFocusEnabled,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(MouseFocusEnabled,1,wxALL|wxALIGN_CENTER,5);

	FlexGridSizer1->Add(MouseRightKeyCtrl,1,(wxALL&~wxBOTTOM)|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticTextMRKC,1,(wxALL&~wxTOP)|wxALIGN_CENTER,0);


	FlexGridSizer1->Add(ScrollDirection,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticText2,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(MouseKeyChoice,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(StaticText3,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(Sensitivity,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);
	FlexGridSizer1->Add(StaticText4,1,wxALL|wxALIGN_CENTER,5);
	FlexGridSizer1->Add(MouseToLineRatio,1,wxALL|wxALIGN_CENTER|wxEXPAND,5);

	this->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	Centre();
	//*)
}
// ----------------------------------------------------------------------------
void cbDragScrollCfg::OnApply()
// ----------------------------------------------------------------------------
{
    pOwnerClass->OnDialogDone(this);
}
// ----------------------------------------------------------------------------
cbDragScrollCfg::~cbDragScrollCfg()
// ----------------------------------------------------------------------------
{
}
// ----------------------------------------------------------------------------
void cbDragScrollCfg::OnDoneButtonClick(wxCommandEvent& event)
// ----------------------------------------------------------------------------
{
    //EndModal(0);
    LOGIT( _T("cbDragScrollCfg::OnDoneButtonClick erroniously called") );
}
// ----------------------------------------------------------------------------
wxString cbDragScrollCfg::GetBitmapBaseName() const
{
    //probing
    //LOGIT( _T("Config:%s"),ConfigManager::GetConfigFolder().GetData()  );
    //LOGIT( _T("Plugins:%s"),ConfigManager::GetPluginsFolder().GetData() );
    //LOGIT( _T("Data:%s"),ConfigManager::GetDataFolder().GetData() );
    //LOGIT( _T("Executable:%s"),ConfigManager::GetExecutableFolder().GetData() );

    wxString pngName = _T("generic-plugin");
    //if file exist "./share/codeblocks/images/settings/cbdragscroll.png";
    #ifdef __WXGTK__
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + _T("/images/settings/dragscroll.png")) )
    #else
     if ( ::wxFileExists(ConfigManager::GetDataFolder() + _T("\\images\\settings\\dragscroll.png")) )
    #endif
    	pngName = _T("dragscroll") ;
    // else return "generic-plugin"
    return pngName;
}
// ----------------------------------------------------------------------------

