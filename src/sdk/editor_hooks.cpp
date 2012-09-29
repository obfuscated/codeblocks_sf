/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "cbeditor.h"
    #include "cbplugin.h"
#endif

#include "editor_hooks.h"

#include "wx/wxscintilla.h"

#include <map>

namespace EditorHooks
{
    typedef std::map<int, HookFunctorBase*> HookFunctorsMap;
    static HookFunctorsMap s_HookFunctorsMap;
    static int             s_UniqueID = 0;
}

int EditorHooks::RegisterHook(EditorHooks::HookFunctorBase* functor)
{
    for (HookFunctorsMap::iterator it = s_HookFunctorsMap.begin(); it != s_HookFunctorsMap.end(); ++it)
    {
        if (it->second == functor)
            return it->first;
    }
    s_HookFunctorsMap[s_UniqueID] = functor;
    return s_UniqueID++;
}

EditorHooks::HookFunctorBase* EditorHooks::UnregisterHook(int id, bool deleteHook)
{
    HookFunctorsMap::iterator it = s_HookFunctorsMap.find(id);
    if (it != s_HookFunctorsMap.end())
    {
        EditorHooks::HookFunctorBase* functor = it->second;
        s_HookFunctorsMap.erase(it);
        if (deleteHook)
        {
            delete functor;
            return 0;
        }
        return functor;
    }
    return 0;
}

bool EditorHooks::HasRegisteredHooks()
{
    return s_HookFunctorsMap.size() != 0;
}

void EditorHooks::CallHooks(cbEditor* editor, wxScintillaEvent& event)
{
    for (HookFunctorsMap::iterator it = s_HookFunctorsMap.begin(); it != s_HookFunctorsMap.end(); ++it)
    {
        EditorHooks::HookFunctorBase* functor = it->second;
        if (functor)
            functor->Call(editor, event);
    }
}

namespace EditorHooks
{
    cbSmartIndentEditorHookFunctor::cbSmartIndentEditorHookFunctor(cbSmartIndentPlugin *plugin):
    m_plugin(plugin){}

    void cbSmartIndentEditorHookFunctor::Call(cbEditor *editor, wxScintillaEvent &event) const
    {
        m_plugin->OnEditorHook(editor, event);
    }
};
