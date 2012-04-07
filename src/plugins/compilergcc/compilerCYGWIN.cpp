/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#ifdef __WXMSW__
// this compiler is valid only in windows

#include "compilerCYGWIN.h"
#include <wx/filefn.h>
#include <wx/msw/registry.h>

CompilerCYGWIN::CompilerCYGWIN()
    : CompilerMINGW(_("Cygwin GCC"), _T("cygwin"))
{
    Reset();
}

CompilerCYGWIN::~CompilerCYGWIN()
{
}

Compiler * CompilerCYGWIN::CreateCopy()
{
    Compiler* c = new CompilerCYGWIN(*this);
    c->SetExtraPaths(m_ExtraPaths); // wxArrayString doesn't seem to be copied with the default copy ctor...
    return c;
}

void CompilerCYGWIN::Reset()
{
    CompilerMINGW::Reset();

    // NOTE: Cygwin's gcc.exe maybe a file link and
    // is not a good default name for running via cmd.exe
    // TODO: May also be gcc-4.exe!!!
    m_Programs.C = _T("gcc-3.exe");
    m_Programs.CPP = _T("g++-3.exe");
    m_Programs.LD = _T("g++-3.exe");
    m_Programs.DBG = _T("gdb.exe");
    m_Programs.DBGconfig = _T("gdb_debugger:Default");
    m_Programs.LIB = _T("ar.exe");
    m_Programs.WINDRES = _T("windres.exe");
    m_Programs.MAKE = _T("make.exe");

    m_Switches.forceFwdSlashes = true;

    m_Options.AddOption(_("Do not use cygwin specific functionality"), _T("-mno-cygwin"), _("General"));
}

AutoDetectResult CompilerCYGWIN::AutoDetectInstallationDir()
{
    AutoDetectResult ret = adrGuessed;
    m_MasterPath = _T("C:\\Cygwin"); // just a guess
    wxString tempMasterPath(m_MasterPath);
    bool validInstallationDir = false;

    // look in registry for Cygwin

    wxRegKey key; // defaults to HKCR
    key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\Cygwin\\setup"));
    if (key.Exists() && key.Open(wxRegKey::Read))
    {
        // found CygWin version 1.7 or newer; read it
        key.QueryValue(_T("rootdir"), tempMasterPath);
        if (wxDirExists(tempMasterPath + wxFILE_SEP_PATH + _T("bin")))
                validInstallationDir = true;
    }
    if (!validInstallationDir)
    {
        key.SetName(_T("HKEY_LOCAL_MACHINE\\Software\\Cygnus Solutions\\Cygwin\\mounts v2\\/"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            // found CygWin version 1.5 or older; read it
            key.QueryValue(_T("native"), tempMasterPath);
            if ( wxDirExists(tempMasterPath + wxFILE_SEP_PATH + _T("bin")) )
                validInstallationDir = true;
        }
    }

    if (!validInstallationDir)
        return ret;

    wxString cProgramDir = tempMasterPath + wxFILE_SEP_PATH + _T("bin") + wxFILE_SEP_PATH;
    wxString cProgramFullname = cProgramDir + m_Programs.C;
    if ( !wxFileExists(cProgramFullname) )
        return ret;

    wxFile pfFile(cProgramFullname);
    if ( !pfFile.IsOpened() )
       return ret;

    char buffer[10] = {0};
    pfFile.Read(buffer,10);
    if (memcmp("!<symlink>", buffer, 10) != 0)
    {
        m_MasterPath = tempMasterPath;
        ret = adrDetected;
    }

    return ret;
}

#endif // __WXMSW__
