/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include <wx/file.h>
    #include <wx/filename.h>
    #include <wx/filesys.h>
    #include <wx/image.h>
    #include <wx/listctrl.h>
    #include <wx/menu.h>

    #include "cbexception.h"
    #include "configmanager.h" // ReadBool
    #include "filemanager.h"
    #include "globals.h"
    #include "logmanager.h"
    #include "manager.h"
    #include "projectmanager.h"
#endif

#include "tinyxml/tinyxml.h"

#include <wx/dirdlg.h>
#include <wx/filefn.h>
#include <wx/fontmap.h>
#include <wx/msgdlg.h>
#include <wx/tokenzr.h>

#include <algorithm>
#include <string>

#include "filefilters.h"
#include "tinyxml/tinywxuni.h"
#include "filegroupsandmasks.h"

#ifndef __WXMSW__
    #include <unistd.h> // readlink
    #include <sys/stat.h> // lstat
#endif

const wxString DEFAULT_WORKSPACE     = _T("default.workspace");
const wxString DEFAULT_ARRAY_SEP     = _T(";");

#ifndef __WXMAC__
const wxString DEFAULT_CONSOLE_TERM  = _T("xterm -T $TITLE -e");
#else
const wxString DEFAULT_CONSOLE_TERM  = _T("osascript -e 'tell app \"Terminal\"' -e 'activate' -e 'do script \"$SCRIPT\"' -e 'end tell'");
#endif
const wxString DEFAULT_CONSOLE_SHELL = _T("/bin/sh -c");

int GetPlatformsFromString(const wxString& platforms)
{
    bool pW = platforms.Contains(_("Windows"));
    bool pU = platforms.Contains(_("Unix"));
    bool pM = platforms.Contains(_("Mac"));
    bool pA = platforms.Contains(_("All"));

    if (pA || (pW && pU && pM))
        return spAll;

    int     p  = 0;
    if (pW) p |= spWindows;
    if (pU) p |= spUnix;
    if (pM) p |= spMac;
    return p;
}

wxString GetStringFromPlatforms(int platforms, bool forceSeparate)
{
    wxString ret;

    if (!forceSeparate)
    {
        int tmpAll = spWindows | spUnix | spMac;
        if (((platforms & tmpAll) == tmpAll) || ((platforms & spAll) == spAll))
            return _("All");
    }

    if (platforms & spWindows)
        ret << _("Windows;");
    if (platforms & spUnix)
        ret << _("Unix;");
    if (platforms & spMac)
        ret << _("Mac;");
    return ret;
}

/*
    Killerbot : 6 Oktober 2007
    The method has been extended with a bool to specify if the seperator should be appended at the end.
    Ultimate goal : every client gives as a value : false.
    Why : well a seperator should separate, there's nothing to separate at the end (like nothing to separate
    at the beginning, we don't put one there ...), but some client code is having problems when the separator is
    not present at the end. So for compatibility issues we have as default value for the new argument true, so the
    old code has the same behaviour as before [TODO: make those clients no longer dependent on this stupid final separator behaviour]
    New code will specify false as the bool value, so hoping rather soon a search on this method will have all hits showing
    false as the last argument ....
*/

wxString GetStringFromArray(const wxArrayString& array, const wxString& separator, bool SeparatorAtEnd)
{
    wxString out;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        out << array[i];
        if (i < array.GetCount() - 1 || SeparatorAtEnd)
            out << separator;
    }
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

std::vector<wxString> GetVectorFromString(const wxString& text, const wxString& separator, bool trimSpaces)
{
    std::vector<wxString> out;
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
                out.push_back(search);
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
            out.push_back(part);
    }
    return out;
}

wxArrayString MakeUniqueArray(const wxArrayString& array, bool caseSens)
{
    wxArrayString out;
    for (unsigned int i = 0; i < array.GetCount(); ++i)
    {
        if (caseSens)
        {
          if (out.Index(array[i]) == wxNOT_FOUND)
              out.Add(array[i]);
        }
        else
        {
          if (out.Index(array[i].Lower()) == wxNOT_FOUND)
              out.Add(array[i].Lower()); // append only lower-case for the moment
        }
    }
    return out;
}

