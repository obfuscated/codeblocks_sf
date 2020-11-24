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
    #include "scriptingmanager.h"
    #include "cbeditor.h"
    #include "cbexception.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "logmanager.h"
    #include "manager.h"

    #include <settings.h>
    #include <wx/msgdlg.h>
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/regex.h>
#endif

#include "crc32.h"
#include "menuitemsmanager.h"
#include "genericmultilinenotesdlg.h"
#include "sc_plugin.h"

#include "squirrel.h"
#include "sqstdaux.h"
#include "sqstdblob.h"
#include "sqstdmath.h"
#include "sqstdstring.h"

template<> ScriptingManager* Mgr<ScriptingManager>::instance = nullptr;
template<> bool  Mgr<ScriptingManager>::isShutdown = false;

static wxString s_ScriptOutput;
static wxString s_ScriptErrors;
static wxString capture;

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

static void ScriptsPrintFunc(HSQUIRRELVM /*v*/, const SQChar * s, ...)
{
    va_list vl;
    va_start(vl,s);
    wxString msg;
    PrintSquirrelToWxString(msg,s,vl);
    va_end(vl);

    s_ScriptOutput << msg;
}

static void ScriptsErrorFunc(HSQUIRRELVM /*v*/, const SQChar * s, ...)
{
    va_list vl;
    va_start(vl,s);
    wxString msg;
    PrintSquirrelToWxString(msg,s,vl);
    va_end(vl);

    s_ScriptErrors << msg;
}

static void CaptureScriptOutput(HSQUIRRELVM /*v*/, const SQChar * s, ...)
{
    va_list vl;
    va_start(vl,s);
    wxString msg;
    PrintSquirrelToWxString(msg,s,vl);
    ::capture.append(msg);
    va_end(vl);
}

BEGIN_EVENT_TABLE(ScriptingManager, wxEvtHandler)
//
END_EVENT_TABLE()

namespace ScriptBindings
{
    void RegisterBindings(HSQUIRRELVM vm);
    void UnregisterBindings(HSQUIRRELVM vm);
} // namespace ScriptBindings

ScriptingManager::ScriptingManager()
    : m_AttachedToMainWindow(false),
    m_MenuItemsManager(false) // not auto-clear
{
    m_vm = sq_open(1024);
    if (m_vm == nullptr)
        cbThrow(_T("Can't create scripting engine!"));

    // FIXME (squirrel) Provide special error function?
    sq_setprintfunc(m_vm, ScriptsPrintFunc, ScriptsErrorFunc);

    sq_pushroottable(m_vm);
    sqstd_register_bloblib(m_vm);
    sqstd_register_mathlib(m_vm);
    sqstd_register_stringlib(m_vm);
    sqstd_seterrorhandlers(m_vm);
    sq_pop(m_vm, 1);

    RefreshTrusts();

    // register types
    ScriptBindings::RegisterBindings(m_vm);
}

ScriptingManager::~ScriptingManager()
{
    //dtor
    // save trusted scripts set
    ConfigManagerContainer::StringToStringMap myMap;
    int i = 0;
    TrustedScripts::iterator it;
    for (it = m_TrustedScripts.begin(); it != m_TrustedScripts.end(); ++it)
    {
        if (!it->second.permanent)
            continue;
        wxString key = wxString::Format(_T("trust%d"), i++);
        wxString value = wxString::Format(_T("%s?%x"), it->first.c_str(), it->second.crc);
        myMap.insert(myMap.end(), std::make_pair(key, value));
    }
    Manager::Get()->GetConfigManager(_T("security"))->Write(_T("/trusted_scripts"), myMap);

    if (m_vm)
    {
        ScriptBindings::UnregisterBindings(m_vm);

        sq_close(m_vm);
        m_vm = nullptr;
    }
}

HSQUIRRELVM ScriptingManager::GetVM()
{
    return m_vm;
}

