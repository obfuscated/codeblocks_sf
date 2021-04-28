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

#include <map>
#include <set>
#include <unordered_map>

#include "crc32.h"
#include "menuitemsmanager.h"
#include "genericmultilinenotesdlg.h"
#include "scripting/bindings/sc_plugin.h"
#include "scripting/bindings/sc_utils.h"
#include "scripting/bindings/sc_typeinfo_all.h"

#include "squirrel.h"
#include "sqstdaux.h"
#include "sqstdblob.h"
#include "sqstdmath.h"
#include "sqstdstring.h"

static_assert(std::is_same<cbHSQUIRRELVM, HSQUIRRELVM>::value, "Types don't match");

template<> ScriptingManager* Mgr<ScriptingManager>::instance = nullptr;
template<> bool  Mgr<ScriptingManager>::isShutdown = false;

static wxString s_ScriptOutput;
static wxString s_ScriptErrors;
static wxString capture;

struct ScriptingManager::Data : wxEvtHandler
{
    Data(ScriptingManager *scriptingManager) :
        m_ScriptingManager(scriptingManager),
        m_AttachedToMainWindow(false),
        m_MenuItemsManager(false), // not auto-clear
        m_lastTypeTag(uint32_t(ScriptBindings::TypeTag::Last))
    {}

    void OnScriptMenu(wxCommandEvent& event);
    void OnScriptPluginMenu(wxCommandEvent& event);

    /// Container for script menus.
    /// Maps "script menuitem_ID" to "script_filename"
    struct MenuBoundScript
    {
        wxString scriptOrFunc;
        bool isFunc;
    };
    typedef std::map<int, MenuBoundScript> MenuIDToScript;
    typedef std::set<wxString> IncludeSet;

    ScriptingManager *m_ScriptingManager;

    /// This the Squirrel VM object we will use everywhere.
    HSQUIRRELVM m_vm;

    TrustedScripts m_TrustedScripts;

    MenuIDToScript m_MenuIDToScript;
    bool m_AttachedToMainWindow;

    ///  \brief This variable stores a stack of currently running script files. The back points to the current running file
    ///
    /// This variable is used to track the current running script for determine the working directory so it is possible to use relative paths with the
    /// "include" script function. This has to be a stack, because includes can go over several levels and every level can use relative paths.
    /// ~~~~~~
    /// main.script  --> include("scripts/include1.script")
    /// scripts
    ///   |----- include1.script     --> include("library/library.script")
    ///   |-----library
    ///            |----- library.script
    /// ~~~~~~
    /// The back of the stack will always point to the current running script and can be used to get the relative path of the include statements.
    /// The stack is pushed and popped in the LoadScript() function
    ///
    std::vector<wxString> m_RunningScriptFileStack;

    IncludeSet m_IncludeSet;
    MenuItemsManager m_MenuItemsManager;

    struct ConstantData
    {
        SQObjectType type;
        union Data
        {
            SQInteger intValue;
            SQBool boolValue;
            HSQOBJECT objectValue;
        } data;
    };

    // FIXME (squirrel) Using std::string here is not efficient
    using ConstantsMap = std::unordered_map<std::string, ConstantData>;
    ConstantsMap m_mapConstants;

    uint32_t m_lastTypeTag;


    friend SQInteger ConstantsGet(HSQUIRRELVM v);
    friend SQInteger ConstantsSet(HSQUIRRELVM v);

