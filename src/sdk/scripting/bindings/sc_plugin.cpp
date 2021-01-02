/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"
#include <wx/dynarray.h>
#include "sc_utils.h"
#include "sc_typeinfo_all.h"

#ifndef CB_PRECOMP
    #include "cbproject.h"
    #include "logmanager.h"
    #include "menuitemsmanager.h"
    #include "scriptingmanager.h"
#endif // CB_PRECOMP

namespace ScriptBindings
{
namespace ScriptPluginWrapper
{

// struct and map for mapping script plugins to menu callbacks
struct MenuCallback
{
    ObjectHandle object;
    int menuIndex;
};

typedef std::map<int, MenuCallback> ModuleMenuCallbacks;
ModuleMenuCallbacks s_MenuCallbacks;

// master list of registered script plugins
typedef std::map<wxString, ObjectHandle> ScriptPlugins;
ScriptPlugins s_ScriptPlugins;

// list of registered script plugins menubar items
typedef std::map<wxString, MenuItemsManager> ScriptPluginsMenus;
ScriptPluginsMenus s_ScriptPluginsMenus;

////////////////////////////////////////////////////////////////////////////////
// ask the script plugin what menus to add in the menubar
// and return an integer array of the menu IDs
////////////////////////////////////////////////////////////////////////////////
wxArrayInt CreateMenu(HSQUIRRELVM v, const wxString& name)
{
    PreserveTop preserveTop(v);

    wxArrayInt ret;

    ScriptPlugins::iterator it = s_ScriptPlugins.find(name);
    if (it == s_ScriptPlugins.end())
        return ret;

    ScriptPluginsMenus::iterator itm = s_ScriptPluginsMenus.find(name);
    if (itm == s_ScriptPluginsMenus.end())
    {
        itm = s_ScriptPluginsMenus.insert(s_ScriptPluginsMenus.end(),
                                          std::make_pair(name, MenuItemsManager(false)));
    }
    MenuItemsManager& mi = itm->second;

    const ObjectHandle &object = it->second;
    Caller caller(v, object.Get());
    const wxArrayString *menuArray;
    if (!caller.CallAndReturn0(_SC("GetMenu"), menuArray))
        return ret;

    if (menuArray)
    {
        for (size_t i = 0; i < menuArray->GetCount(); ++i)
        {
            int id = wxNewId();
            id = mi.CreateFromString((*menuArray)[i], id);

            ret.Add(id);

            MenuCallback callback;
            callback.object = object;
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
    ScriptingManager *scriptManager = Manager::Get()->GetScriptingManager();
    HSQUIRRELVM v = scriptManager->GetVM();
    bool firstItem = true;

    for (ScriptPlugins::iterator it = s_ScriptPlugins.begin(); it != s_ScriptPlugins.end(); ++it)
    {
        PreserveTop preserveTop(v);

        const ObjectHandle &object = it->second;
        Caller caller(v, object.Get());
        const wxArrayString *menuArray;
        if (!caller.CallAndReturn2(_SC("GetModuleMenu"), menuArray, SQInteger(typ), data))
        {
            LogManager *log = Manager::Get()->GetLogManager();
            log->LogError(wxString::Format(_("Calling 'GetModuleMenu' for '%s' failed!"), it->first));

            const wxString error = scriptManager->GetErrorString(true);
            log->LogError(wxString::Format(_T("The Error is: %s"), error));
            log->LogError(_("Check the 'View -> Script console' for details!"));
            continue;
        }

        if (menuArray->GetCount()==1) // exactly one menu entry
        {
            int id = wxNewId();
            if (firstItem)
            {
                firstItem = false;
                menu->AppendSeparator();
            }
            menu->Append(id, (*menuArray)[0]);
            ret.Add(id);

            MenuCallback callback;
            callback.object = it->second;
            callback.menuIndex = 0;
            s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
        }
        else if (menuArray->GetCount()>1) // more menu entries -> create sub-menu
        {
            wxMenu* sub = new wxMenu;
            for (size_t i = 0; i < menuArray->GetCount(); ++i)
            {
                int id = wxNewId();
                sub->Append(id, (*menuArray)[i]);

                ret.Add(id);

                MenuCallback callback;
                callback.object = it->second;
                callback.menuIndex = i;
                s_MenuCallbacks.insert(s_MenuCallbacks.end(), std::make_pair(id, callback));
            }

            if (firstItem)
            {
                firstItem = false;
                menu->AppendSeparator();
            }
            menu->Append(-1, it->first, sub);
        }
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// callback for script plugins menubar entries
////////////////////////////////////////////////////////////////////////////////
void CallScriptMenu(const SQChar *callbackName, int id, const wxString &errorPrefix)
{
    ModuleMenuCallbacks::iterator it;
    it = s_MenuCallbacks.find(id);
    if (it != s_MenuCallbacks.end())
    {
        MenuCallback& callback = it->second;
        HSQUIRRELVM v = callback.object.GetVM();
        PreserveTop preserveTop(v);

        Caller caller(v, callback.object.Get());
        if (!caller.Call1(callbackName, callback.menuIndex))
        {
            const wxString errorMsg = ExtractLastSquirrelError(v, false);
            if (!errorMsg.empty())
                cbMessageBox(errorPrefix + errorMsg, _("Script error"), wxICON_ERROR);
        }
    }
}

void OnScriptMenu(int id)
{
    CallScriptMenu(_SC("OnMenuClicked"), id, "OnScriptMenu: ");
}

////////////////////////////////////////////////////////////////////////////////
// callback for script plugins context menu entries
////////////////////////////////////////////////////////////////////////////////
void OnScriptModuleMenu(int id)
{
    CallScriptMenu(_SC("OnModuleMenuClicked"), id, "OnScriptModuleMenu: ");
}

////////////////////////////////////////////////////////////////////////////////
// get a script plugin squirrel object (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger GetPlugin(HSQUIRRELVM v)
{
    // env table, name
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("GetPlugin"))
        return extractor.ErrorMessage();

    ScriptPlugins::iterator it = s_ScriptPlugins.find(*extractor.p1);
    if (it != s_ScriptPlugins.end())
    {
        // Return the script object.
        it->second.Push();
    }
    else
        sq_pushnull(v);
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// execute a script plugin (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger ExecutePlugin(HSQUIRRELVM v)
{
    // env table, name
    ExtractParams2<SkipParam, const wxString *> extractor(v);
    if (!extractor.Process("ExecutePlugin"))
        return extractor.ErrorMessage();

    ScriptPlugins::iterator it = s_ScriptPlugins.find(*extractor.p1);
    if (it != s_ScriptPlugins.end())
    {
        PreserveTop preserveTop(v);
        ObjectHandle &object = it->second;

        Caller caller(v, object.Get());
        if (!caller.Call0(_SC("Execute")))
            return sq_throwerror(v, _SC("Can't find 'Execute' in script plugin class!"));

        // FIXME (squirrel) Not sure if I need to do better error handling. The original version showed a MessageBox!
    }
    sq_pushinteger(v, -1); // Just return -1, no idea why
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
// register a script plugin (script-bound function)
////////////////////////////////////////////////////////////////////////////////
SQInteger RegisterPlugin(HSQUIRRELVM v)
{
    PreserveTop preserveTop(v);

    // env table, script plugin object
    ExtractParamsBase extractor(v);
    if (!extractor.CheckNumArguments(2, "RegisterPlugin"))
        return extractor.ErrorMessage();
    if (sq_gettype(v, 2) != OT_INSTANCE)
        return sq_throwerror(v, _SC("RegisterPlugin: Parameter 2 is not an instance!"));

    ObjectHandle object(v, 2);
    Caller caller(v, object.Get());
    const PluginInfo *info;
    if (!caller.CallAndReturn0(_SC("GetPluginInfo"), info))
        return sq_throwerror(v, "RegisterPlugin: Call to GetPluginInfo failed!");

    Manager *manager = Manager::Get();
    // look if a script plugin with the same name already exists
    ScriptPlugins::iterator it = s_ScriptPlugins.find(info->name);
    if (it != s_ScriptPlugins.end())
    {
        // already exists; release the old one
        s_ScriptPlugins.erase(it);
        manager->GetLogManager()->Log(_("Script plugin unregistered: ") + info->name);
    }

    // finally, register this script plugin
    it = s_ScriptPlugins.insert(s_ScriptPlugins.end(),
                                ScriptPlugins::value_type(info->name, object));
    manager->GetLogManager()->Log(_("Script plugin registered: ") + info->name);

    manager->GetScriptingManager()->RegisterScriptPlugin(info->name, CreateMenu(v, info->name));

    // this function returns nothing on the squirrel stack
    return 0;
}

} // namespace ScriptPluginWrapper

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

        BindMethod(v, "ExecutePlugin", ScriptPluginWrapper::ExecutePlugin, nullptr);
        BindMethod(v, "GetPlugin", ScriptPluginWrapper::GetPlugin, nullptr);
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
        wxString errorMsg = ExtractLastSquirrelError(v, false);
        const wxString fullMessage = wxString::Format("Filename: cbScriptPlugin\n%sError: %s\nDetails: %s",
                                                      errorType.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptOutput.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);
    }

    // restore the print and error funcs
    sq_setprintfunc(v, oldPrintFunc, oldErrorFunc);
}

void Unregister_ScriptPlugin()
{
    ScriptPluginWrapper::s_MenuCallbacks.clear();
    ScriptPluginWrapper::s_ScriptPluginsMenus.clear();
    ScriptPluginWrapper::s_ScriptPlugins.clear();
}

} // namespace ScriptBindings
