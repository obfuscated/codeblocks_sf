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
        virtual cbConfigurationPanel* GetProjectConfigurationPanel(wxWindow* parent, cbProject* project){ return 0; }

        virtual void BuildMenu(wxMenuBar* menuBar){}
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }

    private:
        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
};

#endif // LOGSWITCH_H_INCLUDED
