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

int ID_ScriptingManager_Debug_Timer=wxNewId();


void PrintSquirrelToWxString(wxString &msg,const SQChar * s, va_list& vl)
{
    int buffer_size = 2048;
    SQChar *tmp_buffer;
    for(;;buffer_size*=2)
    {
        // TODO (bluehazzard#1#): Check if this is UNICODE UTF8 safe
        tmp_buffer = new SQChar [buffer_size];
        int retvalue = vsnprintf(tmp_buffer,buffer_size,s,vl);
        if(retvalue < buffer_size)
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

BEGIN_EVENT_TABLE(ScriptingManager, wxEvtHandler)
//
END_EVENT_TABLE()

ScriptingManager::ScriptingManager()
    : m_AttachedToMainWindow(false),
    m_MenuItemsManager(false), // not auto-clear
    m_DebugerUpdateTimer(this,ID_ScriptingManager_Debug_Timer),
    m_rdbg(nullptr)
{
    //ctor

    // initialize but don't load the IO lib
    m_vm = new ScriptBindings::CBsquirrelVM(1024,(ScriptBindings::VM_LIB_ALL & ~ScriptBindings::VM_LIB_IO));

    if (!m_vm->GetVM())
        cbThrow(_T("Can't create scripting engine!"));

    m_vm->SetPrintFunc(ScriptsPrintFunc,CaptureScriptErrors);
    m_vm->SetMeDefault();

    RefreshTrusts();

    // register types
    m_enable_debugger = false;

     // register types
    ScriptBindings::RegisterBindings(m_vm->GetVM());
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

    // delete all plugins
    scripted_plugin_map::iterator itr;
    for(itr = m_registered_plugins.begin();itr != m_registered_plugins.end();++itr)
    {
        delete itr->second;
    }
    m_registered_plugins.clear();

    if(m_enable_debugger)
    {
        sq_rdbg_shutdown(m_rdbg);

        IncludeSet::iterator itr = m_debugger_created_temp_files.begin();
        for(;itr != m_debugger_created_temp_files.end();++itr)
            wxRemoveFile((*itr));
    }

    //SquirrelVM::Shutdown();
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
                Manager::Get()->GetLogManager()->DebugLog(_("Can't open script ") + filename);
                return false;
            }
        }
    }
    // read file
    wxString contents = wxEmptyString;
    #if wxCHECK_VERSION(3,0,0)
    f.ReadAll(&contents);
    #else
    contents = cbReadFileContents(f);
    #endif // wxCHECK_VERSION
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
    if(m_enable_debugger && !real_path)
    {
        wxFile tmp_file;
        wxString tmp_file_path = wxFileName::CreateTempFileName(debugName,&tmp_file);
        if(tmp_file_path.IsEmpty())
        {
            Manager::Get()->GetLogManager()->LogError(_("ScriptingManager::LoadBuffer: Could not create tmp file, for debugging"));
        } else
        {
            m_debugger_created_temp_files.insert(tmp_file_path);
            if(tmp_file.Write(buffer,wxConvUTF8) == false)
                Manager::Get()->GetLogManager()->LogError(_("ScriptingManager::LoadBuffer: Error on writing in file: ")+ tmp_file_path);

            tmp_file.Close();
            debugName = tmp_file_path;
        }
    }

    // Make sure that the directory has a valid name (don't mix slash and backslash)
    wxFileName tmp_name(debugName);
    debugName = tmp_name.GetFullPath();

    if(m_rdbg)
        m_rdbg->InformNewFileLoaded(m_vm->GetVM(),debugName.ToUTF8().data());


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
        if (cbMessageBox(_("Script errors have occured...\nPress 'Yes' to see the exact errors."),
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

bool ScriptingManager::RegisterScriptPlugin(const wxString& name, cbScriptPlugin* plugin )
{

    // unregister script...
    UnRegisterScriptPlugin(name);

    m_registered_plugins.insert(m_registered_plugins.end(),std::make_pair(name,plugin));
    plugin->CreateMenus();

    Manager::Get()->GetLogManager()->Log(_("Script plugin registered: ") + name);
    return true;
}

bool ScriptingManager::UnRegisterScriptPlugin(const wxString& name)
{
    scripted_plugin_map::iterator itr = m_registered_plugins.find(name);
    if(itr == m_registered_plugins.end())
        return false;

    delete itr->second;
    m_registered_plugins.erase(itr);
    Manager::Get()->GetLogManager()->Log(_("Script plugin unregistered: ") + name);
    return true;
}

bool ScriptingManager::UnInstallScriptPlugin(const wxString& name, bool del_files)
{
    // TODO (bluehazzard#1#): Implement uninstall plugin

    return UnRegisterScriptPlugin(name);
}

bool ScriptingManager::InstallScriptPlugin(const wxString& file)
{
    // TODO (bluehazzard#1#): Implement install plugin
    return false;
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

void ScriptingManager::OnScriptMenu(wxCommandEvent& event)
{
    MenuIDToScript::iterator it = m_MenuIDToScript.find(event.GetId());
    if (it == m_MenuIDToScript.end())
    {
        cbMessageBox(_("No script associated with this menu?!?"), _("Error"), wxICON_ERROR);
        return;
    }

    MenuBoundScript& mbs = it->second;

    // is it a function?
    if (mbs.isFunc)
    {
        Sqrat::Function call_back(Sqrat::RootTable(),mbs.scriptOrFunc.ToUTF8());
        if (call_back.IsNull())
            return;
        call_back();

        if(DisplayErrors())
        {
            //DisplayErrors(sa.GetError());
        }
        return;
    }

    // script loading below

    if (wxGetKeyState(WXK_SHIFT))
    {
        wxString script = ConfigManager::LocateDataFile(mbs.scriptOrFunc, sdScriptsUser | sdScriptsGlobal);
        Manager::Get()->GetEditorManager()->Open(script);
        return;
    }

    if (!LoadScript(mbs.scriptOrFunc))
        cbMessageBox(_("Could not run script: ") + mbs.scriptOrFunc, _("Error"), wxICON_ERROR);

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


int ScriptingManager::ExecutePlugin(wxString Name)
{
    scripted_plugin_map::iterator itr =  m_registered_plugins.find(Name);
    if(itr == m_registered_plugins.end())
        return -3;  // The script with the name _Name_ could not be found
    return itr->second->Execute();
}

cbScriptPlugin* ScriptingManager::GetPlugin(wxString Name)
{
    scripted_plugin_map::iterator itr =  m_registered_plugins.find(Name);
    if(itr == m_registered_plugins.end())
        return nullptr;  // The script with the name _Name_ could not be found
    return itr->second;
}

void ScriptingManager::CreateModuleMenu(const ModuleType type, wxMenu* menu, const FileTreeData* data)
{
    scripted_plugin_map::iterator itr;
    for(itr = m_registered_plugins.begin(); itr != m_registered_plugins.end();++itr)
    {
        itr->second->BuildModuleMenu(type,menu,data);
    }
}

cbScriptPlugin* ScriptingManager::GetPlugin(unsigned int index)
{
    unsigned int i = 0;
    scripted_plugin_map::iterator itr;
    for(itr = m_registered_plugins.begin(); itr != m_registered_plugins.end();++itr)
    {
        if(i == index)
        {
            return itr->second;
        }
        i++;
    }

    return nullptr;
}

ScriptBindings::CBSquirrelThread* ScriptingManager::CreateSandbox()
{
    return m_vm->CreateThread();
}

int ScriptingManager::LoadFileFromZip(wxString path,wxString file,wxString& contents)
{

    wxString filename = path + _T("#zip:") + file;

    contents.Clear();

    wxFileSystem* fs = new wxFileSystem;
    wxFSFile* f = fs->OpenFile(filename);
    if (f)
    {
        wxInputStream* is = f->GetStream();
        wxFileOffset file_length = is->GetLength();
        if(file_length == wxInvalidOffset)
        {
            delete fs;
            delete f;
            Manager::Get()->GetLogManager()->LogWarning(_("file length for ") + filename+ _("is invlaid"));
            return -1;
        }
        char tmp[1024] = {};
        while (!is->Eof() && is->CanRead())
        {
            memset(tmp, 0, sizeof(tmp));
            is->Read(tmp, sizeof(tmp) - 1);
            contents << wxString::FromUTF8((const char*)tmp);
        }
        delete f;
    }
    else
    {
        delete fs;
        Manager::Get()->GetLogManager()->LogWarning(_("Could not find File ") + filename );
        return -2;
    }
    delete fs;

    return 0;
}

void ScriptingManager::ParseDebuggerCMDLine(wxString cmd_line)
{
    if(cmd_line == wxEmptyString)
        return; // Debugging is not enabled

    wxStringTokenizer tokenizer(cmd_line,_(":"));
    if(!tokenizer.HasMoreTokens() || tokenizer.CountTokens() != 2)
        return; // No token. Error?

    int port = wxAtoi(tokenizer.GetNextToken());
    if(port <= 0)
        return; // Port out of range


    bool halt = tokenizer.GetNextToken().Cmp(_("h")) == 0;

    m_rdbg = sq_rdbg_init(m_vm->GetVM(),port,SQFalse);
	if(m_rdbg) {
        Manager::Get()->GetLogManager()->DebugLog(_("Squirrel debugging is enabled"));
		//!! ENABLES DEBUG INFO GENERATION(for the compiler)
		sq_enabledebuginfo(m_vm->GetVM(),SQTrue);
		m_enable_debugger = true;
		if(halt)
		{
		    wxProgressDialog pd(wxString::Format(_("wait for a debugger connect on port %d"),port),wxString::Format(_("wait for a debugger connect on port %d"),port));
		    Manager::Get()->GetLogManager()->DebugLog(_("Squirrel debugger HALT"));
		    pd.Update(50);
		    sq_rdbg_waitforconnections(m_rdbg);
		    pd.Update(100);
		}

		m_DebugerUpdateTimer.Start(500);
		this->Connect(m_DebugerUpdateTimer.GetId(),wxEVT_TIMER,wxTimerEventHandler(ScriptingManager::OnDebugTimer),NULL,this);

	}
}

void ScriptingManager::OnDebugTimer(wxTimerEvent& event)
{
    if(m_rdbg)
    {
        // The debugger needs a periodically update
        sq_rdbg_update(m_rdbg);
    }
}