bool ScriptingManager::LoadScript(const wxString& filename)
{
//    wxCriticalSectionLocker c(cs);

    wxLogNull ln; // own error checking implemented -> avoid debug warnings

    wxString fname(filename);
    wxFile f(fname); // try to open
    if (!f.IsOpened())
    {
        bool found = false;

        // check in same dir as currently running script (if any)
        if (!m_RunningScriptFileStack.empty())
        {
            const wxString& currentlyRunningScriptFile = m_RunningScriptFileStack.back();
            if (!currentlyRunningScriptFile.IsEmpty())
            {
                fname = wxFileName(currentlyRunningScriptFile).GetPath() + _T('/') + filename;
                f.Open(fname);
                found = f.IsOpened();
            }
        }

        if (!found)
        {
            // check in standard script dirs
            fname = ConfigManager::LocateDataFile(filename, sdScriptsUser | sdScriptsGlobal);
            f.Open(fname);
            if (!f.IsOpened())
            {
                Manager::Get()->GetLogManager()->DebugLog(_T("Can't open script ") + filename);
                return false;
            }
        }
    }
    // read file
    wxString contents = cbReadFileContents(f);
    m_RunningScriptFileStack.push_back(fname);
    bool ret = LoadBuffer(contents, fname);
    m_RunningScriptFileStack.pop_back();
    return ret;
}

static wxString ExtractLastSquirrelError(HSQUIRRELVM vm, bool canBeEmpty)
{
    const SQChar *s;
    sq_getlasterror(vm);
    sq_getstring(vm, -1, &s);
    wxString errorMsg;
    if (s)
        errorMsg = wxString(s);
    else if (!canBeEmpty)
        errorMsg = "Unknown error!";
    sq_pop(vm, 1);
    return errorMsg;
}

bool ScriptingManager::LoadBuffer(const wxString& buffer, const wxString& debugName)
{
    // includes guard to avoid recursion
    wxString incName = UnixFilename(debugName);
    if (m_IncludeSet.find(incName) != m_IncludeSet.end())
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Ignoring Include(\"%s\") because it would cause recursion..."), incName.wx_str()));
        return true;
    }
    m_IncludeSet.insert(incName);

//    wxCriticalSectionLocker c(cs);

    s_ScriptOutput.Clear();
    s_ScriptErrors.Clear();

    const wxScopedCharBuffer &utf8Buffer = buffer.utf8_str();

    if (SQ_FAILED(sq_compilebuffer(m_vm, utf8Buffer.data(),
                                   utf8Buffer.length() * sizeof(SQChar),
                                   cbU2C(debugName), 1)))
    {
        const wxString errorMsg = ExtractLastSquirrelError(m_vm, false);
        const wxString fullMessage = wxString::Format("Filename: %s\nError: %s\nDetails: %s",
                                                      debugName.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptErrors.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);
        m_IncludeSet.erase(incName);
        return false;
    }

    sq_pushroottable(m_vm);
    if (SQ_FAILED(sq_call(m_vm, 1, SQFalse, SQTrue)))
    {
        const wxString errorMsg = ExtractLastSquirrelError(m_vm, false);
        const wxString fullMessage = wxString::Format("Filename: %s\nError: %s\nDetails: %s",
                                                      debugName.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptErrors.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);

        m_IncludeSet.erase(incName);
        return false;
    }

    sq_pop(m_vm, 1);

    m_IncludeSet.erase(incName);
    return true;
}


wxString ScriptingManager::LoadBufferRedirectOutput(const wxString& buffer)
{
//    wxCriticalSectionLocker c(cs);

    s_ScriptOutput.Clear();
    s_ScriptErrors.Clear();
    ::capture.Clear();

    // Save the old used print function so we can restore it after the
    // redirected print is finished. This is needed for example if the
    // scripting console redirects the script print output to itself and
    // not the default print function of the ScriptingManager.
    // In this case we have to restore the print function after the call to
    // the scripting console.
    const SQPRINTFUNCTION oldPrintFunc = sq_getprintfunc(m_vm);
    const SQPRINTFUNCTION oldErrorFunc = sq_geterrorfunc(m_vm);

    // redirect the print output to an internal buffer, so we can collect
    // the print output
    sq_setprintfunc(m_vm, CaptureScriptOutput, CaptureScriptOutput);

    // Run the script
    bool res = LoadBuffer(buffer);

    // restore the old print function
    sq_setprintfunc(m_vm, oldPrintFunc, oldErrorFunc);
    // return the internal print buffer if the script executed successfully
    return res ? ::capture : (wxString) wxEmptyString;
}

