/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <wx/textdlg.h>
    #include <globals.h>
    #include <settings.h>
    #include <manager.h>
    #include <logmanager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <projectmanager.h>
    #include <pluginmanager.h>
    #include <cbproject.h>
#endif

#include "scripting/bindings/sc_base_types.h"
#include "scripting/bindings/sq_wx/sq_wx_type_handler.h"

#include <wx/colordlg.h>
#include <wx/numdlg.h>
#include <infowindow.h>

namespace ScriptBindings
{
    // global funcs
    void gDebugLog(const wxString& msg)         { Manager::Get()->GetLogManager()->DebugLog(msg); }
    void gErrorLog(const wxString& msg)         { Manager::Get()->GetLogManager()->LogError(msg); }
    void gWarningLog(const wxString& msg)       { Manager::Get()->GetLogManager()->LogWarning(msg); }
    void gLog(const wxString& msg)              { Manager::Get()->GetLogManager()->Log(msg); }
    int  gMessage(const wxString& msg, const wxString& caption, int buttons){ return cbMessageBox(msg, caption, buttons); }
    void gShowMessage(const wxString& msg)      { cbMessageBox(msg, _("Script message"), wxICON_INFORMATION | wxOK); }
    void gShowMessageWarn(const wxString& msg)  { cbMessageBox(msg, _("Script warning"), wxICON_WARNING | wxOK); }
    void gShowMessageError(const wxString& msg) { cbMessageBox(msg, _("Script error"), wxICON_ERROR | wxOK); }
    void gShowMessageInfo(const wxString& msg)  { cbMessageBox(msg, _("Script information"), wxICON_INFORMATION | wxOK); }

// FIXME (bluehazzard#1#): Fix the scripts, because replace macros only use one parameter, for compatibility this is implemented
    SQInteger gReplaceMacros(HSQUIRRELVM v)
    {
        StackHandler sa(v);

        if (sa.GetParamCount() == 0) {
            return sa.ThrowError(_("ReplaceMacros: wrong number of parameters"));
        }

        bool subrequest = false;
        wxString origin = sa.GetValue<wxString>(2);
        if(sa.GetParamCount() >= 3)
        {
            subrequest = sa.GetValue<bool>(3);
        }
        if(sa.HasError()) {
            return sa.ThrowError(_("ReplaceMacros: something is wrong"));
        }

        wxString ret_val(origin);
        Manager::Get()->GetMacrosManager()->ReplaceMacros(ret_val,nullptr,subrequest);
        sa.PushInstanceCopy(ret_val);

        return SC_RETURN_VALUE;
    }

    SQInteger IsNull(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        SQUserPointer up = nullptr;
        sq_getinstanceup(v, 2, &up, nullptr);
        sa.PushValue<SQBool>((up == nullptr));
        return SC_RETURN_VALUE;
    }

    ProjectManager* getPM()
    {
        return Manager::Get()->GetProjectManager();
    }

    EditorManager* getEM()
    {
        return Manager::Get()->GetEditorManager();
    }

    ConfigManager* getCM()
    {
        return Manager::Get()->GetConfigManager(_T("scripts"));
    }

    CompilerFactory* getCF()
    {
        static CompilerFactory cf; // all its members are static functions anyway
        return &cf;
    }

    UserVariableManager* getUVM()
    {
        return Manager::Get()->GetUserVariableManager();
    }

    ScriptingManager* getSM()
    {
        return Manager::Get()->GetScriptingManager();
    }

    bool InstallPlugin(const wxString& pluginName, bool allUsers, bool confirm)
    {
        if (cbMessageBox(_("A script is trying to install a Code::Blocks plugin.\n"
                            "Do you wish to allow this?\n\n") + pluginName,
                            _("Security warning"), wxICON_WARNING | wxYES_NO) == wxID_NO)
        {
            return false;
        }
        return Manager::Get()->GetPluginManager()->InstallPlugin(pluginName, allUsers, confirm);
    }

    int ExecutePlugin(const wxString& pluginName)
    {
        return Manager::Get()->GetPluginManager()->ExecutePlugin(pluginName);
    }

    int ConfigurePlugin(const wxString& pluginName)
    {
        cbMessageBox(_("\"ConfigurePlugin\" does nothing. Please remove from your code"),_("Squirrel API information"),wxICON_INFORMATION|wxOK);
        return 0; /* leaving script binding intact for compatibility, but this is factually not implemented at all */
    }

