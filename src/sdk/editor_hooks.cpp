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

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
    //put these include directive after #include "editor_hooks.h", as the macro may
    //defined in the header file
    #include <cxxabi.h>  // demangle C++ names
    #include <cstdlib>   // free the memory created by abi::__cxa_demangle
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

#include "wx/wxscintilla.h"

#include <map>


#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
// this function is only used in performance hook
static wxString GetScintillaEventName(wxEventType type)
{
    wxString name;
    if (type == wxEVT_SCI_CHANGE) name = _T("wxEVT_SCI_CHANGE");
    else if (type == wxEVT_SCI_STYLENEEDED) name = _T("wxEVT_SCI_STYLENEEDED");
    else if (type == wxEVT_SCI_CHARADDED) name = _T("wxEVT_SCI_CHARADDED");
    else if (type == wxEVT_SCI_SAVEPOINTREACHED) name = _T("wxEVT_SCI_SAVEPOINTREACHED");
    else if (type == wxEVT_SCI_SAVEPOINTLEFT) name = _T("wxEVT_SCI_SAVEPOINTLEFT");
    else if (type == wxEVT_SCI_ROMODIFYATTEMPT) name = _T("wxEVT_SCI_ROMODIFYATTEMPT");
    else if (type == wxEVT_SCI_KEY) name = _T("wxEVT_SCI_KEY");
    else if (type == wxEVT_SCI_DOUBLECLICK) name = _T("wxEVT_SCI_DOUBLECLICK");
    else if (type == wxEVT_SCI_UPDATEUI) name = _T("wxEVT_SCI_UPDATEUI");
    else if (type == wxEVT_SCI_MODIFIED) name = _T("wxEVT_SCI_MODIFIED");
    else if (type == wxEVT_SCI_MACRORECORD) name = _T("wxEVT_SCI_MACRORECORD");
    else if (type == wxEVT_SCI_MARGINCLICK) name = _T("wxEVT_SCI_MARGINCLICK");
    else if (type == wxEVT_SCI_NEEDSHOWN) name = _T("wxEVT_SCI_NEEDSHOWN");
    else if (type == wxEVT_SCI_PAINTED) name = _T("wxEVT_SCI_PAINTED");
    else if (type == wxEVT_SCI_USERLISTSELECTION) name = _T("wxEVT_SCI_USERLISTSELECTION");
    else if (type == wxEVT_SCI_URIDROPPED) name = _T("wxEVT_SCI_URIDROPPED");
    else if (type == wxEVT_SCI_DWELLSTART) name = _T("wxEVT_SCI_DWELLSTART");
    else if (type == wxEVT_SCI_DWELLEND) name = _T("wxEVT_SCI_DWELLEND");
    else if (type == wxEVT_SCI_START_DRAG) name = _T("wxEVT_SCI_START_DRAG");
    else if (type == wxEVT_SCI_DRAG_OVER) name = _T("wxEVT_SCI_DRAG_OVER");
    else if (type == wxEVT_SCI_DO_DROP) name = _T("wxEVT_SCI_DO_DROP");
    else if (type == wxEVT_SCI_ZOOM) name = _T("wxEVT_SCI_ZOOM");
    else if (type == wxEVT_SCI_HOTSPOT_CLICK) name = _T("wxEVT_SCI_HOTSPOT_CLICK");
    else if (type == wxEVT_SCI_HOTSPOT_DCLICK) name = _T("wxEVT_SCI_HOTSPOT_DCLICK");
    else if (type == wxEVT_SCI_CALLTIP_CLICK) name = _T("wxEVT_SCI_CALLTIP_CLICK");
    else if (type == wxEVT_SCI_AUTOCOMP_SELECTION) name = _T("wxEVT_SCI_AUTOCOMP_SELECTION");
    else if (type == wxEVT_SCI_INDICATOR_CLICK) name = _T("wxEVT_SCI_INDICATOR_CLICK");
    else if (type == wxEVT_SCI_INDICATOR_RELEASE) name = _T("wxEVT_SCI_INDICATOR_RELEASE");
    else name = _T("unknown wxEVT_SCI_EVENT");

    return name;
}
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

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
        {
#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
            wxStopWatch sw;
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE

            functor->Call(editor, event);

#ifdef EDITOR_HOOK_PERFORMANCE_MEASURE
            if(sw.Time() < 10) // only print a handler run longer than 10 ms
                continue;

            const char *p = functor->GetTypeName();
            int   status;
            char *realname;
            realname = abi::__cxa_demangle(p, 0, 0, &status);
            wxString txt;
            // if the demangled C++ function name success, then realname is not NULL
            if (realname != 0)
            {
                txt = wxString::FromUTF8(realname);
                free(realname);
            }
            else
                txt = wxString::FromUTF8(p); // show the mangled(original) name

            wxEventType type = event.GetEventType();
            txt << GetScintillaEventName(type);
            Manager::Get()->GetLogManager()->DebugLog(F(wxT("%s take %ld ms"), txt.wx_str(), sw.Time()));
#endif // EDITOR_HOOK_PERFORMANCE_MEASURE
        }

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
}
