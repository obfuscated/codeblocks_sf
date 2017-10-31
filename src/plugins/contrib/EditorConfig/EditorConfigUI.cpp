#include "EditorConfigUI.h"

//(*InternalHeaders(EditorConfigUI)
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/string.h>
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
//*)

#include <cbproject.h>
#include <logmanager.h>
#include <manager.h>

#include "EditorConfigCommon.h"

//(*IdInit(EditorConfigUI)
const long EditorConfigUI::ID_CHK_ACTIVE = wxNewId();
const long EditorConfigUI::ID_CHK_USE_TABS = wxNewId();
const long EditorConfigUI::ID_CHK_TAB_INDENTS = wxNewId();
const long EditorConfigUI::ID_SPN_TAB_WIDTH = wxNewId();
const long EditorConfigUI::ID_SPN_INDENT = wxNewId();
const long EditorConfigUI::ID_CHO_EOL_MODE = wxNewId();
//*)

BEGIN_EVENT_TABLE(EditorConfigUI, wxPanel)
    //(*EventTable(EditorConfigUI)
    //*)
END_EVENT_TABLE()

EditorConfigUI::EditorConfigUI(wxWindow* parent, wxEvtHandler* eh, cbProject* prj, const TEditorSettings& es) :
    m_NotifiedWindow(eh),
    m_Project(prj)
{
    //(*Initialize(EditorConfigUI)
    wxFlexGridSizer* flsMain;
    wxStaticText* lblEOLMode;
    wxStaticText* lblTab;
    wxStaticText* lblIndent;
    wxStaticText* lblTabWidth;
    wxBoxSizer* bszTab;

    Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("wxID_ANY"));
    flsMain = new wxFlexGridSizer(5, 2, 0, 0);
    flsMain->AddGrowableCol(1);
    chkActive = new wxCheckBox(this, ID_CHK_ACTIVE, _("Activate for project"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_ACTIVE"));
    chkActive->SetValue(false);
    flsMain->Add(chkActive, 1, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    flsMain->Add(-1,-1,1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    lblTab = new wxStaticText(this, wxID_ANY, _("Specify tab behaviour:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblTab, 1, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    bszTab = new wxBoxSizer(wxHORIZONTAL);
    chkUseTabs = new wxCheckBox(this, ID_CHK_USE_TABS, _("Use tabs"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_USE_TABS"));
    chkUseTabs->SetValue(false);
    chkUseTabs->Disable();
    bszTab->Add(chkUseTabs, 1, wxEXPAND, 5);
    chkTabIndents = new wxCheckBox(this, ID_CHK_TAB_INDENTS, _("Tab indents"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHK_TAB_INDENTS"));
    chkTabIndents->SetValue(true);
    chkTabIndents->Disable();
    bszTab->Add(chkTabIndents, 1, wxLEFT|wxEXPAND, 5);
    flsMain->Add(bszTab, 1, wxTOP|wxLEFT|wxRIGHT|wxALIGN_BOTTOM, 5);
    lblTabWidth = new wxStaticText(this, wxID_ANY, _("Specify tab width:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblTabWidth, 1, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    spnTabWidth = new wxSpinCtrl(this, ID_SPN_TAB_WIDTH, _T("4"), wxDefaultPosition, wxDefaultSize, 0, 0, 10, 4, _T("ID_SPN_TAB_WIDTH"));
    spnTabWidth->SetValue(_T("4"));
    spnTabWidth->Disable();
    flsMain->Add(spnTabWidth, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    lblIndent = new wxStaticText(this, wxID_ANY, _("Specify indent width:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblIndent, 1, wxTOP|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    spnIndent = new wxSpinCtrl(this, ID_SPN_INDENT, _T("4"), wxDefaultPosition, wxDefaultSize, 0, 0, 10, 4, _T("ID_SPN_INDENT"));
    spnIndent->SetValue(_T("4"));
    spnIndent->Disable();
    flsMain->Add(spnIndent, 1, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 5);
    lblEOLMode = new wxStaticText(this, wxID_ANY, _("Specify EOL mode:"), wxDefaultPosition, wxDefaultSize, 0, _T("wxID_ANY"));
    flsMain->Add(lblEOLMode, 1, wxTOP|wxBOTTOM|wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    choEOLMode = new wxChoice(this, ID_CHO_EOL_MODE, wxDefaultPosition, wxDefaultSize, 0, 0, 0, wxDefaultValidator, _T("ID_CHO_EOL_MODE"));
    choEOLMode->SetSelection( choEOLMode->Append(_("CR/LF")) );
    choEOLMode->Append(_("CR"));
    choEOLMode->Append(_("LF"));
    choEOLMode->Disable();
    flsMain->Add(choEOLMode, 1, wxALL|wxEXPAND, 5);
    SetSizer(flsMain);
    flsMain->Fit(this);
    flsMain->SetSizeHints(this);

    Connect(ID_CHK_ACTIVE,wxEVT_COMMAND_CHECKBOX_CLICKED,(wxObjectEventFunction)&EditorConfigUI::OnActiveClick);
    //*)

    if (es.active)
    {
        DoActive(true);

        chkActive->SetValue(true);
        chkUseTabs->SetValue(es.use_tabs);
        chkTabIndents->SetValue(es.tab_indents);
        spnTabWidth->SetValue(es.tab_width);
        spnIndent->SetValue(es.indent);
        choEOLMode->SetSelection(es.eol_mode);
   }
}

EditorConfigUI::~EditorConfigUI()
{
    //(*Destroy(EditorConfigUI)
    //*)
}

void EditorConfigUI::OnActiveClick(wxCommandEvent& event)
{
    DoActive(event.IsChecked());
}

void EditorConfigUI::OnApply()
{
    TEditorSettings es;
    es.active      = chkActive->IsChecked();
    es.use_tabs    = chkUseTabs->IsChecked();
    es.tab_indents = chkTabIndents->IsChecked();
    es.tab_width   = spnTabWidth->GetValue();
    es.indent      = spnIndent->GetValue();
    es.eol_mode    = choEOLMode->GetCurrentSelection(); // must be in sync with wxSCI_EOL_CRLF etc...

    EditorSettingsChangedEvent e(wxEVT_EDITOR_SETTINGS_CHANGED_EVENT, 0, es, m_Project);
    if (m_NotifiedWindow)
    {
#if defined(TRACE_EC)
        if (m_Project)
            Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfigUI::OnApply(PROJECT)"));
        Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfigUI::OnApply()"));
#endif
        m_NotifiedWindow->AddPendingEvent(e);
    }
}

void EditorConfigUI::DoActive(bool en)
{
    chkUseTabs->Enable(en);
    chkTabIndents->Enable(en);
    spnTabWidth->Enable(en);
    spnIndent->Enable(en);
    choEOLMode->Enable(en);
}
