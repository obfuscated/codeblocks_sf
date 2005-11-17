#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <angelscript.h>
#include "settings.h"
#include <wx/intl.h>

class DLLIMPORT ScriptingManager
{
    public:
        static ScriptingManager* Get();
        static void Free();

        ~ScriptingManager();

        asIScriptEngine* GetEngine(){ return m_pEngine; }
        /** Loads script and runs the function "int main()".
         * @param filename The script's filename. If it doesn't exist,
         * it is also looked up in <data_path>/scripts/
         * @return If the script has errors, -1 s returned, else the
         * script's return value is returned.
         */
        int LoadScript(const wxString& filename, const wxString& module = _T(""));
        int FindFunctionByDeclaration(const wxString& decl, const wxString& module = _T(""));
        int FindFunctionByName(const wxString& name, const wxString& module = _T(""));

        static wxString GetErrorDescription(int error);
	protected:
        asIScriptEngine* m_pEngine;
	private:
        ScriptingManager();
        bool DoLoadScript(const wxString& filename, wxString& script);
};

#define asCHECK_ERROR_RET(r,what,ret) \
    { \
        if (r < 0) \
        { \
            LOGSTREAM << what << _T(": ") << ScriptingManager::GetErrorDescription(r) << _T(" (") _U(__FILE__) << _T(":") << __LINE__ << _T(")\n"); \
            return ret; \
        } \
    }

#define asCHECK_ERROR(r,what) \
    { \
        if (r < 0) \
        { \
            LOGSTREAM << what << _T(": ") << ScriptingManager::GetErrorDescription(r) << _T(" (") _U(__FILE__) << _T(":") << __LINE__ << _T(")\n"); \
            return; \
        } \
	}

#endif // SCRIPTING_H
