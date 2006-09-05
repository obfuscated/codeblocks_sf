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
* $Revision$
* $Id$
* $HeadURL$
*/

#include "sdk_precomp.h"

#ifdef TIXML_USE_STL
    #include <string>
#endif

#ifndef CB_PRECOMP
    #include "globals.h"
    #include <wx/filename.h>
    #include <wx/file.h>
    #include <wx/image.h>
    #include <wx/listctrl.h>
    #include "cbexception.h"
    #include "manager.h"
    #include "configmanager.h" // ReadBool
#endif

#include "tinyxml/tinyxml.h"
#include <wx/tokenzr.h>
#include <wx/dirdlg.h>
#include <wx/msgdlg.h>
#include <wx/fontmap.h>
#include <algorithm>
#include "filefilters.h"

const wxString DEFAULT_WORKSPACE		= _T("default.workspace");
const wxString DEFAULT_ARRAY_SEP        = _T(";");
const wxString DEFAULT_CONSOLE_TERM     = _T("xterm -T $TITLE -e");
const wxString DEFAULT_CONSOLE_SHELL    = _T("/bin/sh -c");

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

    if (ext.IsSameAs(FileFilters::CPP_EXT) ||
            ext.IsSameAs(FileFilters::C_EXT) ||
            ext.IsSameAs(FileFilters::CC_EXT) ||
            ext.IsSameAs(FileFilters::CXX_EXT) ||
            ext.IsSameAs(FileFilters::D_EXT) ||
            ext.IsSameAs(FileFilters::F_EXT) ||
            ext.IsSameAs(FileFilters::F77_EXT) ||
            ext.IsSameAs(FileFilters::F95_EXT)
       )
        return ftSource;

    else if (ext.IsSameAs(FileFilters::HPP_EXT) ||
             ext.IsSameAs(FileFilters::H_EXT) ||
             ext.IsSameAs(FileFilters::HH_EXT) ||
             ext.IsSameAs(FileFilters::HXX_EXT)
            )
        return ftHeader;

    else if (ext.IsSameAs(FileFilters::CODEBLOCKS_EXT))
        return ftCodeBlocksProject;

    else if (ext.IsSameAs(FileFilters::WORKSPACE_EXT))
        return ftCodeBlocksWorkspace;

    else if (ext.IsSameAs(FileFilters::DEVCPP_EXT))
        return ftDevCppProject;

    else if (ext.IsSameAs(FileFilters::MSVC6_EXT))
        return ftMSVC6Project;

    else if (ext.IsSameAs(FileFilters::MSVC7_EXT))
        return ftMSVC7Project;

    else if (ext.IsSameAs(FileFilters::MSVC6_WORKSPACE_EXT))
        return ftMSVC6Workspace;

    else if (ext.IsSameAs(FileFilters::MSVC7_WORKSPACE_EXT))
        return ftMSVC7Workspace;

    else if (ext.IsSameAs(FileFilters::OBJECT_EXT))
        return ftObject;

    else if (ext.IsSameAs(FileFilters::XRCRESOURCE_EXT))
        return ftXRCResource;

    else if (ext.IsSameAs(FileFilters::RESOURCE_EXT))
        return ftResource;

    else if (ext.IsSameAs(FileFilters::RESOURCEBIN_EXT))
        return ftResourceBin;

    else if (ext.IsSameAs(FileFilters::STATICLIB_EXT))
        return ftStaticLib;

    else if (ext.IsSameAs(FileFilters::DYNAMICLIB_EXT))
        return ftDynamicLib;

    else if (ext.IsSameAs(FileFilters::EXECUTABLE_EXT))
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
        cbMessageBox(currdir);
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
    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return wxEmptyString;

    wxFileName path(dlg.GetPath());
    if (askToMakeRelative && !basePath.IsEmpty())
    {
        // ask the user if he wants it to be kept as relative
        if (cbMessageBox(_("Keep this as a relative path?"),
                         _("Question"),
                         wxICON_QUESTION | wxYES_NO) == wxID_YES)
        {
            path.MakeRelativeTo(basePath);
        }
    }
    return path.GetFullPath();
}

