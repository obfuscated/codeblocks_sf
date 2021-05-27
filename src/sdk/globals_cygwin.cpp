/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifdef __WXMSW__
// Only used on Windows

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
//    #include <wx/choicdlg.h>
//    #include <wx/dcmemory.h>
//    #include <wx/file.h>
//    #include <wx/filename.h>
//    #include <wx/filesys.h>
//    #include <wx/image.h>
//    #include <wx/imaglist.h>
//    #include <wx/listctrl.h>
//    #include <wx/menu.h>
//    #include <wx/settings.h>
//    #include <wx/textdlg.h>
//
//    #include "wx/wxscintilla.h"
//
//    #include "cbexception.h"
//    #include "configmanager.h" // ReadBool
//    #include "filemanager.h"
//    #include "globals.h"
//    #include "logmanager.h"
//    #include "manager.h"
//    #include "projectmanager.h"
#endif

#include "globals_cygwin.h"
#include "cbproject.h"
#include "compilerfactory.h"
#include "logmanager.h"
#include "wx/msw/wrapwin.h"     // Wraps windows.h
#include <wx/msw/registry.h>    // for Registry detection of Cygwin

// Keep a cache of all file paths converted from
// Cygwin path into native path . Only applicable if under Windows and using Cygwin!
static std::map<wxString, wxString> g_fileCache;

static bool m_CygwinPresent = false;    // Assume not found until Cygwin has been found
static wxString m_CygwinCompilerPathRoot = _T("");     // Assume no directory until Cygwin has been found

wxString getCygwinCompilerPathRoot(void)
{
    if (m_CygwinPresent == false)
    {
        m_CygwinPresent = isDetectedCygwinCompiler();
    }
    return m_CygwinCompilerPathRoot;
}

// Routine find if Windows Cygwin compiler has been installed on the PC
bool isDetectedCygwinCompiler(void)
{
    LogManager* pMsg = Manager::Get()->GetLogManager();

    // can only debug projects or attach to processes
    Compiler* actualCompiler = 0;
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* pProject = prjMan->GetActiveProject();

    if (pProject && prjMan->IsProjectStillOpen(pProject))
    {
        ProjectBuildTarget* ActiveBuildTarget = 0;

        wxString sActiveBuildTarget = pProject->GetActiveBuildTarget();
        ActiveBuildTarget = pProject->GetBuildTarget(sActiveBuildTarget);

        pMsg->DebugLog(wxString::Format(wxT("sActiveBuildTarget : %s"), sActiveBuildTarget));
        if (ActiveBuildTarget)
        {
            pMsg->DebugLog(wxString::Format(wxT("ActiveBuildTarget->GetTitle() : %s"), ActiveBuildTarget->GetTitle()));
            if (!ActiveBuildTarget->GetCompilerID().IsSameAs(_T("cygwin")))
            {
                pMsg->DebugLog(wxString::Format(wxT("ActiveBuildTarget->GetCompilerID().IsSameAs(_T(cygwin) is FALSE!")));
                m_CygwinCompilerPathRoot = _T("");
                return false;
            }
        }
        else
        {
            pMsg->DebugLog(wxString::Format(wxT("pProject->GetTitle() : %s"), pProject->GetTitle()));
            if (!pProject->GetCompilerID().IsSameAs(_T("cygwin")))
            {
                pMsg->DebugLog(wxString::Format(wxT("pProject->GetCompilerID().IsSameAs(_T(cygwin) is FALSE!")));
                m_CygwinCompilerPathRoot = _T("");
                return false;
            }
        }

        // find the target's compiler (to see which debugger to use)
        actualCompiler = CompilerFactory::GetCompiler(ActiveBuildTarget ? ActiveBuildTarget->GetCompilerID() : pProject->GetCompilerID());

        if (!actualCompiler)
        {
            pMsg->DebugLog(wxString::Format(wxT("Could not find actual CygWin compiler!!!")));
            m_CygwinCompilerPathRoot = _T("");
            return false;
        }
    }

    // See compilerCYGWIN.CPP AutoDetectResult CompilerCYGWIN::AutoDetectInstallationDir() as this is very similar!!!
    //
    //  Windows 10 x64 with Cygwin installed after 2010 have the following registry entry (or HKEY_CURRENT_USER):
    //  [HKEY_LOCAL_MACHINE\SOFTWARE\Cygwin\setup]
    //  "rootdir"="C:\\cygwin64"
    //
    //  See
    //  https://github.com/mirror/newlib-cygwin/blob/30782f7de4936bbc4c2e666cbaf587039c895fd3/winsup/utils/path.cc
    //  for RegQueryValueExW (...)

    wxString m_MasterPath = _T("C:\\cygwin64"); // just a guess
    wxString tempMasterPath(m_MasterPath);
    bool m_CygwinPresent = false; // Assume not found as starting point

    wxRegKey key; // defaults to HKCR
    key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\Cygwin\\setup"));
    if (key.Exists() && key.Open(wxRegKey::Read))
    {
        // found CygWin version 1.7 or newer; read it
        key.QueryValue(_T("rootdir"), tempMasterPath);
        if (wxDirExists(tempMasterPath + wxFILE_SEP_PATH + _T("bin")))
        {
           m_CygwinPresent = true;
        }
    }
    if (!m_CygwinPresent)
    {
        key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            // found CygWin version 1.5 or older; read it
            key.QueryValue(_T("native"), tempMasterPath);
            if ( wxDirExists(tempMasterPath + wxFILE_SEP_PATH + _T("bin")) )
            {
                m_CygwinPresent = true;
            }
        }
    }
    if (m_CygwinPresent)
    {
        m_CygwinCompilerPathRoot = tempMasterPath; // convert to wxString type for later use
    }
    else
    {
        m_CygwinCompilerPathRoot = _T("");
    }

    return m_CygwinPresent;
}

