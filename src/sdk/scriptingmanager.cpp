#include <sdk_precomp.h>

#ifndef CB_PRECOMP
    #include "scriptingmanager.h"
    #include "cbexception.h"
    #include "manager.h"
    #include "editormanager.h"
    #include "messagemanager.h"
    #include "configmanager.h"
    #include "cbeditor.h"
    #include <settings.h>
    #include "globals.h"

    #include <wx/msgdlg.h>
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/regex.h>
#endif

#include "scripting/sqplus/sqplus.h"
#include "scripting/bindings/scriptbindings.h"

static wxString s_ScriptErrors;
static wxString capture;

static void ScriptsPrintFunc(HSQUIRRELVM v, const SQChar * s, ...)
{
    static SQChar temp[2048];
    va_list vl;
    va_start(vl,s);
    scvsprintf( temp,s,vl);
    wxString msg = cbC2U(temp);
    Manager::Get()->GetMessageManager()->DebugLog(msg);
    va_end(vl);

    s_ScriptErrors << msg;
}

static void CaptureScriptOutput(HSQUIRRELVM v, const SQChar * s, ...)
{
    static SQChar temp[2048];
    va_list vl;
    va_start(vl,s);
    scvsprintf(temp,s,vl);
    ::capture.append(cbC2U(temp));
    va_end(vl);
};

ScriptingManager::ScriptingManager()
{
    //ctor

    // initialize but don't load the IO lib
    SquirrelVM::Init((SquirrelInitFlags)(sqifAll & ~sqifIO));

	if (!SquirrelVM::GetVMPtr())
        cbThrow(_T("Can't create scripting engine!"));

    sq_setprintfunc(SquirrelVM::GetVMPtr(), ScriptsPrintFunc);

    // register types
    ScriptBindings::RegisterBindings();
}

ScriptingManager::~ScriptingManager()
{
    //dtor
	SquirrelVM::Shutdown();
}

bool ScriptingManager::LoadScript(const wxString& filename)
{
    wxCriticalSectionLocker c(cs);
    wxLogNull ln;

    wxString fname = filename;
    wxFile f;
    f.Open(fname);
    if (!f.IsOpened())
    {
        fname = ConfigManager::GetScriptsFolder() + _T("/") + filename;
        f.Open(fname);
        if(!f.IsOpened())
        {
            Manager::Get()->GetMessageManager()->DebugLog(_T("Can't open script %s"), filename.c_str());
            return false;
        }
    }
    // read file
    wxString contents = cbReadFileContents(f);
    return LoadBuffer(contents, filename);
}

bool ScriptingManager::LoadBuffer(const wxString& buffer, const wxString& debugName)
{
    wxCriticalSectionLocker c(cs);

    s_ScriptErrors.Clear();

    // compile script
    SquirrelObject script;
    try
    {
        script = SquirrelVM::CompileBuffer(cbU2C(buffer), cbU2C(debugName));
    }
    catch (SquirrelError e)
    {
        cbMessageBox(wxString::Format(_T("Filename: %s\nError: %s\nDetails: %s"), debugName.c_str(), cbC2U(e.desc).c_str(), s_ScriptErrors.c_str()), _("Script compile error"), wxICON_ERROR);
        return false;
    }

    // run script
    try
    {
        SquirrelVM::RunScript(script);
    }
    catch (SquirrelError e)
    {
        cbMessageBox(wxString::Format(_T("Filename: %s\nError: %s\nDetails: %s"), debugName.c_str(), cbC2U(e.desc).c_str(), s_ScriptErrors.c_str()), _("Script run error"), wxICON_ERROR);
        return false;
    }
    return true;
}


wxString ScriptingManager::LoadBufferRedirectOutput(const wxString& buffer)
{
    wxCriticalSectionLocker c(cs);

    ::capture.Clear();

    sq_setprintfunc(SquirrelVM::GetVMPtr(), CaptureScriptOutput);
    bool res = LoadBuffer(buffer);
    sq_setprintfunc(SquirrelVM::GetVMPtr(), ScriptsPrintFunc);

    return res ? ::capture : (wxString) wxEmptyString;
}

wxString ScriptingManager::GetErrorString(SquirrelError* exception, bool clearErrors)
{
    wxString msg;
    if (exception)
        msg << cbC2U(exception->desc);
    msg << s_ScriptErrors;

    if (clearErrors)
        s_ScriptErrors.Clear();

    return msg;
}

void ScriptingManager::DisplayErrors(SquirrelError* exception, bool clearErrors)
{
    wxString msg = GetErrorString(exception, clearErrors);
    if (!msg.IsEmpty())
        cbMessageBox(msg, _("Script errors"), wxICON_ERROR);
}

void ScriptingManager::InjectScriptOutput(const wxString& output)
{
    s_ScriptErrors << output;
}
