#ifndef WXSSTATICLINE_H
#define WXSSTATICLINE_H

#include "../wxswidget.h"

/** \brief Class for wxsStaticLine widget */

class wxsStaticLine: public wxsWidget
{
    public:

        wxsStaticLine(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

};


#endif
