#ifndef EDITORCONFIG_H
#define EDITORCONFIG_H

// For compilers that support pre-compilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#endif

#include <cbplugin.h> // for cbPlugin

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

    // Not used:
    virtual void BuildModuleMenu(const ModuleType, wxMenu*, const FileTreeData* = 0) { ; };
    virtual bool BuildToolBar(wxToolBar* /*toolBar*/)                                { return false; };

public:
    static EditorSettings ParseProjectSettings(const cbProject &project);
    static void SetProjectSettings(cbProject &project, const EditorSettings &es);
private:

    void OnEditorActivated(CodeBlocksEvent& event);

    bool ApplyEditorSettings(EditorBase* eb);

    // Reload Editor Config menu item
    void OnReloadEditorConfig(wxCommandEvent& event);
};

#endif // EDITORCONFIG_H
