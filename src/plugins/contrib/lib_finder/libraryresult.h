#ifndef LIBRARYRESULT_H
#define LIBRARYRESULT_H

#include <wx/string.h>

struct LibraryResult
{
    wxString GlobalVar;
    wxString LibraryName;
    wxString BasePath;
    wxString IncludePath;
    wxString LibPath;
    wxString ObjPath;
    wxString CFlags;
    wxString LFlags;
};

#endif //LIBRARYRESULT_H
