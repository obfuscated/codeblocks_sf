/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
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

#include "globals.h"
#include <wx/tokenzr.h>
#include <wx/filename.h>

wxString GetStringFromArray(const wxArrayString& array, const wxString& separator)
{
    wxString out;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
        out << array[i] << separator;
    return out;
}

wxArrayString GetArrayFromString(const wxString& text, const wxString& separator)
{
    wxArrayString out;
	wxStringTokenizer tkz(text, separator);
	while (tkz.HasMoreTokens())
		out.Add(tkz.GetNextToken());

    return out;
}

bool CreateDirRecursively(const wxString& full_path, int perms)
{
    wxFileName tmp(full_path);
    wxString sep = wxFileName::GetPathSeparator();
    wxString currdir = tmp.GetVolume() + tmp.GetVolumeSeparator() + sep;
    wxArrayString dirs = tmp.GetDirs();
    for (size_t i = 0; i < dirs.GetCount(); ++i)
    {
        currdir << dirs[i];
        if (!wxDirExists(currdir) && !wxMkdir(currdir, perms))
            return false;
        currdir << sep;
    }
    return true;
}

wxString UnixFilename(const wxString& filename)
{
    if (filename.IsEmpty())
        return wxEmptyString;
    wxString unixname = filename;
#ifndef __WXMSW__
    unixname.Replace("\\", "/");
#else
    if (wxGetOsVersion() == wxWIN95)
    {
        // win95 and win98 don't understand \\ in filenames as path separator
        unixname.Replace("\\\\", "\\");
        return unixname;
    }

    for (unsigned int i = 0; i < unixname.Length(); ++i)
    {
        if (unixname[i] == '\\' && i < unixname.Length() - 1)
        {
            if (unixname[i + 1] != ' ' && unixname[i + 1] != '\\')
                unixname.insert(i, '\\'); // escape it
            ++i;
        }
    }
#endif

    return unixname;
}

FileType FileTypeOf(const wxString& filename)
{
	wxFileName fname(filename);
	wxString ext = fname.GetExt().Lower();
	
	if (ext.Matches(CODEBLOCKS_EXT))
		return ftCodeBlocksProject;

	else if (ext.Matches(WORKSPACE_EXT))
		return ftCodeBlocksWorkspace;

	else if (ext.Matches(DEVCPP_EXT))
		return ftDevCppProject;

	else if (ext.Matches(MSVC_EXT))
		return ftMSVCProject;

	else if (ext.Matches(MSVS_EXT))
		return ftMSVSProject;

	else if (ext.Matches(CPP_EXT) ||
		ext.Matches(C_EXT) ||
		ext.Matches(CC_EXT) ||
		ext.Matches(CXX_EXT)
		)
		return ftSource;

	else if (ext.Matches(HPP_EXT) ||
		ext.Matches(H_EXT) ||
		ext.Matches(HH_EXT) ||
		ext.Matches(HXX_EXT)
		)
		return ftHeader;

	else if (ext.Matches(OBJECT_EXT))
		return ftObject;

	else if (ext.Matches(XRCRESOURCE_EXT))
		return ftXRCResource;

	else if (ext.Matches(RESOURCE_EXT))
		return ftResource;

	else if (ext.Matches(RESOURCEBIN_EXT))
		return ftResourceBin;

	else if (ext.Matches(STATICLIB_EXT))
		return ftStaticLib;

	else if (ext.Matches(DYNAMICLIB_EXT))
		return ftDynamicLib;

	else if (ext.Matches(EXECUTABLE_EXT))
		return ftExecutable;

	else if (ext.Matches(LUA_EXT))
		return ftLua;

	return ftOther;
}

bool DoRememberExpandedNodes(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths, wxString& path)
{
    // remember expanded tree nodes of this tree
    if (!parent)
        return false;
        
    wxString originalPath = path;
    bool found = false;
    long int cookie = 0;
    wxTreeItemId child = tree->GetFirstChild(parent, cookie);
    while (child)
    {
        if (tree->ItemHasChildren(child) && tree->IsExpanded(child))
        {
            found = true;
            path << "/" << tree->GetItemText(child);
            DoRememberExpandedNodes(tree, child, nodePaths, path);
            nodePaths.Add(path);
            path = originalPath;
        }
        child = tree->GetNextChild(parent, cookie);
    }
    return found;
}

void DoExpandRememberedNode(wxTreeCtrl* tree, const wxTreeItemId& parent, const wxString& path)
{
    if (!path.IsEmpty())
    {
        //Manager::Get()->GetMessageManager()->Log(mltDevDebug, path);
        wxString tmpPath;
        tmpPath = path;
        wxString folder;
        int pos = tmpPath.Find('/');
        while (pos == 0)
        {
            tmpPath = tmpPath.Right(tmpPath.Length() - pos - 1);
            pos = tmpPath.Find('/');
        }
        
        if (pos < 0) // no '/'
        {
            folder = tmpPath;
            tmpPath.Clear();
        }
        else
        {
            folder = tmpPath.Left(pos);
            tmpPath = tmpPath.Right(tmpPath.Length() - pos - 1);
        }

        //Manager::Get()->GetMessageManager()->Log(mltDevDebug, "%s, %s", folder.c_str(), tmpPath.c_str());

        long int cookie = 0;
        wxTreeItemId child = tree->GetFirstChild(parent, cookie);
        while (child)
        {
            wxString itemText = tree->GetItemText(child);
            if (itemText.Matches(folder))
            {
                tree->Expand(child);
                DoExpandRememberedNode(tree, child, tmpPath);
                break;
            }
            child = tree->GetNextChild(parent, cookie);
        }
    }
}

void SaveTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths)
{
    nodePaths.Clear();
    wxString tmp;
    DoRememberExpandedNodes(tree, parent, nodePaths, tmp);
}

void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths)
{
    for (unsigned int i = 0; i < nodePaths.GetCount(); ++i)
        DoExpandRememberedNode(tree, parent, nodePaths[i]);
    nodePaths.Clear();
}