static void GetCygwinPath(wxString& path, bool bWindowsPath)
{
    // Check if we already have the file cached before
    if(g_fileCache.find(path) != g_fileCache.end())
    {
        // Manager::Get()->GetLogManager()->DebugLog(wxString::Format(wxT("g_fileCache found for: %s\n\r                     : %s"), path, g_fileCache.find(path)->second));
        path = g_fileCache.find(path)->second;
    }
    else
    {
        wxString pathOriginal = path;
        unsigned int i=0, EscCount=0;

        // preserve any escape characters at start of path - this is true for
        // breakpoints - value is 2, but made dynamic for safety as we
        // are only checking for the prefix not any furthur correctness
        // Keep this code otherwise you will encounter strange debugging file issues.
        if (path.GetChar(0) == g_EscapeChar)
        {
            while ( (i<path.Len()) && (path.GetChar(i)==g_EscapeChar) )
            {
                // get character
                EscCount++;
                i++;
            }
        }
        wxString PathWithoutEsc(path);
        PathWithoutEsc.Remove(0, EscCount);
        wxString resultPath = PathWithoutEsc;

        // File Examples
        //   "C:\cygwin64\usr\include\ctype.h"
        //   "/cygdrive/C/cygwin64/usr/include/ctype.h"
        //   "C:\\cygwin64\\usr\\src\\debug\\zziplib-0.13.68-1\\zzip\\file.c";
        // The following use cygpath.exe to resolve the true Windows path:
        //    "/usr/src/debug/zziplib-0.13.68-1/zzip/file.c";
        //    "/lib/...."
        //    "/usr/local/bin"
        //    "/usr/bin"
        //    "/usr/sbin"
        //    "/bin"
        //    "/sbin"
        //    other "/..."

        // By default we use the 'resultPath' as our file name (esc removed).
        // As we are under Windows we check 'resultPath' tom see if it starts with '/cygdrive' and
        // if it does then process the path as a cygwin path.
        // But if the path does not then check if the path/file exists and if it does then use it,
        // otherwise we check if it is a cygwin path starting with '/' and if it is then we call cygpath.exe
        // if we have not allready seen the path and if we have not then later we add the path to the cache
        if ((resultPath.StartsWith(wxT("/cygdrive/"))) || (resultPath.StartsWith(wxT("\\cygdrive\\"))))
        {
            // Needed if debugging a Cygwin build app in codeblocks. Convert GDB cygwin filemname to mingw filename!!!!
            // /cygdrive/x/... to c:/...
            wxString tmpfilename  = PathWithoutEsc;
            tmpfilename.Remove(0,11);

            resultPath.Clear();
            resultPath.Append(PathWithoutEsc[10]);
            resultPath.Append(_T(":"));
            resultPath.Append(tmpfilename);
        }
        else
        {
            // Check if path or file exists on the disk
            if (((!wxDirExists(resultPath)) && (!wxFileName::FileExists(resultPath))) || !bWindowsPath)
            {
                // Double check that starts with a forward slash "/" and if it is
                // then assume it is a special Cygwin path that cygpath.exe can resolve
                // to a valid Windows path.
                if (resultPath.StartsWith(wxT("/")) || !bWindowsPath)
                {
                    // Check if we allready have the file cached before
                    if(g_fileCache.find(path) != g_fileCache.end())
                    {
                        resultPath = g_fileCache.find(path)->second;
                    }
                    else
                    {
                        // file attribute also contains cygwin path
                        wxString cygwinConvertCMD = m_CygwinCompilerPathRoot + "\\bin\\cygpath.exe";
                        if (wxFileName::FileExists(cygwinConvertCMD))
                        {
                            cygwinConvertCMD.Trim().Trim(false);
                            // we got a conversion command from the user, use it
                            if (bWindowsPath)
                            {
                                cygwinConvertCMD.Append(wxT(" -w "));
                            }
                            else
                            {
                                cygwinConvertCMD.Append(wxT(" -u "));
                            }
                            cygwinConvertCMD.Append(wxString::Format(wxT("%s"), resultPath.c_str()));
                            wxArrayString cmdOutput;

                            long resExecute = wxExecute(_T("cmd /c ") + cygwinConvertCMD, cmdOutput, wxEXEC_SYNC, NULL );

                            if ((resExecute== 0) && (!cmdOutput.IsEmpty()))
                            {
                                cmdOutput.Item(0).Trim().Trim(false);
                                wxString outputPath = cmdOutput.Item(0);

                                // Check if path or file exists on the disk
                                if ((wxDirExists(outputPath)) || (wxFileName::FileExists(outputPath)))
                                {
                                    resultPath = outputPath;
                                }
                                else
                                {
                                    if (!bWindowsPath)
                                    {
                                        resultPath = outputPath;
                                    }
                                }
                            }
                            else
                            {
                                Manager::Get()->GetLogManager()->DebugLog(wxString::Format(wxT("cygwinConvertCMD error: %d"), resExecute));
                            }
                        }
                    }
                }
            }
        }

        if (bWindowsPath)
        {
            // Convert Unix filenames to Windows
            resultPath.Replace(wxT("/"), wxT("\\"));
        }

        // Compile corrected path
        path = wxEmptyString;
        for (i=0; i<EscCount; i++)
            path += g_EscapeChar;
        path += resultPath;

        // Add the file to the cache ( regardless of the validity of the result)
        // If result is invalid then next time we do not do any checks, so it is faster!!!
        g_fileCache[pathOriginal] = path;
    }
}

void GetWindowsPathFromCygwinPath(wxString& path)
{
    GetCygwinPath(path, true);
}
void GetCygwinPathFromWindowsPath(wxString& path)
{
    GetCygwinPath(path, false);
}

#endif // __WXMSW__
