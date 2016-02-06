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
#endif

#include "sc_base_types.h"

#include <wx/colordlg.h>
#include <wx/numdlg.h>
#include <infowindow.h>

namespace ScriptBindings
{
    // global funcs
    void gDebugLog(const wxString& msg){ Manager::Get()->GetLogManager()->DebugLog(msg); }
    void gErrorLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogError(msg); }
    void gWarningLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogWarning(msg); }
    void gLog(const wxString& msg){ Manager::Get()->GetLogManager()->Log(msg); }
    int gMessage(const wxString& msg, const wxString& caption, int buttons){ return cbMessageBox(msg, caption, buttons); }
    void gShowMessage(const wxString& msg){ cbMessageBox(msg, _("Script message"), wxICON_INFORMATION | wxOK); }
    void gShowMessageWarn(const wxString& msg){ cbMessageBox(msg, _("Script warning"), wxICON_WARNING | wxOK); }
    void gShowMessageError(const wxString& msg){ cbMessageBox(msg, _("Script error"), wxICON_ERROR | wxOK); }
    void gShowMessageInfo(const wxString& msg){ cbMessageBox(msg, _("Script information"), wxICON_INFORMATION | wxOK); }
    wxString gReplaceMacros(const wxString& buffer){ return Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer); }

    SQInteger IsNull(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        SQUserPointer up = nullptr;
        sq_getinstanceup(v, 2, &up, nullptr);
        return sa.Return(up == nullptr);
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
        return 0; /* leaving script binding intact for compatibility, but this is factually not implemented at all */
    }
    // locate and call a menu from string (e.g. "/Valgrind/Run Valgrind::MemCheck")
    void CallMenu(const wxString& menuPath)
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
                        if ( !mbar->ProcessEvent(evt) )
                        {
                            wxString msg; msg.Printf(_("Calling the menu '%s' with ID %d failed."), menuPath.wx_str(), id);
                            cbMessageBox(msg, _("Script error"), wxICON_WARNING);
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
                    break; // failed
            }
            pos = nextPos; // prepare for next loop
        }
    }
    void Include(const wxString& filename)
    {
        getSM()->LoadScript(filename);
    }
    SQInteger Require(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        const wxString& filename = *SqPlus::GetInstance<wxString,false>(v, 2);
        if (!getSM()->LoadScript(filename))
        {
            wxString msg = wxString::Format(_("Failed to load required script: %s"), filename.c_str());
            return sa.ThrowError(cbU2C(msg));
        }
        return sa.Return(static_cast<SQInteger>(0));
    }
    SQInteger wx_GetColourFromUser(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        const wxColour& c = sa.GetParamCount() == 2 ? *SqPlus::GetInstance<wxColour,false>(v, 2) : *wxBLACK;
        return SqPlus::ReturnCopy(v, wxGetColourFromUser(Manager::Get()->GetAppWindow(), c));
    }
    long wx_GetNumberFromUser(const wxString& message, const wxString& prompt, const wxString& caption, long value)
    {
        return wxGetNumberFromUser(message, prompt, caption, value);
    }
    wxString wx_GetPasswordFromUser(const wxString& message, const wxString& caption, const wxString& default_value)
    {
        return wxGetPasswordFromUser(message, caption, default_value);
    }
    wxString wx_GetTextFromUser(const wxString& message, const wxString& caption, const wxString& default_value)
    {
        return cbGetTextFromUser(message, caption, default_value);
    }

    long wxString_ToLong(wxString const &str)
    {
        long value;
        if(!str.ToLong(&value))
            return -1;
        return value;
    }


    void Register_Globals()
    {
        // global funcs
        SqPlus::RegisterGlobal(gLog, "Log");
        SqPlus::RegisterGlobal(gDebugLog, "LogDebug");
        SqPlus::RegisterGlobal(gWarningLog, "LogWarning");
        SqPlus::RegisterGlobal(gErrorLog, "LogError");

        SqPlus::RegisterGlobal(gMessage, "Message");
        SqPlus::RegisterGlobal(gShowMessage, "ShowMessage");
        SqPlus::RegisterGlobal(gShowMessageWarn, "ShowWarning");
        SqPlus::RegisterGlobal(gShowMessageError, "ShowError");
        SqPlus::RegisterGlobal(gShowMessageInfo, "ShowInfo");
        SqPlus::RegisterGlobal(gReplaceMacros, "ReplaceMacros");

        SqPlus::RegisterGlobal(getPM, "GetProjectManager");
        SqPlus::RegisterGlobal(getEM, "GetEditorManager");
        SqPlus::RegisterGlobal(getCM, "GetConfigManager");
        SqPlus::RegisterGlobal(getUVM, "GetUserVariableManager");
        SqPlus::RegisterGlobal(getSM, "GetScriptingManager");
        SqPlus::RegisterGlobal(getCF, "GetCompilerFactory");

        // from globals.h
        SqPlus::RegisterGlobal(GetArrayFromString, "GetArrayFromString");
        SqPlus::RegisterGlobal(GetStringFromArray, "GetStringFromArray");
        SqPlus::RegisterGlobal(EscapeSpaces, "EscapeSpaces");
        SqPlus::RegisterGlobal(UnixFilename, "UnixFilename");
        SqPlus::RegisterGlobal(FileTypeOf, "FileTypeOf");
        SqPlus::RegisterGlobal(URLEncode, "URLEncode");
        SqPlus::RegisterGlobal(NotifyMissingFile, "NotifyMissingFile");
        SqPlus::RegisterGlobal(GetPlatformsFromString, "GetPlatformsFromString");
        SqPlus::RegisterGlobal(GetStringFromPlatforms, "GetStringFromPlatforms");

        SqPlus::RegisterGlobal(ConfigManager::GetFolder, "GetFolder");
        SqPlus::RegisterGlobal(ConfigManager::LocateDataFile, "LocateDataFile");

        SqPlus::RegisterGlobal(ExecutePlugin, "ExecuteToolPlugin");
        SqPlus::RegisterGlobal(ConfigurePlugin, "ConfigureToolPlugin");
        SqPlus::RegisterGlobal(InstallPlugin, "InstallPlugin");

        SqPlus::RegisterGlobal(CallMenu, "CallMenu");

        SqPlus::RegisterGlobal(Include, "Include");
        SquirrelVM::CreateFunctionGlobal(Require, "Require", "*");

        SqPlus::RegisterGlobal(InfoWindow::Display, "InfoWindow");

        SquirrelVM::CreateFunctionGlobal(IsNull, "IsNull", "*");

        // now for some wx globals (utility) functions
        SqPlus::RegisterGlobal(wxLaunchDefaultBrowser, "wxLaunchDefaultBrowser");
        SquirrelVM::CreateFunctionGlobal(wx_GetColourFromUser, "wxGetColourFromUser", "*");
        SqPlus::RegisterGlobal(wx_GetNumberFromUser, "wxGetNumberFromUser");
        SqPlus::RegisterGlobal(wx_GetPasswordFromUser, "wxGetPasswordFromUser");
        SqPlus::RegisterGlobal(wx_GetTextFromUser, "wxGetTextFromUser");

        SqPlus::RegisterGlobal(wxString_ToLong, "wxString_ToLong");
    }
}
