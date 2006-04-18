#ifndef SCRIPTING_H
#define SCRIPTING_H

#include "settings.h"
#include "manager.h"
#include <wx/intl.h>

class asIScriptEngine;

/** All about scripting in Code::Blocks.
  *
  * The scripting engine used here is AngelScript (http:://www.angelcode.com/angelscript).
  *
  * Example to load and execute a script's main function follows:
  * <code>
  * if (Manager::Get()->GetScriptingManager()->LoadScript(_T("some.script"), _T("arbitrary_module_name")))
  * {
  *     Manager::Get()->GetScriptingManager()->Compile(_T("arbitrary_module_name"), true);
  * }
  * </code>
  */
class DLLIMPORT ScriptingManager : public Mgr<ScriptingManager>
{
        friend class Mgr<ScriptingManager>;
    public:
        ~ScriptingManager();

        /** @return A pointer to the engine. */
        asIScriptEngine* GetEngine(){ return m_pEngine; }

        /** Compiles a script module and runs the function "int main()" (if needed/exists).
         * @param module The module to compile. One or more calls to LoadScript() must have
         * been performed before to load scripts to a module.
         * @param autorunMain If true, runs the "int main()" function (if it exists).
         * @return Zero (0) for no errors.
         */
        int Compile(const wxString& module = _T(""), bool autorunMain = true);

        /** Loads a script into a module. When you 're done loading your scripts, you must call Compile()...
         * @param filename The script's filename. If it doesn't exist,
         * it is also looked up in <data_path>/scripts/
         * @param module The module to add this script into. Don't forget to call Compile() at the end...
         * @return True if no errors occurred.
         */
        bool LoadScript(const wxString& filename, const wxString& module = _T(""));

        /** Convenience function to load a single script from file and run it
          * using one single command (i.e. the old way).
         * @param module The module to compile.
         * @param filename The script's filename. If it doesn't exist,
         * it is also looked up in <data_path>/scripts/
         * @param autorunMain If true, runs the "int main()" function (if it exists).
         * @return Zero (0) for no errors.
          */
        int LoadAndRunScript(const wxString& filename, const wxString& module = _T(""), bool autorunMain = true);

        int FindFunctionByDeclaration(const wxString& decl, const wxString& module = _T(""));
        int FindFunctionByName(const wxString& name, const wxString& module = _T(""));

        static wxString GetErrorDescription(int error);
	protected:
        asIScriptEngine* m_pEngine;
	private:
        ScriptingManager();
};

#define asCHECK_ERROR_RET(r,what,ret) \
    { \
        if (r < 0) \
        { \
            LOGSTREAM << what << _T(": ") << ScriptingManager::GetErrorDescription(r) << _T(" (") cbC2U(__FILE__) << _T(":") << wxString::Format(_T("%d"), __LINE__) << _T(")\n"); \
            return ret; \
        } \
    }

#define asCHECK_ERROR(r,what) \
    { \
        if (r < 0) \
        { \
            LOGSTREAM << what << _T(": ") << ScriptingManager::GetErrorDescription(r) << _T(" (") cbC2U(__FILE__) << _T(":") << wxString::Format(_T("%d"), __LINE__) << _T(")\n"); \
            return; \
        } \
	}

#endif // SCRIPTING_H
