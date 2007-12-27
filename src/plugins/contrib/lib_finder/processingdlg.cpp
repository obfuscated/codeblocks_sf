/*
* This file is part of lib_finder plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
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

#include "processingdlg.h"

#include <wx/arrstr.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <prep.h>

#include "libraryconfigmanager.h"
#include "resultmap.h"

//(*InternalHeaders(ProcessingDlg)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//(*IdInit(ProcessingDlg)
const long ProcessingDlg::ID_STATICTEXT1 = wxNewId();
const long ProcessingDlg::ID_GAUGE1 = wxNewId();
const long ProcessingDlg::ID_BUTTON1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ProcessingDlg,wxDialog)
	//(*EventTable(ProcessingDlg)
	//*)
END_EVENT_TABLE()

ProcessingDlg::ProcessingDlg(wxWindow* parent,LibraryConfigManager& Manager,PkgConfigManager& PkgConfig,ResultMap& Results,wxWindowID id):
    StopFlag(false),
    m_Manager(Manager),
    m_PkgConfig(PkgConfig),
    m_Results(Results)
{
	//(*Initialize(ProcessingDlg)
	Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION, _T("id"));
	FlexGridSizer1 = new wxFlexGridSizer(0, 1, 0, 0);
	StaticBoxSizer1 = new wxStaticBoxSizer(wxVERTICAL, this, _("Processing"));
	Status = new wxStaticText(this, ID_STATICTEXT1, _("Waiting"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT1"));
	StaticBoxSizer1->Add(Status, 0, wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	Gauge1 = new wxGauge(this, ID_GAUGE1, 100, wxDefaultPosition, wxSize(402,12), 0, wxDefaultValidator, _T("ID_GAUGE1"));
	StaticBoxSizer1->Add(Gauge1, 1, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	FlexGridSizer1->Add(StaticBoxSizer1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	StopBtn = new wxButton(this, ID_BUTTON1, _("Stop"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
	FlexGridSizer1->Add(StopBtn, 1, wxBOTTOM|wxLEFT|wxRIGHT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);
	
	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&ProcessingDlg::OnButton1Click);
	//*)
}

ProcessingDlg::~ProcessingDlg()
{
}

void ProcessingDlg::OnButton1Click(wxCommandEvent& event)
{
    StopBtn->Disable();
    StopFlag = true;
}

bool ProcessingDlg::ReadDirs(const wxArrayString& Dirs)
{
    Gauge1->SetRange(Dirs.Count());
    for ( size_t i = 0; i<Dirs.Count(); i++ )
    {
        if ( StopFlag ) return false;
        Gauge1->SetValue(i);

        wxString DirName = Dirs[i];
        if ( DirName.empty() ) continue;

        // Cutting off last character if it is path separator
        wxChar LastChar = DirName[DirName.Len()-1];
        if ( wxFileName::GetPathSeparators().Find(LastChar) != -1 )
        {
            DirName.RemoveLast();
        }

        // Reading dir content
        ReadDir(DirName);
    }
    return !StopFlag;
}

void ProcessingDlg::ReadDir(const wxString& DirName)
{
    wxDir Dir(DirName);

    if ( !Dir.IsOpened() ) return;

    Status->SetLabel(_T("Reading dir: ") + DirName);
    ::wxYield();
    if ( StopFlag ) return;

    wxString Name;

    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_FILES|wxDIR_HIDDEN) )
    {
        do
        {
            Map[Name].Add(DirName + wxFileName::GetPathSeparator() + Name);
        }
        while ( Dir.GetNext(&Name) );
    }

    if ( Dir.GetFirst(&Name,wxEmptyString,wxDIR_DIRS|wxDIR_HIDDEN) )
    {
        do
        {
            Map[Name].Add(DirName + wxFileName::GetPathSeparator() + Name);
            ReadDir(DirName + wxFileName::GetPathSeparator() + Name);
        }
        while ( Dir.GetNext(&Name) );
    }
}

bool ProcessingDlg::ProcessLibs()
{
    Gauge1->SetRange(m_Manager.GetLibraryCount());

    for ( int i=0; i<m_Manager.GetLibraryCount(); ++i )
    {
        if ( StopFlag ) return false;
        ProcessLibrary(m_Manager.GetLibrary(i));
    }

    return !StopFlag;
}

void ProcessingDlg::ProcessLibrary(const LibraryConfig* Config)
{
    Status->SetLabel(
        wxString::Format(
            _("Searching variable \"%s\""),
            Config->GlobalVar.c_str()));

    CheckFilter(_T(""),wxStringStringMap(),wxArrayString(),Config,0);
}

void ProcessingDlg::CheckFilter(
    const wxString& OldBasePath,
    const wxStringStringMap& OldVars,
    const wxArrayString& OldCompilers,
    const LibraryConfig* Config,
    int WhichFilter)
{
    if ( (int)Config->Filters.size() <= WhichFilter )
    {
        FoundLibrary(OldBasePath,OldVars,OldCompilers,Config);
        return;
    }

    const LibraryFilter& Filter = Config->Filters[WhichFilter];

    switch ( Filter.Type )
    {
        case LibraryFilter::File:
        {
            // Split path
            wxArrayString Pattern;
            SplitPath(Filter.Value,Pattern);

            // Fetch list of files with filename matching last pattern's element
            const wxArrayString& PathArray = Map[Pattern[Pattern.Count()-1]];
            if ( PathArray.empty() ) return;

            // Process those files
            for ( size_t i=0; i<PathArray.Count(); i++ )
            {
                wxArrayString Path;
                wxStringStringMap Vars = OldVars;
                SplitPath(PathArray[i],Path);

                int path_index = (int)Path.Count() - 1;
                int pattern_index = (int)Pattern.Count() - 1;

                // Check if patterns do match
                while ( ( path_index >= 0 ) && ( pattern_index >= 0 ) )
                {
                    wxString& PatternPart = Pattern[pattern_index];
                    if ( IsVariable(PatternPart) )
                    {
                        wxString VarName = PatternPart.Mid(3,PatternPart.Len()-4);
                        if ( Vars[VarName].empty() )
                        {
                            Vars[VarName] = Path[path_index];
                        }
                        else
                        {
                            if ( Vars[VarName] != Path[path_index] ) break;
                        }
                    }
                    else
                    {
                        if ( PatternPart != Path[path_index] ) break;
                    }
                    path_index--;
                    pattern_index--;
                }

                // This is when patterns did not match
                if ( pattern_index >= 0 ) continue;

                // Construct base path from the rest of file's name
                wxString BasePath;
                for ( int j=0; j<=path_index; j++ )
                {
                    BasePath += Path[j] + wxFileName::GetPathSeparator();
                }

                // And check if base path match the previous one
                if ( !OldBasePath.IsEmpty() )
                {
                    if ( BasePath != OldBasePath ) continue;
                }

                // Ok, this filter matches, let's advance to next filet
                CheckFilter(BasePath,Vars,OldCompilers,Config,WhichFilter+1);
            }
            break;
        }

        case LibraryFilter::Platform:
        {
            wxStringTokenizer Tokenizer(Filter.Value,_T("| \t"));
            bool IsPlatform = false;
            while ( Tokenizer.HasMoreTokens() )
            {
                wxString Platform = Tokenizer.GetNextToken();

                if ( platform::windows )
                {
                    if ( Platform==_T("win") || Platform==_T("windows") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::macosx )
                {
                    if ( Platform==_T("mac") || Platform==_T("macosx") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::linux )
                {
                    if ( Platform==_T("lin") || Platform==_T("linux") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::freebsd )
                {
                    if ( Platform==_T("bsd") || Platform==_T("freebsd") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::netbsd )
                {
                    if ( Platform==_T("bsd") || Platform==_T("netbsd") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::openbsd )
                {
                    if ( Platform==_T("bsd") || Platform==_T("openbsd") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::darwin )
                {
                    if ( Platform==_T("darwin") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::solaris )
                {
                    if ( Platform==_T("solaris") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }

                if ( platform::unix )
                {
                    if ( Platform==_T("unix") || Platform==_T("un*x") )
                    {
                        IsPlatform = true;
                        break;
                    }
                }
            }

            if ( IsPlatform )
            {
                CheckFilter(OldBasePath,OldVars,OldCompilers,Config,WhichFilter+1);
            }
            break;
        }

        case LibraryFilter::Exec:
        {
            bool IsExec = false;
            if ( wxIsAbsolutePath(Filter.Value) )
            {
                // If this is absolute path, we don't search in PATH evironment var
                IsExec = wxFileName::IsFileExecutable(Filter.Value);
            }
            else
            {
                // Let's search for the name in search paths
                wxString Path;
                if ( wxGetEnv(_T("PATH"),&Path) )
                {
                    wxString Splitter = _T(":");
                    if ( platform::windows ) Splitter = _T(";");
                    wxStringTokenizer Tokenizer(Path,Splitter);
                    while ( Tokenizer.HasMoreTokens() )
                    {
                        wxString OnePath = Tokenizer.GetNextToken();

                        // Let's skip relative paths (f.ex. ".")
                        if ( !wxIsAbsolutePath(OnePath) ) continue;

                        OnePath += wxFileName::GetPathSeparator()+Filter.Value;
                        if ( wxFileName::IsFileExecutable(OnePath) )
                        {
                            IsExec = true;
                            break;
                        }
                    }
                }
            }

            if ( IsExec )
            {
                CheckFilter(OldBasePath,OldVars,OldCompilers,Config,WhichFilter+1);
            }
            break;
        }

        case LibraryFilter::PkgConfig:
        {
            if ( m_PkgConfig.GetLibraries().IsGlobalVar(Filter.Value) )
            {
                CheckFilter(OldBasePath,OldVars,OldCompilers,Config,WhichFilter+1);
            }
            break;
        }

        case LibraryFilter::Compiler:
        {
            if ( OldCompilers.IsEmpty() )
            {
                // If this is the first compiler filter, let's build new list and continue
                CheckFilter(OldBasePath,OldVars,wxStringTokenize(Filter.Value,_T("| \t")),Config,WhichFilter+1);
            }
            else
            {
                // We've set compiler list before, leave only the intersection
                // of previous and current list
                wxArrayString Compilers;
                wxStringTokenizer Tokenizer(Filter.Value,_T("| \t"));
                while ( Tokenizer.HasMoreTokens() )
                {
                    wxString Comp = Tokenizer.GetNextToken();
                    if ( OldCompilers.Index(Comp) != wxNOT_FOUND )
                    {
                        Compilers.Add(Comp);
                    }
                }

                if ( !Compilers.IsEmpty() )
                {
                    CheckFilter(OldBasePath,OldVars,Compilers,Config,WhichFilter+1);
                }
            }
            break;
        }

        case LibraryFilter::None:
        {
            CheckFilter(OldBasePath,OldVars,OldCompilers,Config,WhichFilter+1);
            break;
        }
    }
}

void ProcessingDlg::SplitPath(const wxString& FileName,wxArrayString& Split)
{
    wxStringTokenizer Tknz(FileName,_T("\\/"));
    while (Tknz.HasMoreTokens()) Split.Add(Tknz.GetNextToken());
}

bool ProcessingDlg::IsVariable(const wxString& NamePart) const
{
    if ( NamePart.Len() < 5 ) return false;
    if ( NamePart[0] != _T('*') ) return false;
    if ( NamePart[1] != _T('$') ) return false;
    if ( NamePart[2] != _T('(') ) return false;
    if ( NamePart[NamePart.Len()-1] != _T(')') ) return false;
    return true;
}

void ProcessingDlg::FoundLibrary(const wxString& OldBasePath,const wxStringStringMap& OldVars,const wxArrayString& Compilers,const LibraryConfig* Config)
{
    wxStringStringMap Vars = OldVars;
    wxString BasePath = OldBasePath;

    BasePath.RemoveLast();
    Vars[_T("BASE_DIR")] = BasePath;
    LibraryResult* Result = new LibraryResult();

    Result->GlobalVar = Config->GlobalVar;
    Result->LibraryName = FixVars(Config->LibraryName,Vars);
    Result->BasePath = FixPath(BasePath);
    Result->PkgConfigVar = Config->PkgConfigVar;
    Result->Description = FixVars(Config->Description,Vars);

    Result->Compilers = Compilers;
    Result->Categories = Config->Categories;

    for ( size_t i=0; i<Config->IncludePaths.Count(); i++ )
    {
        Result->IncludePath.Add(FixPath(FixVars(Config->IncludePaths[i],Vars)));
    }

    for ( size_t i=0; i<Config->LibPaths.Count(); i++ )
    {
        Result->LibPath.Add(FixPath(FixVars(Config->LibPaths[i],Vars)));
    }

    for ( size_t i=0; i<Config->ObjPaths.Count(); i++ )
    {
        Result->ObjPath.Add(FixPath(FixVars(Config->ObjPaths[i],Vars)));
    }

    for ( size_t i=0; i<Config->CFlags.Count(); i++ )
    {
        Result->CFlags.Add(FixVars(Config->CFlags[i],Vars));
    }

    for ( size_t i=0; i<Config->LFlags.Count(); i++ )
    {
        Result->LFlags.Add(FixVars(Config->LFlags[i],Vars));
    }

    ResultArray& Array = m_Results.GetGlobalVar(Config->GlobalVar);
    Array.Add(Result);
}

wxString ProcessingDlg::FixVars(wxString Original,const wxStringStringMap& Vars)
{
    for ( wxStringStringMap::const_iterator it = Vars.begin();
          it != Vars.end();
          ++it )
    {
        wxString SearchString = _T("$(") + it->first + _T(")");
        wxString ReplaceWith = it->second;
        Original.Replace(SearchString,ReplaceWith);
    }

    return Original;
}

wxString ProcessingDlg::FixPath(wxString Original)
{
    return wxFileName(Original).GetFullPath();
}
