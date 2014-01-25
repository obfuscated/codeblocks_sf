#include "sdk.h"
#include "debuggersettingsdlg.h"

#ifndef CB_PRECOMP
	//(*InternalHeadersPCH(DebuggerSettingsDlg)
	#include <wx/string.h>
	#include <wx/intl.h>
	//*)

    #include <wx/choicdlg.h>

    #include "cbexception.h"
    #include "cbplugin.h"
#endif
//(*InternalHeaders(DebuggerSettingsDlg)
#include <wx/button.h>
#include <wx/font.h>
//*)

#include "debuggermanager.h"
#include "debuggersettingspanel.h"
#include "debuggersettingscommonpanel.h"

//(*IdInit(DebuggerSettingsDlg)
const long DebuggerSettingsDlg::ID_LABEL_ACTIVE_INFO = wxNewId();
const long DebuggerSettingsDlg::ID_TREEBOOK = wxNewId();
//*)

BEGIN_EVENT_TABLE(DebuggerSettingsDlg, wxScrollingDialog)
	//(*EventTable(DebuggerSettingsDlg)
	//*)
	EVT_BUTTON(wxID_OK, DebuggerSettingsDlg::OnOK)
END_EVENT_TABLE()

DebuggerSettingsDlg::DebuggerSettingsDlg(wxWindow* parent)
{
	//(*Initialize(DebuggerSettingsDlg)
	wxStaticLine* staticLine;
	wxBoxSizer* headerSizer;
	wxBoxSizer* mainSizer;
	wxStdDialogButtonSizer* stdDialogButtons;
	wxPanel* header;

	Create(parent, wxID_ANY, _("Debugger settings"), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxCLOSE_BOX|wxMAXIMIZE_BOX|wxMINIMIZE_BOX, _T("wxID_ANY"));
	mainSizer = new wxBoxSizer(wxVERTICAL);
	header = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTRANSPARENT_WINDOW, _T("wxID_ANY"));
	header->SetBackgroundColour(wxColour(0,64,128));
	headerSizer = new wxBoxSizer(wxHORIZONTAL);
	m_activeInfo = new wxStaticText(header, ID_LABEL_ACTIVE_INFO, _("Active debugger config"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTRANSPARENT_WINDOW, _T("ID_LABEL_ACTIVE_INFO"));
	m_activeInfo->SetForegroundColour(wxColour(255,255,255));
	m_activeInfo->SetBackgroundColour(wxColour(0,64,128));
	wxFont m_activeInfoFont(12,wxDEFAULT,wxFONTSTYLE_NORMAL,wxBOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
	m_activeInfo->SetFont(m_activeInfoFont);
	headerSizer->Add(m_activeInfo, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	header->SetSizer(headerSizer);
	headerSizer->Fit(header);
	headerSizer->SetSizeHints(header);
	mainSizer->Add(header, 0, wxEXPAND|wxALIGN_RIGHT|wxALIGN_BOTTOM, 5);
	m_treebook = new wxTreebook(this, ID_TREEBOOK, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT, _T("ID_TREEBOOK"));
	mainSizer->Add(m_treebook, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(10,-1), wxLI_HORIZONTAL, _T("wxID_ANY"));
	mainSizer->Add(staticLine, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	stdDialogButtons = new wxStdDialogButtonSizer();
	stdDialogButtons->AddButton(new wxButton(this, wxID_OK, wxEmptyString));
	stdDialogButtons->AddButton(new wxButton(this, wxID_CANCEL, wxEmptyString));
	stdDialogButtons->Realize();
	mainSizer->Add(stdDialogButtons, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(mainSizer);
	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);
	Center();

	Connect(ID_TREEBOOK,wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED,(wxObjectEventFunction)&DebuggerSettingsDlg::OnPageChanged);
	//*)

    m_commonPanel = new DebuggerSettingsCommonPanel(m_treebook);
    m_treebook->AddPage(m_commonPanel, _("Common"));

    const DebuggerManager::RegisteredPlugins &plugins = Manager::Get()->GetDebuggerManager()->GetAllDebuggers();
    for (DebuggerManager::RegisteredPlugins::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        const DebuggerManager::PluginData &data = it->second;
        m_treebook->AddPage(new DebuggerSettingsPanel(m_treebook, this, it->first), it->first->GetGUIName());

        for (DebuggerManager::ConfigurationVector::const_iterator itConfig = data.GetConfigurations().begin();
             itConfig != data.GetConfigurations().end();
             ++itConfig)
        {
            wxPanel *panel = (*itConfig)->MakePanel(m_treebook);
            m_treebook->AddSubPage(panel, (*itConfig)->GetName());

            Config conf;
            conf.plugin = it->first;
            conf.pluginGUIName = it->first->GetGUIName();
            conf.config = (*itConfig)->Clone();
            m_mapPanelToConfig[panel] = conf;
        }
    }

    for (size_t ii = 0; ii < m_treebook->GetPageCount(); ++ii)
        m_treebook->ExpandNode(ii);

    mainSizer->SetSizeHints(this);
    CentreOnParent();
}

DebuggerSettingsDlg::~DebuggerSettingsDlg()
{
    for (MapPanelToConfiguration::iterator it = m_mapPanelToConfig.begin(); it != m_mapPanelToConfig.end(); ++it)
        delete it->second.config;
    m_mapPanelToConfig.clear();

	//(*Destroy(DebuggerSettingsDlg)
	//*)
}

void DebuggerSettingsDlg::OnOK(cb_unused wxCommandEvent &event)
{
    wxString t;

    m_commonPanel->SaveChanges();

    DebuggerManager *dbgManager = Manager::Get()->GetDebuggerManager();

    const DebuggerManager::RegisteredPlugins &plugins = dbgManager->GetAllDebuggers();
    ConfigManager *mainConfig = Manager::Get()->GetConfigManager(wxT("debugger_common"));

    for (DebuggerManager::RegisteredPlugins::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        wxString path(wxT("/sets/"));
        path << it->first->GetSettingsName();

        mainConfig->DeleteSubPath(path);
    }

    for (size_t ii = 0; ii < m_treebook->GetPageCount(); ++ii)
    {
        wxWindow *page = m_treebook->GetPage(ii);
        MapPanelToConfiguration::iterator it = m_mapPanelToConfig.find(page);
        if (it != m_mapPanelToConfig.end())
        {
            cbDebuggerConfiguration *c = it->second.config;

            if (!c->GetConfig().IsValid())
                c->SetConfig(dbgManager->NewConfig(it->second.plugin, c->GetName()));
            if (!c->GetConfig().IsValid())
                break;

            wxString namePath = c->GetConfig().GetBasepath();
            namePath.Remove(namePath.length() - 7); // trim the "values/" from the path
            mainConfig->Write(namePath + wxT("name"), c->GetName());

            t += it->second.pluginGUIName + wxT(" - ") + c->GetName() + wxT("\n");
            if (!c->SaveChanges(static_cast<wxPanel*>(it->first)))
                break;
        }
    }

    int normalIndex = -1;
    dbgManager->GetLogger(normalIndex);

    cbDebuggerPlugin *activePlugin = dbgManager->GetActiveDebugger();
    for (DebuggerManager::RegisteredPlugins::const_iterator it = plugins.begin(); it != plugins.end(); ++it)
    {
        it->first->SetupLog(normalIndex);
        it->first->OnConfigurationChange(activePlugin == it->first);
    }

    dbgManager->RebuildAllConfigs();

    EndModal(wxID_OK);
}

inline size_t FindPageIndex(wxTreebook *treebook, wxWindow *page)
{
    size_t pageIndex = treebook->GetPageCount();
    for (size_t p = 0; p < treebook->GetPageCount(); ++p)
    {
        if (treebook->GetPage(p) == page)
        {
            pageIndex = p;
            break;
        }
    }
    return pageIndex;
}

// only return false when the name is not unique
bool DebuggerSettingsDlg::CreateConfig(wxWindow *panel, cbDebuggerPlugin *plugin, const wxString &name)
{
    size_t pageIndex = FindPageIndex(m_treebook, panel);
    if (pageIndex == m_treebook->GetPageCount())
        return true;

    for (size_t p = 0; p < m_treebook->GetPageCount(); ++p)
    {
        if (m_treebook->GetPageParent(p) == static_cast<int>(pageIndex) && m_treebook->GetPageText(p) == name)
            return false;
    }

    cbDebuggerConfiguration *pluginConfig = plugin->LoadConfig(ConfigManagerWrapper());
    if (!pluginConfig)
        return true;
    pluginConfig->SetName(name);

    wxPanel *subPanel = pluginConfig->MakePanel(m_treebook);
    m_treebook->InsertSubPage(pageIndex, subPanel, pluginConfig->GetName());

    Config conf;
    conf.plugin = plugin;
    conf.pluginGUIName = plugin->GetGUIName();
    conf.config = pluginConfig;
    m_mapPanelToConfig[subPanel] = conf;
    return true;
}

void DebuggerSettingsDlg::DeleteConfig(wxWindow *panel, cbDebuggerPlugin *plugin)
{
    size_t pageIndex = FindPageIndex(m_treebook, panel);
    if (pageIndex == m_treebook->GetPageCount())
        return;

    wxArrayString choices;
    std::vector<wxWindow*> panels;
    for (size_t p = 0; p < m_treebook->GetPageCount(); ++p)
    {
        if (m_treebook->GetPageParent(p) == static_cast<int>(pageIndex) && p != pageIndex)
        {
            choices.push_back(m_treebook->GetPageText(p));
            panels.push_back(m_treebook->GetPage(p));
        }
    }

    wxMultiChoiceDialog dialog(panel, _("Choose which configurations to be deleted"), _("Choose"), choices);
    PlaceWindow(&dialog);
    while (dialog.ShowModal() == wxID_OK)
    {
        const wxArrayInt &selection = dialog.GetSelections();
        if (selection.GetCount() == choices.GetCount())
        {
            cbMessageBox(_("Can't delete all configurations. There should be at least one."),
                         _("Error"), wxICON_ERROR, this);
            continue;
        }
        for (size_t s = 0; s < selection.GetCount(); ++s)
        {
            int index = selection[s];
            size_t p = FindPageIndex(m_treebook, panels[index]);
            if (p < m_treebook->GetPageCount())
            {
                MapPanelToConfiguration::iterator it = m_mapPanelToConfig.find(panels[index]);
                cbAssert(plugin == it->second.plugin);
                delete it->second.config;

                m_mapPanelToConfig.erase(it);
                m_treebook->DeletePage(p);
            }
        }
        break;
    }
}

void DebuggerSettingsDlg::ResetConfig(wxWindow *panel, cbDebuggerPlugin *plugin)
{
    size_t pageIndex = FindPageIndex(m_treebook, panel);
    if (pageIndex == m_treebook->GetPageCount())
        return;

    for (size_t p = m_treebook->GetPageCount(); p > 0; --p)
    {
        size_t index = p - 1;
        if (m_treebook->GetPageParent(index) == static_cast<int>(pageIndex))
        {
            wxString title = m_treebook->GetPageText(index);
            MapPanelToConfiguration::iterator it = m_mapPanelToConfig.find(m_treebook->GetPage(index));
            cbAssert(plugin == it->second.plugin);
            delete it->second.config;

            m_mapPanelToConfig.erase(it);
            m_treebook->DeletePage(index);
        }
    }

    CreateConfig(panel, plugin, wxT("Default"));
}

void DebuggerSettingsDlg::OnPageChanged(wxNotebookEvent& event)
{
    wxString caption = m_treebook->GetPageText(event.GetSelection());
    int parent = m_treebook->GetPageParent(event.GetSelection());
    if (parent != wxNOT_FOUND)
        caption = m_treebook->GetPageText(parent) + wxT(" : ") + caption;
    m_activeInfo->SetLabel(caption);
}
