#include <sdk.h> // Code::Blocks SDK

#ifndef CB_PRECOMP
    #include <wx/event.h>
    #include <wx/msgdlg.h>
    #include <wx/menu.h>

    #include <tinyxml/tinyxml.h>

    #include <cbproject.h>
    #include <cbeditor.h>
    #include <configurationpanel.h>
    #include <editorbase.h>
    #include <editormanager.h>
    #include <manager.h>
#endif

#include <cbstyledtextctrl.h>
#include <projectloader_hooks.h>

#include "EditorConfig.h"
#include "EditorConfigUI.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<EditorConfig> reg(_T("EditorConfig"));
}

BEGIN_EVENT_TABLE(EditorConfig, cbPlugin)
    EVT_EDITOR_SETTINGS_CHANGED_EVENT(-1 /* all */, EditorConfig::OnProjectSettingsChanged)
END_EVENT_TABLE()

EditorConfig::EditorConfig()
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if (!Manager::LoadResource(_T("EditorConfig.zip")))
        NotifyMissingFile(_T("EditorConfig.zip"));

    // hook to project loading procedure
    ProjectLoaderHooks::HookFunctorBase* ec_hook =
        new ProjectLoaderHooks::HookFunctor<EditorConfig>(this, &EditorConfig::OnProjectLoadingHook);

    m_ECHookID = ProjectLoaderHooks::RegisterHook(ec_hook);
    m_InitDone = false;
}

int EditorConfig::GetConfigurationGroup() const
{
    return cgEditor;
}

cbConfigurationPanel* EditorConfig::GetProjectConfigurationPanel(wxWindow* parent, cbProject* prj)
{
  TEditorSettings es;
  if (m_ECSettings.find(prj) == m_ECSettings.end())
      es.active = false; // not to be used
  else
  {
      es.active      = m_ECSettings[prj].active;
      es.use_tabs    = m_ECSettings[prj].use_tabs;
      es.tab_indents = m_ECSettings[prj].tab_indents;
      es.tab_width   = m_ECSettings[prj].tab_width;
      es.indent      = m_ECSettings[prj].indent;
      es.eol_mode    = m_ECSettings[prj].eol_mode;
  }

  // deleted by the caller
#if defined(TRACE_EC)
  if (prj)
      Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::GetProjectConfigurationPanel(PROJECT)"));
  else
      Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::GetProjectConfigurationPanel(NULL)"));
#endif
  return (new EditorConfigUI(parent, this, prj, es));
}

void EditorConfig::OnAttach()
{
    m_InitDone = false;

    // register event
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,
        new cbEventFunctor<EditorConfig, CodeBlocksEvent>(this, &EditorConfig::OnEditorActivated));
}

void EditorConfig::OnRelease(bool appShutDown)
{
    if (m_InitDone)
        ProjectLoaderHooks::UnregisterHook(m_ECHookID, true);

    m_InitDone = false;
}

void EditorConfig::OnEditorActivated(CodeBlocksEvent& event)
{
    event.Skip();

    if (IsAttached() && m_InitDone)
        ApplyEditorSettings(event.GetEditor());
}

void EditorConfig::OnProjectSettingsChanged(wxCommandEvent& event)
{
  TEditorSettings es  = (static_cast<EditorSettingsChangedEvent&>(event)).GetEditorSettings();
  cbProject*      prj = (static_cast<EditorSettingsChangedEvent&>(event)).GetProject();

#if defined(TRACE_EC)
  Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectSettingsChanged()"));
#endif

  if (prj)
  {
#if defined(TRACE_EC)
      Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectSettingsChanged(TRUE)"));
#endif
      m_ECSettings[prj] = es;
  }
}

void EditorConfig::BuildMenu(wxMenuBar* menuBar)
{
    // Add a menu under "Plugins"
    int pluginMenuPos = menuBar->FindMenu(_T("Plugins"));
    if (pluginMenuPos == wxNOT_FOUND)
        return;

    wxMenu* pluginMenu = menuBar->GetMenu(pluginMenuPos);
    if (!pluginMenu)
        return;

    long idReload = wxNewId();
    pluginMenu->Prepend(idReload, _T("Reload &EditorConfig"), _T("Reload EditorConfig"));
    Connect(idReload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorConfig::OnReloadEditorConfig));
}

