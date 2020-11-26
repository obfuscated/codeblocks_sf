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
#endif // CB_PRECOMP

#include "sc_utils.h"
#include "sc_typeinfo_all.h"

#include <wx/numdlg.h>

namespace ScriptBindings
{
    void gDebugLog(const wxString& msg){ Manager::Get()->GetLogManager()->DebugLog(msg); }
    void gErrorLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogError(msg); }
    void gWarningLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogWarning(msg); }
    void gLog(const wxString& msg){ Manager::Get()->GetLogManager()->Log(msg); }

    void gShowMessage(const wxString& msg)
    {
        cbMessageBox(msg, _("Script message"), wxICON_INFORMATION | wxOK);
    }
    void gShowMessageWarn(const wxString& msg)
    {
        cbMessageBox(msg, _("Script warning"), wxICON_WARNING | wxOK);
    }
    void gShowMessageError(const wxString& msg)
    {
        cbMessageBox(msg, _("Script error"), wxICON_ERROR | wxOK);
    }
    void gShowMessageInfo(const wxString& msg)
    {
        cbMessageBox(msg, _("Script information"), wxICON_INFORMATION | wxOK);
    }

    template<void (*func)(const wxString &)>
    SQInteger NoReturnSingleWxStringParam(HSQUIRRELVM v)
    {
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("NoReturnSingleWxStringParam"))
            return extractor.ErrorMessage();
        func(*extractor.p1);
        return 0;
    }

    SQInteger MessageBoxFunc(HSQUIRRELVM v)
    {
        // env table, msg, caption, buttons
        ExtractParams4<SkipParam, const wxString *, const wxString *, SQInteger> extractor(v);
        if (!extractor.Process("MessageBoxFunc"))
            return extractor.ErrorMessage();
        const int result = cbMessageBox(*extractor.p1, *extractor.p2, extractor.p3);
        sq_pushinteger(v, result);
        return 1;
    }

    SQInteger gReplaceMacros(HSQUIRRELVM v)
    {
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("ReplaceMacros"))
            return extractor.ErrorMessage();

        const wxString &result = Manager::Get()->GetMacrosManager()->ReplaceMacros(*extractor.p1);
        return ConstructAndReturnInstance(v, result);
    }

    template<typename ManagerType, ManagerType* (Manager::*func)() const>
    SQInteger GetManager(HSQUIRRELVM v)
    {
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(1, "GetManager"))
            return extractor.ErrorMessage();
        ManagerType *manager = (Manager::Get()->*func)();
        return ConstructAndReturnNonOwnedPtr(v, manager);
    }

    SQInteger GetCM(HSQUIRRELVM v)
    {
        ExtractParams1<SkipParam> extractor(v);
        if (!extractor.Process("GetConfigManager"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, Manager::Get()->GetConfigManager(_T("scripts")));
    }

    SQInteger GetCF(HSQUIRRELVM v)
    {
        static CompilerFactory cf; // all its members are static functions anyway
        ExtractParams1<SkipParam> extractor(v);
        if (!extractor.Process("GetCompilerFactory"))
            return extractor.ErrorMessage();
        return ConstructAndReturnNonOwnedPtr(v, &cf);
    }

    SQInteger gGetArrayFromString(HSQUIRRELVM v)
    {
        // env table, text, separator, trim spaces
        ExtractParams4<SkipParam, const wxString *, const wxString *, bool> extractor(v);
        if (!extractor.Process("GetArrayFromString"))
            return extractor.ErrorMessage();

        const wxArrayString &result = GetArrayFromString(*extractor.p1, *extractor.p2,
                                                         extractor.p3);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger gGetStringFromArray(HSQUIRRELVM v)
    {
        // env table, array, separator, separator at end
        ExtractParams4<SkipParam, const wxArrayString *, const wxString *, bool> extractor(v);
        if (!extractor.Process("GetStringFromArray"))
            return extractor.ErrorMessage();

        const wxString &result = GetStringFromArray(*extractor.p1, *extractor.p2, extractor.p3);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger gEscapeSpaces(HSQUIRRELVM v)
    {
        // env table, str
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("EscapeSpaces"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, EscapeSpaces(*extractor.p1));
    }

    SQInteger gUnixFilename(HSQUIRRELVM v)
    {
        // env table, filename, format
        ExtractParams3<SkipParam, const wxString *, SQInteger> extractor(v);
        if (!extractor.Process("UnixFilename"))
            return extractor.ErrorMessage();

        if (extractor.p2 < wxPATH_NATIVE || extractor.p2 >= wxPATH_MAX)
            return sq_throwerror(v, _SC("UnixFilename: format out of range!"));

        return ConstructAndReturnInstance(v, UnixFilename(*extractor.p1,
                                                          wxPathFormat(extractor.p2)));
    }

    SQInteger gFileTypeOf(HSQUIRRELVM v)
    {
        // env table, filename
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("FileTypeOf"))
            return extractor.ErrorMessage();
        // FIXME: (squirrel) This returns int, but it could return an enum!
        sq_pushinteger(v, FileTypeOf(*extractor.p1));
        return 1;
    }

    SQInteger gURLEncode(HSQUIRRELVM v)
    {
        // env table, str
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("URLEncode"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, URLEncode(*extractor.p1));
    }

    SQInteger gGetPlatformsFromString(HSQUIRRELVM v)
    {
        // env table, platforms
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("GetPlatformsFromString"))
            return extractor.ErrorMessage();
        // FIXME: (squirrel) This returns int, but it could return an enum!
        sq_pushinteger(v, GetPlatformsFromString(*extractor.p1));
        return 1;
    }

    SQInteger gGetStringFromPlatforms(HSQUIRRELVM v)
    {
        // env table, platforms, force separate
        ExtractParams3<SkipParam, SQInteger, bool> extractor(v);
        if (!extractor.Process("GetStringFromPlatforms"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, GetStringFromPlatforms(extractor.p1, extractor.p2));
    }

    SQInteger ConfigManager_GetFolder(HSQUIRRELVM v)
    {
        // env table, dir
        ExtractParams2<SkipParam, SQInteger> extractor(v);
        if (!extractor.Process("GetFolder"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, ConfigManager::GetFolder(SearchDirs(extractor.p1)));
    }

    SQInteger ConfigManager_LocateDataFile(HSQUIRRELVM v)
    {
        // env table, filename, dir
        ExtractParams3<SkipParam, const wxString *, SQInteger> extractor(v);
        if (!extractor.Process("LocateDataFile"))
            return extractor.ErrorMessage();

        return ConstructAndReturnInstance(v, ConfigManager::LocateDataFile(*extractor.p1,
                                                                           SearchDirs(extractor.p2)));
    }

    SQInteger ExecutePlugin(HSQUIRRELVM v)
    {
        // env table, pluginName
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("ExecutePlugin"))
            return extractor.ErrorMessage();
        sq_pushinteger(v, Manager::Get()->GetPluginManager()->ExecutePlugin(*extractor.p1));
        return 1;
    }

    SQInteger InstallPlugin(HSQUIRRELVM v)
    {
        // env table, pluginName, allUsers, confirm
        ExtractParams4<SkipParam, const wxString *, bool, bool> extractor(v);
        if (!extractor.Process("InstallPlugin"))
            return extractor.ErrorMessage();

        const wxString &pluginName = *extractor.p1;
        if (cbMessageBox(_("A script is trying to install a Code::Blocks plugin.\n"
                           "Do you wish to allow this?\n\n") + pluginName,
                        _("Security warning"), wxICON_WARNING | wxYES_NO) == wxID_NO)
        {
            return false;
        }

        const bool result = Manager::Get()->GetPluginManager()->InstallPlugin(pluginName,
                                                                              extractor.p2,
                                                                              extractor.p3);
        sq_pushbool(v, result);
        return 1;
    }

    SQInteger ConfigurePlugin(HSQUIRRELVM v)
    {
        // Leaving script binding intact for compatibility, but this is factually not implemented at
        // all.
        sq_pushinteger(v, 0);
        return 1;
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

    SQInteger Include(HSQUIRRELVM v)
    {
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("Include"))
            return extractor.ErrorMessage();

        ScriptingManager *sm = Manager::Get()->GetScriptingManager();
        if (!sm->LoadScript(*extractor.p1))
        {
            wxString msg = wxString::Format(_("Include: Failed to load required script: '%s'"),
                                            extractor.p1->wx_str());
            return sq_throwerror(v, cbU2C(msg));
        }

        return 0;
    }
    SQInteger Require(HSQUIRRELVM v)
    {
        ExtractParams2<SkipParam, const wxString *> extractor(v);
        if (!extractor.Process("Require"))
            return extractor.ErrorMessage();

        ScriptingManager *sm = Manager::Get()->GetScriptingManager();
        if (sm->LoadScript(*extractor.p1))
        {
            sq_pushinteger(v, 0);
            return 1;
        }
        else
        {
            wxString msg = wxString::Format(_("Require: Failed to load required script: '%s'"),
                                            extractor.p1->wx_str());
            return sq_throwerror(v, cbU2C(msg));
        }
    }

    SQInteger InfoWindow_Display(HSQUIRRELVM v)
    {
        // env table, title, message, delay, hysteresis
        ExtractParams5<SkipParam, const wxString *, const wxString *, SQInteger, SQInteger> extractor(v);
        if (!extractor.Process("InfoWindow::Display"))
            return extractor.ErrorMessage();

        InfoWindow::Display(*extractor.p1, *extractor.p2, extractor.p3, extractor.p4);
        return 0;
    }

    SQInteger IsNull(HSQUIRRELVM v)
    {
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(2, "IsNull"))
            return extractor.ErrorMessage();
        SQUserPointer up = nullptr;
        sq_getinstanceup(v, 2, &up, nullptr);
        sq_pushbool(v, (up == nullptr));
        return 1;
    }

    SQInteger gWxLaunchDefaultBrowser(HSQUIRRELVM v)
    {
        // env table, url, flags
        ExtractParams3<SkipParam, const wxString *, SQInteger> extractor(v);
        if (!extractor.Process("wxLaunchDefaultBrowser"))
            return extractor.ErrorMessage();

        sq_pushbool(v, wxLaunchDefaultBrowser(*extractor.p1, extractor.p2));
        return 1;
    }

    SQInteger gWxGetColourFromUser(HSQUIRRELVM v)
    {
        // env table, colInit (optional)
        ExtractParamsBase extractor(v);
        if (!extractor.CheckNumArguments(1, 2, "wxGetColourFromUser"))
            return extractor.ErrorMessage();

        const wxColour *initColour = wxBLACK;
        const int numArgs = sq_gettop(v);
        if (numArgs == 2)
        {
            if (!extractor.ProcessParam(initColour, 2, "wxGetColourFromUser"))
                return extractor.ErrorMessage();
        }

        const wxColour &result = wxGetColourFromUser(Manager::Get()->GetAppWindow(), *initColour);
        return ConstructAndReturnInstance(v, result);
    }

    SQInteger gWxGetNumberFromUser(HSQUIRRELVM v)
    {
        // env table, message, prompt, caption, value
        ExtractParams5<SkipParam, const wxString *, const wxString *, const wxString *, SQInteger> extractor(v);
        if (!extractor.Process("wxGetNumberFromUser"))
            return extractor.ErrorMessage();

        sq_pushinteger(v, wxGetNumberFromUser(*extractor.p1, *extractor.p2, *extractor.p3,
                                              extractor.p4));
        return 1;
    }

    wxString wx_GetPasswordFromUser(const wxString& message, const wxString& caption,
                                    const wxString& default_value)
    {
        return wxGetPasswordFromUser(message, caption, default_value);
    }
    wxString wx_GetTextFromUser(const wxString& message, const wxString& caption,
                                const wxString& default_value)
    {
        return cbGetTextFromUser(message, caption, default_value);
    }

    template <wxString (*func)(const wxString&, const wxString &, const wxString&)>
    SQInteger gWxGetTextFromUser(HSQUIRRELVM v)
    {
        // env table, message, caption, default_value
        ExtractParams4<SkipParam, const wxString *, const wxString *, const wxString *> extractor(v);
        if (!extractor.Process("gWxGetTextFromUser"))
            return extractor.ErrorMessage();

        const wxString &result = func(*extractor.p1, *extractor.p2, *extractor.p3);
        return ConstructAndReturnInstance(v, result);
    }

    void Register_Globals(HSQUIRRELVM v)
    {
        PreserveTop preserve(v);

        sq_pushroottable(v);

        BindMethod(v, _SC("Log"), NoReturnSingleWxStringParam<gLog>, nullptr);
        BindMethod(v, _SC("LogDebug"), NoReturnSingleWxStringParam<&gDebugLog>, nullptr);
        BindMethod(v, _SC("LogWarning"), NoReturnSingleWxStringParam<&gWarningLog>, nullptr);
        BindMethod(v, _SC("LogError"), NoReturnSingleWxStringParam<&gErrorLog>, nullptr);

        BindMethod(v, _SC("Message"), MessageBoxFunc, nullptr);
        BindMethod(v, _SC("ShowMessage"), NoReturnSingleWxStringParam<&gShowMessage>, nullptr);
        BindMethod(v, _SC("ShowWarning"), NoReturnSingleWxStringParam<&gShowMessageWarn>, nullptr);
        BindMethod(v, _SC("ShowError"), NoReturnSingleWxStringParam<&gShowMessageError>, nullptr);
        BindMethod(v, _SC("ShowInfo"), NoReturnSingleWxStringParam<&gShowMessageInfo>, nullptr);

        BindMethod(v, _SC("ReplaceMacros"), gReplaceMacros, nullptr);

        BindMethod(v, _SC("GetProjectManager"),
                   GetManager<ProjectManager, &Manager::GetProjectManager>, nullptr);
        BindMethod(v, _SC("GetEditorManager"),
                   GetManager<EditorManager, &Manager::GetEditorManager>, nullptr);
        BindMethod(v, _SC("GetConfigManager"), GetCM, nullptr);
        BindMethod(v, _SC("GetUserVariableManager"),
                   GetManager<UserVariableManager, &Manager::GetUserVariableManager>, nullptr);
        BindMethod(v, _SC("GetScriptingManager"),
                   GetManager<ScriptingManager, &Manager::GetScriptingManager>, nullptr);
        BindMethod(v, _SC("GetCompilerFactory"), GetCF, nullptr);

        // from globals.h
        BindMethod(v, _SC("GetArrayFromString"), gGetArrayFromString, nullptr);
        BindMethod(v, _SC("GetStringFromArray"), gGetStringFromArray, nullptr);
        BindMethod(v, _SC("EscapeSpaces"), gEscapeSpaces, nullptr);
        BindMethod(v, _SC("UnixFilename"), gUnixFilename, nullptr);
        BindMethod(v, _SC("FileTypeOf"), gFileTypeOf, nullptr);
        BindMethod(v, _SC("URLEncode"), gURLEncode, nullptr);
        BindMethod(v, _SC("NotifyMissingFile"), NoReturnSingleWxStringParam<&NotifyMissingFile>, nullptr);
        BindMethod(v, _SC("GetPlatformsFromString"), gGetPlatformsFromString, nullptr);
        BindMethod(v, _SC("GetStringFromPlatforms"), gGetStringFromPlatforms, nullptr);

        BindMethod(v, _SC("GetFolder"), ConfigManager_GetFolder, nullptr);
        BindMethod(v, _SC("LocateDataFile"), ConfigManager_LocateDataFile, nullptr);

        BindMethod(v, _SC("ExecuteToolPlugin"), ExecutePlugin, nullptr);
        BindMethod(v, _SC("ConfigureToolPlugin"), ConfigurePlugin, nullptr);
        BindMethod(v, _SC("InstallPlugin"), InstallPlugin, nullptr);

        BindMethod(v, _SC("CallMenu"), NoReturnSingleWxStringParam<CallMenu>, nullptr);

        BindMethod(v, _SC("Include"), Include, nullptr);
        BindMethod(v, _SC("Require"), Require, nullptr);

        BindMethod(v, _SC("InfoWindow"), InfoWindow_Display, nullptr);
        BindMethod(v, _SC("IsNull"), IsNull, nullptr);

        // now for some wx globals (utility) functions
        BindMethod(v, _SC("wxLaunchDefaultBrowser"), gWxLaunchDefaultBrowser, nullptr);
        BindMethod(v, _SC("wxGetColourFromUser"), gWxGetColourFromUser, nullptr);
        BindMethod(v, _SC("wxGetNumberFromUser"), gWxGetNumberFromUser, nullptr);
        BindMethod(v, _SC("wxGetPasswordFromUser"), gWxGetTextFromUser<wx_GetPasswordFromUser>,
                   nullptr);
        BindMethod(v, _SC("wxGetTextFromUser"), gWxGetTextFromUser<wx_GetTextFromUser>, nullptr);

        sq_pop(v, 1); // root table
    }
}
