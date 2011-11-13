/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk_precomp.h"
#include <map>
#include <wx/arrstr.h>
#include "filefilters.h"
#include "globals.h"

typedef std::map<wxString, wxString> FileFiltersMap;
static FileFiltersMap s_Filters;

static size_t s_LastFilterAllIndex = 0;

// Let's add some default extensions.
// The rest will be added by editor lexers ;)
void FileFilters::AddDefaultFileFilters()
{
    if (s_Filters.size() != 0)
        return;

    Add(_("Code::Blocks workspace files"),         _T("*.workspace"));
    Add(_("Code::Blocks project files"),           _T("*.cbp"));
    Add(_("Code::Blocks project/workspace files"), _T("*.workspace;*.cbp"));
    Add(_("Bloodshed Dev-C++ project files"),      _T("*.dev"));
    Add(_("MS Visual C++ 6.0 project files"),      _T("*.dsp"));
    Add(_("MS Visual Studio 7.0+ project files"),  _T("*.vcproj"));
    Add(_("MS Visual C++ 6.0 workspace files"),    _T("*.dsw"));
    Add(_("MS Visual Studio 7.0+ solution files"), _T("*.sln"));
    Add(_("Apple Xcode 1.x project bundles"),      _T("*.xcode"));
    Add(_("Apple Xcode 2.x project bundles"),      _T("*.xcodeproj"));
}

bool FileFilters::Add(const wxString& name, const wxString& mask)
{
    if (name.IsEmpty() || mask.IsEmpty())
        return false; // both must be valid

    if (mask.Index(_T(',')) != wxString::npos)
    {
        // replace commas with semicolons
        wxString tmp = mask;
        while (tmp.Replace(_T(","), _T(";")))
            ;
        s_Filters[name] = tmp;
    }
    else
        s_Filters[name] = mask;

    return true;
}

wxString FileFilters::GetFilterString(const wxString& ext)
{
    size_t count = 0;
    wxString ret;
    for (FileFiltersMap::iterator it = s_Filters.begin(); it != s_Filters.end(); ++it)
    {
        if (!ext.IsEmpty())
        {
            // filter based on parameter
            bool match = false;
            wxArrayString array = GetArrayFromString(it->second, _T(";"), true);
            for (size_t i = 0; i < array.GetCount(); ++i)
            {
                if (ext.Matches(array[i]))
                {
                    match = true;
                    break;
                }
            }
            if (!match)
                continue; // filtered
        }
        ++count;
        if (!ret.IsEmpty())
            ret << _T('|');
        ret << it->first << _T('|') << it->second;
    }

    // last filter is always "All"
    if (!ret.IsEmpty())
        ret << _T('|');
    ret << GetFilterAll();

    s_LastFilterAllIndex = count;

    return ret;
}

wxString FileFilters::GetFilterAll()
{
    s_LastFilterAllIndex = 0;
    if (platform::windows)
        return _("All files (*.*)|*.*");

    return _("All files (*)|*");
}

size_t FileFilters::GetIndexForFilterAll()
{
    return s_LastFilterAllIndex;
}

bool FileFilters::GetFilterIndexFromName(const wxString& FiltersList, const wxString& FilterName, int& Index)
{
    bool bFound = false;
    // the List will contain 2 entries per type (description, mask)
    wxArrayString List = GetArrayFromString(FiltersList, _T("|"), true);
    int LoopEnd = static_cast<int>(List.GetCount());
    for(int idxList = 0; idxList < LoopEnd; idxList+=2)
    {
        if (List[idxList] == FilterName)
        {
            Index = idxList/2;
            bFound = true;
            break;
        }
    } // end for : idx : idxList
    return bFound;
} // end of GetFilterIndexFromName

bool FileFilters::GetFilterNameFromIndex(const wxString& FiltersList, int Index, wxString& FilterName)
{    // we return the name (not the mask)
    bool bFound = false;
    // the List will contain 2 entries per type (description, mask)
    wxArrayString List = GetArrayFromString(FiltersList, _T("|"), true);
    int LoopEnd = static_cast<int>(List.GetCount());
    if (2*Index < LoopEnd)
    {
        FilterName = List[2*Index];
        bFound = true;
    }
    return bFound;
} // end of GetFilterStringFromIndex

// define some constants used throughout C::B

