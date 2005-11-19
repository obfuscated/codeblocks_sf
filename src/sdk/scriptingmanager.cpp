#include <sdk_precomp.h>
#include "scriptingmanager.h"
#include "cbexception.h"
#include "manager.h"
#include "messagemanager.h"
#include "configmanager.h"
#include "scriptingcall.h"
#include "as/bindings/scriptbindings.h"
#include <wx/msgdlg.h>
#include <wx/file.h>
#include <wx/regex.h>
#include <settings.h>

static wxString s_Errors;

class asCOutputStream : public asIOutputStream
{
    public:
        void Write(const char *text)
        {
            s_Errors << _U(text);
        }
};

//statics
static asCOutputStream asOut;
static ScriptingManager* pScripting = 0;
ScriptingManager* ScriptingManager::Get()
{
    if (!pScripting)
        pScripting = new ScriptingManager;
    return pScripting;
}

void ScriptingManager::Free()
{
    assert(pScripting);
    delete pScripting;
    pScripting = 0;
}

ScriptingManager::ScriptingManager()
{
    //ctor
	m_pEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (!m_pEngine)
        cbThrow(_T("Can't create scripting engine!"));

    m_pEngine->SetCommonMessageStream(&asOut);

    // register types
    RegisterBindings(m_pEngine);
}

ScriptingManager::~ScriptingManager()
{
    //dtor
	m_pEngine->Release();
}

// static
wxString ScriptingManager::GetErrorDescription(int error)
{
    if (error >= 0) return _T("SUCCESS");

    switch (error)
    {
        case asERROR: return _T("ERROR");
        case asCONTEXT_ACTIVE: return _T("CONTEXT_ACTIVE");
        case asCONTEXT_NOT_FINISHED: return _T("CONTEXT_NOT_FINISHED");
        case asCONTEXT_NOT_PREPARED: return _T("CONTEXT_NOT_PREPARED");
        case asINVALID_ARG: return _T("INVALID_ARG");
        case asNO_FUNCTION: return _T("NO_FUNCTION");
        case asNOT_SUPPORTED: return _T("NOT_SUPPORTED");
        case asINVALID_NAME: return _T("INVALID_NAME");
        case asNAME_TAKEN: return _T("NAME_TAKEN");
        case asINVALID_DECLARATION: return _T("INVALID_DECLARATION");
        case asINVALID_OBJECT: return _T("INVALID_OBJECT");
        case asINVALID_TYPE: return _T("INVALID_TYPE");
        case asALREADY_REGISTERED: return _T("ALREADY_REGISTERED");
        case asMULTIPLE_FUNCTIONS: return _T("MULTIPLE_FUNCTIONS");
        case asNO_MODULE: return _T("NO_MODULE");
        case asNO_GLOBAL_VAR: return _T("NO_GLOBAL_VAR");
        case asINVALID_CONFIGURATION: return _T("INVALID_CONFIGURATION");
        case asINVALID_INTERFACE: return _T("INVALID_INTERFACE");
        case asCANT_BIND_ALL_FUNCTIONS: return _T("CANT_BIND_ALL_FUNCTIONS");
        case asLOWER_ARRAY_DIMENSION_NOT_REGISTERED: return _T("LOWER_ARRAY_DIMENSION_NOT_REGISTERED");
        case asWRONG_CONFIG_GROUP: return _T("WRONG_CONFIG_GROUP");
        case asCONFIG_GROUP_IS_IN_USE: return _T("CONFIG_GROUP_IS_IN_USE");
        default: return _T("NeverHere");
    }
    return _T("NeverHere");
}

bool ScriptingManager::DoLoadScript(const wxString& filename, wxString& script)
{
//    LOGSTREAM << _T("Trying to open script: ") << filename << '\n';
    wxLogNull ln;
    wxFile file(filename);
    return cbRead(file, script);
}

int ScriptingManager::LoadScript(const wxString& filename, const wxString& module)
{
//    wxString script;
    // try to load as-passed
//    if (!DoLoadScript(filename, script))
//    {
//        // try in <data_path>/scripts/
//        if (!DoLoadScript(ConfigManager::GetScriptsFolder() + _T("/") + filename, script))
//        {
////            wxMessageBox(_("Can't open script ") + filename, _("Error"), wxICON_ERROR);
//            return -1;
//        }
//    }

    wxString fname = filename;
    FILE* fp = fopen(_C(fname), "r");
    if (!fp)
    {
        fname = ConfigManager::GetScriptsFolder() + _T("/") + filename;
        fp = fopen(_C(fname), "r");
        if(!fp)
        {
            Manager::Get()->GetMessageManager()->DebugLog(_("Can't open script %s"), filename.c_str());
            return -1;
        }
    }
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    char* script = new char[size + 1];
    memset(script, 0, size + 1);
    size = fread(script, 1, size, fp);
    script[size] = 0;
    fclose(fp);

    s_Errors.Clear();

    // build script
	m_pEngine->AddScriptSection(_C(module), _C(filename), script, strlen(script), 0, false);
	m_pEngine->Build(_C(module));

    // locate and run "int main()"
	int funcID = FindFunctionByDeclaration(_T("int main()"), module);
	if (funcID < 0)
        Manager::Get()->GetMessageManager()->DebugLog(_T("No 'int main()' in '%s': no autorun"), filename.c_str());
	Executor<int> exec(funcID);
	int ret = exec.Call();
	if (!exec.Success())
	{
        if (wxMessageBox(_("An exception has been raised from the script:\n\n") +
                        exec.CreateErrorString() +
                        _("\n\nDo you want to open this script in the editor?"), _T("Script error"), wxICON_ERROR | wxYES_NO) == wxYES)
        {
            cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname);
            if (ed && exec.GetLineNumber() != 0)
            {
                ed->GotoLine(exec.GetLineNumber() - 1);
                ed->GetControl()->SetFocus();
            }
        }
	}

    // display errors (if any)
    if (!s_Errors.IsEmpty())
    {
//        LOGSTREAM << s_Errors << '\n';
        // startup.script (6, 2) : Error   : Expected ';'
        wxRegEx re(_T("\\(([0-9]+), ([0-9]+)\\)[ \t]:[ \t][Ee]rror[ \t]+:[ \t](.*)"));
        if (re.Matches(s_Errors))
        {
            if (wxMessageBox(s_Errors + _T("\n\nDo you want to open this script in the editor?"), _("Script error"), wxICON_ERROR | wxYES_NO) == wxYES)
            {
                cbEditor* ed = Manager::Get()->GetEditorManager()->Open(fname);
                if (ed)
                {
                    long line = 0;
                    long column = 0;
                    re.GetMatch(s_Errors, 1).ToLong(&line);
                    re.GetMatch(s_Errors, 2).ToLong(&column);
                    if (line != 0)
                    {
                        int pos = ed->GetControl()->PositionFromLine(line - 1);
                        ed->GotoLine(line - 1);
                        ed->GetControl()->GotoPos(pos + column - 1);
                        ed->GetControl()->SetFocus();
                    }
                }
            }
        }
    }

    delete[] script;
	return ret;
}

int ScriptingManager::FindFunctionByDeclaration(const wxString& decl, const wxString& module)
{
	return m_pEngine->GetFunctionIDByDecl(_C(module), _C(decl));
}

int ScriptingManager::FindFunctionByName(const wxString& name, const wxString& module)
{
	return m_pEngine->GetFunctionIDByName(_C(module), _C(name));
}
