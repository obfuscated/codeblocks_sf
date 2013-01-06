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

CompilerIAR8051::CompilerIAR8051()
    : Compiler(_("IAR 8051 Compiler"), _T("iar8051"))
{
    m_Weight = 75;
    Reset();
}

CompilerIAR8051::~CompilerIAR8051()
{
    //dtor
}

Compiler * CompilerIAR8051::CreateCopy()
{
    return (new CompilerIAR8051(*this));
}

AutoDetectResult CompilerIAR8051::AutoDetectInstallationDir()
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
        if (m_MasterPath.IsEmpty())
        {
            wxDir dir(wxT("C:\\Program Files\\IAR Systems"));
            if (wxDirExists(dir.GetName()) && dir.IsOpened())
            {
                wxString filename;
                bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
                while (cont)
                {
                    if ( filename.StartsWith(wxT("Embedded Workbench")) )
                    {
                        wxFileName fn(dir.GetName() + wxFILE_SEP_PATH + filename + wxFILE_SEP_PATH +
                                      wxT("8051") + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C);
                        if (   wxFileName::IsFileExecutable(fn.GetFullPath())
                            && (m_MasterPath.IsEmpty() || fn.GetPath() > m_MasterPath) )
                        {
                            m_MasterPath = dir.GetName() + wxFILE_SEP_PATH + filename + wxFILE_SEP_PATH + wxT("8051");
                        }
                    }
                    cont = dir.GetNext(&filename);
                }
            }
        }
        if (m_MasterPath.IsEmpty())
        {
            // just a guess; the default installation dir
            m_MasterPath = wxT("C:\\Program Files\\IAR Systems\\Embedded Workbench\\8051");
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
    AddLinkerOption(wxT("-f \"") + m_MasterPath + wxFILE_SEP_PATH + wxT("config") + wxFILE_SEP_PATH +
                    wxT("devices") + wxFILE_SEP_PATH + wxT("_generic") + wxFILE_SEP_PATH +
                    wxT("lnk51ew_plain.xcl\""));

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}