const wxString FileFilters::WORKSPACE_EXT           = _T("workspace");
const wxString FileFilters::CODEBLOCKS_EXT          = _T("cbp");
const wxString FileFilters::DEVCPP_EXT              = _T("dev");
const wxString FileFilters::MSVC6_EXT               = _T("dsp");
const wxString FileFilters::MSVC7_EXT               = _T("vcproj");
const wxString FileFilters::MSVC6_WORKSPACE_EXT     = _T("dsw");
const wxString FileFilters::MSVC7_WORKSPACE_EXT     = _T("sln");
const wxString FileFilters::XCODE1_EXT              = _T("xcode");
const wxString FileFilters::XCODE2_EXT              = _T("xcodeproj");
const wxString FileFilters::ASM_EXT                 = _T("asm");
const wxString FileFilters::D_EXT                   = _T("d");
const wxString FileFilters::F_EXT                   = _T("f");
const wxString FileFilters::F77_EXT                 = _T("f77");
const wxString FileFilters::F90_EXT                 = _T("f90");
const wxString FileFilters::F95_EXT                 = _T("f95");
const wxString FileFilters::FOR_EXT                 = _T("for");
const wxString FileFilters::FPP_EXT                 = _T("fpp");
const wxString FileFilters::F03_EXT                 = _T("f03");
const wxString FileFilters::F08_EXT                 = _T("f08");
const wxString FileFilters::JAVA_EXT                = _T("java");
const wxString FileFilters::C_EXT                   = _T("c");
const wxString FileFilters::CC_EXT                  = _T("cc");
const wxString FileFilters::CPP_EXT                 = _T("cpp");
const wxString FileFilters::CXX_EXT                 = _T("cxx");
const wxString FileFilters::INL_EXT                 = _T("inl");
const wxString FileFilters::H_EXT                   = _T("h");
const wxString FileFilters::HH_EXT                  = _T("hh");
const wxString FileFilters::HPP_EXT                 = _T("hpp");
const wxString FileFilters::HXX_EXT                 = _T("hxx");
const wxString FileFilters::S_EXT                   = _T("s");
const wxString FileFilters::SS_EXT                  = _T("ss");
const wxString FileFilters::S62_EXT                 = _T("s62");
const wxString FileFilters::OBJECT_EXT              = _T("o");
const wxString FileFilters::XRCRESOURCE_EXT         = _T("xrc");
const wxString FileFilters::STATICLIB_EXT           = _T("a");
const wxString FileFilters::RESOURCE_EXT            = _T("rc");
const wxString FileFilters::RESOURCEBIN_EXT         = _T("res");
const wxString FileFilters::XML_EXT                 = _T("xml");
const wxString FileFilters::SCRIPT_EXT              = _T("script");
#ifdef __WXMSW__
    const wxString FileFilters::DYNAMICLIB_EXT      = _T("dll");
    const wxString FileFilters::EXECUTABLE_EXT      = _T("exe");
    const wxString FileFilters::NATIVE_EXT          = _T("sys");
#elif __WXMAC__
    const wxString FileFilters::DYNAMICLIB_EXT      = _T("dylib");
    const wxString FileFilters::EXECUTABLE_EXT      = _T("");
    const wxString FileFilters::NATIVE_EXT          = _T("");
#else
    const wxString FileFilters::DYNAMICLIB_EXT      = _T("so");
    const wxString FileFilters::EXECUTABLE_EXT      = _T("");
    const wxString FileFilters::NATIVE_EXT          = _T("");
#endif

