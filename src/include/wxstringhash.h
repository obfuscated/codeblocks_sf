#ifndef WXSTRING_HASH
#define WXSTRING_HASH

#include <wx/version.h>

#if !wxCHECK_VERSION(3, 1, 0)

// The code blow is copied 1:1 from wxWidgets 3.1.x

#include <wx/string.h>

// Don't do this if ToStdWstring() is not available. We could work around it
// but, presumably, if using std::wstring is undesirable, then so is using
// std::hash<> anyhow.
#if wxUSE_STD_STRING

#include <functional>

namespace std
{
    template<>
    struct hash<wxString>
    {
        size_t operator()(const wxString& s) const
        {
            return std::hash<std::wstring>()(s.ToStdWstring());
        }
    };
} // namespace std

#endif // wxUSE_STD_STRING
#endif // !wxCHECK_VERSION(3, 1, 0)

#endif // WXSTRING_HASH
