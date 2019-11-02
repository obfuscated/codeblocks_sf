/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef LOGSWITCH_H_INCLUDED
#define LOGSWITCH_H_INCLUDED

class wxMenuBar;
class wxMenu;
class wxWindow;
class cbProject;
class FileTreeData;

#include "cbplugin.h"

class LogHacker : public cbPlugin
{
    public:
        LogHacker();
        virtual ~LogHacker();

        virtual int GetConfigurationPriority() const { return 50; }
        virtual int GetConfigurationGroup() const { return cgUnknown; }

        virtual cbConfigurationPanel* GetConfigurationPanel(wxWindow* parent);
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(cb_unused wxWindow* parent, cb_unused cbProject* project){ return 0; }

        virtual void BuildMenu(cb_unused wxMenuBar* menuBar){}
        virtual void BuildModuleMenu(cb_unused const ModuleType type, cb_unused wxMenu* menu, cb_unused const FileTreeData* data = 0){}
        virtual bool BuildToolBar(cb_unused wxToolBar* toolBar){ return false; }

    private:
        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
};

#endif // LOGSWITCH_H_INCLUDED
