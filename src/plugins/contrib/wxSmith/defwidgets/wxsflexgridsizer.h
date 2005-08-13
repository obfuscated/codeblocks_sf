#ifndef WXSFLEXGRIDSIZER_H
#define WXSFLEXGRIDSIZER_H

#include "../wxsdefsizer.h"

wxsDSDeclareBegin(FlexGridSizer,wxsFlexGridSizerId)
    int Cols;
    int Rows;
    int VGap;
    int HGap;
    wxString GrowableRows;
    wxString GrowableCols;
    
public:
    static wxArrayInt GetArray(const wxString& String,bool* Valid = NULL);
    static bool FixupList(wxString& List);
wxsDSDeclareEnd()

#endif
