/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "compilerMINGW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <wx/filefn.h>
#include "manager.h"
#include "macrosmanager.h"
#include "logmanager.h"
#include "compilerMINGWgenerator.h"

#include <configmanager.h>

#ifdef __WXMSW__
    #include <wx/dir.h>
    #include <wx/msw/registry.h>
#endif

CompilerMINGW::CompilerMINGW(const wxString& name, const wxString& ID)
    : Compiler(name, ID)
{
    m_Weight = 4;
    Reset();
}

CompilerMINGW::~CompilerMINGW()
{
    //dtor
}

Compiler * CompilerMINGW::CreateCopy()
{
    return (new CompilerMINGW(*this));
}

CompilerCommandGenerator* CompilerMINGW::GetCommandGenerator(cbProject *project)
{
    CompilerMINGWGenerator *generator = new CompilerMINGWGenerator;
    generator->Init(project);
    return generator;
}

AutoDetectResult CompilerMINGW::AutoDetectInstallationDir()
{
    // try to find MinGW in environment variable PATH first
    wxString pathValues;
    wxGetEnv(_T("PATH"), &pathValues);
    if (!pathValues.IsEmpty())
    {
        wxString sep = platform::windows ? _T(";") : _T(":");
        wxChar pathSep = platform::windows ? _T('\\') : _T('/');
        wxArrayString pathArray = GetArrayFromString(pathValues, sep);
        for (size_t i = 0; i < pathArray.GetCount(); ++i)
        {
            if (wxFileExists(pathArray[i] + pathSep + m_Programs.C))
            {
                if (pathArray[i].AfterLast(pathSep).IsSameAs(_T("bin")))
                {
                    m_MasterPath = pathArray[i].BeforeLast(pathSep);
                    return adrDetected;
                }
            }
        }
    }

    wxString sep = wxFileName::GetPathSeparator();
    if (platform::windows)
    {
        // look first if MinGW was installed with Code::Blocks (new in beta6)
        m_MasterPath = ConfigManager::GetExecutableFolder();
        if (!wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
            // if that didn't do it, look under C::B\MinGW, too (new in 08.02)
            m_MasterPath += sep + _T("MinGW");
        if (!wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
        {
            // no... search for MinGW installation dir
            wxString windir = wxGetOSDirectory();
            wxFileConfig ini(_T(""), _T(""), windir + _T("/MinGW.ini"), _T(""), wxCONFIG_USE_LOCAL_FILE | wxCONFIG_USE_NO_ESCAPE_CHARACTERS);
            m_MasterPath = ini.Read(_T("/InstallSettings/InstallPath"), _T("C:\\MinGW"));
            if (!wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
            {
#ifdef __WXMSW__ // for wxRegKey
                // not found...
                // look for dev-cpp installation
                wxRegKey key; // defaults to HKCR
                key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\Dev-C++"));
                if (key.Exists() && key.Open(wxRegKey::Read))
                {
                    // found; read it
                    key.QueryValue(_T("Install_Dir"), m_MasterPath);
                }
                else
                {
                    // installed by inno-setup
                    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Minimalist GNU for Windows 4.1_is1
                    // HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\TDM-GCC
                    wxString name;
                    long index;
                    key.SetName(_T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));
                    for (int i = 0; i < 2; ++i)
                    {
                        bool ok = key.GetFirstKey(name, index);
                        while (ok && !name.StartsWith(wxT("Minimalist GNU for Windows")) && name != wxT("TDM-GCC"))
                        {
                            ok = key.GetNextKey(name, index);
                        }
                        if (ok)
                        {
                            name = key.GetName() + wxT("\\") + name;
                            key.SetName(name);
                            if (key.Exists() && key.Open(wxRegKey::Read))
                            {
                                key.QueryValue(wxT("InstallLocation"), m_MasterPath);
                                // determine configuration, eg: "x86_64-w64-mingw32-gcc.exe"
                                wxDir binFolder(m_MasterPath + sep + wxT("bin"));
                                if (binFolder.IsOpened() && binFolder.GetFirst(&name, wxT("*mingw32-gcc*.exe"), wxDIR_FILES))
                                {
                                    m_Programs.C = name;
                                    while (binFolder.GetNext(&name))
                                    {
                                        if (name.Length() < m_Programs.C.Length())
                                            m_Programs.C = name; // avoid "x86_64-w64-mingw32-gcc-4.8.1.exe"
                                    }
                                    m_Programs.CPP = m_Programs.C;
                                    m_Programs.CPP.Replace(wxT("mingw32-gcc"), wxT("mingw32-g++"));
                                    m_Programs.LD = m_Programs.CPP;
                                    break;
                                }
                            }
                        }
                        // on 64 bit Windows
                        key.SetName(wxT("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall"));
                    }
                }
#endif
            }
            // check for PortableApps.com installation
            if (!wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C))
            {
                wxString drive = wxFileName(ConfigManager::GetExecutableFolder()).GetVolume() + wxT(":\\");
                if (wxFileExists(drive + wxT("PortableApps\\CommonFiles\\MinGW\\bin\\") + m_Programs.C))
                    m_MasterPath = drive + wxT("PortableApps\\CommonFiles\\MinGW");
                else if (wxFileExists(drive + wxT("CommonFiles\\MinGW\\bin\\") + m_Programs.C))
                    m_MasterPath = drive + wxT("CommonFiles\\MinGW");
                else if (wxFileExists(drive + wxT("MinGW\\bin\\") + m_Programs.C))
                    m_MasterPath = drive + wxT("MinGW");
            }
        }
        else
            m_Programs.MAKE = _T("make.exe"); // we distribute "make" not "mingw32-make"
    }
    else
        m_MasterPath = _T("/usr");

    AutoDetectResult ret = wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
    // don't add lib/include dirs. GCC knows where its files are located

    SetVersionString();
    return ret;
}

void CompilerMINGW::SetVersionString()
{
//    Manager::Get()->GetLogManager()->DebugLog(_T("Compiler detection for compiler ID: '") + GetID() + _T("' (parent ID= '") + GetParentID() + _T("')"));

    wxArrayString output, errors;
    wxString sep = wxFileName::GetPathSeparator();
    wxString master_path = m_MasterPath;
    wxString compiler_exe = m_Programs.C;

    /* We should read the master path from the configuration manager as
     * the m_MasterPath is empty if AutoDetectInstallationDir() is not
     * called
     */
    ConfigManager* cmgr = Manager::Get()->GetConfigManager(_T("compiler"));
    if (cmgr)
    {
        wxString settings_path;
        wxString compiler_path;
        /* Differ between user-defined compilers (copies of base compilers) */
        if (GetParentID().IsEmpty())
        {
            settings_path = _T("/sets/")      + GetID() + _T("/master_path");
            compiler_path = _T("/sets/")      + GetID() + _T("/c_compiler");
        }
        else
        {
            settings_path = _T("/user_sets/") + GetID() + _T("/master_path");
            compiler_path = _T("/user_sets/") + GetID() + _T("/c_compiler");
        }
        cmgr->Read(settings_path, &master_path);
        cmgr->Read(compiler_path, &compiler_exe);
    }
    if (master_path.IsEmpty())
    {
        /* Notice: In general this is bad luck as e.g. all copies of a
         * compiler have a different path, most likely.
         * Thus the following might even return a wrong command!
         */
        if (platform::windows)
            master_path = _T("C:\\MinGW");
        else
            master_path = _T("/usr");
    }
    wxString gcc_command = master_path + sep + _T("bin") + sep + compiler_exe;

    Manager::Get()->GetMacrosManager()->ReplaceMacros(gcc_command);
    if (!wxFileExists(gcc_command))
    {
//        Manager::Get()->GetLogManager()->DebugLog(_T("Compiler version detection: Compiler not found: ") + gcc_command);
        return;
    }

//    Manager::Get()->GetLogManager()->DebugLog(_T("Compiler version detection: Issuing command: ") + gcc_command);

    int flags = wxEXEC_SYNC;
#if wxCHECK_VERSION(2, 9, 0)
    // Stop event-loop while wxExecute runs, to avoid a deadlock on startup,
    // that occurs from time to time on wx2.9
    flags |= wxEXEC_NOEVENTS;
#else
    flags |= wxEXEC_NODISABLE;
#endif
    long result = wxExecute(gcc_command + _T(" --version"), output, errors, flags );
    if(result != 0)
    {
//        Manager::Get()->GetLogManager()->DebugLog(_T("Compiler version detection: Error executing command."));
    }
    else
    {
        if (output.GetCount() > 0)
        {
//            Manager::Get()->GetLogManager()->DebugLog(_T("Extracting compiler version from: ") + output[0]);
            wxRegEx reg_exp;
            if (reg_exp.Compile(_T("[0-9][.][0-9][.][0-9]")) && reg_exp.Matches(output[0]))
            {
                m_VersionString = reg_exp.GetMatch(output[0]);
//                Manager::Get()->GetLogManager()->DebugLog(_T("Compiler version via RegExp: ") + m_VersionString);
            }
            else
            {
                m_VersionString = output[0].Mid(10);
                m_VersionString = m_VersionString.Left(5);
                m_VersionString.Trim(false);
//                Manager::Get()->GetLogManager()->DebugLog(_T("Compiler version: ") + m_VersionString);
            }
        }
    }
}