    // locate and call a menu from string (e.g. "/Valgrind/Run Valgrind::MemCheck")
    int CallMenu(const wxString& menuPath)
    {
        // this code is partially based on MenuItemsManager::CreateFromString()
        wxMenuBar* mbar = Manager::Get()->GetAppFrame()->GetMenuBar();
        wxMenu* menu = nullptr;
        size_t pos = 0;
        while (true)
        {
            // ignore consecutive slashes
            while (pos < menuPath.Length() && menuPath.GetChar(pos) == _T('/'))
                ++pos;

            // find next slash
            size_t nextPos = pos;
            while (nextPos < menuPath.Length() && menuPath.GetChar(++nextPos) != _T('/'))
                ;

            wxString current = menuPath.Mid(pos, nextPos - pos);
            if (current.IsEmpty())
                break;
            bool isLast = nextPos >= menuPath.Length();
            // current holds the current search string

            if (!menu) // no menu yet? look in menubar
            {
                int menuPos = mbar->FindMenu(current);
                if (menuPos == wxNOT_FOUND)
                    break; // failed
                else
                    menu = mbar->GetMenu(menuPos);
            }
            else
            {
                if (isLast)
                {
                    int id = menu->FindItem(current);
                    if (id != wxNOT_FOUND)
                    {
                        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, id);
                        #if wxCHECK_VERSION(3, 0, 0)
                        mbar->GetEventHandler()->ProcessEvent(evt);
                        #else
                        if(!Manager::Get()->GetAppWindow()->ProcessEvent(evt))
                        //if ( !mbar->ProcessEvent(evt) )
                        {
                            // TODO (bluehazzard#1#): Report the error to squirrel
                            wxString msg;
                            msg.Printf(_("Calling the menu '%s' with ID %d failed."), menuPath.wx_str(), id);
                            cbMessageBox(msg, _("Script error"), wxICON_WARNING);
                            return -1;
                        }
                        #endif
                        // done
                    }
                    break;
                }
                int existing = menu->FindItem(current);
                if (existing != wxNOT_FOUND)
                    menu = menu->GetMenuItems()[existing]->GetSubMenu();
                else
                    return -2; // failed
            }
            pos = nextPos; // prepare for next loop
            return 0;
        }
    }

    void Include(const wxString& filename)
    {
        getSM()->LoadScript(filename);
    }

    SQInteger Require(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        wxString filename = sa.GetValue<wxString>(2);
        if (!getSM()->LoadScript(filename))
        {
            wxString msg = wxString::Format(_("Failed to load required script: %s"), filename.c_str());
            return sa.ThrowError(msg);
        }
        sa.PushValue<SQInteger>(0);
        return SC_RETURN_VALUE;
    }

    bool LoadResource(wxString& res)
    {
        // Do not use this function. Use the function incorporated in the wxDialog class, so resources can get unloaded when they are not needed any more
        return Manager::LoadResource(res);
    }


    void Register_Globals(HSQUIRRELVM vm)
    {
          // global funcs
        Sqrat::RootTable(vm).Func("Log",        gLog);
        Sqrat::RootTable(vm).Func("LogDebug",   gDebugLog);
        Sqrat::RootTable(vm).Func("LogWarning", gWarningLog);
        Sqrat::RootTable(vm).Func("LogError",   gErrorLog);

        Sqrat::RootTable(vm).Func("Message",    gMessage);
        Sqrat::RootTable(vm).Func("ShowMessage",gShowMessage);
        Sqrat::RootTable(vm).Func("ShowWarning",gShowMessageWarn);
        Sqrat::RootTable(vm).Func("ShowError",  gShowMessageError);
        Sqrat::RootTable(vm).Func("ShowInfo",   gShowMessageInfo);

        Sqrat::RootTable(vm).SquirrelFunc("ReplaceMacros",gReplaceMacros);

        Sqrat::RootTable(vm).Func("GetProjectManager",  getPM);
        Sqrat::RootTable(vm).Func("GetEditorManager",   getEM);
        Sqrat::RootTable(vm).Func("GetConfigManager",   getCM);
        Sqrat::RootTable(vm).Func("GetUserVariableManager",getUVM);
        Sqrat::RootTable(vm).Func("GetScriptingManager",getSM);
        Sqrat::RootTable(vm).Func("GetCompilerFactory", getCF);

        // from globals.h
        Sqrat::RootTable(vm).Func("GetArrayFromString", GetArrayFromString);
        Sqrat::RootTable(vm).Func("GetStringFromArray", GetStringFromArray);
        Sqrat::RootTable(vm).Func("EscapeSpaces",       EscapeSpaces);
        Sqrat::RootTable(vm).Func("UnixFilename",       UnixFilename);
        Sqrat::RootTable(vm).Func("FileTypeOf",         FileTypeOf);
        Sqrat::RootTable(vm).Func("URLEncode",          URLEncode);
        Sqrat::RootTable(vm).Func("NotifyMissingFile",  NotifyMissingFile);
        Sqrat::RootTable(vm).Func("GetPlatformsFromString",GetPlatformsFromString);
        Sqrat::RootTable(vm).Func("GetStringFromPlatforms",GetStringFromPlatforms);

        Sqrat::RootTable(vm).Func("GetFolder",          ConfigManager::GetFolder);
        Sqrat::RootTable(vm).Func("LocateDataFile",     ConfigManager::LocateDataFile);

        Sqrat::RootTable(vm).Func("ExecuteToolPlugin",  ExecutePlugin);
        Sqrat::RootTable(vm).Func("ConfigureToolPlugin",ConfigurePlugin);
        Sqrat::RootTable(vm).Func("InstallPlugin",      InstallPlugin);

        Sqrat::RootTable(vm).Func("CallMenu",   CallMenu);


        //Sqrat::RootTable(vm).Func("LoadResource",   LoadResource);

        Sqrat::RootTable(vm).Func("Include",        Include);
        Sqrat::RootTable(vm).SquirrelFunc("Require",Require);

        Sqrat::RootTable(vm).Func("InfoWindow", InfoWindow::Display);

        Sqrat::RootTable(vm).SquirrelFunc("IsNull",IsNull);
    }
}