// Reads a wxString from a non-unicode file. File must be open. File is closed automatically.
bool cbRead(wxFile& file, wxString& st, wxFontEncoding encoding)
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

    wxCSConv conv(encoding);
    st = wxString((const char *)buff, conv);

    delete[] buff;
#else

    char* buff = st.GetWriteBuf(len); // GetWriteBuf already handles the extra '\0'.
    file.Read((void*)buff, len);
    file.Close();
    st.UngetWriteBuf();
#endif

    return true;
}

wxString cbReadFileContents(wxFile& file, wxFontEncoding encoding)
{
    wxString st;
    cbRead(file, st, encoding);
    return st;
}

// Writes a wxString to a non-unicode file. File must be open. File is closed automatically.
bool cbWrite(wxFile& file, const wxString& buff, wxFontEncoding encoding)
{
    bool result = false;
    if (file.IsOpened())
    {
        wxCSConv conv(encoding);
        result = file.Write(buff,conv);
        if(result)
            file.Flush();
        file.Close();
    }
    return result;
}

// Writes a wxString to a file. Takes care of unicode and uses a temporary file
// to save first and then it copies it over the original.
bool cbSaveToFile(const wxString& filename, const wxString& contents, wxFontEncoding encoding, bool bom)
{
    wxCSConv conv(encoding);

    wxTempFile file(filename);
    if (file.IsOpened())
    {
        if (bom)
        {
            char* mark = NULL;
            int mark_length = 0;
            /* TODO: write byte order mark */
            switch (encoding)
            {
            case wxFONTENCODING_UTF7:
                /* TODO: utf-7 bom is weird */
                break;
            case wxFONTENCODING_UTF8:
                mark = "\xEF\xBB\xBF";
                mark_length = 3;
                break;
            case wxFONTENCODING_UTF16BE:
                mark = "\xFE\xFF";
                mark_length = 2;
                break;
            case wxFONTENCODING_UTF16LE:
                mark = "\xFF\xFE";
                mark_length = 2;
                break;
            case wxFONTENCODING_UTF32BE:
                mark = "\x00\x00\xFE\xFF";
                mark_length = 4;
                break;
            case wxFONTENCODING_UTF32LE:
                mark = "\xFF\xFE\x00\x00";
                mark_length = 4;
                break;
            case wxFONTENCODING_SYSTEM:
            default:
                /* can't do anything here */
                break;
            }
            if (mark_length>0 && mark)
            {
                if (!file.Write((void*)mark, mark_length))
                    return false;
            }
        }
        if (!file.Write(contents, conv))
            return false;
        if (!file.Commit())
            return false;
    }
    else
        return false;
    return true;
}

// Saves a TinyXML document correctly, even if the path contains unicode characters.
bool cbSaveTinyXMLDocument(TiXmlDocument* doc, const wxString& filename)
{
    if (!doc)
        return false;

    const char *buffer; // UTF-8 encoded data
  	size_t len;

  	#ifdef TIXML_USE_STL
        std::string outSt;
        outSt << *doc;
        buffer = outSt.c_str();
        len = outSt.length();
  	#else
        TiXmlOutStream outSt;
        outSt << *doc;
        buffer = outSt.c_str();
        len = outSt.length();
  	#endif

    wxTempFile file(filename);
    if (file.IsOpened())
    {
        if (!file.Write(buffer, strlen(buffer)))
            return false;
        if (!file.Commit())
            return false;
    }
    else
        return false;
  	return true;
}

// Return @c str as a proper unicode-compatible string
wxString cbC2U(const char* str)
{
#if wxUSE_UNICODE
    return wxString(str, wxConvUTF8);
#else
    return wxString(str);
#endif
}

