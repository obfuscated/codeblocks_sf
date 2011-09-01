#include "configmanager.h"
#include "dragscrollcfg.h"

BEGIN_EVENT_TABLE(cbDragScrollCfg,cbConfigurationPanel)
//	//(*EventTable(cbDragScrollCfg)
//	EVT_BUTTON(ID_DONEBUTTON,cbDragScrollCfg::OnDoneButtonClick)
//	//*)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
cbDragScrollCfg::cbDragScrollCfg(wxWindow* parent, cbDragScroll* pOwner, wxWindowID /*id*/)
// ----------------------------------------------------------------------------
    :pOwnerClass(pOwner)
{
    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	bSizer2->SetMinSize(wxSize( -1,50 ));
	StaticText1 = new wxStaticText( this, wxID_ANY, wxT("Mouse Drag Scrolling Configuration"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( StaticText1, 0, wxALIGN_CENTER|wxALL, 5 );

	bSizer2->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer1->Add( bSizer2, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	ScrollEnabled = new wxCheckBox( this, wxID_ANY, wxT("Scrolling Enabled"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer5->Add( ScrollEnabled, 0, wxALL, 5 );

	bSizer1->Add( bSizer5, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	EditorFocusEnabled = new wxCheckBox( this, wxID_ANY, wxT("Auto Focus Editors"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer6->Add( EditorFocusEnabled, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	MouseFocusEnabled = new wxCheckBox( this, wxID_ANY, wxT("Focus follows Mouse"), wxDefaultPosition, wxDefaultSize, 0 );

	bSizer6->Add( MouseFocusEnabled, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer1->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* MouseWheelSizer1;
	MouseWheelSizer1 = new wxBoxSizer( wxHORIZONTAL );
	MouseWheelZoom = new wxCheckBox( this, wxID_ANY, wxT("Log MouseWheelZoom"), wxDefaultPosition, wxDefaultSize, 0 );
	MouseWheelSizer1->Add( MouseWheelZoom, 0, wxALL, 5 );
	PropagateLogZoomSize = new wxCheckBox( this, wxID_ANY, wxT("Propagate Log Zooms"), wxDefaultPosition, wxDefaultSize, 0 );
	MouseWheelSizer1->Add( PropagateLogZoomSize, 0, wxALL, 5 );
	bSizer1->Add( MouseWheelSizer1, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	wxString ScrollDirectionChoices[] = { wxT("With Mouse"), wxT("Opposite Mouse") };
	int ScrollDirectionNChoices = sizeof( ScrollDirectionChoices ) / sizeof( wxString );
	ScrollDirection = new wxRadioBox( this, wxID_ANY, wxT("Scroll Direction"), wxDefaultPosition, wxDefaultSize, ScrollDirectionNChoices, ScrollDirectionChoices, 2, wxRA_SPECIFY_COLS );
	bSizer3->Add( ScrollDirection, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	bSizer1->Add( bSizer3, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	StaticText2 = new wxStaticText( this, wxID_ANY, wxT("Mouse Key To Use:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( StaticText2, 0, wxALL, 5 );

	wxString MouseKeyChoiceChoices[] = { wxT("Right"), wxT("Middle") };
	int MouseKeyChoiceNChoices = sizeof( MouseKeyChoiceChoices ) / sizeof( wxString );
	MouseKeyChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, MouseKeyChoiceNChoices, MouseKeyChoiceChoices, 0 );
	bSizer4->Add( MouseKeyChoice, 0, wxALL, 5 );

	bSizer1->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	bSizer7->SetMinSize(wxSize( 300,-1 ));
	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	StaticText3 = new wxStaticText( this, wxID_ANY, wxT("-- Adaptive Mouse Speed Sensitivity --"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( StaticText3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	Sensitivity = new wxSlider( this, wxID_ANY, 8, 1, 10, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS|wxSL_TOP );
	if ( 1 ) Sensitivity->SetTickFreq(1,0);
	if ( 1 ) Sensitivity->SetPageSize(1);
	if ( 0 ) Sensitivity->SetLineSize(0);
	if ( 0 ) Sensitivity->SetThumbLength(0);
	if ( 1 ) Sensitivity->SetTick(1);
	if ( 1 ) Sensitivity->SetSelection(1,10);
	bSizer7->Add( Sensitivity, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	StaticText4 = new wxStaticText( this, wxID_ANY, wxT("-- Mouse Movement to Text Scroll Ratio --"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( StaticText4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	MouseToLineRatio = new wxSlider( this, wxID_ANY, 30, 10, 100, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS|wxSL_TOP );
	if ( 1 ) MouseToLineRatio->SetTickFreq(10,10);
	if ( 1 ) MouseToLineRatio->SetPageSize(10);
	if ( 0 ) MouseToLineRatio->SetLineSize(0);
	if ( 0 ) MouseToLineRatio->SetThumbLength(0);
	if ( 1 ) MouseToLineRatio->SetTick(10);
	if ( 1 ) MouseToLineRatio->SetSelection(10,100);
	bSizer7->Add( MouseToLineRatio, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	StaticText5 = new wxStaticText( this, wxID_ANY, wxT("-- Unix Context Menu Watch for Drag (millisecs) --"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( StaticText5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	MouseContextDelay = new wxSlider( this, wxID_ANY, 50, 10, 500, wxDefaultPosition, wxDefaultSize, wxSL_AUTOTICKS|wxSL_HORIZONTAL|wxSL_LABELS|wxSL_TOP );
	// Above values are: default mil delay, min, max
	if ( 1 ) MouseContextDelay->SetTickFreq(10,10);
	if ( 1 ) MouseContextDelay->SetPageSize(10);
	if ( 1 ) MouseContextDelay->SetLineSize(10);
	if ( 1 ) MouseContextDelay->SetThumbLength(10);
	if ( 1 ) MouseContextDelay->SetTick(100);
	if ( 1 ) MouseContextDelay->SetSelection(10,500);
	bSizer7->Add( MouseContextDelay, 0, wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );

	bSizer7->Add( 0, 0, 1, wxEXPAND, 0 );

	bSizer1->Add( bSizer7, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();

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
void cbDragScrollCfg::OnDoneButtonClick(wxCommandEvent& /*event*/)
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
//
