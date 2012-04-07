#include <UnitTest++.h>

#include <wx/string.h>

inline std::ostream& operator<<(std::ostream &stream, wxString const &s)
{
    return stream << s.utf8_str().data();
}

inline std::ostream& operator<<(std::ostream &stream, wxChar const *s)
{
    return stream << wxString(s).utf8_str().data();
}


class cbWatch;
std::ostream& operator<<(std::ostream &stream, cbWatch const &w);
bool operator == (wxString const &s, cbWatch const &w);
