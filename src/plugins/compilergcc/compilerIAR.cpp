/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include <prep.h>
#include "compilerIAR.h"
#include <wx/dir.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerIAR::CompilerIAR(wxString arch)
    : Compiler(_("IAR ") + arch + _(" Compiler"), _T("iar") + arch)
{
    m_Weight = 75;
    m_Arch = arch;
    Reset();
}

CompilerIAR::~CompilerIAR()
{
    //dtor
}

Compiler * CompilerIAR::CreateCopy()
{
    return (new CompilerIAR(*this));
}

AutoDetectResult CompilerIAR::AutoDetectInstallationDir()
{
    if (platform::windows)
    {
        m_MasterPath.Clear();
#ifdef __WXMSW__ // for wxRegKey
        wxRegKey key;   // defaults to HKCR
        key.SetName(wxT("HKEY_LOCAL_MACHINE\\Software\\IAR Systems\\Installed Products"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            wxString subkeyname;
            long idx;
            if (key.GetFirstKey(subkeyname, idx))
            {
                do
                {
                    wxRegKey keys;
                    keys.SetName(key.GetName() + wxFILE_SEP_PATH + subkeyname);
                    if (!keys.Exists() || !keys.Open(wxRegKey::Read))
                        continue;
                    keys.QueryValue(wxT("TargetDir"), m_MasterPath);
                    if (!m_MasterPath.IsEmpty())
                    {
                        if (wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C))
                            break;
                        m_MasterPath.Clear();
                    }
                } while (key.GetNextKey(subkeyname, idx));
            }
        }
#endif // __WXMSW__
        wxString env_path = wxGetenv(_T("ProgramFiles(x86)"));
        if (m_MasterPath.IsEmpty())
        {
            wxDir dir(env_path + wxT("\\IAR Systems"));
            if (wxDirExists(dir.GetName()) && dir.IsOpened())
            {
                wxString filename;
                bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
                while (cont)
                {
                    if ( filename.StartsWith(wxT("Embedded Workbench")) )
                    {
                        wxFileName fn(dir.GetName() + wxFILE_SEP_PATH + filename + wxFILE_SEP_PATH +
                                      m_Arch + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C);
                        if (   wxFileName::IsFileExecutable(fn.GetFullPath())
                            && (m_MasterPath.IsEmpty() || fn.GetPath() > m_MasterPath) )
                        {
                            m_MasterPath = dir.GetName() + wxFILE_SEP_PATH + filename + wxFILE_SEP_PATH + m_Arch;
                        }
                    }
                    cont = dir.GetNext(&filename);
                }
            }
        }
        if (m_MasterPath.IsEmpty())
        {
            // just a guess; the default installation dir
            m_MasterPath = env_path + wxT("\\IAR Systems\\Embedded Workbench\\" + m_Arch);
        }

        if ( wxDirExists(m_MasterPath) )
        {
            AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("include"));
            AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib") + wxFILE_SEP_PATH + wxT("clib"));
            m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("bin"));
        }
    }
    else
    {
        m_MasterPath=_T("/usr/local"); // default
    }
    if (m_Arch == wxT("8051"))
    {
        AddLinkerOption(wxT("-f \"") + m_MasterPath + wxFILE_SEP_PATH + wxT("config") + wxFILE_SEP_PATH +
                        wxT("devices") + wxFILE_SEP_PATH + wxT("_generic") + wxFILE_SEP_PATH +
                        wxT("lnk51ew_plain.xcl\""));
    }
    else // IAR
    {
        AddCompilerOption(wxT("--no_wrap_diagnostics"));
    }
    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}
