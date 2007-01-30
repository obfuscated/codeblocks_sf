#ifndef WXSFLEXGRIDSIZER_H
#define WXSFLEXGRIDSIZER_H

#include "../wxssizer.h"

class wxsFlexGridSizer: public wxsSizer
{
    public:

        wxsFlexGridSizer(wxsItemResData* Data);

    private:

        virtual wxSizer* OnBuildSizerPreview(wxWindow* Parent);
        virtual void OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumSizerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);
        virtual bool OnIsPointer() { return true; }

        long Cols;
        long Rows;
        wxsDimensionData VGap;
        wxsDimensionData HGap;
        wxString GrowableRows;
        wxString GrowableCols;
};

#endif