// Return multibyte (C string) representation of the string
wxWX2MBbuf cbU2C(const wxString& str)
{
#if wxUSE_UNICODE
    return str.mb_str(wxConvUTF8);
#else
    return (wxChar*)str.mb_str();
#endif
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

bool NormalizePath(wxFileName& f,const wxString& base)
{
    bool result = true;
//    if(!f.IsAbsolute())
    {
        wxLogNull null_logger; // we do all file checks ourselves
        f.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
        result = f.IsOk();
    }
    return result;
}

// function to check the common controls version
// (should it be moved in sdk globals?)
#ifdef __WXMSW__
#include <windows.h>
#include <shlwapi.h>
bool UsesCommonControls6()
{
    bool result = false;
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;
    hinstDll = LoadLibrary("comctl32.dll");
    if(hinstDll)
    {
        DLLGETVERSIONPROC pDllGetVersion;
        pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(hinstDll, "DllGetVersion");

        if (pDllGetVersion)
        {
            DLLVERSIONINFO dvi;
            HRESULT hr;

            ZeroMemory(&dvi, sizeof(dvi));
            dvi.cbSize = sizeof(dvi);

            hr = (*pDllGetVersion)(&dvi);

            if (SUCCEEDED(hr))
            {
               dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
               result = dvi.dwMajorVersion == 6;
            }
        }

        FreeLibrary(hinstDll);
    }
    return result;
}
#endif

wxBitmap cbLoadBitmap(const wxString& filename, int bitmapType)
{
#ifdef __WXMSW__
    // cache this, can't change while we 're running :)
    static bool oldCommonControls = !UsesCommonControls6();
#else
    // irrelevant for this platform
    static bool oldCommonControls = false;
#endif
    wxImage im;
    im.LoadFile(filename, bitmapType);
    if (oldCommonControls && im.HasAlpha())
        im.ConvertAlphaToMask();

    return wxBitmap(im);
}

void SetSettingsIconsStyle(wxListCtrl* lc, SettingsIconsStyle style)
{
// this doesn't work under wxGTK...
#ifdef __WXMSW__
    long flags = lc->GetWindowStyleFlag();
    switch (style)
    {
        case sisNoIcons: flags = (flags & ~wxLC_ICON) | wxLC_SMALL_ICON; break;
        default: flags = (flags & ~wxLC_SMALL_ICON) | wxLC_ICON; break;
    }
    lc->SetWindowStyleFlag(flags);
#endif
}

SettingsIconsStyle GetSettingsIconsStyle(wxListCtrl* lc)
{
// this doesn't work under wxGTK...
#ifdef __WXMSW__
    long flags = lc->GetWindowStyleFlag();
    if (flags & wxLC_SMALL_ICON)
        return sisNoIcons;
#endif
    return sisLargeIcons;
}

#ifdef __WXMSW__

void PlaceWindow(wxWindow *w, cbPlaceDialogMode mode, bool enforce)
{
    HMONITOR hMonitor;
    MONITORINFO mi;
    RECT        r;

    int the_mode;

    if(!w)
        cbThrow(_T("Passed NULL pointer to PlaceWindow."));

    wxWindow *referenceWindow = Manager::Get()->GetAppWindow();

    if(!referenceWindow)    // no application window available, so this is as good as we can get
        referenceWindow = w;

    wxRect windowRect = w->GetRect();

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if(!enforce && cfg->ReadBool(_T("/dialog_placement/do_place")) == false)
        return;

    if(mode == pdlBest)
        the_mode = cfg->ReadInt(_T("/dialog_placement/dialog_position"), (int) pdlCentre);
    else
        the_mode = (int) mode;


    hMonitor = MonitorFromWindow((HWND) referenceWindow->GetHandle(), MONITOR_DEFAULTTONEAREST);

    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);
    r = mi.rcWork;

    int monitorWidth  = r.right - r.left;
    int monitorHeight = r.bottom - r. top;

    switch(the_mode)
    {
        case pdlCentre:
        {
            windowRect.x = r.left + (monitorWidth  - windowRect.width)/2;
            windowRect.y = r.top  + (monitorHeight - windowRect.height)/2;
        }
        break;


        case pdlHead:
        {
            windowRect.x = r.left + (monitorWidth  - windowRect.width)/2;
            windowRect.y = r.top  + (monitorHeight - windowRect.height)/3;
        }
        break;


        case pdlConstrain:
        {
            int x1 = windowRect.x;
            int x2 = windowRect.x + windowRect.width;
            int y1 = windowRect.y;
            int y2 = windowRect.y + windowRect.height;

            if(windowRect.width > monitorWidth) // cannot place without clipping, so centre it
            {
                x1 = r.left + (monitorWidth  - windowRect.width)/2;
                x2 = x1 + windowRect.width;
            }
            else
            {
                x2 = std::min((int) r.right, windowRect.GetRight());
                x1 = std::max(x2 - windowRect.width, (int) r.left);
                x2 = x1 + windowRect.width;
            }
            if(windowRect.height > monitorHeight) // cannot place without clipping, so centre it
            {
                y1 = r.top + (monitorHeight  - windowRect.height)/2;
                y2 = y1 + windowRect.height;
            }
            else
            {
                y2 = std::min((int) r.bottom, windowRect.GetBottom());
                y1 = std::max(y2 - windowRect.height, (int) r.top);
                y2 = y1 + windowRect.height;
            }
            windowRect = wxRect(x1, y1, x2-x1, y2-y1);
        }
        break;


        case pdlClip:
        {
            int x1 = windowRect.x;
            int x2 = windowRect.x + windowRect.width;
            int y1 = windowRect.y;
            int y2 = windowRect.y + windowRect.height;

            x1 = std::max(x1, (int) r.left);
            x2 = std::min(x2, (int) r.right);
            y1 = std::max(y1, (int) r.top);
            y2 = std::min(y2, (int) r.bottom);

            windowRect = wxRect(x1, y1, x2-x1, y2-y1);
        }
        break;
    }

    w->SetSize(windowRect.x,  windowRect.y, windowRect.width, windowRect.height, wxSIZE_ALLOW_MINUS_ONE);
}


