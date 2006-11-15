#include <sdk_precomp.h>
#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include <globals.h>
    #include <settings.h>
    #include <manager.h>
    #include <messagemanager.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <projectmanager.h>
    #include <pluginmanager.h>
#endif

#include "sc_base_types.h"

namespace ScriptBindings
{
    // global funcs
    void gDebugLog(const wxString& msg){ DBGLOG(msg); }
    void gErrorLog(const wxString& msg){ LOG_ERROR(msg); }
    void gWarningLog(const wxString& msg){ LOG_WARN(msg); }
    void gLog(const wxString& msg){ LOG(msg); }
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
        SqPlus::RegisterGlobal(getCF, "GetCompilerFactory");

        SqPlus::RegisterGlobal(ConfigManager::GetFolder, "GetFolder");
        SqPlus::RegisterGlobal(ConfigManager::LocateDataFile, "LocateDataFile");
        SqPlus::RegisterGlobal(InstallPlugin, "InstallPlugin");

        SquirrelVM::CreateFunctionGlobal(IsNull, "IsNull", "*");
    }
}