void AppendArray(const wxArrayString& from, wxArrayString& to)
{
    for (unsigned int i = 0; i < from.GetCount(); ++i)
        to.Add(from[i]);
}

wxString UnixFilename(const wxString& filename, wxPathFormat format)
{
    wxString result = filename;

    if (format == wxPATH_NATIVE)
    {
        if (platform::windows)
            format = wxPATH_WIN;
        else
            format = wxPATH_UNIX;
    }

    // Unc-names always override platform specific settings otherwise they become corrupted
    bool unc_name = result.StartsWith(_T("\\\\"));
    if (format == wxPATH_WIN || unc_name) // wxPATH_WIN == wxPATH_DOS == wxPATH_OS2
    {
        result.Replace(wxT("/"), wxT("\\"));
        while (result.Replace(wxT("\\\\"), wxT("\\")))
            ; // loop for recursive removal of duplicate slashes
        if (unc_name)
            result.Prepend(wxT("\\"));
    }
    else
    {
        result.Replace(wxT("\\"), wxT("/"));
        while (result.Replace(wxT("//"), wxT("/")))
            ; // loop for recursive removal of duplicate slashes
    }

    return result;
}

void QuoteStringIfNeeded(wxString& str)
{
    bool hasSpace = str.Find(_T(' ')) != -1;
    bool hasParen = !platform::windows && (str.Find(_T('(')) != -1 || str.Find(_T(')')) != -1);
    if (!str.IsEmpty() && str.GetChar(0) != _T('"') && (hasSpace || hasParen))
        str = wxString(_T("\"")) + str + _T("\"");
}

wxString EscapeSpaces(const wxString& str)
{
    wxString ret = str;
    if (!ret.IsEmpty() && ret[0] != _T('"') && ret[0] != _T('\''))
    {
        // TODO: make it faster
        ret.Replace(_T(" "), _T("\\ "));
        ret.Replace(_T("\t"), _T("\\\t"));
    }
    return ret;
}