void EditorConfig::OnProjectLoadingHook(cbProject* prj, TiXmlElement* elem, bool loading)
{
    m_InitDone = true;

    if (!prj || !elem)
        return; // ?! Should actually NOT happen...

    if (loading)
    {
#if defined(TRACE_EC)
        Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectLoadingHook(LOAD)"));
#endif

        TiXmlElement* node = elem->FirstChildElement("editor_config");
        if (node)
        {
            int             attr;
            TEditorSettings es;

            if (node->QueryIntAttribute("active",      &attr) == TIXML_SUCCESS)
              es.active      = attr ? true : false;
            if (node->QueryIntAttribute("use_tabs",    &attr) == TIXML_SUCCESS)
              es.use_tabs    = attr ? true : false;
            if (node->QueryIntAttribute("tab_indents", &attr) == TIXML_SUCCESS)
              es.tab_indents = attr ? true : false;
            if (node->QueryIntAttribute("tab_width",   &attr) == TIXML_SUCCESS)
              es.tab_width   = attr;
            if (node->QueryIntAttribute("indent",      &attr) == TIXML_SUCCESS)
              es.indent      = attr;
            if (node->QueryIntAttribute("eol_mode",    &attr) == TIXML_SUCCESS)
              es.eol_mode    = attr;

            m_ECSettings[prj] = es;
#if defined(TRACE_EC)
            Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectLoadingHook(LOADED)"));
#endif
        }
    }
    else if (m_ECSettings.find(prj) != m_ECSettings.end())
    {
#if defined(TRACE_EC)
        Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectLoadingHook(SAVE)"));
#endif

        // Hook called when saving project file and this project is tracke

        TiXmlElement* node = elem->FirstChildElement("editor_config");
        if (!node && !m_ECSettings[prj].active)
            return; // nothing to do, do not insert a node if not needed

        if ( node && !m_ECSettings[prj].active)
        {
          // ToDo: Remove node and return?!
        }

        if (!node)
            node = elem->InsertEndChild(TiXmlElement("editor_config"))->ToElement();

        node->Clear();
        node->SetAttribute("active",      m_ECSettings[prj].active      ? 1 : 0);
        node->SetAttribute("use_tabs",    m_ECSettings[prj].use_tabs    ? 1 : 0);
        node->SetAttribute("tab_indents", m_ECSettings[prj].tab_indents ? 1 : 0);
        node->SetAttribute("tab_width",   m_ECSettings[prj].tab_width          );
        node->SetAttribute("indent",      m_ECSettings[prj].indent             );
        node->SetAttribute("eol_mode",    m_ECSettings[prj].eol_mode           );

#if defined(TRACE_EC)
        Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::OnProjectLoadingHook(SAVED)"));
#endif
    }
}

void EditorConfig::OnReloadEditorConfig(wxCommandEvent& event)
{
    // Reload EditorConfig
    if ( ApplyEditorSettings(Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()) )
        wxMessageDialog(NULL, _T("Editor configuration successfully re-loaded."), wxT("EditorConfig"), wxOK).ShowModal();
    else
        wxMessageDialog(NULL, _("Error re-loading editor configuration."),        wxT("EditorConfig"), wxOK).ShowModal();
}

bool EditorConfig::ApplyEditorSettings(EditorBase* eb)
{
#if defined(TRACE_EC)
    Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::ApplyEditorSettings()"));
#endif

    if (!eb)
        return false;

    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinEditor(eb);
    if (!ed)
        return false;

    cbStyledTextCtrl* control = ed->GetControl();
    if (!control)
        return false;

    // Check, if the files belongs to a project -> if not, we cannot apply settings
    ProjectFile* pf = ed->GetProjectFile();
    if (!pf)
        return true;

    cbProject* prj = pf->GetParentProject();
    if (!prj)
        return true;

    if (m_ECSettings.find(prj) == m_ECSettings.end())
        return true;

    // Check, if there are settings to be applied at all
    if (!m_ECSettings[prj].active)
        return true;

#if defined(TRACE_EC)
    Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::ApplyEditorSettings(TRUE)"));
#endif

    control->SetUseTabs(m_ECSettings[prj].use_tabs);
    control->SetTabIndents(m_ECSettings[prj].tab_indents);
    if (m_ECSettings[prj].indent > 0)
        control->SetIndent(m_ECSettings[prj].indent);
    if (m_ECSettings[prj].tab_width > 0)
        control->SetTabWidth(m_ECSettings[prj].tab_width);
    if (m_ECSettings[prj].indent < 0) /* set indent to tab_width here */
        control->SetIndent(control->GetTabWidth());

    if      (m_ECSettings[prj].eol_mode == wxSCI_EOL_CRLF)
        control->SetEOLMode(wxSCI_EOL_CRLF);
    else if (m_ECSettings[prj].eol_mode == wxSCI_EOL_CR)
        control->SetEOLMode(wxSCI_EOL_CR);
    else if (m_ECSettings[prj].eol_mode == wxSCI_EOL_LF)
        control->SetEOLMode(wxSCI_EOL_LF);

    return true;
}
