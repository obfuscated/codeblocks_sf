#ifndef WXSGRIDSIZER_H
#define WXSGRIDSIZER_H

#include "../wxssizer.h"

class wxsGridSizer: public wxsSizer
{
    public:

        wxsGridSizer(wxsItemResData* Data);

    private:

        virtual wxSizer* OnBuildSizerPreview(wxWindow* Parent);
        virtual void OnBuildSizerCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual void OnEnumSizerProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        long Cols;
        long Rows;
        wxsDimensionData VGap;
        wxsDimensionData HGap;
};

#endif
