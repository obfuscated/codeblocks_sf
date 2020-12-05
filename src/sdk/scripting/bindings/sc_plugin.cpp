/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#if 0

#include <sdk_precomp.h>
#include "sc_plugin.h"
#include <manager.h>
#include <scriptingmanager.h>
#include <wx/menu.h>

#include <map>

namespace ScriptBindings {
namespace ScriptPluginWrapper {

// struct and map for mapping script plugins to menu callbacks
struct MenuCallback
{
    SquirrelObject object;
    int menuIndex;
};
typedef std::map<int, MenuCallback> ModuleMenuCallbacks;
ModuleMenuCallbacks s_MenuCallbacks;

// master list of registered script plugins
typedef std::map<wxString, SquirrelObject> ScriptPlugins;
ScriptPlugins s_ScriptPlugins;

// list of registered script plugins menubar items
typedef std::map<wxString, MenuItemsManager> ScriptPluginsMenus;
ScriptPluginsMenus s_ScriptPluginsMenus;

////////////////////////////////////////////////////////////////////////////////
// ask the script plugin what menus to add in the menubar
// and return an integer array of the menu IDs
////////////////////////////////////////////////////////////////////////////////
wxArrayInt CreateMenu(const wxString& name)
{
    wxArrayInt ret;

    ScriptPlugins::iterator it = s_ScriptPlugins.find(name);
    if (it == s_ScriptPlugins.end())
        return ret;
    SquirrelObject& o = it->second;

    ScriptPluginsMenus::iterator itm = s_ScriptPluginsMenus.find(name);
    if (itm == s_ScriptPluginsMenus.end())
    {
        itm = s_ScriptPluginsMenus.insert(s_ScriptPluginsMenus.end(), std::make_pair(name, MenuItemsManager(false)));
    }
    MenuItemsManager& mi = itm->second;

    SqPlus::SquirrelFunction<wxArrayString&> f(o, "GetMenu");
    if (f.func.IsNull())
        return ret;

    wxArrayString arr;
    try
    {
        arr = f();
    }
    catch (SquirrelError e)
    {
        cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
        return ret;
    }

    if (arr.GetCount())
    {
        for (size_t i = 0; i < arr.GetCount(); ++i)
        {
            int id = wxNewId();
            id = mi.CreateFromString(arr[i], id);

            ret.Add(id);

            MenuCallback callback;
            callback.object = it->second;
            callback.menuIndex = i;

            ModuleMenuCallbacks::iterator mmcIt = s_MenuCallbacks.find(id);
            if (mmcIt == s_MenuCallbacks.end())
                s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
            else
            {
                s_MenuCallbacks.erase(mmcIt);
                s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
            }
        }
    }

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// ask the script plugin what items to add in the context menu
// and return an integer array of the menu IDs
////////////////////////////////////////////////////////////////////////////////
wxArrayInt CreateModuleMenu(const ModuleType typ, wxMenu* menu, const FileTreeData* data)
{
    wxArrayInt ret;

    ScriptPlugins::iterator it;
    for (it = s_ScriptPlugins.begin(); it != s_ScriptPlugins.end(); ++it)
    {
        SquirrelObject& o = it->second;
        SqPlus::SquirrelFunction<wxArrayString&> f(o, "GetModuleMenu");
        if (f.func.IsNull())
            continue;

        wxArrayString arr;
        try
        {
            arr = f(typ, data);
        }
        catch (SquirrelError e)
        {
            cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
            continue;
        }

        if (arr.GetCount()==1) // exactly one menu entry
        {
            int id = wxNewId();
            menu->Append(id, arr[0]);
            ret.Add(id);

            MenuCallback callback;
            callback.object = it->second;
            callback.menuIndex = 0;
            s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
        }
        else if (arr.GetCount()>1) // more menu entries -> create sub-menu
        {
            wxMenu* sub = new wxMenu;
            for (size_t i = 0; i < arr.GetCount(); ++i)
            {
                int id = wxNewId();
                sub->Append(id, arr[i]);

                ret.Add(id);

                MenuCallback callback;
                callback.object = it->second;
                callback.menuIndex = i;
                s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
            }
            menu->Append(-1, it->first, sub);
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// callback for script plugins menubar entries
////////////////////////////////////////////////////////////////////////////////
void OnScriptMenu(int id)
{
    ModuleMenuCallbacks::iterator it;
    it = s_MenuCallbacks.find(id);
    if (it != s_MenuCallbacks.end())
    {
        MenuCallback& callback = it->second;
        SqPlus::SquirrelFunction<void> f(callback.object, "OnMenuClicked");
        if (!f.func.IsNull())
        {
            try
            {
                f(callback.menuIndex);
            }
            catch (SquirrelError e)
            {
                cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// callback for script plugins context menu entries
////////////////////////////////////////////////////////////////////////////////
void OnScriptModuleMenu(int id)
{
    ModuleMenuCallbacks::iterator it;
    it = s_MenuCallbacks.find(id);
    if (it != s_MenuCallbacks.end())
    {
        MenuCallback& callback = it->second;
        SqPlus::SquirrelFunction<void> f(callback.object, "OnModuleMenuClicked");
        if (!f.func.IsNull())
        {
            try
            {
                f(callback.menuIndex);
            }
            catch (SquirrelError e)
            {
                cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// register a script plugin (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger RegisterPlugin(HSQUIRRELVM v)
{
    // get squirrel object to register from stack
    SquirrelObject o;
    o.AttachToStackObject(2);

    // first verify that there is a member function to retrieve the plugin info
    if (!o.Exists("GetPluginInfo"))
        return sq_throwerror(v, "Not a script plugin!");

    // ask for its registration name
    SqPlus::SquirrelFunction<PluginInfo&> f(o, "GetPluginInfo");
    PluginInfo& info = f();
    wxString s = info.name;

    // look if a script plugin with the same name already exists
    ScriptPlugins::iterator it = s_ScriptPlugins.find(s);
    if (it != s_ScriptPlugins.end())
    {
        // already exists; release the old one
        s_ScriptPlugins.erase(it);
        Manager::Get()->GetLogManager()->Log(_("Script plugin unregistered: ") + s);
    }

    // finally, register this script plugin
    it = s_ScriptPlugins.insert(s_ScriptPlugins.end(), std::make_pair(s, o));
    Manager::Get()->GetLogManager()->Log(_("Script plugin registered: ") + s);

    Manager::Get()->GetScriptingManager()->RegisterScriptPlugin(s, CreateMenu(s));

    // this function returns nothing on the squirrel stack
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// get a script plugin squirrel object (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger GetPlugin(HSQUIRRELVM v)
{
    StackHandler sa(v);

    // get the script plugin's name
    const wxString& name = *SqPlus::GetInstance<wxString,false>(v, 2);

    // search for it in the registered script plugins list
    ScriptPlugins::iterator it = s_ScriptPlugins.find(name);
    if (it != s_ScriptPlugins.end())
    {
        // found; return the squirrel object
        return sa.Return(it->second);
    }

    // not found; return nothing
    return sa.Return();
}

////////////////////////////////////////////////////////////////////////////////
// execute a script plugin (script-bound function)
////////////////////////////////////////////////////////////////////////////////
int ExecutePlugin(const wxString& name)
{
    // look for script plugin
    ScriptPlugins::iterator it = s_ScriptPlugins.find(name);
    if (it != s_ScriptPlugins.end())
    {
        // found; execute it
        SquirrelObject& o = it->second;
        SqPlus::SquirrelFunction<int> f(o, "Execute");
        if (!f.func.IsNull())
        {
            try
            {
                f();
            }
            catch (SquirrelError e)
            {
                cbMessageBox(cbC2U(e.desc), _("Script error"), wxICON_ERROR);
            }
        }
    }
    return -1;
}

}; // namespace ScriptPluginWrapper
#else

#include "sdk.h"
#include <wx/dynarray.h>
#include "sc_utils.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
    #include "logmanager.h"
#endif // CB_PRECOMP

namespace ScriptBindings
{
namespace ScriptPluginWrapper
{
wxArrayInt CreateMenu(const wxString& name)
{
    return wxArrayInt();
}
wxArrayInt CreateModuleMenu(const ModuleType typ, wxMenu* menu, const FileTreeData* data)
{
    return wxArrayInt();
}
void OnScriptMenu(int id)
{
}
void OnScriptModuleMenu(int id)
{
}


////////////////////////////////////////////////////////////////////////////////
// register a script plugin (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger RegisterPlugin(HSQUIRRELVM v)
{
    // FIXME (squirrel) Implement me

    // this function returns nothing on the squirrel stack
    return 0;
}

} // namespace ScriptPluginWrapper
#endif // 0

// base script plugin class
const char* s_cbScriptPlugin = R"(
class cbScriptPlugin
{
    info = PluginInfo();
    constructor()
    {
        info.name = _T("cbScriptPlugin");
        info.title = _T("Base script plugin");
        info.version = _T("0.1a");
        info.license = _T("GPL");
    }
    function GetPluginInfo()
    {
        return info;
    }
    function GetMenu()
    {
        return wxArrayString();
    }
    function GetModuleMenu(who,data)
    {
        return wxArrayString();
    }
    function Execute()
    {
        LogDebug(info.name + _T("::Run() : not implemented"));
        return -1;
    }
    function OnMenuClicked(index)
    {
        LogDebug(info.name + _T(": menu clicked: ") + index);
    }
    function OnModuleMenuClicked(index)
    {
        LogDebug(info.name + _T(": module menu clicked: ") + index);
    }
};

)";

// FIXME (squirrel) This is duplicated! Try to deduplicate.
static wxString s_ScriptOutput;

// FIXME (squirrel) This is duplicated! Try to deduplicate.
static void PrintSquirrelToWxString(wxString& msg, const SQChar* s, va_list& vl)
{
    int buffer_size = 2048;
    SQChar* tmp_buffer;
    for (;;buffer_size*=2)
    {
        // FIXME (squirrel) Optimize this
        tmp_buffer = new SQChar [buffer_size];
        int retvalue = scvsprintf(tmp_buffer, buffer_size, s, vl);
        if (retvalue < buffer_size)
        {
            // Buffersize was large enough
            msg = cbC2U(tmp_buffer);
            delete[] tmp_buffer;
            break;
        }
        // Buffer size was not enough
        delete[] tmp_buffer;
    }
}

// FIXME (squirrel) This is duplicated! Try to deduplicate.
static void ScriptsPrintFunc(HSQUIRRELVM /*v*/, const SQChar * s, ...)
{
    va_list vl;
    va_start(vl,s);
    wxString msg;
    PrintSquirrelToWxString(msg,s,vl);
    va_end(vl);

    s_ScriptOutput << msg;
}

////////////////////////////////////////////////////////////////////////////////
// register the script plugin framework
////////////////////////////////////////////////////////////////////////////////
void Register_ScriptPlugin(HSQUIRRELVM v)
{
    PreserveTop preserveTop(v);

    {
        sq_pushroottable(v);

// FIXME (squirrel) Reimplement sc_plugin
//    SqPlus::RegisterGlobal(&ScriptPluginWrapper::ExecutePlugin, "ExecutePlugin");
//    SquirrelVM::CreateFunctionGlobal(&ScriptPluginWrapper::GetPlugin, "GetPlugin", "*");
        BindMethod(v, "RegisterPlugin", ScriptPluginWrapper::RegisterPlugin, nullptr);

        sq_pop(v, 1); // root table
    }

    // load base script plugin

    // WARNING: we CANNOT use ScriptingManager::LoadBuffer() because we have reached here
    // by a call from inside ScriptingManager's constructor. This would cause an infinite
    // loop and the app would die with a stack overflow. We got to load the script manually...
    // we also have to disable the print and error func for a while

    SQPRINTFUNCTION oldPrintFunc = sq_getprintfunc(v);
    SQPRINTFUNCTION oldErrorFunc = sq_geterrorfunc(v);
    sq_setprintfunc(v, ScriptsPrintFunc, ScriptsPrintFunc);

    wxString errorType;

    if (SQ_SUCCEEDED(sq_compilebuffer(v, s_cbScriptPlugin,
                                      (strlen(s_cbScriptPlugin) + 1) * sizeof(SQChar),
                                      "cbScriptPlugin", 1)))
    {
        sq_pushroottable(v); // this is the parameter for the script closure

        if (SQ_FAILED(sq_call(v, 1, SQFalse, SQTrue)))
            errorType = "Call";
        sq_pop(v, 1); // pop the closure
    }
    else
        errorType = "Compile";

    if (!errorType.empty())
    {
        const SQChar *s;
        sq_getlasterror(v);
        sq_getstring(v, -1, &s);
        wxString errorMsg;
        if (s)
            errorMsg = wxString(s);
        else
            errorMsg = "Unknown error!";
        sq_pop(v, 1);

        const wxString fullMessage = wxString::Format("Filename: cbScriptPlugin\n%sError: %s\nDetails: %s",
                                                      errorType.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptOutput.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);
    }

    // restore the print and error funcs
    sq_setprintfunc(v, oldPrintFunc, oldErrorFunc);
}

} // namespace ScriptBindings
