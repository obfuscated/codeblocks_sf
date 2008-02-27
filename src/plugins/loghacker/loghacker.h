/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef LOGSWITCH_H_INCLUDED
#define LOGSWITCH_H_INCLUDED

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h>


const int id_text = wxNewId();
const int id_channel = wxNewId();
const int id_logger = wxNewId();


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

    protected:
        virtual void OnAttach();
        virtual void OnRelease(bool appShutDown);
};

#endif