    static Data* GetData(HSQUIRRELVM v)
    {
        ScriptingManager* manager = reinterpret_cast<ScriptingManager*>(sq_getforeignptr(v));
        return manager->m_data;
    }
private:
    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(ScriptingManager::Data, wxEvtHandler)
END_EVENT_TABLE()

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

SQInteger ConstantsGet(HSQUIRRELVM v)
{
    ScriptBindings::ExtractParams2<ScriptBindings::SkipParam, const SQChar*> extractor(v);
    if (!extractor.Process("constants_get"))
        return extractor.ErrorMessage();

    ScriptingManager::Data *data = ScriptingManager::Data::GetData(v);
    cbAssert(data);

    ScriptingManager::Data::ConstantsMap::const_iterator it = data->m_mapConstants.find(extractor.p1);
    if (it != data->m_mapConstants.end())
    {
        const ScriptingManager::Data::ConstantData &constant = it->second;
        switch (constant.type)
        {
        case OT_INTEGER:
            sq_pushinteger(v, constant.data.intValue);
            return 1;
        case OT_BOOL:
            sq_pushbool(v, constant.data.boolValue);
            return 1;
        case OT_INSTANCE:
            sq_pushobject(v, constant.data.objectValue);
            return 1;
        default:
            return sq_throwerror(v, _SC("Unsupported type for the constant!"));
        }
    }
    return ScriptBindings::ThrowIndexNotFound(v);
}

SQInteger ConstantsSet(HSQUIRRELVM v)
{
    ScriptBindings::ExtractParamsBase extractor(v);
    if (!extractor.CheckNumArguments(3, "constants_set"))
        return extractor.ErrorMessage();
    const SQChar *name = nullptr;
    if (!extractor.ProcessParam(name, 2, "constants_set"))
        return extractor.ErrorMessage();

    ScriptingManager::Data *data = ScriptingManager::Data::GetData(v);
    cbAssert(data);

    ScriptingManager::Data::ConstantsMap::const_iterator it = data->m_mapConstants.find(name);
    if (it != data->m_mapConstants.end())
        return sq_throwerror(v, _SC("Trying to modify global constant is not allowed!"));

    return ScriptBindings::ThrowIndexNotFound(v);
}

namespace ScriptBindings
{
    void RegisterBindings(HSQUIRRELVM vm, ScriptingManager *manager);
    void UnregisterBindings();
} // namespace ScriptBindings

ScriptingManager::ScriptingManager() : m_data(new Data(this))
{
    m_data->m_vm = sq_open(1024);
    if (m_data->m_vm == nullptr)
        cbThrow(_T("Can't create scripting engine!"));

    // FIXME (squirrel) Provide special error function?
    sq_setprintfunc(m_data->m_vm, ScriptsPrintFunc, ScriptsErrorFunc);

    sq_setforeignptr(m_data->m_vm, this);

    sq_pushroottable(m_data->m_vm);
    sqstd_register_bloblib(m_data->m_vm);
    sqstd_register_mathlib(m_data->m_vm);
    sqstd_register_stringlib(m_data->m_vm);
    sqstd_seterrorhandlers(m_data->m_vm);
    sq_pop(m_data->m_vm, 1);

    RefreshTrusts();

    // Setup the constant system.
    {
        // Setup root table delegate which could make constants appear as constants.
        ScriptBindings::PreserveTop preserveTop(m_data->m_vm);
        sq_pushroottable(m_data->m_vm);

        sq_newtable(m_data->m_vm);
        ScriptBindings::BindMethod(m_data->m_vm, _SC("_get"), ConstantsGet, nullptr);
        ScriptBindings::BindMethod(m_data->m_vm, _SC("_set"), ConstantsSet, nullptr);
        sq_setdelegate(m_data->m_vm, -2);

        sq_pop(m_data->m_vm, 1); // pop root table
    }

    // register types
    ScriptBindings::RegisterBindings(m_data->m_vm, this);
}

ScriptingManager::~ScriptingManager()
{
    //dtor
    // save trusted scripts set
    ConfigManagerContainer::StringToStringMap myMap;
    int i = 0;
    TrustedScripts::iterator it;
    for (it = m_data->m_TrustedScripts.begin(); it != m_data->m_TrustedScripts.end(); ++it)
    {
        if (!it->second.permanent)
            continue;
        wxString key = wxString::Format(_T("trust%d"), i++);
        wxString value = wxString::Format(_T("%s?%x"), it->first.c_str(), it->second.crc);
        myMap.insert(myMap.end(), std::make_pair(key, value));
    }
    Manager::Get()->GetConfigManager(_T("security"))->Write(_T("/trusted_scripts"), myMap);

    if (m_data->m_vm)
    {
        for (Data::ConstantsMap::value_type &v : m_data->m_mapConstants)
        {
            if (v.second.type == OT_INSTANCE)
                sq_release(m_data->m_vm, &v.second.data.objectValue);
        }
        m_data->m_mapConstants.clear();

        ScriptBindings::UnregisterBindings();

        sq_close(m_data->m_vm);
        m_data->m_vm = nullptr;
    }

    delete m_data;
}

HSQUIRRELVM ScriptingManager::GetVM()
{
    return m_data->m_vm;
}

bool ScriptingManager::LoadScript(const wxString& filename)
{
    wxLogNull ln; // own error checking implemented -> avoid debug warnings

    wxString fname(filename);
    wxFile f(fname); // try to open
    if (!f.IsOpened())
    {
        bool found = false;

        // check in same dir as currently running script (if any)
        if (!m_data->m_RunningScriptFileStack.empty())
        {
            const wxString& currentlyRunningScriptFile = m_data->m_RunningScriptFileStack.back();
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
    m_data->m_RunningScriptFileStack.push_back(fname);
    bool ret = LoadBuffer(contents, fname);
    m_data->m_RunningScriptFileStack.pop_back();
    return ret;
}

bool ScriptingManager::LoadBuffer(const wxString& buffer, const wxString& debugName)
{
    // includes guard to avoid recursion
    wxString incName = UnixFilename(debugName);
    if (m_data->m_IncludeSet.find(incName) != m_data->m_IncludeSet.end())
    {
        Manager::Get()->GetLogManager()->LogWarning(F(_T("Ignoring Include(\"%s\") because it would cause recursion..."), incName.wx_str()));
        return true;
    }
    m_data->m_IncludeSet.insert(incName);

    s_ScriptOutput.Clear();
    s_ScriptErrors.Clear();

    const wxScopedCharBuffer &utf8Buffer = buffer.utf8_str();

    if (SQ_FAILED(sq_compilebuffer(m_data->m_vm, utf8Buffer.data(),
                                   utf8Buffer.length() * sizeof(SQChar),
                                   cbU2C(debugName), 1)))
    {
        const wxString errorMsg = ScriptBindings::ExtractLastSquirrelError(m_data->m_vm, false);
        const wxString fullMessage = wxString::Format("LoadBuffer failed\n  Filename: %s\n  Error: %s\n  Details: %s",
                                                      debugName.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptErrors.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);
        m_data->m_IncludeSet.erase(incName);
        return false;
    }

    sq_pushroottable(m_data->m_vm); // this is the parameter for the script closure
    if (SQ_FAILED(sq_call(m_data->m_vm, 1, SQFalse, SQTrue)))
    {
        const wxString errorMsg = ScriptBindings::ExtractLastSquirrelError(m_data->m_vm, false);
        const wxString fullMessage = wxString::Format("LoadBuffer failed\n  Filename: %s\n  Error: %s\n  Details: %s",
                                                      debugName.wx_str(), errorMsg.wx_str(),
                                                      s_ScriptErrors.wx_str());
        Manager::Get()->GetLogManager()->LogError(fullMessage);

        m_data->m_IncludeSet.erase(incName);
        return false;
    }

    sq_pop(m_data->m_vm, 1); // pop the closure

    m_data->m_IncludeSet.erase(incName);
    return true;
}


wxString ScriptingManager::LoadBufferRedirectOutput(const wxString& buffer)
{
    s_ScriptOutput.Clear();
    s_ScriptErrors.Clear();
    ::capture.Clear();

    // Save the old used print function so we can restore it after the
    // redirected print is finished. This is needed for example if the
    // scripting console redirects the script print output to itself and
    // not the default print function of the ScriptingManager.
    // In this case we have to restore the print function after the call to
    // the scripting console.
    const SQPRINTFUNCTION oldPrintFunc = sq_getprintfunc(m_data->m_vm);
    const SQPRINTFUNCTION oldErrorFunc = sq_geterrorfunc(m_data->m_vm);

    // redirect the print output to an internal buffer, so we can collect
    // the print output
    sq_setprintfunc(m_data->m_vm, CaptureScriptOutput, CaptureScriptOutput);

    // Run the script
    bool res = LoadBuffer(buffer);

    // restore the old print function
    sq_setprintfunc(m_data->m_vm, oldPrintFunc, oldErrorFunc);
    // return the internal print buffer if the script executed successfully
    return res ? ::capture : (wxString) wxEmptyString;
}

wxString ScriptingManager::GetErrorString(bool clearErrors)
{
    wxString msg = ScriptBindings::ExtractLastSquirrelError(m_data->m_vm, true);
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
    if (!m_data->m_AttachedToMainWindow)
    {
        Manager::Get()->GetAppWindow()->PushEventHandler(m_data);
        m_data->m_AttachedToMainWindow = true;
    }

    for (size_t i = 0; i < ids.GetCount(); ++i)
    {
        m_data->Connect(ids[i], -1, wxEVT_COMMAND_MENU_SELECTED,
                        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                        &ScriptingManager::Data::OnScriptPluginMenu);
    }
    return true;
}

bool ScriptingManager::RegisterScriptMenu(const wxString& menuPath, const wxString& scriptOrFunc,
                                          bool isFunction)
{
    // attach this event handler in the main window (one-time run)
    if (!m_data->m_AttachedToMainWindow)
    {
        Manager::Get()->GetAppWindow()->PushEventHandler(m_data);
        m_data->m_AttachedToMainWindow = true;
    }

    int id = wxNewId();
    id = m_data->m_MenuItemsManager.CreateFromString(menuPath, id);
    wxMenuItem* item = Manager::Get()->GetAppFrame()->GetMenuBar()->FindItem(id);
    if (item)
    {
        if (!isFunction)
            item->SetHelp(_("Press SHIFT while clicking this menu item to edit the assigned script in the editor"));

        m_data->Connect(id, -1, wxEVT_COMMAND_MENU_SELECTED,
                        (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
                        &ScriptingManager::Data::OnScriptMenu);

        Data::MenuBoundScript mbs;
        mbs.scriptOrFunc = scriptOrFunc;
        mbs.isFunc = isFunction;
        m_data->m_MenuIDToScript.insert(m_data->m_MenuIDToScript.end(), std::make_pair(id, mbs));


        Manager::Get()->GetLogManager()->Log(F(_("Script/function '%s' registered under menu '%s'"),
                                               scriptOrFunc.wx_str(), menuPath.wx_str()));
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
    m_data->m_MenuItemsManager.Clear();
    return true;
}

static_assert(sizeof(int64_t) >= sizeof(SQInteger),
              "We need to be able to store as much info in int64_t as in SQInteger!");

void ScriptingManager::BindIntConstant(const char *name, int64_t value)
{
    std::pair<Data::ConstantsMap::iterator, bool> result;

    Data::ConstantData v;
    v.type = OT_INTEGER;
    v.data.intValue = value;

    result = m_data->m_mapConstants.insert(Data::ConstantsMap::value_type(name, v));
    cbAssert(result.second);
}

void ScriptingManager::BindBoolConstant(const char *name, bool value)
{
    std::pair<Data::ConstantsMap::iterator, bool> result;

    Data::ConstantData v;
    v.type = OT_BOOL;
    v.data.boolValue = value;

    result = m_data->m_mapConstants.insert(Data::ConstantsMap::value_type(name, v));
    cbAssert(result.second);
}

void ScriptingManager::BindWxStringConstant(const char *name, const wxString &value)
{
    using namespace ScriptBindings;
    PreserveTop preserve(m_data->m_vm);

    UserDataForType<wxString> *data = CreateInlineInstance<wxString>(m_data->m_vm);
    if (data)
    {
        new (&data->userdata) wxString(value);

        Data::ConstantData v;
        v.type = OT_INSTANCE;
        sq_resetobject(&v.data.objectValue);
        sq_getstackobj(m_data->m_vm, -1, &v.data.objectValue);
        sq_addref(m_data->m_vm, &v.data.objectValue);
        sq_pop(m_data->m_vm, 1);

        std::pair<Data::ConstantsMap::iterator, bool> result;
        result = m_data->m_mapConstants.insert(Data::ConstantsMap::value_type(name, v));
        cbAssert(result.second);
    }
}

bool ScriptingManager::IsScriptTrusted(const wxString& script)
{
    TrustedScripts::iterator it = m_data->m_TrustedScripts.find(script);
    if (it == m_data->m_TrustedScripts.end())
        return false;
    // check the crc too
    wxUint32 crc = wxCrc32::FromFile(script);
    if (crc == it->second.crc)
        return true;
    cbMessageBox(script + _T("\n\n") + _("The script was marked as \"trusted\" but it has been modified "
                    "since then.\nScript not trusted anymore."),
                _("Warning"), wxICON_WARNING);
    m_data->m_TrustedScripts.erase(it);
    return false;
}

bool ScriptingManager::IsCurrentlyRunningScriptTrusted()
{
    if (m_data->m_RunningScriptFileStack.empty())
        return false;

    return IsScriptTrusted(m_data->m_RunningScriptFileStack.back());
}

void ScriptingManager::TrustScript(const wxString& script, bool permanently)
{
    // TODO: what should happen when script is empty()?

    TrustedScripts::iterator it = m_data->m_TrustedScripts.find(script);
    if (it != m_data->m_TrustedScripts.end())
    {
        // already trusted, remove it from the trusts (we recreate the trust below)
        m_data->m_TrustedScripts.erase(it);
    }

    TrustedScriptProps props;
    props.permanent = permanently;
    props.crc = wxCrc32::FromFile(script);

    m_data->m_TrustedScripts.insert(m_data->m_TrustedScripts.end(), std::make_pair(script, props));
}

void ScriptingManager::TrustCurrentlyRunningScript(bool permanently)
{
    if (!m_data->m_RunningScriptFileStack.empty())
    {
        const wxString currentlyRunningScriptFile = m_data->m_RunningScriptFileStack.back();
        TrustScript(currentlyRunningScriptFile, permanently);
    }
}

bool ScriptingManager::RemoveTrust(const wxString& script)
{
    TrustedScripts::iterator it = m_data->m_TrustedScripts.find(script);
    if (it != m_data->m_TrustedScripts.end())
    {
        // already trusted, remove it from the trusts (we recreate the trust below)
        m_data->m_TrustedScripts.erase(it);
        return true;
    }
    return false;
}

void ScriptingManager::RefreshTrusts()
{
    // reload trusted scripts set
    m_data->m_TrustedScripts.clear();
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
        m_data->m_TrustedScripts.insert(m_data->m_TrustedScripts.end(), std::make_pair(key, props));
    }
}

const ScriptingManager::TrustedScripts& ScriptingManager::GetTrustedScripts()
{
    return m_data->m_TrustedScripts;
}

void ScriptingManager::Data::OnScriptMenu(wxCommandEvent& event)
{
    Data::MenuIDToScript::iterator it = m_MenuIDToScript.find(event.GetId());
    if (it == m_MenuIDToScript.end())
    {
        cbMessageBox(_("No script associated with this menu?!?"), _("Error"), wxICON_ERROR);
        return;
    }

    Data::MenuBoundScript& mbs = it->second;

    // is it a function?
    if (mbs.isFunc)
    {
        ScriptBindings::Caller caller(m_vm);
        if (!caller.CallByName0(cbU2C(mbs.scriptOrFunc)))
            m_ScriptingManager->DisplayErrors();
        return;
    }

    // script loading below
    if (wxGetKeyState(WXK_SHIFT))
    {
        wxString script = ConfigManager::LocateDataFile(mbs.scriptOrFunc, sdScriptsUser | sdScriptsGlobal);
        Manager::Get()->GetEditorManager()->Open(script);
        return;
    }

    // run script
    if (!m_ScriptingManager->LoadScript(mbs.scriptOrFunc))
        cbMessageBox(_("Could not run script: ") + mbs.scriptOrFunc, _("Error"), wxICON_ERROR);
}

void ScriptingManager::Data::OnScriptPluginMenu(wxCommandEvent& event)
{
    ScriptBindings::ScriptPluginWrapper::OnScriptMenu(event.GetId());
}

uint32_t ScriptingManager::RequestClassTypeTag()
{
    return m_data->m_lastTypeTag++;
}
