#include "sdk.h"
#include "debuggersettingscommonpanel.h"

#ifndef CB_PRECOMP
    //(*InternalHeadersPCH(DebuggerSettingsCommonPanel)
    #include <wx/string.h>
    #include <wx/intl.h>
    //*)

    #include <wx/fontutil.h>
#endif

#include <wx/fontdlg.h>

#include "debuggermanager.h"

//(*InternalHeaders(DebuggerSettingsCommonPanel)
//*)

//(*IdInit(DebuggerSettingsCommonPanel)
const long DebuggerSettingsCommonPanel::ID_AUTOBUILD = wxNewId();
const long DebuggerSettingsCommonPanel::ID_AUTOSWITCH = wxNewId();
const long DebuggerSettingsCommonPanel::ID_DEBUGGERS_LOG = wxNewId();
const long DebuggerSettingsCommonPanel::ID_JUMP_ON_DOUBLE_CLICK = wxNewId();
const long DebuggerSettingsCommonPanel::ID_REQUIRE_CTRL_FOR_TOOLTIPS = wxNewId();
const long DebuggerSettingsCommonPanel::ID_VALUE_TOOLTIP_LABEL = wxNewId();
const long DebuggerSettingsCommonPanel::ID_BUTTON_CHOOSE_FONT = wxNewId();
const long DebuggerSettingsCommonPanel::ID_CHOICE_PERSPECTIVE = wxNewId();
//*)

BEGIN_EVENT_TABLE(DebuggerSettingsCommonPanel,wxPanel)
	//(*EventTable(DebuggerSettingsCommonPanel)
	//*)
END_EVENT_TABLE()