#else // ----- non-Windows ----------------------------------------------


void PlaceWindow(wxWindow *w, cbPlaceDialogMode mode, bool enforce)
// TODO (thomas#1#): The non-Windows implementation is *pathetic*.
// However, I don't know how to do it well under GTK / X / Xinerama / whatever.
// Anyone?
{
    int the_mode;

    wxFrame *referenceWindow = Manager::Get()->GetAppWindow();
    if(!referenceWindow) // let's not crash on shutdown
        return;

    if(!w)
        cbThrow(_T("Passed NULL pointer to PlaceWindow."));


    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if(!enforce && cfg->ReadBool(_T("/dialog_placement/do_place")) == false)
        return;

    if(mode == pdlBest)
        the_mode = cfg->ReadInt(_T("/dialog_placement/dialog_position"), (int) pdlCentre);
    else
        the_mode = (int) mode;


    if(the_mode == pdlCentre || the_mode == pdlHead)
    {
        if(w->GetParent())
            w->CentreOnParent(); // poo!
        else
            w->CentreOnScreen();

        return;
    }
    else
    {
        wxRect windowRect = w->GetRect();
        wxRect parentRect = referenceWindow->GetRect();   // poo again!

        int x1 = windowRect.x;
        int x2 = windowRect.x + windowRect.width;
        int y1 = windowRect.y;
        int y2 = windowRect.y + windowRect.height;

        x1 = std::max(x1, parentRect.x);
        x2 = std::min(x2, parentRect.GetRight());
        y1 = std::max(y1, parentRect.y);
        y2 = std::min(y2, parentRect.GetBottom());

        w->SetSize(x1, y1, x2-x1, y2-y1, wxSIZE_ALLOW_MINUS_ONE);
    }
}

#endif //platform-specific placement code
