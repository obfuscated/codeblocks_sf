/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>
#include "compilerMSVC8.h"
#include <wx/wx.h>
#include <wx/intl.h>
#include <wx/regex.h>
#include <wx/config.h>
#ifdef __WXMSW__
    #include <wx/msw/registry.h>
#endif // __WXMSW__

CompilerMSVC8::CompilerMSVC8()
    : Compiler(_("Microsoft Visual C++ 2005/2008"), _T("msvc8"))
{
    m_Weight = 12;
    Reset();
}

CompilerMSVC8::~CompilerMSVC8()
{
    //dtor
}

Compiler * CompilerMSVC8::CreateCopy()
{
    return (new CompilerMSVC8(*this));
}

AutoDetectResult CompilerMSVC8::AutoDetectInstallationDir()
{
    wxString sep = wxFileName::GetPathSeparator();
    wxString idepath;

    // Read the VCToolkitInstallDir environment variable
    wxGetEnv(_T("VS90COMNTOOLS"), &m_MasterPath);
    if(m_MasterPath.IsEmpty())
    {
        wxGetEnv(_T("VS80COMNTOOLS"), &m_MasterPath);
    }

    if ( !m_MasterPath.IsEmpty() )
    {
        wxFileName name = wxFileName::DirName(m_MasterPath);

        name.RemoveLastDir();
        name.AppendDir(_T("IDE"));
        idepath = name.GetPath();
        if ( !wxDirExists(idepath) )
            idepath = _T("");

        name.RemoveLastDir();
        name.RemoveLastDir();
        name.AppendDir(_T("VC"));
        m_MasterPath = name.GetPath();
        if ( !wxDirExists(m_MasterPath) )
            m_MasterPath = _T("");
    }

    if (m_MasterPath.IsEmpty())
    {
        // just a guess; the default installation dir
        wxString Programs = _T("C:\\Program Files");
        // what's the "Program Files" location
        // TO DO : support 64 bit ->    32 bit apps are in "ProgramFiles(x86)"
        //                              64 bit apps are in "ProgramFiles"
        wxGetEnv(_T("ProgramFiles"), &Programs);
        m_MasterPath = Programs + _T("\\Microsoft Visual Studio 9.0\\VC");
        idepath = Programs + _T("\\Microsoft Visual Studio 9.0\\Common7\\IDE");
        if(!wxDirExists(m_MasterPath))
        {
            m_MasterPath = Programs + _T("\\Microsoft Visual Studio 8\\VC");
            idepath = Programs + _T("\\Microsoft Visual Studio 8\\Common7\\IDE");
        }
    }

    if (!m_MasterPath.IsEmpty())
    {
        bool sdkfound = false;
        wxString dir;

        // we need to add the IDE path, as the compiler requires some DLL present there
        m_ExtraPaths.Add(idepath);

#ifdef __WXMSW__
        wxRegKey key; // defaults to HKCR
        // try to detect Platform SDK (old versions)
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\Win32SDK\\Directories"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            key.QueryValue(_T("Install Dir"), dir);
            if (!dir.IsEmpty() && wxDirExists(dir))
                sdkfound = true;
            key.Close();
        }

        // try to detect Platform SDK (newer versions)
        wxString msPsdkKeyName[2] = { _T("HKEY_CURRENT_USER\\Software\\Microsoft\\MicrosoftSDK\\InstalledSDKs"),
                                      _T("HKEY_CURRENT_USER\\Software\\Microsoft\\Microsoft SDKs\\Windows") };
        wxString msPsdkKeyValue[2] = { _T("Install Dir"), _T("InstallationFolder") };
        for (int i = 0; i < 2; ++i)
        {
            key.SetName(msPsdkKeyName[i]);
            if (!sdkfound && key.Exists() && key.Open(wxRegKey::Read))
            {
                wxString name;
                long idx;
                bool cont = key.GetFirstKey(name, idx);

                while(cont)
                {
                    wxRegKey subkey(key.GetName(), name);

                    if (subkey.Open(wxRegKey::Read) &&
                        (subkey.QueryValue(msPsdkKeyValue[i], dir), !dir.IsEmpty()) &&
                        wxDirExists(dir))
                    {
                        sdkfound = true;
                        cont = false;
                    }
                    else
                        cont = key.GetNextKey(name, idx);

                    subkey.Close();
                }
                key.Close();
            }

            if (sdkfound)
                break;
        }
#endif // __WXMSW__

        // take a guess
        if (!sdkfound)
        {
            dir = wxT("C:\\Program Files");
            wxGetEnv(wxT("ProgramFiles"), &dir);
            dir +=  wxT("\\Microsoft SDKs\\Windows\\v");
            wxArrayString vers = GetArrayFromString(wxT("7.1;7.0A;7.0;6.1;6.0A;6.0"));
            for (size_t i = 0; i < vers.GetCount(); ++i)
            {
                if (wxDirExists(dir + vers[i]))
                {
                    dir += vers[i];
                    sdkfound = true;
                    break;
                }
            }
        }

        // add include dirs for MS Platform SDK too (let them come before compiler's path)
        if (sdkfound)
        {
            if (dir.GetChar(dir.Length() - 1) != '\\')
                dir += sep;
            AddIncludeDir(dir + _T("include"));
            AddResourceIncludeDir(dir + _T("include"));
            AddLibDir(dir + _T("lib"));
            m_ExtraPaths.Add(dir + _T("bin"));
        }

        // now the compiler's include directories
        AddIncludeDir(m_MasterPath + sep + _T("include"));
        AddLibDir(m_MasterPath + sep + _T("lib"));
        AddResourceIncludeDir(m_MasterPath + sep + _T("include"));

#ifdef __WXMSW__
        // add extra paths for "Debugging tools" too
        key.SetName(_T("HKEY_CURRENT_USER\\Software\\Microsoft\\DebuggingTools"));
        if (key.Exists() && key.Open(wxRegKey::Read))
        {
            key.QueryValue(_T("WinDbg"), dir);
            if (!dir.IsEmpty() && wxDirExists(dir))
            {
                if (dir.GetChar(dir.Length() - 1) == '\\')
                    dir.Remove(dir.Length() - 1, 1);
                m_ExtraPaths.Add(dir);
            }
        }
        key.Close();
#endif // __WXMSW__
    }

    return wxFileExists(m_MasterPath + sep + _T("bin") + sep + m_Programs.C) ? adrDetected : adrGuessed;
}
