/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/string.h>
    #include "cbproject.h"
    #include "compilerfactory.h"
    #include "logmanager.h"
    #include <map>
#endif

#if defined(__WXMSW__)
    #include <wx/msw/registry.h>
#endif // defined(__WXMSW__)

#include "cygwin.h"

// Keep a cache of all file paths converted from
// Cygwin path into native path. Only applicable if under Windows and using Cygwin!
static std::map<wxString, wxString> g_WindowsFileCache;
static std::map<wxString, wxString> g_CygwinFileCache;

static bool g_CygwinPresent = false; // Assume not found until Cygwin has been found
static wxString g_CygwinCompilerPathRoot; // Assume no directory until Cygwin has been found

wxString cbGetCygwinCompilerPathRoot(void)
{
    if (g_CygwinPresent == false)
        g_CygwinPresent = cbIsDetectedCygwinCompiler();
    return g_CygwinCompilerPathRoot;
}

// Routine find if Windows Cygwin compiler has been installed on the PC
bool cbIsDetectedCygwinCompiler(void)
{
    if (!platform::windows)
        return false;

    LogManager *pMsg = Manager::Get()->GetLogManager();

    // can only debug projects or attach to processes
    ProjectManager *prjMan = Manager::Get()->GetProjectManager();
    cbProject *pProject = prjMan->GetActiveProject();

    if (pProject && prjMan->IsProjectStillOpen(pProject))
    {
        ProjectBuildTarget *ActiveBuildTarget = nullptr;

        wxString sActiveBuildTarget = pProject->GetActiveBuildTarget();
        ActiveBuildTarget = pProject->GetBuildTarget(sActiveBuildTarget);

        wxString compilerID;
        pMsg->DebugLog(wxString::Format("sActiveBuildTarget : %s", sActiveBuildTarget));
        if (ActiveBuildTarget)
        {
            pMsg->DebugLog(wxString::Format("ActiveBuildTarget->GetTitle() : %s",
                                            ActiveBuildTarget->GetTitle()));
            compilerID = ActiveBuildTarget->GetCompilerID();
            if (!compilerID.IsSameAs("cygwin"))
            {
                pMsg->DebugLog("ActiveBuildTarget->GetCompilerID().IsSameAs(cygwin) is FALSE!");
                g_CygwinCompilerPathRoot = wxString();
                return false;
            }
        }
        else
        {
            pMsg->DebugLog(wxString::Format("pProject->GetTitle() : %s", pProject->GetTitle()));
            compilerID = pProject->GetCompilerID();
            if (!compilerID.IsSameAs("cygwin"))
            {
                pMsg->DebugLog("pProject->GetCompilerID().IsSameAs(cygwin) is FALSE!");
                g_CygwinCompilerPathRoot = wxString();
                return false;
            }
        }

        // find the target's compiler (to see which debugger to use)
        Compiler *actualCompiler = CompilerFactory::GetCompiler(compilerID);
        if (!actualCompiler)
        {
            pMsg->DebugLog("Could not find actual CygWin compiler!!!");
            g_CygwinCompilerPathRoot = wxString();
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

    bool present = false; // Assume not found as starting point

#if defined(__WXMSW__)
    wxString masterPath("C:\\cygwin64");
    wxRegKey key; // defaults to HKCR
    key.SetName("HKEY_LOCAL_MACHINE\\Software\\Cygwin\\setup");
    if (key.Exists() && key.Open(wxRegKey::Read))
    {
        // found CygWin version 1.7 or newer; read it
        key.QueryValue("rootdir", masterPath);
        if (wxDirExists(masterPath + wxFILE_SEP_PATH + "bin"))
            present = true;
    }
    if (!present)
    {
        key.SetName("HKEY_LOCAL_MACHINE\\Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/");
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            // found CygWin version 1.5 or older; read it
            key.QueryValue("native", masterPath);
            if (wxDirExists(masterPath + wxFILE_SEP_PATH + "bin"))
                present = true;
        }
    }

    // Found registry keys or the default path is valid
    if (present || wxDirExists(masterPath + wxFILE_SEP_PATH + "bin"))
    {
        present = true;  // Set to true in case no registry found, but default directory exists.
        g_CygwinCompilerPathRoot = masterPath; // convert to wxString type for later use
    }
    else
    {
        g_CygwinCompilerPathRoot = wxString();
    }
#else
    g_CygwinCompilerPathRoot = wxString();
#endif // defined(__WXMSW__)

    return present;
}

static wxString GetCygwinPath(const wxString& path, bool windowsPath)
{
    std::map<wxString, wxString> &fileCache = (windowsPath ? g_WindowsFileCache : g_CygwinFileCache);

    // Check if we already have the file cached before
    std::map<wxString, wxString>::const_iterator it = fileCache.find(path);
    if (it != fileCache.end())
        return it->second;

    wxString resultPath = path;

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


    // As we are under Windows we check 'resultPath' to see if it starts with '/cygdrive' and
    // if it does then process the path as a cygwin path.
    // But if the path does not then check if the path/file exists and if it does then use it,
    // otherwise we check if it is a cygwin path starting with '/' and if it is then we call cygpath.exe
    // if we have not already seen the path and if we have not then later we add the path to the cache
    if ((resultPath.StartsWith("/cygdrive/")) || (resultPath.StartsWith("\\cygdrive\\")))
    {
        // Needed if debugging a Cygwin build app in codeblocks. Convert GDB cygwin filename to mingw filename!!!!
        // /cygdrive/x/... to c:/...
        resultPath = wxString::Format("%c:%s", path[10], path.Mid(11));
    }
    else
    {
        // Check if path or file exists on the disk
        if (((!wxDirExists(resultPath)) && (!wxFileName::FileExists(resultPath))) || !windowsPath)
        {
            // Double check that starts with a forward slash "/" and if it is
            // then assume it is a special Cygwin path that cygpath.exe can resolve
            // to a valid Windows path.
            if (resultPath.StartsWith("/") || !windowsPath)
            {
                // file attribute also contains cygwin path
                wxString cygwinConvertCMD = g_CygwinCompilerPathRoot + "\\bin\\cygpath.exe";
                if (wxFileName::FileExists(cygwinConvertCMD))
                {
                    cygwinConvertCMD.Trim().Trim(false);
                    // we got a conversion command from the user, use it
                    if (windowsPath)
                        cygwinConvertCMD.Append(" -w ");
                    else
                        cygwinConvertCMD.Append(" -u ");
                    cygwinConvertCMD.Append(resultPath);

                    wxArrayString cmdOutput;
                    const long resExecute = wxExecute("cmd /c " + cygwinConvertCMD, cmdOutput,
                                                      wxEXEC_SYNC, nullptr);
                    if ((resExecute== 0) && (!cmdOutput.IsEmpty()))
                    {
                        cmdOutput.Item(0).Trim().Trim(false);
                        const wxString &outputPath = cmdOutput.Item(0);

                        // Check if path or file exists on the disk
                        if ((wxDirExists(outputPath)) || (wxFileName::FileExists(outputPath)))
                            resultPath = outputPath;
                        else if (!windowsPath)
                            resultPath = outputPath;
                    }
                    else
                    {
                        const wxString msg = wxString::Format("cygwinConvertCMD error: %d",
                                                              resExecute);
                        Manager::Get()->GetLogManager()->DebugLog(msg);
                    }
                }
            }
        }
    }

    if (windowsPath)
    {
        // Convert Unix filenames to Windows
        resultPath.Replace("/", "\\");
    }

    fileCache.insert(std::map<wxString, wxString>::value_type(path, resultPath));
    return resultPath;
}

void cbGetWindowsPathFromCygwinPath(wxString& path)
{
    path = GetCygwinPath(path, true);
}
void cbGetCygwinPathFromWindowsPath(wxString& path)
{
    path = GetCygwinPath(path, false);
}
