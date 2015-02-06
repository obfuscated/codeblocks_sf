/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef COMPILEOPTIONSBASE_H
#define COMPILEOPTIONSBASE_H

#include "globals.h"
#include <wx/hashmap.h>

WX_DECLARE_STRING_HASH_MAP(wxString, StringHash);

/**
  * This is a base class for all classes needing compilation parameters. It
  * offers functions to get/set the following:\n
  * \li Compiler options
  * \li Linker options
  * \li Compiler include dirs
  * \li Resource compiler include dirs
  * \li Linker include dirs
  * \li Custom commands to be executed before/after build
  * \li The settings modification status
  * \n\n
  * These settings are used by the compiler plugins to construct the necessary
  * compilation commands.
  */
class DLLIMPORT CompileOptionsBase
{
    public:
        CompileOptionsBase();
        virtual ~CompileOptionsBase();

        virtual void AddPlatform(int platform);
        virtual void RemovePlatform(int platform);
        virtual void SetPlatforms(int platforms);
        virtual int GetPlatforms() const;
        virtual bool SupportsCurrentPlatform() const;

        virtual void SetLinkerOptions(const wxArrayString& linkerOpts);
        virtual const wxArrayString& GetLinkerOptions() const;
        virtual void AddLinkerOption(const wxString& option);
        virtual void RemoveLinkerOption(const wxString& option);

        virtual void SetLinkLibs(const wxArrayString& linkLibs);
        virtual const wxArrayString& GetLinkLibs() const;
        virtual void AddLinkLib(const wxString& lib);
        virtual void RemoveLinkLib(const wxString& lib);

        virtual void SetCompilerOptions(const wxArrayString& compilerOpts);
        virtual const wxArrayString& GetCompilerOptions() const;
        virtual void AddCompilerOption(const wxString& option);
        virtual void RemoveCompilerOption(const wxString& option);

        virtual void SetResourceCompilerOptions(const wxArrayString& resourceCompilerOpts);
        virtual const wxArrayString& GetResourceCompilerOptions() const;
        virtual void AddResourceCompilerOption(const wxString& option);
        virtual void RemoveResourceCompilerOption(const wxString& option);

        virtual void SetIncludeDirs(const wxArrayString& includeDirs);
        virtual const wxArrayString& GetIncludeDirs() const;
        virtual void AddIncludeDir(const wxString& option);
        virtual void RemoveIncludeDir(const wxString& option);

        virtual void SetResourceIncludeDirs(const wxArrayString& resIncludeDirs);
        virtual const wxArrayString& GetResourceIncludeDirs() const;
        virtual void AddResourceIncludeDir(const wxString& option);
        virtual void RemoveResourceIncludeDir(const wxString& option);

        virtual void SetLibDirs(const wxArrayString& libDirs);
        virtual const wxArrayString& GetLibDirs() const;
        virtual void AddLibDir(const wxString& option);
        virtual void RemoveLibDir(const wxString& option);

        virtual void SetCommandsBeforeBuild(const wxArrayString& commands);
        virtual const wxArrayString& GetCommandsBeforeBuild() const;
        virtual void AddCommandsBeforeBuild(const wxString& command);
        virtual void RemoveCommandsBeforeBuild(const wxString& command);

        virtual void SetCommandsAfterBuild(const wxArrayString& commands);
        virtual const wxArrayString& GetCommandsAfterBuild() const;
        virtual void AddCommandsAfterBuild(const wxString& command);
        virtual void RemoveCommandsAfterBuild(const wxString& command);

        virtual void SetBuildScripts(const wxArrayString& scripts);
        virtual const wxArrayString& GetBuildScripts() const;
        virtual void AddBuildScript(const wxString& script);
        virtual void RemoveBuildScript(const wxString& script);

        virtual bool GetModified() const;
        virtual void SetModified(bool modified);

        virtual bool GetAlwaysRunPostBuildSteps() const;
        virtual void SetAlwaysRunPostBuildSteps(bool always);

        virtual bool SetVar(const wxString& key, const wxString& value, bool onlyIfExists = false);
        virtual bool UnsetVar(const wxString& key);
        virtual void UnsetAllVars();
        virtual bool HasVar(const wxString& key) const;
        virtual const wxString& GetVar(const wxString& key) const;
        virtual const StringHash& GetAllVars() const;
    protected:
        int m_Platform;
        wxArrayString m_LinkerOptions;
        wxArrayString m_LinkLibs;
        wxArrayString m_CompilerOptions;
        wxArrayString m_ResourceCompilerOptions;
        wxArrayString m_IncludeDirs;
        wxArrayString m_ResIncludeDirs;
        wxArrayString m_LibDirs;
        wxArrayString m_CmdsBefore;
        wxArrayString m_CmdsAfter;
        wxArrayString m_Scripts;
        bool m_Modified;
        bool m_AlwaysRunPostCmds;
        StringHash m_Vars;
    private:
};

#endif // COMPILEOPTIONSBASE_H

