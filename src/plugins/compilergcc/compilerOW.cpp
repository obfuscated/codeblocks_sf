/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "compilerOW.h"
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#include <wx/utils.h>

#include <logmanager.h>
#include <manager.h>
#include "compilerOWgenerator.h"

#include <wx/utils.h>
#include <wx/filefn.h>

CompilerOW::CompilerOW()
    : Compiler(wxT("OpenWatcom (W32) Compiler"), _T("ow"))
{
    m_Weight = 28;
    Reset();
}

CompilerOW::~CompilerOW()
{
	//dtor
}

Compiler * CompilerOW::CreateCopy()
{
    return (new CompilerOW(*this));
}

CompilerCommandGenerator* CompilerOW::GetCommandGenerator(cbProject *project)
{
    CompilerOWGenerator *generator = new CompilerOWGenerator;
    generator->Init(project);
    return generator;
}

AutoDetectResult CompilerOW::AutoDetectInstallationDir()
{
    /* Following code is Not necessary as OpenWatcom does not write to
       Registry anymore */
    /*wxRegKey key; // defaults to HKCR
    key.SetName(wxT("HKEY_LOCAL_MACHINE\\Software\\Open Watcom\\c_1.0"));
    if (key.Open())
        // found; read it
        key.QueryValue(wxT("Install Location"), m_MasterPath);*/

    if (m_MasterPath.IsEmpty())
        // just a guess; the default installation dir
        m_MasterPath = wxT("C:\\watcom");

    if (!m_MasterPath.IsEmpty())
    {
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h"));
        AddIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h") + wxFILE_SEP_PATH + wxT("nt"));
        AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib386"));
        AddLibDir(m_MasterPath + wxFILE_SEP_PATH + wxT("lib386") + wxFILE_SEP_PATH + wxT("nt"));
        AddResourceIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h"));
        AddResourceIncludeDir(m_MasterPath + wxFILE_SEP_PATH + wxT("h") + wxFILE_SEP_PATH + wxT("nt"));
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("binnt"));
        m_ExtraPaths.Add(m_MasterPath + wxFILE_SEP_PATH + wxT("binw"));
    }
    wxSetEnv(wxT("WATCOM"), m_MasterPath);

    return wxFileExists(m_MasterPath + wxFILE_SEP_PATH + wxT("binnt") + wxFILE_SEP_PATH + m_Programs.C) ? adrDetected : adrGuessed;
}

void CompilerOW::LoadSettings(const wxString& baseKey)
{
    Compiler::LoadSettings(baseKey);
    wxSetEnv(wxT("WATCOM"), m_MasterPath);
}

void CompilerOW::SetMasterPath(const wxString& path)
{
    Compiler::SetMasterPath(path);
    wxSetEnv(wxT("WATCOM"), m_MasterPath);
}
