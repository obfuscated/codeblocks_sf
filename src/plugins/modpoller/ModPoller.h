/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef MODPOLLER_H_INCLUDED
#define MODPOLLER_H_INCLUDED

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <cbplugin.h>

class ModPoller : public cbPlugin
{
    public:
        ModPoller(){};
        virtual ~ModPoller(){};

        virtual void BuildMenu(wxMenuBar* menuBar){}
        virtual void BuildModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data = 0){}
        virtual bool BuildToolBar(wxToolBar* toolBar){ return false; }

    protected:

        virtual void OnAttach()
		{
        	Manager::Get()->RegisterEventSink(cbEVT_EDITOR_ACTIVATED, new cbEventFunctor<ModPoller, CodeBlocksEvent>(this, &ModPoller::OnEditorActivated));
		}
        void OnEditorActivated(CodeBlocksEvent& event)
        {
        	EditorManager::Get()->CheckForExternallyModifiedFiles();
		}

        virtual void OnRelease(bool appShutDown){};
};

#endif