FileType FileTypeOf(const wxString& filename)
{
    wxString ext = filename.AfterLast(_T('.')).Lower();

    if (ext.IsSameAs(FileFilters::ASM_EXT) ||
        ext.IsSameAs(FileFilters::C_EXT) ||
        ext.IsSameAs(FileFilters::CC_EXT) ||
        ext.IsSameAs(FileFilters::CPP_EXT) ||
        ext.IsSameAs(FileFilters::CXX_EXT) ||
        ext.IsSameAs(FileFilters::CPLPL_EXT) ||
        ext.IsSameAs(FileFilters::S_EXT) ||
        ext.IsSameAs(FileFilters::SS_EXT) ||
        ext.IsSameAs(FileFilters::S62_EXT) ||
        ext.IsSameAs(FileFilters::D_EXT) ||
        ext.IsSameAs(FileFilters::F_EXT) ||
        ext.IsSameAs(FileFilters::F77_EXT) ||
        ext.IsSameAs(FileFilters::F90_EXT) ||
        ext.IsSameAs(FileFilters::F95_EXT) ||
        ext.IsSameAs(FileFilters::FOR_EXT) ||
        ext.IsSameAs(FileFilters::FPP_EXT) ||
        ext.IsSameAs(FileFilters::F03_EXT) ||
        ext.IsSameAs(FileFilters::F08_EXT) ||
        ext.IsSameAs(FileFilters::JAVA_EXT)
       )
        return ftSource;

    else if (ext.IsSameAs(FileFilters::H_EXT) ||
             ext.IsSameAs(FileFilters::HH_EXT) ||
             ext.IsSameAs(FileFilters::HPP_EXT) ||
             ext.IsSameAs(FileFilters::HXX_EXT) ||
             ext.IsSameAs(FileFilters::HPLPL_EXT) ||
             ext.IsSameAs(FileFilters::INL_EXT)
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

    else if (ext.IsSameAs(FileFilters::MSVC10_EXT))
        return ftMSVC10Project;

    else if (ext.IsSameAs(FileFilters::MSVC6_WORKSPACE_EXT))
        return ftMSVC6Workspace;

    else if (ext.IsSameAs(FileFilters::MSVC7_WORKSPACE_EXT))
        return ftMSVC7Workspace;

    else if (ext.IsSameAs(FileFilters::XCODE1_EXT))
        return ftXcode1Project; // Xcode 1.0+ (Mac OS X 10.3)

    else if (ext.IsSameAs(FileFilters::XCODE2_EXT))
        return ftXcode2Project; // Xcode 2.1+ (Mac OS X 10.4)

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

    else if (ext.IsSameAs(FileFilters::NATIVE_EXT))
        return ftNative;

    else if (ext.IsSameAs(FileFilters::EXECUTABLE_EXT))
        return ftExecutable;

    else if (ext.IsSameAs(FileFilters::XML_EXT))
        return ftXMLDocument;

    else if (ext.IsSameAs(FileFilters::SCRIPT_EXT))
        return ftScript;

    // DrewBoo: Before giving up, see if the ProjectManager
    // considers this extension a source or header
    // TODO (Morten#5#): Do what DrewBoo said: Try removing the above code
    // TODO (Morten#3#): This code should actually be a method of filegroups and masks or alike. So we collect all extension specific things in one place. As of now this would break ABI compatibilty with 08.02 so this should happen later.
    else
    {
        ProjectManager *prjMgr = Manager::Get()->GetProjectManager();
        if ( prjMgr )
        {
            const FilesGroupsAndMasks* fgm = prjMgr->GetFilesGroupsAndMasks();
            if (fgm)
            {
               for (unsigned int i = 0; i != fgm->GetGroupsCount(); ++i)
               {
                    if (fgm->GetGroupName(i) == _T("Sources") && fgm->MatchesMask(ext, i))
                        return ftSource;
                    if (fgm->GetGroupName(i) == _T("Headers") && fgm->MatchesMask(ext, i))
                        return ftHeader;
               }
            }
        }
    }

    return ftOther;
}

wxString cbFindFileInPATH(const wxString &filename)
{
    wxString pathValues;
    wxGetEnv(_T("PATH"), &pathValues);
    if (pathValues.empty())
        return wxEmptyString;

    const wxString &sep = platform::windows ? _T(";") : _T(":");
    wxChar pathSep = wxFileName::GetPathSeparator();
    const wxArrayString &pathArray = GetArrayFromString(pathValues, sep);
    for (size_t i = 0; i < pathArray.GetCount(); ++i)
    {
        if (wxFileExists(pathArray[i] + pathSep + filename))
        {
            if (pathArray[i].AfterLast(pathSep).IsSameAs(_T("bin")))
                return pathArray[i];
        }
    }
    return wxEmptyString;
}

void DoRememberSelectedNodes(wxTreeCtrl* tree, wxArrayString& selectedItemPaths)
{
    wxArrayTreeItemIds items;

    if (tree->GetSelections(items) < 1 )
        return;

    for (size_t i=0; i < items.GetCount(); ++i)
    {
        wxString path = wxEmptyString;
        wxTreeItemId item = items[i];
        while(item.IsOk())
        {
            path = _T("/") + tree->GetItemText(item) + path;
            item = tree->GetItemParent(item);
        }
        if (path != wxEmptyString)
            selectedItemPaths.Add(path);
    }
}

void DoSelectRememberedNode(wxTreeCtrl* tree, const wxTreeItemId& parent, wxString& selectedItemPath)
{
    if (tree && !selectedItemPath.IsEmpty())
    {
        wxString tmpPath;
        wxString folder;
        tmpPath = selectedItemPath;
        int pos = tmpPath.Find(_T('/'));
        while (pos == 0)
        {
            tmpPath = tmpPath.Right(tmpPath.Length() - pos - 1);
            pos = tmpPath.Find(_T('/'));
        }

        folder = tmpPath.Left(pos);
        tmpPath = tmpPath.Right(tmpPath.Length() - pos - 1);
        wxTreeItemId item = parent;
        wxTreeItemIdValue cookie = 0;

        while (item.IsOk())
        {
            if (tree->GetItemText(item) != folder)
                item = tree->GetNextSibling(item);
            else
            {
                if (pos < 0)
                {
                    tree->SelectItem(item);
                    break;
                }
                else
                {
                    item = tree->GetNextChild(item, cookie);
                    DoSelectRememberedNode(tree, item, tmpPath);
                }
            }
        }

    }
}

bool DoRememberExpandedNodes(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths, wxString& path)
{
    // remember expanded tree nodes of this tree
    if (!tree || !parent.IsOk())
        return false;

    wxString originalPath = path;
    bool found = false;

    wxTreeItemIdValue cookie = 0;

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
        //Manager::Get()->GetLogManager()->Log(mltDevDebug, path);
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

        //Manager::Get()->GetLogManager()->Log(mltDevDebug, "%s, %s", folder.c_str(), tmpPath.c_str());

        wxTreeItemIdValue cookie = 0;

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

void SaveTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths, wxArrayString& selectedItemPaths)
{
    nodePaths.Clear();
    if (!parent.IsOk() || !tree || !tree->ItemHasChildren(parent) || !tree->IsExpanded(parent))
        return;

    wxString tmp;
    if (!DoRememberExpandedNodes(tree, parent, nodePaths, tmp))
        nodePaths.Add(tmp); // just the tree root

    selectedItemPaths.Clear();
    DoRememberSelectedNodes(tree, selectedItemPaths);
}

void RestoreTreeState(wxTreeCtrl* tree, const wxTreeItemId& parent, wxArrayString& nodePaths, wxArrayString& selectedItemPaths)
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
    for (unsigned int i = 0; i < selectedItemPaths.GetCount(); ++i)
        DoSelectRememberedNode(tree, tree->GetRootItem(), selectedItemPaths[i]);

    selectedItemPaths.Clear();
}

