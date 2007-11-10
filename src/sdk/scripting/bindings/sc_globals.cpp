#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
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
#include <wx/textdlg.h>
#include <infowindow.h>

namespace ScriptBindings
{
    // global funcs
    void gDebugLog(const wxString& msg){ Manager::Get()->GetLogManager()->DebugLog(msg); }
    void gErrorLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogError(msg); }
    void gWarningLog(const wxString& msg){ Manager::Get()->GetLogManager()->LogWarning(msg); }
    void gLog(const wxString& msg){ Manager::Get()->GetLogManager()->Log(msg); }
    int gMessage(const wxString& msg, const wxString& caption, int buttons){ return cbMessageBox(msg, caption, buttons); }
    void gShowMessage(const wxString& msg){ cbMessageBox(msg, _("Script message")); }
    void gShowMessageWarn(const wxString& msg){ cbMessageBox(msg, _("Script warning"), wxICON_WARNING); }
    void gShowMessageError(const wxString& msg){ cbMessageBox(msg, _("Script error"), wxICON_ERROR); }
    void gShowMessageInfo(const wxString& msg){ cbMessageBox(msg, _("Script information"), wxICON_INFORMATION); }
    wxString gReplaceMacros(const wxString& buffer){ return Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer); }

    SQInteger IsNull(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        SQUserPointer up = 0;
        sq_getinstanceup(v, 2, &up, 0);
        return sa.Return(up == 0L);
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
    void Include(const wxString& filename)
    {
        getSM()->LoadScript(filename);
    }
    SQInteger Require(HSQUIRRELVM v)
    {
        StackHandler sa(v);
        const wxString& filename = *SqPlus::GetInstance<wxString>(v, 2);
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
        const wxColour& c = sa.GetParamCount() == 2 ? *SqPlus::GetInstance<wxColour>(v, 2) : *wxBLACK;
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
        return wxGetTextFromUser(message, caption, default_value);
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
        SqPlus::RegisterGlobal(InstallPlugin, "InstallPlugin");

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
    }
}
