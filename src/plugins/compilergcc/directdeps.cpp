/*
* This file is part of Code::Blocks, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Id$
* $Date$
*/

#include "directdeps.h"
#include <wx/log.h>
#include <wx/filename.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <manager.h>
#include <messagemanager.h>
#include <cbproject.h>
#include <projectbuildtarget.h>

wxString buffer;
size_t bufferLen = 0;

inline bool IsEof(size_t idx)
{
    return idx >= bufferLen;
}

inline bool SkipWhite(size_t& idx)
{
    while (!IsEof(idx))
    {
        wxChar c = buffer.GetChar(idx);
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        ++idx;
    }
    return !IsEof(idx);
}

inline bool SkipLine(size_t& idx)
{
    while (!IsEof(idx))
    {
        wxChar c = buffer.GetChar(idx);
        if (c == '\r' || c == '\n')
            break;
        ++idx;
    }
    ++idx; // skip newline char
    while (!IsEof(idx))
    {
        wxChar c = buffer.GetChar(idx);
        if (c != '\r' || c != '\n')
            break;
        ++idx;
    }
    return !IsEof(idx);
}

bool DirectDeps::ReadDependencies(const wxString& filename, wxArrayString& deps)
{
    wxFileInputStream file(filename);
    if (!file.Ok())
        return false; // error opening file???

    wxTextInputStream input(file);
    while (!file.Eof())
    {
        wxString line = input.ReadLine();
        line.Trim(false);
        line.Trim(true);
        if (line.GetChar(line.Length() - 1) == '\\')
        {
            // GNU "make" style dependencies file detected
            deps.Clear();
            return false;
        }
//        Manager::Get()->GetMessageManager()->Log(m_PageIndex, "Line: '%s'", line.c_str());
        if (!line.IsEmpty())
            deps.Add(line);
    }
    return true;
}

/// Creates a list of files this project file depends on, by scanning for #include directives
/// This list of files is deps
bool DirectDeps::GetDependenciesOf(const wxString& filename, wxArrayString& deps,
                                    int pageIndex,
                                    cbProject* project,
                                    ProjectBuildTarget* target)
{
    wxLogNull ln;

    wxFileName fname(filename);

    // check if we already scanned this file (to avoid infinite loop)
    if (deps.Index(fname.GetFullPath()) != wxNOT_FOUND)
        return true; // already scanned
//    Manager::Get()->GetMessageManager()->Log(m_PageIndex, "    DBG: %s not scanned yet", fname.GetFullPath().c_str());

    // open file
    wxFile file(fname.GetFullPath());
    if (!file.IsOpened())
        return false;
//    Manager::Get()->GetMessageManager()->Log(pageIndex, "    DBG: scanning %s", fname.GetFullPath().c_str());

    char* buff = buffer.GetWriteBuf(file.Length());
    file.Read(buff, file.Length());
    buffer.UngetWriteBuf();
	bufferLen = buffer.Length();

    wxArrayString includes;

    deps.Add(fname.GetFullPath());
    size_t baseIdx = 0;
    while (baseIdx < bufferLen)
    {
        if (!SkipWhite(baseIdx))
            break;
        
        // only lines containing preprocessor directives
        if (buffer.GetChar(baseIdx) != '#')
        {
            SkipLine(baseIdx);
            continue;
        }
        
        // remove #, so that we can trim the rest up to the directive
        ++baseIdx;
        if (!SkipWhite(baseIdx))
            break;

        // check if it is an #include directive
        if (strncmp((buffer.c_str() + baseIdx), "include", 7) == 0)
        {
            baseIdx += 7;
            if (!SkipWhite(baseIdx))
                break;
            wxChar c = buffer.GetChar(baseIdx);
            if (c != '"' && c != '<')
            {
                SkipLine(baseIdx);
                continue; // invalid token?
            }

//            bool isLocal = c == '"';

            // now "rest" must hold either "some/file.name" or <some/file.name>
            ++baseIdx;
            
            size_t idx = 0;
            while (true)
            {
                c = buffer.GetChar(baseIdx + idx);
                if (c == '\0')
                {
                    // we reached the end of line and didn't find the string :(
                    idx = 0;
                    break;
                }
                if (c == '"' || c == '>')
                {
                    break;
                }
                ++idx;
            }

            // if rest is not empty, we got an included filename :)
            if (idx != 0)
            {
                wxString rest = buffer.Mid(baseIdx, idx);
                includes.Add(rest);
//                Manager::Get()->GetMessageManager()->Log(pageIndex, "    DBG: found included file: %s (from %s)", rest.c_str(), fname.GetFullPath().c_str());
            }
        }
    }

    // free memory
    buffer.Clear();
    bufferLen = 0;

    // parse included files
    for (size_t inc = 0; inc < includes.GetCount(); ++inc)
    {
        wxString& rest = includes[inc];

        // local search
        wxFileName tmp(fname.GetPath(wxPATH_GET_SEPARATOR) + rest);
        tmp.Normalize(wxPATH_NORM_ALL, project->GetBasePath());
        tmp.MakeRelativeTo(project->GetBasePath());
        if (wxFileExists(tmp.GetFullPath()))
        {
            GetDependenciesOf(tmp.GetFullPath(), deps, pageIndex, project, target);
            continue;
        }

        // try scanning the file by prepending all the globals and project include dirs until it's found
        wxString newfilename;
        wxString sep = wxFileName::GetPathSeparator();
        bool found = false;
                    
        // target include dirs first
        if (target)
        {
            const wxArrayString& tgt_incs = target->GetIncludeDirs();
            for (unsigned int i = 0; i < tgt_incs.GetCount(); ++i)
            {
                newfilename = tgt_incs[i] + sep + rest;
                if (wxFileExists(newfilename))
                {
                    GetDependenciesOf(newfilename, deps, pageIndex, project, target);
                    found = true;
                    break;
                }
            }
        }
        if (found)
            continue;

        // project include dirs last
        const wxArrayString& prj_incs = project->GetIncludeDirs();
        for (unsigned int i = 0; i < prj_incs.GetCount(); ++i)
        {
            newfilename = prj_incs[i] + sep + rest;
            if (wxFileExists(newfilename))
            {
                GetDependenciesOf(newfilename, deps, pageIndex, project, target);
                break;
            }
        }
    }
    return true;
}