DebuggerSettingsCommonPanel::DebuggerSettingsCommonPanel(wxWindow* parent)
{
	//(*Initialize(DebuggerSettingsCommonPanel)
	wxFlexGridSizer* flexSizer;
	wxBoxSizer* mainSizer;
	wxButton* chooseFont;
	wxStaticBoxSizer* valueTooltipSizer;
	wxBoxSizer* BoxSizer1;
	wxStaticText* labelPerspective;

	Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
	mainSizer = new wxBoxSizer(wxVERTICAL);
	flexSizer = new wxFlexGridSizer(0, 1, 0, 0);
	m_autoBuild = new wxCheckBox(this, ID_AUTOBUILD, _("Auto-build project if it is not up-to-date"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_AUTOBUILD"));
	m_autoBuild->SetValue(false);
	flexSizer->Add(m_autoBuild, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
	m_autoSwitch = new wxCheckBox(this, ID_AUTOSWITCH, _("When stopping, auto-switch to the first frame with valid source info"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_AUTOSWITCH"));
	m_autoSwitch->SetValue(false);
	flexSizer->Add(m_autoSwitch, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
	m_debuggersLog = new wxCheckBox(this, ID_DEBUGGERS_LOG, _("Full (Debug) log"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_DEBUGGERS_LOG"));
	m_debuggersLog->SetValue(false);
	flexSizer->Add(m_debuggersLog, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
	m_jumpOnDoubleClick = new wxCheckBox(this, ID_JUMP_ON_DOUBLE_CLICK, _("Jump on Double-click in Stack trace window"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_JUMP_ON_DOUBLE_CLICK"));
	m_jumpOnDoubleClick->SetValue(false);
	flexSizer->Add(m_jumpOnDoubleClick, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
	m_requireCtrlForTooltips = new wxCheckBox(this, ID_REQUIRE_CTRL_FOR_TOOLTIPS, _("Require Control key to show the \'Evaluate expression\' tooltips"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_REQUIRE_CTRL_FOR_TOOLTIPS"));
	m_requireCtrlForTooltips->SetValue(false);
	flexSizer->Add(m_requireCtrlForTooltips, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_TOP, 5);
	valueTooltipSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, _("Value Tooltip Font"));
	m_valueTooltipLabel = new wxStaticText(this, ID_VALUE_TOOLTIP_LABEL, _("This is a sample text"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_VALUE_TOOLTIP_LABEL"));
	valueTooltipSizer->Add(m_valueTooltipLabel, 1, wxALIGN_CENTER_VERTICAL, 5);
	chooseFont = new wxButton(this, ID_BUTTON_CHOOSE_FONT, _("Choose"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON_CHOOSE_FONT"));
	valueTooltipSizer->Add(chooseFont, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
	flexSizer->Add(valueTooltipSizer, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
	labelPerspective = new wxStaticText(this, wxID_ANY, _("Perspective:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
	BoxSizer1->Add(labelPerspective, 0, wxALIGN_CENTER_VERTICAL, 5);
	m_perspective = new wxChoice(this, ID_CHOICE_PERSPECTIVE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHOICE_PERSPECTIVE"));
	m_perspective->Append(_("Only one perspective"));
	m_perspective->Append(_("One perspective per Debugger"));
	m_perspective->SetSelection( m_perspective->Append(_("One perspective per Debugger configuration")) );
	BoxSizer1->Add(m_perspective, 1, wxLEFT|wxEXPAND, 5);
	flexSizer->Add(BoxSizer1, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
	mainSizer->Add(flexSizer, 1, wxALL|wxALIGN_LEFT, 0);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	Connect(ID_BUTTON_CHOOSE_FONT,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&DebuggerSettingsCommonPanel::OnChooseFontClick);
	//*)

    m_autoBuild->SetValue(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::AutoBuild));
    m_autoBuild->SetToolTip(_("Automatic project build before debug session is started."));

    m_autoSwitch->SetValue(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::AutoSwitchFrame));
    m_autoSwitch->SetToolTip(_("When stopping, auto-switch to first frame with valid source info."));

    m_debuggersLog->SetValue(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::ShowDebuggersLog));
    m_debuggersLog->SetToolTip(_("If enabled, the debugger's raw input/output will be logged in a separate log page."));

    m_jumpOnDoubleClick->SetValue(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::JumpOnDoubleClick));

    m_requireCtrlForTooltips->SetValue(cbDebuggerCommonConfig::GetFlag(cbDebuggerCommonConfig::RequireCtrlForTooltips));
    m_requireCtrlForTooltips->SetToolTip(_("If enabled, show 'Evaluate expression' tooltips only if the Control key is pressed."));

    m_valueTooltipFontInfo = cbDebuggerCommonConfig::GetValueTooltipFont();
    UpdateValueTooltipFont();

    m_perspective->Select(cbDebuggerCommonConfig::GetPerspective());
}

DebuggerSettingsCommonPanel::~DebuggerSettingsCommonPanel()
{
	//(*Destroy(DebuggerSettingsCommonPanel)
	//*)
}

void DebuggerSettingsCommonPanel::SaveChanges()
{
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::AutoBuild, m_autoBuild->GetValue());
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::AutoSwitchFrame, m_autoSwitch->GetValue());
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::ShowDebuggersLog, m_debuggersLog->GetValue());
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::JumpOnDoubleClick, m_jumpOnDoubleClick->GetValue());
    cbDebuggerCommonConfig::SetFlag(cbDebuggerCommonConfig::RequireCtrlForTooltips,
                                    m_requireCtrlForTooltips->GetValue());
    cbDebuggerCommonConfig::SetValueTooltipFont(m_valueTooltipFontInfo);
    cbDebuggerCommonConfig::SetPerspective(m_perspective->GetSelection());
}

void DebuggerSettingsCommonPanel::OnChooseFontClick(cb_unused wxCommandEvent& event)
{
    wxNativeFontInfo fontInfo;
    fontInfo.FromString(m_valueTooltipFontInfo);

    wxFontData data;
    data.SetInitialFont(wxFont(fontInfo));
    wxFontDialog dlg(this, data);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        m_valueTooltipFontInfo = dlg.GetFontData().GetChosenFont().GetNativeFontInfo()->ToString();
        UpdateValueTooltipFont();
    }
}

void DebuggerSettingsCommonPanel::UpdateValueTooltipFont()
{
    wxNativeFontInfo fontInfo;
    fontInfo.FromString(m_valueTooltipFontInfo);
    m_valueTooltipLabel->SetFont(wxFont(fontInfo));
}

