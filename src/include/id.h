#ifndef ID_H
#define ID_H
#undef new

#include <wx/string.h>

/*
* Base class for providing a string ID
*/
class ID
{
    wxString p__id;

public:
    ID(){};
    ID(const wxString& id) : p__id(id){};
    explicit ID(const ID& cpy) : p__id(cpy.id()){};

    wxString id() const
    {
        return p__id;
    };

    void set_id(const wxString& s)
    {
        p__id.assign(s);
    };

    bool operator==(const ID& other) const
    {
        return id().IsSameAs(other.id());
    };

    bool operator==(const wxString& other) const
    {
        return id().IsSameAs(other);
    };
};

#endif
