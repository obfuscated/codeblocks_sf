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

#include "sdk_precomp.h"
#include "globals.h"
#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/file.h>

wxString GetStringFromArray(const wxArrayString& array, const wxString& separator)
{
    wxString out;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
        out << array[i] << separator;
    return out;
}

wxArrayString GetArrayFromString(const wxString& text, const wxString& separator, bool trimSpaces)
{
    wxArrayString out;
    wxString search = text;
    int seplen = separator.Length();
    while (true)
    {
        int idx = search.Find(separator);
        if (idx == -1)
        {
            if (trimSpaces)
            {
                search.Trim(false);
                search.Trim(true);
            }
            if (!search.IsEmpty())
                out.Add(search);
            break;
        }
        wxString part = search.Left(idx);
        search.Remove(0, idx + seplen);
        if (trimSpaces)
        {
            part.Trim(false);
            part.Trim(true);
        }
        if (!part.IsEmpty())
            out.Add(part);
    }
    return out;
}

void AppendArray(const wxArrayString& from, wxArrayString& to)
{
    for (unsigned int i = 0; i < from.GetCount(); ++i)
    {
        to.Add(from[i]);
    }
}

wxString UnixFilename(const wxString& filename)
{
    wxString result = filename;
#ifdef __WXMSW__
    bool unc_name = result.StartsWith(_T("\\\\"));

    while (result.Replace(_T("/"), _T("\\")))
        ;
    while (result.Replace(_T("\\\\"), _T("\\")))
        ;

    if (unc_name)
        result = _T("\\") + result;
#else

    while (result.Replace(_T("\\"), _T("/")))
        ;
    while (result.Replace(_T("//"), _T("/")))
        ;
#endif

    return result;
}

void QuoteStringIfNeeded(wxString& str)
{
    if (!str.IsEmpty() && str.GetChar(0) != _T('"') && str.Find(_T(' ')) != -1)
        str = wxString(_T("\"")) + str + _T("\"");
}