bool CreateDirRecursively(const wxString& full_path, int perms)
{
    if (wxDirExists(full_path)) // early out
        return true;

    wxArrayString dirs;
    wxString currdir;
    wxFileName tmp(full_path);
    currdir = tmp.GetVolume() + tmp.GetVolumeSeparator() + wxFILE_SEP_PATH;
    dirs = tmp.GetDirs();

    for (size_t i = 0; i < dirs.GetCount(); ++i)
    {
        currdir << dirs[i];
        if (!wxDirExists(currdir) && !wxMkdir(currdir, perms))
            return false;
        currdir << wxFILE_SEP_PATH;
    }
    return true;
}

bool CreateDir(const wxString& full_path, int perms)
{
    if (!wxDirExists(full_path) && !wxMkdir(full_path, perms))
        return false;

    return true;
}

wxString ChooseDirectory(wxWindow* parent,
                         const wxString& message,
                         const wxString& initialPath,
                         const wxString& basePath,
                         bool askToMakeRelative, // relative to initialPath
                         bool showCreateDirButton) // where supported
{
    wxDirDialog dlg(parent, message, _T(""),
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

// Reads a wxString from a file. File must be open. File is closed automatically.
bool cbRead(wxFile& file, wxString& st, wxFontEncoding encoding)
{
    st.Empty();
    if (!file.IsOpened())
        return false;

    int len = file.Length();
    if (!len)
    {
        file.Close();
        return true;
    }

    char* buff = new char[len+1];
    if (!buff) // remark by killerbot : this is useless, since when out of mem --> exception (this is not malloc you know)
    {
        file.Close();
        return false;
    }
    file.Read((void*)buff, len);
    file.Close();
    buff[len]='\0';

    DetectEncodingAndConvert(buff, st, encoding);
    delete [] buff;

    return true;
}

wxString cbReadFileContents(wxFile& file, wxFontEncoding encoding)
{
    wxString st;
    cbRead(file, st, encoding);
    return st;
}

// Writes a wxString to a file. File must be open. File is closed automatically.
bool cbWrite(wxFile& file, const wxString& buff, wxFontEncoding encoding)
{
    bool result = false;
    if (file.IsOpened())
    {
        wxCSConv conv(encoding);
        result = file.Write(buff,conv);
        if (result)
            file.Flush();
        file.Close();
    }
    return result;
}

// Writes a wxString to a file. Takes care of unicode and uses a temporary file
// to save first and then it copies it over the original.
bool cbSaveToFile(const wxString& filename, const wxString& contents, wxFontEncoding encoding, bool bom)
{
    return Manager::Get()->GetFileManager()->Save(filename, contents, encoding, bom);
}

// Save a TinyXML document correctly, even if the path contains unicode characters.
bool cbSaveTinyXMLDocument(TiXmlDocument* doc, const wxString& filename)
{
    return TinyXML::SaveDocument(filename, doc);
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
const wxWX2MBbuf cbU2C(const wxString& str)
{
    if (platform::unicode)
        return str.mb_str(wxConvUTF8);
    else
        return str.mb_str();
}

// Try converting a C-string from different encodings until a possible match is found.
// This tries the following encoding converters (in the same order):
// utf8, system, default and iso8859-1 to iso8859-15
wxFontEncoding DetectEncodingAndConvert(const char* strIn, wxString& strOut, wxFontEncoding possibleEncoding)
{
    wxFontEncoding encoding = possibleEncoding;
    strOut.Clear();

    if (platform::unicode)
    {
        if (possibleEncoding != wxFONTENCODING_UTF16 &&
            possibleEncoding != wxFONTENCODING_UTF16LE &&
            possibleEncoding != wxFONTENCODING_UTF16BE &&
            possibleEncoding != wxFONTENCODING_UTF32 &&
            possibleEncoding != wxFONTENCODING_UTF32LE &&
            possibleEncoding != wxFONTENCODING_UTF32BE)
        {
            // crashes deep in the runtime (windows, at least)
            // if one of the above encodings, hence the guard
            wxCSConv conv(possibleEncoding);
            strOut = wxString(strIn, conv);

            if (strOut.Length() == 0)
            {
                // oops! wrong encoding...

                // try utf8 first, if that was not what was asked for
                if (possibleEncoding != wxFONTENCODING_UTF8)
                {
                    encoding = wxFONTENCODING_UTF8;
                    strOut = wxString(strIn, wxConvUTF8);
                }

                // check again: if still not right, try system encoding, default encoding and then iso8859-1 to iso8859-15
                if (strOut.Length() == 0)
                {
                    for (int i = wxFONTENCODING_SYSTEM; i < wxFONTENCODING_ISO8859_MAX; ++i)
                    {
                        encoding = (wxFontEncoding)i;
                        if (encoding == possibleEncoding)
                            continue; // skip if same as what was asked
                        wxCSConv conv(encoding);
                        strOut = wxString(strIn, conv);
                        if (strOut.Length() != 0)
                            break; // got it!
                    }
                }
            }
        }
        else
        {
            strOut = (const wxChar*) strIn;
        }
    }
    else
    {
        strOut = (const wxChar*) strIn;
    }
    return encoding;
}

wxString URLEncode(const wxString &str) // not sure this is 100% standards compliant, but I hope so
{
    wxString ret;
    wxString t;
    for (unsigned int i = 0; i < str.length(); ++i)
    {
        wxChar c = str[i];
        if (  (c >= _T('A') && c <= _T('Z'))
           || (c >= _T('a') && c <= _T('z'))
           || (c >= _T('0') && c <= _T('9'))
           || (c == _T('.'))
           || (c == _T('-'))
           || (c == _T('_')) )

            ret.Append(c);
        else if (c == _T(' '))
            ret.Append(_T('+'));
        else
        {
            t.sprintf(_T("%%%02X"), (unsigned int) c);
            ret.Append(t);
        }
    }
    return ret;
}

/** Adds support for backtick'd expressions under Windows. */
typedef std::map<wxString, wxString> BackticksMap;
BackticksMap m_Backticks; // all calls share the same cache
wxString ExpandBackticks(wxString& str) // backticks are written in-place to str
{
    wxString ret;

    // this function is not windows-only anymore because we parse the backticked command's output
    // for compiler/linker search dirs

    size_t start = str.find(_T('`'));
    if (start == wxString::npos)
        return ret; // no backticks here
    size_t end = str.find(_T('`'), start + 1);
    if (end == wxString::npos)
        return ret; // no ending backtick; error?

    while (start != wxString::npos && end != wxString::npos)
    {
        wxString cmd = str.substr(start + 1, end - start - 1);
        cmd.Trim(true);
        cmd.Trim(false);
        if (cmd.IsEmpty())
            break;

        wxString bt;
        BackticksMap::iterator it = m_Backticks.find(cmd);
        if (it != m_Backticks.end())
        {
            // in cache :)
            bt = it->second;
        }
        else
        {
            Manager::Get()->GetLogManager()->DebugLog(F(_T("Caching result of `%s`"), cmd.wx_str()));
            wxArrayString output;
            if (platform::WindowsVersion() >= platform::winver_WindowsNT2000)
                wxExecute(_T("cmd /c ") + cmd, output, wxEXEC_NODISABLE);
            else
                wxExecute(cmd, output, wxEXEC_NODISABLE);
            bt = GetStringFromArray(output, _T(" "));
            // add it in the cache
            m_Backticks[cmd] = bt;
            Manager::Get()->GetLogManager()->DebugLog(_T("Cached"));
        }
        ret << bt << _T(' ');
        str = str.substr(0, start) + bt + str.substr(end + 1, wxString::npos);

        // find next occurrence
        start = str.find(_T('`'));
        end = str.find(_T('`'), start + 1);
    }

    return ret; // return a list of the replaced expressions
}

wxMenu* CopyMenu(wxMenu* mnu, bool with_accelerators)
{
    if (!mnu || mnu->GetMenuItemCount() < 1)
        return nullptr;
    wxMenu* theMenu = new wxMenu();

    for (size_t i = 0; i < mnu->GetMenuItemCount();++i)
    {
        wxMenuItem* tmpItem = mnu->FindItemByPosition(i);
        wxMenuItem* theItem = new wxMenuItem(NULL,
                                             tmpItem->GetId(),
                                             with_accelerators?tmpItem->GetItemLabel():tmpItem->GetItemLabelText(),
                                             tmpItem->GetHelp(),
                                             tmpItem->GetKind(),
                                             CopyMenu(tmpItem->GetSubMenu()));
        theMenu->Append(theItem);
    }
    return theMenu;
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
//    if (!f.IsAbsolute())
    {
        f.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, base);
        result = f.IsOk();
    }
    return result;
}

// Checks whether 'suffix' could be a suffix of 'path' and therefore represents
// the same path. This is used to check whether a relative path could represent
// the same path as absolute path. For instance, for
// suffix = sdk/globals.cpp
// path = /home/user/codeblocks/trunk/src/sdk/globals.cpp
// it returns true. The function expects that 'path' is normalized and compares
// 'path' with 'suffix' starting from the end of the path. When it reaches .. in
// 'suffix' it gives up (there is no way to check relative filename names
// exactly) and if the path compared so far is identical, it returns true
bool IsSuffixOfPath(wxFileName const & suffix, wxFileName const & path)
{
    if (path.GetFullName() != suffix.GetFullName())
    {
        return false;
    }

    wxArrayString suffixDirArray = suffix.GetDirs();
    wxArrayString pathDirArray = path.GetDirs();

    int j = pathDirArray.GetCount() - 1;
    for (int i = suffixDirArray.GetCount() - 1; i >= 0; i--)
    {
        // skip paths like /./././ and ////
        if (suffixDirArray[i] == _T(".") || suffixDirArray[i] == _T(""))
            continue;

        // suffix has more directories than path - cannot represent the same path
        if (j < 0)
            return false;

        // suffix contains ".." - from now on we cannot precisely determine
        // whether suffix and path match - we assume that they do
        if (suffixDirArray[i] == _T(".."))
            return true;
        // the corresponding directories of the two paths differ
        else if (suffixDirArray[i] != pathDirArray[j])
            return false;

        j--;
    }

    if (suffix.IsAbsolute() && (j >= 0 || suffix.GetVolume() != path.GetVolume()))
        return false;

    // 'suffix' is a suffix of 'path'
    return true;
}

// function to check the common controls version
#ifdef __WXMSW__
#include <windows.h>
#include <shlwapi.h>
bool UsesCommonControls6()
{
    bool result = false;
    HINSTANCE hinstDll;
    hinstDll = LoadLibrary(_T("comctl32.dll"));
    if (hinstDll)
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
               result = dvi.dwMajorVersion == 6;
        }

        FreeLibrary(hinstDll);
    }
    return result;
}
#else
bool UsesCommonControls6()
{
    // for non-windows platforms, return true
    // as this is only used for knowing if bitmaps support transparency or not
    return true;
}
#endif

