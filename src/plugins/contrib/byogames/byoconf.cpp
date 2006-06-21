#include <sdk.h>
#include "byoconf.h"
#include "byogamebase.h"

BEGIN_EVENT_TABLE(byoConf,wxPanel)
	//(*EventTable(byoConf)
	EVT_CHECKBOX(ID_CHECKBOX1,byoConf::BTWSRefresh)
	EVT_CHECKBOX(ID_CHECKBOX2,byoConf::BTWSRefresh)
	EVT_CHECKBOX(ID_CHECKBOX3,byoConf::BTWSRefresh)
	EVT_BUTTON(ID_BUTTON1,byoConf::ColChangeClick)
	EVT_BUTTON(ID_BUTTON2,byoConf::ColChangeClick)
	EVT_BUTTON(ID_BUTTON3,byoConf::ColChangeClick)
	EVT_BUTTON(ID_BUTTON4,byoConf::ColChangeClick)
	EVT_BUTTON(ID_BUTTON5,byoConf::ColChangeClick)
	EVT_BUTTON(ID_BUTTON6,byoConf::ColChangeClick)
	//*)
END_EVENT_TABLE()

byoConf::byoConf(wxWindow* parent,wxWindowID id)
{
	//(*Initialize(byoConf)
	Create(parent,id,wxDefaultPosition,wxDefaultSize,wxTAB_TRAVERSAL);
	FlexGridSizer1 = new wxFlexGridSizer(0,1,0,0);
	FlexGridSizer1->AddGrowableCol(0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Back-To-Work settings"));
	FlexGridSizer2 = new wxFlexGridSizer(0,2,0,0);
	FlexGridSizer2->AddGrowableCol(1);
	m_MaxPlaytimeChk = new wxCheckBox(this,ID_CHECKBOX1,_("Maximum play time (sec)"),wxDefaultPosition,wxDefaultSize,0);
	m_MaxPlaytimeChk->SetValue(true);
	m_MaxPlaytimeSpn = new wxSpinCtrl(this,ID_SPINCTRL1,_("1"),wxDefaultPosition,wxDefaultSize,0,1,1000000);
	m_MinWorkChk = new wxCheckBox(this,ID_CHECKBOX2,_("Minimum work time (sec)"),wxDefaultPosition,wxDefaultSize,0);
	m_MinWorkChk->SetValue(true);
	m_MinWorkSpn = new wxSpinCtrl(this,ID_SPINCTRL2,_("1"),wxDefaultPosition,wxDefaultSize,0,1,1000000);
	m_OverworkChk = new wxCheckBox(this,ID_CHECKBOX3,_("Overwork time (Sec)"),wxDefaultPosition,wxDefaultSize,0);
	m_OverworkChk->SetValue(true);
	m_OverworkSpn = new wxSpinCtrl(this,ID_SPINCTRL3,_("1"),wxDefaultPosition,wxDefaultSize,0,1,1000000);
	FlexGridSizer2->Add(m_MaxPlaytimeChk,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer2->Add(m_MaxPlaytimeSpn,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer2->Add(m_MinWorkChk,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer2->Add(m_MinWorkSpn,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer2->Add(m_OverworkChk,0,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer2->Add(m_OverworkSpn,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	StaticBoxSizer1->Add(FlexGridSizer2,1,wxALIGN_CENTER|wxEXPAND,4);
	StaticBoxSizer2 = new wxStaticBoxSizer(wxHORIZONTAL,this,_("Default colours"));
	FlexGridSizer3 = new wxFlexGridSizer(0,8,0,0);
	FlexGridSizer3->AddGrowableCol(1);
	FlexGridSizer3->AddGrowableCol(4);
	FlexGridSizer3->AddGrowableCol(7);
	FlexGridSizer3->AddGrowableRow(0);
	FlexGridSizer3->AddGrowableRow(1);
	StaticText1 = new wxStaticText(this,ID_STATICTEXT1,_("1"),wxDefaultPosition,wxDefaultSize,0);
	m_Col1 = new wxButton(this,ID_BUTTON1,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col1->SetDefault();
	StaticText2 = new wxStaticText(this,ID_STATICTEXT2,_("3"),wxDefaultPosition,wxDefaultSize,0);
	m_Col3 = new wxButton(this,ID_BUTTON2,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col3->SetDefault();
	StaticText3 = new wxStaticText(this,ID_STATICTEXT3,_("5"),wxDefaultPosition,wxDefaultSize,0);
	m_Col5 = new wxButton(this,ID_BUTTON3,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col5->SetDefault();
	StaticText4 = new wxStaticText(this,ID_STATICTEXT4,_("2"),wxDefaultPosition,wxDefaultSize,0);
	m_Col2 = new wxButton(this,ID_BUTTON4,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col2->SetDefault();
	StaticText5 = new wxStaticText(this,ID_STATICTEXT5,_("4"),wxDefaultPosition,wxDefaultSize,0);
	m_Col4 = new wxButton(this,ID_BUTTON5,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col4->SetDefault();
	StaticText6 = new wxStaticText(this,ID_STATICTEXT6,_("6"),wxDefaultPosition,wxDefaultSize,0);
	m_Col6 = new wxButton(this,ID_BUTTON6,_T(""),wxDefaultPosition,wxDefaultSize,wxBU_EXACTFIT);
	if (false) m_Col6->SetDefault();
	FlexGridSizer3->Add(StaticText1,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col1,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer3->Add(-1,-1,1);
	FlexGridSizer3->Add(StaticText2,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col3,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer3->Add(-1,-1,1);
	FlexGridSizer3->Add(StaticText3,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col5,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer3->Add(StaticText4,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col2,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer3->Add(-1,-1,1);
	FlexGridSizer3->Add(StaticText5,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col4,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer3->Add(-1,-1,1);
	FlexGridSizer3->Add(StaticText6,1,wxLEFT|wxTOP|wxBOTTOM|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL,4);
	FlexGridSizer3->Add(m_Col6,1,wxALL|wxALIGN_CENTER|wxEXPAND,4);
	StaticBoxSizer2->Add(FlexGridSizer3,1,wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer1->Add(StaticBoxSizer1,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	FlexGridSizer1->Add(StaticBoxSizer2,1,wxLEFT|wxRIGHT|wxBOTTOM|wxALIGN_CENTER|wxEXPAND,4);
	this->SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	//*)

    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("byogames"));
    m_MaxPlaytimeChk->SetValue(cfg->ReadBool(_T("/ismaxplaytime"),true));
    m_MaxPlaytimeSpn->SetValue(cfg->ReadInt(_T("/maxplaytime"),60*10));
    m_MinWorkChk->SetValue(cfg->ReadBool(_T("/isminworktime"),true));
    m_MinWorkSpn->SetValue(cfg->ReadInt(_T("/minworktime"),60*60));
    m_OverworkChk->SetValue(cfg->ReadBool(_T("/isoverworktime"),true));
    m_OverworkSpn->SetValue(cfg->ReadInt(_T("/overworktime"),3*60*60));

    m_Col1->SetBackgroundColour(cfg->ReadColour(_T("/col01"),wxColour(0xFF,0,0)));
    m_Col2->SetBackgroundColour(cfg->ReadColour(_T("/col02"),wxColour(0,0xFF,0)));
    m_Col3->SetBackgroundColour(cfg->ReadColour(_T("/col03"),wxColour(0,0,0xFF)));
    m_Col4->SetBackgroundColour(cfg->ReadColour(_T("/col04"),wxColour(0xFF,0xFF,0)));
    m_Col5->SetBackgroundColour(cfg->ReadColour(_T("/col05"),wxColour(0xFF,0,0xFF)));
    m_Col6->SetBackgroundColour(cfg->ReadColour(_T("/col06"),wxColour(0,0xFF,0xFF)));

    SetSize(500,500);
}

byoConf::~byoConf()
{
}

void byoConf::OnApply()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("byogames"));
    cfg->Write(_T("/ismaxplaytime"),(bool)m_MaxPlaytimeChk->GetValue());
    cfg->Write(_T("/maxplaytime"),(int)m_MaxPlaytimeSpn->GetValue());
    cfg->Write(_T("/isminworktime"),(bool)m_MinWorkChk->GetValue());
    cfg->Write(_T("/minworktime"),(int)m_MinWorkSpn->GetValue());
    cfg->Write(_T("/isoverworktime"),(bool)m_OverworkChk->GetValue());
    cfg->Write(_T("/overworktime"),(int)m_OverworkSpn->GetValue());
    cfg->Write(_T("/col01"),m_Col1->GetBackgroundColour());
    cfg->Write(_T("/col02"),m_Col2->GetBackgroundColour());
    cfg->Write(_T("/col03"),m_Col3->GetBackgroundColour());
    cfg->Write(_T("/col04"),m_Col4->GetBackgroundColour());
    cfg->Write(_T("/col05"),m_Col5->GetBackgroundColour());
    cfg->Write(_T("/col06"),m_Col6->GetBackgroundColour());

    byoGameBase::ReloadFromConfig();
}

void byoConf::BTWSRefresh(wxCommandEvent& event)
{
    if ( m_MaxPlaytimeChk->GetValue() )
    {
        m_MaxPlaytimeSpn->Enable();
        m_MinWorkChk->Enable();
        m_MinWorkSpn->Enable(m_MinWorkChk->GetValue());
    }
    else
    {
        m_MaxPlaytimeSpn->Disable();
        m_MinWorkChk->Disable();
        m_MinWorkSpn->Disable();
    }
    m_OverworkSpn->Enable(m_OverworkChk->GetValue());
}

void byoConf::ColChangeClick(wxCommandEvent& event)
{
    wxWindow* wnd = wxDynamicCast(event.GetEventObject(),wxWindow);
    if ( !wnd ) return;
    wxColour Col = ::wxGetColourFromUser(NULL,wnd->GetBackgroundColour());
    if ( Col.Ok() )
    {
        wnd->SetBackgroundColour(Col);
    }
}
