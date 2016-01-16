/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "compilerCYGWIN.h"
#include <wx/filefn.h>
#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif // __WXMSW__

CompilerCYGWIN::CompilerCYGWIN()
    : CompilerMINGW(_("Cygwin GCC"), _T("cygwin"))
{
    m_Weight = 32;
    Reset();
}

CompilerCYGWIN::~CompilerCYGWIN()
{
}

Compiler * CompilerCYGWIN::CreateCopy()
{
    return (new CompilerCYGWIN(*this));
}

AutoDetectResult CompilerCYGWIN::AutoDetectInstallationDir()
{
    AutoDetectResult ret = adrGuessed;
    m_MasterPath = _T("C:\\Cygwin"); // just a guess
    wxString tempMasterPath(m_MasterPath);
    bool validInstallationDir = false;

    // look in registry for Cygwin

#ifdef __WXMSW__
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
#endif // __WXMSW__

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
