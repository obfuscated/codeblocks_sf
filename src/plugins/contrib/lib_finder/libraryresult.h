#ifndef LIBRARYRESULT_H
#define LIBRARYRESULT_H

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

#endif
