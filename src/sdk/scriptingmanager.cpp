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
    #include <wx/tokenzr.h>
    #include <wx/filesys.h>

#endif

#include "crc32.h"
#include "menuitemsmanager.h"
#include "genericmultilinenotesdlg.h"
#include "scripting/sqrat.h"
#include "scripting/bindings/scriptbindings.h"
#include "scripting/bindings/sc_plugin.h"
#include "scripting/squirrel/sqstdstring.h"

#include <wx/progdlg.h>


template<> ScriptingManager* Mgr<ScriptingManager>::instance = nullptr;
template<> bool  Mgr<ScriptingManager>::isShutdown = false;

static wxString s_ScriptErrors;
static wxString capture;

void PrintSquirrelToWxString(wxString& msg, const SQChar* s, va_list& vl)
{
    int buffer_size = 2048;
    SQChar* tmp_buffer;
    for(;;buffer_size*=2)
    {
        tmp_buffer = new SQChar [buffer_size];
        int retvalue = vsnprintf(tmp_buffer, buffer_size, s, vl);
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

static void CaptureScriptErrors(HSQUIRRELVM /*v*/, const SQChar * s, ...)
{
    va_list vl;
    va_start(vl,s);
    wxString msg;
    PrintSquirrelToWxString(msg,s,vl);
    va_end(vl);

    s_ScriptErrors << msg;
    Manager::Get()->GetLogManager()->LogError(_("Script error: ") + msg);
}

static void ScriptsPrintFunc(HSQUIRRELVM /*v*/, const SQChar * s, ...)
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

ScriptingManager::ScriptingManager()
    : m_AttachedToMainWindow(false),
    m_MenuItemsManager(false) // not auto-clear
{
    //ctor

    // initialize but don't load the IO lib
     m_vm = new ScriptBindings::CBsquirrelVM(1024,(ScriptBindings::VM_LIB_ALL & ~ScriptBindings::VM_LIB_IO));

    if (!m_vm->GetSqVM())
        cbThrow(_T("Can't create scripting engine!"));

    m_vm->SetPrintFunc(ScriptsPrintFunc,CaptureScriptErrors);
    m_vm->SetMeDefault();
    RefreshTrusts();

    // register types
    ScriptBindings::RegisterBindings(m_vm->GetSqVM());
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

    //Delete all Sqrat::Object elements, before the vm shuts down
    ScriptBindings::ScriptPluginWrapper::ClearPlugins();

    m_vm->Shutdown();
}

void ScriptingManager::RegisterScriptFunctions()
{
    // done in scriptbindings.cpp
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
        if (!m_CurrentlyRunningScriptFile.IsEmpty())
        {
            fname = wxFileName(m_CurrentlyRunningScriptFile).GetPath() + _T('/') + filename;
            f.Open(fname);
            found = f.IsOpened();
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
    wxString contents= cbReadFileContents(f,wxFONTENCODING_UTF8); // Script files are UTF8 encoded, on windows are problems with the automatic detection of the encoding
    m_CurrentlyRunningScriptFile = fname;
    bool ret = LoadBuffer(contents, fname,true);
    //m_CurrentlyRunningScriptFile.Clear(); // I'm not 100% sure if this is the correct approach to search for local scripts (and if we want to allow local scripts)
    return ret;
}

bool ScriptingManager::LoadBuffer(const wxString& buffer,wxString debugName,bool real_path)
{
    // includes guard to avoid recursion
    wxString incName = UnixFilename(debugName);
    if (m_IncludeSet.find(incName) != m_IncludeSet.end())
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_("Ignoring Include(\"%s\") because it would cause recursion..."), incName.wx_str()));
        return true;
    }
    m_IncludeSet.insert(incName);

//    wxCriticalSectionLocker c(cs);

    s_ScriptErrors.Clear();

    // compile script
    // Make sure that the directory has a valid name (don't mix slash and backslash)
    wxFileName tmp_name(debugName);
    debugName = tmp_name.GetFullPath();

    SC_ERROR_STATE ret = m_vm->doString(buffer,debugName);
    if(ret != SC_NO_ERROR)
    {
        DisplayErrors();
        m_IncludeSet.erase(incName);
        return false;
    }

    m_IncludeSet.erase(incName);
    return true;
}


