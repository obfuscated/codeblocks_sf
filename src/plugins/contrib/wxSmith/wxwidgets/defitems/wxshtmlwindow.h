#ifndef __WXSHTMLWINDOW_H
#define __WXSHTMLWINDOW_H

#include "../wxswidget.h"

class wxsHtmlWindow: public wxsWidget
{
    public:

        wxsHtmlWindow(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        wxsDimensionData Borders;
        wxString Url;
        wxString HtmlCode;

};

#endif
