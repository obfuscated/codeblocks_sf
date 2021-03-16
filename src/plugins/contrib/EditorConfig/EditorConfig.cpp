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

#include "EditorConfig.h"
#include "EditorConfigUI.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<EditorConfig> reg(_T("EditorConfig"));
}

EditorConfig::EditorConfig()
{
}

int EditorConfig::GetConfigurationGroup() const
{
    return cgEditor;
}

cbConfigurationPanel* EditorConfig::GetProjectConfigurationPanel(wxWindow* parent, cbProject* prj)
{
    EditorSettings es = ParseProjectSettings(*prj);

    return new EditorConfigUI(parent, this, prj, es);
}

void EditorConfig::OnAttach()
{
    // register event
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED,
        new cbEventFunctor<EditorConfig, CodeBlocksEvent>(this, &EditorConfig::OnEditorActivated));
}

void EditorConfig::OnRelease(bool /*appShutDown*/)
{
}

void EditorConfig::OnEditorActivated(CodeBlocksEvent& event)
{
    event.Skip();

    if (IsAttached())
        ApplyEditorSettings(event.GetEditor());
}

void EditorConfig::BuildMenu(wxMenuBar* menuBar)
{
    // Add a menu under "Plugins"
    int pluginMenuPos = menuBar->FindMenu(_("P&lugins"));
    if (pluginMenuPos == wxNOT_FOUND)
        return;

    wxMenu* pluginMenu = menuBar->GetMenu(pluginMenuPos);
    if (!pluginMenu)
        return;

    long idReload = wxNewId();
    pluginMenu->Prepend(idReload, _("Reload &EditorConfig"), _("Reload EditorConfig"));
    Connect(idReload, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(EditorConfig::OnReloadEditorConfig));
}

EditorSettings EditorConfig::ParseProjectSettings(const cbProject &project)
{
    EditorSettings es;
    const TiXmlNode *rootNode = project.GetExtensionsNode();
    if (!rootNode)
        return es;
    const TiXmlElement* elem = rootNode->ToElement();
    if (!elem)
        return es;

    const TiXmlElement* node = elem->FirstChildElement("editor_config");
    if (node)
    {
        int attr;

        if (node->QueryIntAttribute("active", &attr) == TIXML_SUCCESS)
            es.active = attr ? true : false;
        if (node->QueryIntAttribute("use_tabs", &attr) == TIXML_SUCCESS)
            es.use_tabs = attr ? true : false;
        if (node->QueryIntAttribute("tab_indents", &attr) == TIXML_SUCCESS)
            es.tab_indents = attr ? true : false;
        if (node->QueryIntAttribute("tab_width", &attr) == TIXML_SUCCESS)
            es.tab_width = attr;
        if (node->QueryIntAttribute("indent", &attr) == TIXML_SUCCESS)
            es.indent = attr;
        if (node->QueryIntAttribute("eol_mode", &attr) == TIXML_SUCCESS)
            es.eol_mode = attr;
    }
    return es;
}

void EditorConfig::SetProjectSettings(cbProject &project, const EditorSettings &es)
{
    TiXmlNode *rootNode = project.GetExtensionsNode();
    if (!rootNode)
        return;
    TiXmlElement* elem = rootNode->ToElement();
    if (!elem)
        return;

    TiXmlElement* node = elem->FirstChildElement("editor_config");
    if (!node && !es.active)
        return; // nothing to do, do not insert a node if not needed

    if ( node && !es.active)
    {
      // ToDo: Remove node and return?!
    }

    if (!node)
        node = elem->InsertEndChild(TiXmlElement("editor_config"))->ToElement();

    node->Clear();
    node->SetAttribute("active", (es.active ? 1 : 0));
    node->SetAttribute("use_tabs", (es.use_tabs ? 1 : 0));
    node->SetAttribute("tab_indents", (es.tab_indents ? 1 : 0));
    node->SetAttribute("tab_width", es.tab_width);
    node->SetAttribute("indent", es.indent);
    node->SetAttribute("eol_mode", es.eol_mode);
}

void EditorConfig::OnReloadEditorConfig(wxCommandEvent& /*event*/)
{
    // Reload EditorConfig
    if (ApplyEditorSettings(Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor()))
    {
        wxMessageDialog dlg(nullptr, _("Editor configuration successfully re-loaded."),
                            _("Editor Config"), wxOK);
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
    else
    {
        wxMessageDialog dlg(nullptr, _("Error re-loading editor configuration."),
                            _("Editor Config"), wxOK);
        PlaceWindow(&dlg);
        dlg.ShowModal();
    }
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

    const EditorSettings &es=ParseProjectSettings(*prj);

    // Check, if there are settings to be applied at all
    if (!es.active)
        return true;

#if defined(TRACE_EC)
    Manager::Get()->GetLogManager()->DebugLog(_T("EditorConfig::ApplyEditorSettings(TRUE)"));
#endif

    control->SetUseTabs(es.use_tabs);
    control->SetTabIndents(es.tab_indents);
    if (es.indent > 0)
        control->SetIndent(es.indent);
    if (es.tab_width > 0)
        control->SetTabWidth(es.tab_width);
    if (es.indent < 0) /* set indent to tab_width here */
        control->SetIndent(control->GetTabWidth());

    // This works only if in sync with wxscintilla.h
    if      (es.eol_mode == wxSCI_EOL_CRLF)
        control->SetEOLMode(wxSCI_EOL_CRLF);
    else if (es.eol_mode == wxSCI_EOL_CR)
        control->SetEOLMode(wxSCI_EOL_CR);
    else if (es.eol_mode == wxSCI_EOL_LF)
        control->SetEOLMode(wxSCI_EOL_LF);
    // else do nothing because its set to "use settings > editor"

    return true;
}
