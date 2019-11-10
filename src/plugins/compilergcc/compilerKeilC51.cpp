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
#include "compilerKeilC51.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>

#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif

CompilerKeilC51::CompilerKeilC51()
    : Compiler(_("Keil C51 Compiler"), _T("keilc51"))
{
    m_Weight = 73;
    Reset();
}

CompilerKeilC51::CompilerKeilC51(const wxString& name, const wxString& ID)
    : Compiler(name, ID)
{
    Reset();
}

CompilerKeilC51::~CompilerKeilC51()
{
    //dtor
}

Compiler * CompilerKeilC51::CreateCopy()
{
    return (new CompilerKeilC51(*this));
}

AutoDetectResult CompilerKeilC51::AutoDetectInstallationDir()
{
    if (platform::windows)
    {
#ifdef __WXMSW__ // for wxRegKey
        wxRegKey key;   // defaults to HKCR
        key.SetName(wxT("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Keil \265Vision3")); // 'backslash265' is the mu character
        if (key.Exists() && key.Open(wxRegKey::Read)) // found; read it
            key.QueryValue(wxT("LastInstallDir"), m_MasterPath);
#endif // __WXMSW__

        if (m_MasterPath.IsEmpty())
        {
            // just a guess; the default installation dir
            m_MasterPath = wxT("C:\\Keil");
        }

        m_MasterPath = m_MasterPath + wxFILE_SEP_PATH + wxT("C51");

        if ( wxDirExists(m_MasterPath) )
        {
            AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("inc"));
            AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib"));
            m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("bin"));
        }
    }
    else
        m_MasterPath=_T("/usr/local"); // default

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("bin") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}

//------------------------------------------------------------

CompilerKeilCX51::CompilerKeilCX51()
    : CompilerKeilC51(_("Keil CX51 Compiler"), _T("keilcx51"))
{
    m_Weight = 74;
}

CompilerKeilCX51::~CompilerKeilCX51()
{
    //dtor
}

Compiler * CompilerKeilCX51::CreateCopy()
{
    return (new CompilerKeilCX51(*this));
}