FileType FileTypeOf(const wxString& filename)
{
    wxString ext = filename.AfterLast(_T('.')).Lower();

    if (ext.Matches(CPP_EXT) ||
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

    else if (ext.Matches(CODEBLOCKS_EXT))
        return ftCodeBlocksProject;

    else if (ext.Matches(WORKSPACE_EXT))
        return ftCodeBlocksWorkspace;

    else if (ext.Matches(DEVCPP_EXT))
        return ftDevCppProject;

    else if (ext.Matches(MSVC_EXT))
        return ftMSVCProject;

    else if (ext.Matches(MSVS_EXT))
        return ftMSVSProject;

    else if (ext.Matches(MSVC_WORKSPACE_EXT))
        return ftMSVCWorkspace;

    else if (ext.Matches(MSVS_WORKSPACE_EXT))
        return ftMSVSWorkspace;

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

    return ftOther;
}

bool DoRememberExpandedNodes(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths, wxString& path)
{
    // remember expanded tree nodes of this tree
    if (!tree || !parent.IsOk())
        return false;

    wxString originalPath = path;
    bool found = false;
#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)

    long int cookie = 0;
#else

    wxTreeItemIdValue cookie; //2.6.0
#endif

    wxTreeItemId child = tree->GetFirstChild(parent, cookie);
    while (child.IsOk())
    {
        if (tree->ItemHasChildren(child) && tree->IsExpanded(child))
        {
            found = true;
            path << _T("/") << tree->GetItemText(child);
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
        int pos = tmpPath.Find(_T('/'));
        while (pos == 0)
        {
            tmpPath = tmpPath.Right(tmpPath.Length() - pos - 1);
            pos = tmpPath.Find(_T('/'));
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

#if (wxMAJOR_VERSION == 2) && (wxMINOR_VERSION < 5)
        long int cookie = 0;
#else

        wxTreeItemIdValue cookie; //2.6.0
#endif

        wxTreeItemId child = tree->GetFirstChild(parent, cookie);
        while (child.IsOk())
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
    if (!parent.IsOk() || !tree || !tree->ItemHasChildren(parent) || !tree->IsExpanded(parent))
        return;
    wxString tmp;
    if (!DoRememberExpandedNodes(tree, parent, nodePaths, tmp))
        nodePaths.Add(tmp); // just the tree root
}

void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths)
{
    if (!parent.IsOk() || !tree)
        return;
    if (nodePaths.GetCount() == 0)
    {
        tree->Collapse(parent);
        return;
    }
    for (unsigned int i = 0; i < nodePaths.GetCount(); ++i)
        DoExpandRememberedNode(tree, parent, nodePaths[i]);
    nodePaths.Clear();
}

bool CreateDirRecursively(const wxString& full_path, int perms)
{
    wxArrayString dirs;
    wxString currdir;

#ifdef __WXMSW__
    // hack to support for UNC filenames
    if (full_path.StartsWith(_T("\\\\")))
    {
        wxFileName tmp(_T("C:") + full_path.SubString(1, full_path.Length()));
        dirs = tmp.GetDirs();
        currdir = _T("\\\\") + dirs[0] + wxFILE_SEP_PATH;
        wxMessageBox(currdir);
        dirs.RemoveAt(0);
    }
    else
#endif
    {
        wxFileName tmp(full_path);
        currdir = tmp.GetVolume() + tmp.GetVolumeSeparator() + wxFILE_SEP_PATH;
        dirs = tmp.GetDirs();
    }
    for (size_t i = 0; i < dirs.GetCount(); ++i)
    {
        currdir << dirs[i];
        if (!wxDirExists(currdir) && !wxMkdir(currdir, perms))
            return false;
        currdir << wxFILE_SEP_PATH;
    }
    return true;
}

wxString ChooseDirectory(wxWindow* parent,
                         const wxString& message,
                         const wxString& initialPath,
                         const wxString& basePath,
                         bool askToMakeRelative, // relative to initialPath
                         bool showCreateDirButton) // where supported
{
    wxDirDialog dlg(parent,
                    message,
                    _T(""),
                    (showCreateDirButton ? wxDD_NEW_DIR_BUTTON : 0) | wxRESIZE_BORDER);
    dlg.SetPath(initialPath);
    if (dlg.ShowModal() != wxID_OK)
        return wxEmptyString;

    wxFileName path(dlg.GetPath());
    if (askToMakeRelative && !basePath.IsEmpty())
    {
        // ask the user if he wants it to be kept as relative
        if (wxMessageBox(_("Keep this as a relative path?"),
                         _("Question"),
                         wxICON_QUESTION | wxYES_NO) == wxYES)
        {
            path.MakeRelativeTo(basePath);
        }
    }
    return path.GetFullPath();
}

/// Reads a wxString from a non-unicode file. File must be open. File is closed automatically.
bool cbRead(wxFile& file, wxString& st)
{
    st.Empty();
    if (!file.IsOpened())
        return false;
    int len = file.Length();
    if(!len)
    {
        file.Close();
        return true;
    }
#if wxUSE_UNICODE
    char* buff = new char[len+1];
    if (!buff)
    {
        file.Close();
        return false;
    }
    file.Read((void*)buff, len);
    file.Close();
    buff[len]='\0';
    st = wxString((const char *)buff, wxConvLocal);
    delete[] buff;
#else

    char* buff = st.GetWriteBuf(len); // GetWriteBuf already handles the extra '\0'.
    file.Read((void*)buff, len);
    file.Close();
    st.UngetWriteBuf();
#endif

    return true;
}

wxString cbReadFileContents(wxFile& file)
{
    wxString st;
    cbRead(file,st);
    return st;
}

/// Writes a wxString to a non-unicode file. File must be open. File is closed automatically.
bool cbWrite(wxFile& file, const wxString& buff)
{
    bool result = false;
    if (file.IsOpened())
    {
        result = file.Write(buff,wxConvUTF8);
        if(result)
            file.Flush();
        file.Close();
    }
    return result;
}

/// Writes a wxString to a file. Takes care of unicode and uses a temporary file
/// to save first and then it copies it over the original.
bool cbSaveToFile(const wxString& filename, const wxString& contents)
{
    wxTempFile file(filename);
    if (file.IsOpened())
    {
        if (!file.Write(contents, wxConvLocal))
            return false;
        if (!file.Commit())
            return false;
    }
    else
        return false;
    return true;
}


wxString URLEncode(const wxString &str) // not sure this is 100% standards compliant, but I hope so
{
    wxString ret;
    wxString t;
    for(unsigned int i = 0; i < str.length(); ++i)
    {
        wxChar c = str[i];
        if( (c >= _T('A') && c <= _T('Z'))
                || (c >= _T('a') && c <= _T('z'))
                || (c >= _T('0') && c <= _T('9'))
                || c == _T('.') || c == _T('-')|| c == _T('_') )

            ret.Append(c);
        else if(c == _T(' '))
            ret.Append(_T('+'));
        else
        {
            t.sprintf(_T("%%%02X"), (unsigned int) c);
            ret.Append(t);
        }
    }
    return ret;
}

bool IsWindowReallyShown(wxWindow* win)
{
    while (win && win->IsShown())
    {
        win = win->GetParent();
        if (!win)
            return true;
    }
    return false;
}