// dot.ext version
const wxString FileFilters::WORKSPACE_DOT_EXT       = _T('.') + FileFilters::WORKSPACE_EXT;
const wxString FileFilters::CODEBLOCKS_DOT_EXT      = _T('.') + FileFilters::CODEBLOCKS_EXT;
const wxString FileFilters::DEVCPP_DOT_EXT          = _T('.') + FileFilters::DEVCPP_EXT;
const wxString FileFilters::MSVC6_DOT_EXT           = _T('.') + FileFilters::MSVC6_EXT;
const wxString FileFilters::MSVC7_DOT_EXT           = _T('.') + FileFilters::MSVC7_EXT;
const wxString FileFilters::MSVC6_WORKSPACE_DOT_EXT = _T('.') + FileFilters::MSVC6_WORKSPACE_EXT;
const wxString FileFilters::MSVC7_WORKSPACE_DOT_EXT = _T('.') + FileFilters::MSVC7_WORKSPACE_EXT;
const wxString FileFilters::XCODE1_DOT_EXT          = _T('.') + FileFilters::XCODE1_EXT;
const wxString FileFilters::XCODE2_DOT_EXT          = _T('.') + FileFilters::XCODE2_EXT;
const wxString FileFilters::ASM_DOT_EXT             = _T('.') + FileFilters::ASM_EXT;
const wxString FileFilters::D_DOT_EXT               = _T('.') + FileFilters::D_EXT;
const wxString FileFilters::F_DOT_EXT               = _T('.') + FileFilters::F_EXT;
const wxString FileFilters::F77_DOT_EXT             = _T('.') + FileFilters::F77_EXT;
const wxString FileFilters::F90_DOT_EXT             = _T('.') + FileFilters::F90_EXT;
const wxString FileFilters::F95_DOT_EXT             = _T('.') + FileFilters::F95_EXT;
const wxString FileFilters::FOR_DOT_EXT             = _T('.') + FileFilters::FOR_EXT;
const wxString FileFilters::FPP_DOT_EXT             = _T('.') + FileFilters::FPP_EXT;
const wxString FileFilters::F03_DOT_EXT             = _T('.') + FileFilters::F03_EXT;
const wxString FileFilters::F08_DOT_EXT             = _T('.') + FileFilters::F08_EXT;
const wxString FileFilters::JAVA_DOT_EXT            = _T('.') + FileFilters::JAVA_EXT;
const wxString FileFilters::C_DOT_EXT               = _T('.') + FileFilters::C_EXT;
const wxString FileFilters::CC_DOT_EXT              = _T('.') + FileFilters::CC_EXT;
const wxString FileFilters::CPP_DOT_EXT             = _T('.') + FileFilters::CPP_EXT;
const wxString FileFilters::INL_DOT_EXT             = _T('.') + FileFilters::INL_EXT;
const wxString FileFilters::CXX_DOT_EXT             = _T('.') + FileFilters::CXX_EXT;
const wxString FileFilters::H_DOT_EXT               = _T('.') + FileFilters::H_EXT;
const wxString FileFilters::HH_DOT_EXT              = _T('.') + FileFilters::HH_EXT;
const wxString FileFilters::HPP_DOT_EXT             = _T('.') + FileFilters::HPP_EXT;
const wxString FileFilters::HXX_DOT_EXT             = _T('.') + FileFilters::HXX_EXT;
const wxString FileFilters::S_DOT_EXT               = _T('.') + FileFilters::S_EXT;
const wxString FileFilters::SS_DOT_EXT              = _T('.') + FileFilters::SS_EXT;
const wxString FileFilters::S62_DOT_EXT             = _T('.') + FileFilters::S62_EXT;
const wxString FileFilters::OBJECT_DOT_EXT          = _T('.') + FileFilters::OBJECT_EXT;
const wxString FileFilters::XRCRESOURCE_DOT_EXT     = _T('.') + FileFilters::XRCRESOURCE_EXT;
const wxString FileFilters::STATICLIB_DOT_EXT       = _T('.') + FileFilters::STATICLIB_EXT;
const wxString FileFilters::RESOURCE_DOT_EXT        = _T('.') + FileFilters::RESOURCE_EXT;
const wxString FileFilters::RESOURCEBIN_DOT_EXT     = _T('.') + FileFilters::RESOURCEBIN_EXT;
const wxString FileFilters::XML_DOT_EXT             = _T('.') + FileFilters::XML_EXT;
const wxString FileFilters::SCRIPT_DOT_EXT          = _T('.') + FileFilters::SCRIPT_EXT;
#ifdef __WXMSW__
    const wxString FileFilters::DYNAMICLIB_DOT_EXT  = _T('.') + FileFilters::DYNAMICLIB_EXT;
    const wxString FileFilters::EXECUTABLE_DOT_EXT  = _T('.') + FileFilters::EXECUTABLE_EXT;
    const wxString FileFilters::NATIVE_DOT_EXT      = _T('.') + FileFilters::NATIVE_EXT;
#else
    const wxString FileFilters::DYNAMICLIB_DOT_EXT  = _T('.') + FileFilters::DYNAMICLIB_EXT;
    const wxString FileFilters::EXECUTABLE_DOT_EXT  = EXECUTABLE_EXT; // no dot, since no extension
    const wxString FileFilters::NATIVE_DOT_EXT      = NATIVE_EXT; // no dot, since no extension
#endif