wxString ScriptingManager::LoadBufferRedirectOutput(const wxString& buffer,const wxString& name)
{
//    wxCriticalSectionLocker c(cs);

    s_ScriptErrors.Clear();
    ::capture.Clear();

    // FIXME (bluehazzard#1#): Here is a absolute mess with the error handling...

    SQPRINTFUNCTION old_printFunc = nullptr;
    SQPRINTFUNCTION old_errorFunc = nullptr;

    m_vm->GetPrintFunc(old_printFunc,old_errorFunc);

    // Redirect the script output to the ::capture string
    m_vm->SetPrintFunc(CaptureScriptOutput,CaptureScriptErrors);

    bool res = LoadBuffer(buffer,name);

    // restore the old output
    m_vm->SetPrintFunc(old_printFunc,old_errorFunc);

    return res ? ::capture : (wxString) wxEmptyString;
}

wxString ScriptingManager::GetErrorString( bool clearErrors)
{
    //ScriptBindings::StackHandler sa(m_vm->GetVM());
    return m_vm->getLastErrorMsg();
    //return sa.GetError(clearErrors);
}

bool ScriptingManager::DisplayErrorsAndText(wxString pre_error, bool clearErrors)
{
    if(!m_vm->HasError())   // If no error return
        return false;

    pre_error.Append(GetErrorString(clearErrors));
    return DisplayErrors(pre_error,clearErrors);
}

bool ScriptingManager::DisplayErrors(wxString error_msg, bool clearErrors)
{

    if(error_msg == wxEmptyString)
    {
        if(!m_vm->HasError())   // If no error return
            return false;

        error_msg = GetErrorString(clearErrors);
    }


    if (!error_msg.IsEmpty())
    {
        if (cbMessageBox(_("Script errors have occurred...\nPress 'Yes' to see the exact errors."),
                            _("Script errors"),
                            wxICON_ERROR | wxYES_NO | wxNO_DEFAULT) == wxID_YES)
        {
            GenericMultiLineNotesDlg dlg(Manager::Get()->GetAppWindow(),
                                        _("Script errors"),
                                        error_msg,
                                        true);
            dlg.ShowModal();
        }
        Manager::Get()->GetLogManager()->DebugLog(_("Scripting error: ") + error_msg);
        return true;
    }
    return false;
}

void ScriptingManager::InjectScriptOutput(const wxString& output)
{
    s_ScriptErrors << output;
}

int ScriptingManager::Configure()
{
    return -1;
}

bool ScriptingManager::RegisterScriptPlugin(const wxString& name, const wxArrayInt& ids)
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

void ScriptingManager::OnScriptPluginMenu(wxCommandEvent& event)
{
    ScriptBindings::ScriptPluginWrapper::OnScriptMenu(event.GetId());
    DisplayErrors();
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
    Manager::Get()->GetLogManager()->DebugLog(_("ScriptingManager::UnRegisterScriptMenu() not implemented"));
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
    return IsScriptTrusted(m_CurrentlyRunningScriptFile);
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
    TrustScript(m_CurrentlyRunningScriptFile, permanently);
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
        cbMessageBox(_T("No script associated with this menu?!?"), _T("Error"), wxICON_ERROR);
        return;
    }

    MenuBoundScript& mbs = it->second;

    // is it a function?
    if (mbs.isFunc)
    {

        Sqrat::Function f(m_vm->GetRootTable(),cbU2C(mbs.scriptOrFunc));
        if(f.IsNull())
        {
            cbMessageBox(_T("Error on finding:") + mbs.scriptOrFunc, _T("Error"), wxICON_ERROR);
            return;
        }
        f();
        DisplayErrors();
        return;
    }

    // script loading below
    wxString script = ConfigManager::LocateDataFile(mbs.scriptOrFunc, sdScriptsUser | sdScriptsGlobal);
    if (wxGetKeyState(WXK_SHIFT))
    {
        Manager::Get()->GetEditorManager()->Open(script);
        return;
    }

    // run script

    if (m_vm->doFile(script) != SC_NO_ERROR)
    {
        cbMessageBox(_("Could not run script: ") + script, _("Error"), wxICON_ERROR);
    }

    DisplayErrors();
}
