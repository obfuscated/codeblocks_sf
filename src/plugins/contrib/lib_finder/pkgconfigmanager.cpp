/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision: 4504 $
* $Id: wxsmithpluginregistrants.cpp 4504 2007-10-02 21:52:30Z byo $
* $HeadURL: svn+ssh://byo@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/plugins/contrib/wxSmith/plugin/wxsmithpluginregistrants.cpp $
*/

#include "pkgconfigmanager.h"
#include <wx/utils.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>

PkgConfigManager::PkgConfigManager()
{
    m_PkgConfigVersion = -1;
}

PkgConfigManager::~PkgConfigManager()
{
}

void PkgConfigManager::RefreshData()
{
    if ( !DetectVersion() || !LoadLibraries() )
    {
        m_PkgConfigVersion = -1;
    }
}

bool PkgConfigManager::DetectVersion()
{
    wxArrayString Output;
    if ( wxExecute(_T("pkg-config --version"),Output,wxEXEC_NODISABLE) != 0 )
    {
        // Some error, we can not talk to pkg-config
        return false;
    }

    if ( Output.Count() < 1 )
    {
        // Did not receive version string
        return false;
    }

    wxStringTokenizer VerTok(Output[0],_T("."));
    long VersionNumbers[4] = { 0,0,0,0 };
    int CurrentVersionToken = 0;

    while ( VerTok.HasMoreTokens() && CurrentVersionToken<4 )
    {
        if ( !VerTok.GetNextToken().ToLong(&VersionNumbers[CurrentVersionToken++],10) )
        {
            // Incorrect version
            return false;
        }
    }

    if ( CurrentVersionToken==0 )
    {
        // No suitable version number found
        return false;
    }

    m_PkgConfigVersion =
        ((VersionNumbers[0]&0xFFL) << 24) |
        ((VersionNumbers[1]&0xFFL) << 16) |
        ((VersionNumbers[2]&0xFFL) <<  8) |
        ((VersionNumbers[3]&0xFFL) <<  0);

    return true;
}

bool PkgConfigManager::LoadLibraries()
{
    wxArrayString Output;
    if ( wxExecute(_T("pkg-config --list-all"),Output,wxEXEC_NODISABLE) != 0 )
    {
        // Some error, we can not talk to pkg-config
        return false;
    }

    m_Libraries.Clear();
    for ( size_t i=0; i<Output.Count(); i++ )
    {
        wxString Name;
        wxString& Line = Output[i];

        // Get the name
        // NOTE: This may not be accurate since library name is mapped to filename
        //       so white chars can be used as part of the name
        size_t j;
        for ( j=0; j<Line.Length(); j++ )
        {
            wxChar ch = Line[j];
            if ( ch==_T('\0') || ch==_T(' ') || ch==_T('\t') )
            {
                break;
            }
            Name += ch;
        }
        if ( Name.IsEmpty() )
        {
            // Woow, what was that ?
            continue;
        }

        // Eat white
        while ( j<Line.Length() && (Line[j]==_T(' ') || Line[j]==_T('\t')) ) j++;
        // After that, we have description

        LibraryResult* Result = new LibraryResult;
//        Result->LibraryName =
        Result->GlobalVar = Name;
        Result->PkgConfigVar = Name;
        Result->Description = Line.Mid(j);
        m_Libraries.GetGlobalVar(Name).push_back(Result);
    }

    return true;
}

void PkgConfigManager::Clear()
{
    m_Libraries.Clear();
}

bool PkgConfigManager::UpdateTarget(const wxString& VarName,CompileTargetBase* Target,bool Force)
{
    if ( m_Libraries.IsGlobalVar(VarName) || Force )
    {
        Target->AddCompilerOption(_T("`pkg-config ") + VarName + _T(" --cflags`"));
        Target->AddLinkerOption  (_T("`pkg-config ") + VarName + _T(" --libs`"  ));
    }
    return m_Libraries.IsGlobalVar(VarName);
}
