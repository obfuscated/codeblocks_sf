#ifndef LIBRARYRESULT_H
#define LIBRARYRESULT_H

#include <wx/string.h>

struct LibraryResult
{
    wxString LibraryName;
    wxString GlobalVar;
    wxString BasePath;
    wxString Description;
    wxString PkgConfigVar;
    wxArrayString Categories;
    wxArrayString IncludePath;
    wxArrayString LibPath;
    wxArrayString ObjPath;
    wxArrayString CFlags;
    wxArrayString LFlags;
    wxArrayString Compilers;
};

#endif
