#include <map>
#include <sdk_precomp.h>
#include "filefilters.h"

typedef std::map<wxString, wxString> FileFiltersMap;
static FileFiltersMap s_Filters;

static size_t s_LastFilterAllIndex = 0;

// Let's add some default extensions.
// The rest will be added by editor lexers ;)
void FileFilters::AddDefaultFileFilters()
{
    if (s_Filters.size() != 0)
        return;
    Add(_("Code::Blocks workspace files"),             _T("*.workspace"));
    Add(_("Code::Blocks project files"),               _T("*.cbp"));
    Add(_("Bloodshed Dev-C++ project files"),          _T("*.dev"));
    Add(_("MS Visual C++ 6.0 project files"),          _T("*.dsp"));
    Add(_("MS Visual Studio 7.0+ project files"),     _T("*.vcproj"));
    Add(_("MS Visual C++ 6.0 workspace files"),        _T("*.dsw"));
    Add(_("MS Visual Studio 7.0+ solution files"),     _T("*.sln"));
}

bool FileFilters::Add(const wxString& name, const wxString& mask)
{
    if (name.IsEmpty() || mask.IsEmpty())
        return false; // both must be valid
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
            wxArrayString array = GetArrayFromString(it->second, _T(","), true);
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
#ifdef __WXMSW__
    return _("All files (*.*)|*.*");
#else
    return _("All files (*)|*");
#endif
}

size_t FileFilters::GetIndexForFilterAll()
{
    return s_LastFilterAllIndex;
}

// define some constants used throughout C::B

const wxString FileFilters::WORKSPACE_EXT                = _T("workspace");
const wxString FileFilters::CODEBLOCKS_EXT               = _T("cbp");
const wxString FileFilters::DEVCPP_EXT                   = _T("dev");
const wxString FileFilters::MSVC6_EXT                    = _T("dsp");
const wxString FileFilters::MSVC7_EXT                    = _T("vcproj");
const wxString FileFilters::MSVC6_WORKSPACE_EXT          = _T("dsw");
const wxString FileFilters::MSVC7_WORKSPACE_EXT          = _T("sln");
const wxString FileFilters::CPP_EXT                      = _T("cpp");
const wxString FileFilters::D_EXT                        = _T("d");
const wxString FileFilters::C_EXT                        = _T("c");
const wxString FileFilters::CC_EXT                       = _T("cc");
const wxString FileFilters::CXX_EXT                      = _T("cxx");
const wxString FileFilters::HPP_EXT                      = _T("hpp");
const wxString FileFilters::H_EXT                        = _T("h");
const wxString FileFilters::HH_EXT                       = _T("hh");
const wxString FileFilters::HXX_EXT                      = _T("hxx");
const wxString FileFilters::OBJECT_EXT                   = _T("o");
const wxString FileFilters::XRCRESOURCE_EXT              = _T("xrc");
const wxString FileFilters::STATICLIB_EXT                = _T("a");
#ifdef __WXMSW__
    const wxString FileFilters::DYNAMICLIB_EXT           = _T("dll");
    const wxString FileFilters::EXECUTABLE_EXT           = _T("exe");
    const wxString FileFilters::RESOURCE_EXT             = _T("rc");
    const wxString FileFilters::RESOURCEBIN_EXT          = _T("res");
#else
    const wxString FileFilters::DYNAMICLIB_EXT	        = _T("so");
    const wxString FileFilters::EXECUTABLE_EXT	        = _T("");
    const wxString FileFilters::RESOURCE_EXT             = _T("");
    const wxString FileFilters::RESOURCEBIN_EXT          = _T("");
#endif