wxBitmap cbLoadBitmap(const wxString& filename, wxBitmapType bitmapType)
{
    // cache this, can't change while we 're running :)
    static bool oldCommonControls = !UsesCommonControls6();

    wxImage im;
    wxFileSystem* fs = new wxFileSystem;
    wxFSFile* f = fs->OpenFile(filename);
    if (f)
    {
        wxInputStream* is = f->GetStream();
        im.LoadFile(*is, bitmapType);
        delete f;
    }
    delete fs;
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

typedef APIENTRY HMONITOR (*MonitorFromWindow_t)(HWND, DWORD);
typedef APIENTRY BOOL (*GetMonitorInfo_t)(HMONITOR, LPMONITORINFO);

void PlaceWindow(wxTopLevelWindow *w, cbPlaceDialogMode mode, bool enforce)
{
    HMONITOR hMonitor;
    MONITORINFO mi;
    RECT        r;

    int the_mode;

    if (!w)
        cbThrow(_T("Passed NULL pointer to PlaceWindow."));

    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (!enforce && cfg->ReadBool(_T("/dialog_placement/do_place")) == false)
        return;

    wxWindow* referenceWindow = Manager::Get()->GetAppWindow();
    if (!referenceWindow)    // no application window available, so this is as good as we can get
        referenceWindow = w;

    wxRect windowRect = w->GetRect();

    if (mode == pdlBest)
        the_mode = cfg->ReadInt(_T("/dialog_placement/dialog_position"), (int) pdlCentre);
    else
        the_mode = (int) mode;


    static MonitorFromWindow_t MonitorFromWindowProc = (MonitorFromWindow_t) GetProcAddress(GetModuleHandle(_T("user32.dll")), "MonitorFromWindow");
    static GetMonitorInfo_t    GetMonitorInfoProc    = (GetMonitorInfo_t)    GetProcAddress(GetModuleHandle(_T("user32.dll")), "GetMonitorInfoA");
    int monitorWidth;
    int monitorHeight;

    if (GetMonitorInfoProc)
    {
        hMonitor = MonitorFromWindowProc((HWND) referenceWindow->GetHandle(), MONITOR_DEFAULTTONEAREST);

        mi.cbSize = sizeof(mi);
        GetMonitorInfoProc(hMonitor, &mi);
        r = mi.rcWork;

        monitorWidth  = r.right - r.left;
        monitorHeight = r.bottom - r. top;
    }
    else // Win95, NT4: support only single monitor
    {
        wxDisplaySize(&monitorWidth, &monitorHeight);
        r.left = r.top = 0;
    }


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

            if (windowRect.width > monitorWidth) // cannot place without clipping, so centre it
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
            if (windowRect.height > monitorHeight) // cannot place without clipping, so centre it
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


void PlaceWindow(wxTopLevelWindow *w, cbPlaceDialogMode mode, bool enforce)
// TODO (thomas#1#): The non-Windows implementation is *pathetic*.
// However, I don't know how to do it well under GTK / X / Xinerama / whatever.
// Anyone?
{
    int the_mode;

    wxWindow* referenceWindow = Manager::Get()->GetAppWindow();
    if (!referenceWindow) // let's not crash on shutdown
        return;

    if (!w)
        cbThrow(_T("Passed NULL pointer to PlaceWindow."));


    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
    if (!enforce && cfg->ReadBool(_T("/dialog_placement/do_place")) == false)
        return;

    if (mode == pdlBest)
        the_mode = cfg->ReadInt(_T("/dialog_placement/dialog_position"), (int) pdlCentre);
    else
        the_mode = (int) mode;


    if (the_mode == pdlCentre || the_mode == pdlHead)
    {
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

DirAccessCheck cbDirAccessCheck(const wxString& dir)
{
    wxString actualDir = dir;
    // append ending path separator if needed
    if (actualDir.Last() != _T('/') && actualDir.Last() != _T('\\'))
        actualDir << wxFILE_SEP_PATH;

    if (!wxDirExists(actualDir))
        return dacInvalidDir;

    wxString testFile = wxFileName::CreateTempFileName(actualDir);
    if (!testFile.IsEmpty())
    {
        // ok, write-access confirmed
        // now remove the temporary file and return success
        wxRemoveFile(testFile);
        return dacReadWrite;
    }

    // if we reached here, the directory is not writable
    return dacReadOnly;
}


namespace platform
{
    windows_version_t cb_get_os()
    {
        if (!platform::windows)
        {
            return winver_NotWindows;
        }
        else
        {

            int famWin95 = wxOS_WINDOWS_9X;
            int famWinNT = wxOS_WINDOWS_NT;

            int Major = 0;
            int Minor = 0;
            int family = wxGetOsVersion(&Major, &Minor);

            if (family == famWin95)
                 return winver_Windows9598ME;

            if (family == famWinNT)
            {
                if (Major == 5 && Minor == 0)
                    return winver_WindowsNT2000;

                if (Major == 5 && Minor == 1)
                    return winver_WindowsXP;

                if (Major == 5 && Minor == 2)
                    return winver_WindowsServer2003;

                if (Major == 6 && Minor == 0)
                    return winver_WindowsVista;

                if (Major == 6 && Minor == 1)
                    return winver_Windows7;
            }

            return winver_UnknownWindows;
        }
    };

    windows_version_t WindowsVersion()
    {
        static const windows_version_t theOS = cb_get_os();
        return theOS;
    }
}

// returns the real path of a file by resolving symlinks
// not yet optimal but should do for now
// one thing that's not checked yet are circular symlinks - watch out!
wxString realpath(const wxString& path)
{
#ifdef __WXMSW__
    // no symlinks support on windows
    return path;
#else
    char buf[2048] = {};
    struct stat buffer;
    std::string ret = (const char*)cbU2C(path);
    size_t lastPos = 0;
    size_t slashPos = ret.find('/', lastPos);
    while (slashPos != std::string::npos)
    {
        if (lstat(ret.substr(0, slashPos).c_str(), &buffer) == 0)
        {
            if (S_ISLNK(buffer.st_mode))
            {
                int s = readlink(ret.substr(0, slashPos).c_str(), buf, sizeof(buf));
                buf[s] = 0;
                if (s > 0 && buf[0] != '/' && buf[0] != '~')
                {
                    // relative
                    ret = ret.substr(0, lastPos) + buf + ret.substr(slashPos, ret.size() - slashPos);
                }
                else
                {
                    // absolute
                    ret = buf + ret.substr(slashPos, ret.size() - slashPos);

                    // start again at the beginning in case the path returned also
                    // has symlinks. For example if using /etc/alternatives this will
                    // be the case
                    s = 0;
                }
                slashPos = s;
            }
        }

        while (ret[++slashPos] == '/')
            ;
        lastPos = slashPos;
        slashPos = ret.find('/', slashPos);
    }
    return cbC2U(ret.c_str());
#endif
}

int cbMessageBox(const wxString& message, const wxString& caption, int style, wxWindow *parent, int x, int y)
{
    if (!parent)
        parent = Manager::Get()->GetAppWindow();

    // Cannot create a wxMessageDialog with a NULL as parent
    if (!parent)
    {
      // wxMessage*Box* returns any of: wxYES, wxNO, wxCANCEL, wxOK.
      int answer = wxMessageBox(message, caption, style, parent, x, y);
      switch (answer)
      {
        // map answer to the one of wxMessage*Dialog* to ensure compatibility
        case (wxOK):
          return wxID_OK;
        case (wxCANCEL):
          return wxID_CANCEL;
        case (wxYES):
          return wxID_YES;
        case (wxNO):
          return wxID_NO;
        default:
          return -1; // NOTE: Cannot happen unless wxWidgets API changes
      }
    }

    wxMessageDialog dlg(parent, message, caption, style, wxPoint(x,y));
    PlaceWindow(&dlg);
    // wxMessage*Dialog* returns any of wxID_OK, wxID_CANCEL, wxID_YES, wxID_NO
    return dlg.ShowModal();
}
