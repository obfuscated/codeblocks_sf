/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "compilerMSVC.h"
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif // __WXMSW__

CompilerMSVC::CompilerMSVC()
    : Compiler(_("Microsoft Visual C++ Toolkit 2003"), _T("msvctk"))
{
    m_Weight = 8;
    Reset();
}

CompilerMSVC::~CompilerMSVC()
{
    //dtor
}

Compiler * CompilerMSVC::CreateCopy()
{
    return (new CompilerMSVC(*this));
}

AutoDetectResult CompilerMSVC::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();

    // Read the VCToolkitInstallDir environment variable
    wxGetEnv(_T("VCToolkitInstallDir"), &m_MasterPath);

    if (m_MasterPath.IsEmpty())
    {
        // just a guess; the default installation dir
        wxString Programs = _T("C:\\Program Files");
        // what's the "Program Files" location
        // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
        //                              64 bit apps are in "ProgramFiles"
        wxGetEnv(_T("ProgramFiles"), &Programs);
        m_MasterPath = Programs + _T("\\Microsoft Visual C++ Toolkit 2003");
    }
    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));

#ifdef __WXMSW__
        // add include dirs for MS Platform SDK too
        wxRegKey key; // defaults to HKCR
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\Win32SDK\\Directories"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            wxString dir;
            key.QueryValue(_T("Install Dir"), dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) != '\\')
                    dir += sep;
                AddIncludeDir(dir + _T("include"));
                AddLibDir(dir + _T("lib"));
                m_ExtraPaths.Add(dir + _T("bin"));
            }
        }

        // add extra paths for "Debugging tools" too
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\DebuggingTools"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            wxString dir;
            key.QueryValue(_T("WinDbg"), dir);
            if (!dir.IsEmpty())
            {
                if (dir.GetChar(dir.Length() - 1) == '\\')
                    dir.Remove(dir.Length() - 1, 1);
                m_ExtraPaths.Add(dir);
            }
        }
#endif // __WXMSW__
    }

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}
