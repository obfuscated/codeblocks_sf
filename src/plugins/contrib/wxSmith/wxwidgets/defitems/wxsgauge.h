#ifndef WXSGAUGE_H
#define WXSGAUGE_H

#include "../wxswidget.h"

/** \brief Class for wxsGauge widget */
class wxsGauge: public wxsWidget
{
    public:

        wxsGauge(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        int Range;
        int Value;
        int Shadow;
        int Bezel;
};

#endif
