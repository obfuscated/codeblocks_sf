#ifndef WXSCHART_H
#define WXSCHART_H

#include <wxwidgets/wxswidget.h>


class wxsChart : public wxsWidget
{
    public:
        wxsChart(wxsItemResData* Data);
        virtual ~wxsChart();

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        long m_Flags;
};

#endif
