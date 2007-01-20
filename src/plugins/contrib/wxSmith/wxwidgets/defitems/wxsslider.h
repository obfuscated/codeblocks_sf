#ifndef WXSSLIDER_H
#define WXSSLIDER_H

#include "../wxswidget.h"

/** \brief Class for wxsSlider widget */
class wxsSlider: public wxsWidget
{
    public:

        wxsSlider(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        int Value;
        int Min;
        int Max;
        int TickFrequency;
        int PageSize;
        int LineSize;
        int ThumbLength;
        int Tick;
        int SelMin;
        int SelMax;
};

#endif
