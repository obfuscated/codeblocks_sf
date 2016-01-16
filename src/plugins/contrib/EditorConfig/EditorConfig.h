#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

// For compilers that support pre-compilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#endif

#include <map>

#include <cbplugin.h> // for cbPlugin
#include <globals.h>  // for ModuleType

#include "EditorConfigCommon.h"

class wxCommandEvent;
class wxMenu;
class wxMenuBar;
class wxToolBar;

class TiXmlElement;

class EditorBase;
class cbProject;
class CodeBlocksEvent;

class EditorConfig : public cbPlugin
{
public:
    EditorConfig();
    virtual ~EditorConfig() { ; };

    virtual int GetConfigurationGroup() const;
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* prj);

    // Not used:
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* /*parent*/) { return 0; };

protected:
    virtual void OnAttach();
    virtual void OnRelease(bool appShutDown);
    virtual void BuildMenu(wxMenuBar* /*menuBar*/);

    void OnProjectLoadingHook(cbProject* prj, TiXmlElement* elem, bool loading);

    // Not used:
    virtual void BuildModuleMenu(const ModuleType, wxMenu*, const FileTreeData* = 0) { ; };
    virtual bool BuildToolBar(wxToolBar* /*toolBar*/)                                { return false; };

private:
    typedef std::map<cbProject*, TEditorSettings> ProjectSettingsMap;

    void OnEditorActivated(CodeBlocksEvent& event);
    void OnProjectSettingsChanged(wxCommandEvent& event);

    bool ApplyEditorSettings(EditorBase* eb);

    // Reload Editor Config menu item
    void OnReloadEditorConfig(wxCommandEvent& event);

    ProjectSettingsMap m_ECSettings; //!< settings for currently active project
    int                m_ECHookID;   //!< project loader hook ID
	  bool               m_InitDone;   //!< flag, if initialisation was done and at least one project uses plugin

    DECLARE_EVENT_TABLE()
};

#endif // EDITORCONFIG_H
