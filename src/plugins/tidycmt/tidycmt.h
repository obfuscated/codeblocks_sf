/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef TIDYCMT_H
#define TIDYCMT_H

class wxMenuBar;
class wxMenu;
class FileTreeData;

#include "cbplugin.h"

#include "tidycmtconfig.h"

class TidyCmt : public cbPlugin
{
  public:
    TidyCmt() { ; }
    virtual ~TidyCmt() { ; }

		virtual int GetConfigurationGroup() const { return cgEditor; }
    virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
    static void ConfigurePlugin(const TidyCmtConfig& tcc);

    // Not used:
    virtual cbConfigurationPanel* GetProjectConfigurationPanel(cb_unused wxWindow* parent, cb_unused cbProject* prj) { return 0; }

  protected:
    virtual void OnAttach();
    virtual void OnRelease(bool appShutDown);

    // Not used:
    virtual void BuildMenu(cb_unused wxMenuBar* menuBar) { ; }
    virtual void BuildModuleMenu(cb_unused const ModuleType type, cb_unused wxMenu* menu, cb_unused const FileTreeData* data = 0) { ; }
    virtual bool BuildToolBar(cb_unused wxToolBar* toolBar) { return false; }

  private:

    void OnSave(CodeBlocksEvent& event);
};

#endif // TIDYCMT_H