wxString ScriptingManager::GetErrorString(bool clearErrors)
{
    wxString msg = ExtractLastSquirrelError(m_vm, true);
    if (!msg.empty())
        msg << "\n";
    msg << s_ScriptErrors;

    if (clearErrors)
        s_ScriptErrors.Clear();

    return msg;
}

void ScriptingManager::DisplayErrors(bool clearErrors)
{
    wxString msg = GetErrorString(clearErrors);
    if (!msg.IsEmpty())
    {
        if (cbMessageBox(_("Script errors have occured...\nPress 'Yes' to see the exact errors."),
                            _("Script errors"),
                            wxICON_ERROR | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
        {
            GenericMultiLineNotesDlg dlg(Manager::Get()->GetAppWindow(),
                                        _("Script errors"),
                                        msg,
                                        true);
            PlaceWindow(&dlg);
            dlg.ShowModal();
        }
    }
}

int ScriptingManager::Configure()
{
    return -1;
}

bool ScriptingManager::RegisterScriptPlugin(const wxString& /*name*/, const wxArrayInt& ids)
{
    // attach this event handler in the main window (one-time run)
    if (!m_AttachedToMainWindow)
    {
        Manager::Get()->GetAppWindow()->PushEventHandler(this);
        m_AttachedToMainWindow = true;
    }

    for (size_t i = 0; i < ids.GetCount(); ++i)
    {
        Connect(ids[i], -1, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &ScriptingManager::OnScriptPluginMenu);
    }
    return true;
}

bool ScriptingManager::RegisterScriptMenu(const wxString& menuPath, const wxString& scriptOrFunc, bool isFunction)
{
    // attach this event handler in the main window (one-time run)
    if (!m_AttachedToMainWindow)
    {
        Manager::Get()->GetAppWindow()->PushEventHandler(this);
        m_AttachedToMainWindow = true;
    }

    int id = wxNewId();
    id = m_MenuItemsManager.CreateFromString(menuPath, id);
    wxMenuItem* item = Manager::Get()->GetAppFrame()->GetMenuBar()->FindItem(id);
    if (item)
    {
        if (!isFunction)
            item->SetHelp(_("Press SHIFT while clicking this menu item to edit the assigned script in the editor"));

        Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                &ScriptingManager::OnScriptMenu);

        MenuBoundScript mbs;
        mbs.scriptOrFunc = scriptOrFunc;
        mbs.isFunc = isFunction;
        m_MenuIDToScript.insert(m_MenuIDToScript.end(), std::make_pair(id, mbs));
        #if wxCHECK_VERSION(3, 0, 0)
        Manager::Get()->GetLogManager()->Log(F(_("Script/function '%s' registered under menu '%s'"), scriptOrFunc.wx_str(), menuPath.wx_str()));
        #else
        Manager::Get()->GetLogManager()->Log(F(_("Script/function '%s' registered under menu '%s'"), scriptOrFunc.c_str(), menuPath.c_str()));
        #endif

        return true;
    }

    Manager::Get()->GetLogManager()->Log(_("Error registering script menu: ") + menuPath);
    return false;
}

bool ScriptingManager::UnRegisterScriptMenu(cb_unused const wxString& menuPath)
{
    // TODO: not implemented
    Manager::Get()->GetLogManager()->DebugLog(_T("ScriptingManager::UnRegisterScriptMenu() not implemented"));
    return false;
}

bool ScriptingManager::UnRegisterAllScriptMenus()
{
    m_MenuItemsManager.Clear();
    return true;
}

bool ScriptingManager::IsScriptTrusted(const wxString& script)
{
    TrustedScripts::iterator it = m_TrustedScripts.find(script);
    if (it == m_TrustedScripts.end())
        return false;
    // check the crc too
    wxUint32 crc = wxCrc32::FromFile(script);
    if (crc == it->second.crc)
        return true;
    cbMessageBox(script + _T("\n\n") + _("The script was marked as \"trusted\" but it has been modified "
                    "since then.\nScript not trusted anymore."),
                _("Warning"), wxICON_WARNING);
    m_TrustedScripts.erase(it);
    return false;
}

bool ScriptingManager::IsCurrentlyRunningScriptTrusted()
{
    if (m_RunningScriptFileStack.empty())
        return false;

    return IsScriptTrusted(m_RunningScriptFileStack.back());
}

void ScriptingManager::TrustScript(const wxString& script, bool permanently)
{
    // TODO: what should happen when script is empty()?

    TrustedScripts::iterator it = m_TrustedScripts.find(script);
    if (it != m_TrustedScripts.end())
    {
        // already trusted, remove it from the trusts (we recreate the trust below)
        m_TrustedScripts.erase(it);
    }

    TrustedScriptProps props;
    props.permanent = permanently;
    props.crc = wxCrc32::FromFile(script);

    m_TrustedScripts.insert(m_TrustedScripts.end(), std::make_pair(script, props));
}

void ScriptingManager::TrustCurrentlyRunningScript(bool permanently)
{
    if (!m_RunningScriptFileStack.empty())
    {
        const wxString currentlyRunningScriptFile = m_RunningScriptFileStack.back();
        TrustScript(currentlyRunningScriptFile, permanently);
    }
}

bool ScriptingManager::RemoveTrust(const wxString& script)
{
    TrustedScripts::iterator it = m_TrustedScripts.find(script);
    if (it != m_TrustedScripts.end())
    {
        // already trusted, remove it from the trusts (we recreate the trust below)
        m_TrustedScripts.erase(it);
        return true;
    }
    return false;
}

void ScriptingManager::RefreshTrusts()
{
    // reload trusted scripts set
    m_TrustedScripts.clear();
    ConfigManagerContainer::StringToStringMap myMap;
    Manager::Get()->GetConfigManager(_T("security"))->Read(_T("/trusted_scripts"), &myMap);
    ConfigManagerContainer::StringToStringMap::iterator it;
    for (it = myMap.begin(); it != myMap.end(); ++it)
    {
        wxString key = it->second.BeforeFirst(_T('?'));
        wxString value = it->second.AfterFirst(_T('?'));

        TrustedScriptProps props;
        props.permanent = true;
        unsigned long tmp;
        value.ToULong(&tmp, 16);
        props.crc = tmp;
        m_TrustedScripts.insert(m_TrustedScripts.end(), std::make_pair(key, props));
    }
}

const ScriptingManager::TrustedScripts& ScriptingManager::GetTrustedScripts()
{
    return m_TrustedScripts;
}

void ScriptingManager::OnScriptMenu(wxCommandEvent& event)
{
    MenuIDToScript::iterator it = m_MenuIDToScript.find(event.GetId());
    if (it == m_MenuIDToScript.end())
    {
        cbMessageBox(_("No script associated with this menu?!?"), _("Error"), wxICON_ERROR);
        return;
    }

    MenuBoundScript& mbs = it->second;

// FIXME (squirrel) Reimplement OnScriptMenu 1
/*
    // is it a function?
    if (mbs.isFunc)
    {
        try
        {
            SqPlus::SquirrelFunction<void> f(cbU2C(mbs.scriptOrFunc));
            f();
        }
        catch (SquirrelError exception)
        {
            DisplayErrors(&exception);
        }
        return;
    }
*/
    // script loading below

    if (wxGetKeyState(WXK_SHIFT))
    {
        wxString script = ConfigManager::LocateDataFile(mbs.scriptOrFunc, sdScriptsUser | sdScriptsGlobal);
        Manager::Get()->GetEditorManager()->Open(script);
        return;
    }

// FIXME (squirrel) Reimplement OnScriptMenu 2
/*
    // run script
    try
    {
        if (!LoadScript(mbs.scriptOrFunc))
            cbMessageBox(_("Could not run script: ") + mbs.scriptOrFunc, _("Error"), wxICON_ERROR);
    }
    catch (SquirrelError exception)
    {
        DisplayErrors(&exception);
    }
*/
}

void ScriptingManager::OnScriptPluginMenu(wxCommandEvent& event)
{
    ScriptBindings::ScriptPluginWrapper::OnScriptMenu(event.GetId());
}
