#ifndef WXSTRING_HASH
#define WXSTRING_HASH

#include <unordered_set>
#include <unordered_map>
#include <wx/version.h>

#if wxVERSION_NUMBER < 3100
#include <wx/string.h>
#include <functional>

namespace std
{
    template <>
    struct hash<wxString>
    {
        //std::size_t operator()(const wxString& s) const { return hash<std::wstring>{}(s.ToStdWstring()); }
        //(wx28 2019/04/20) wx28 has no such function as wxString::ToStdWstring()
        std::size_t operator()(const wxString& s) const //(wx28 2019/04/18)
        {
            std::string str(s.mb_str());
            return hash<std::string>{}(str );
        }
    };
}
#endif // wxVERSION_NUMBER < 3100
#endif // WXSTRING_HASH
