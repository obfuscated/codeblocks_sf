#ifndef WXSSCROLLBAR_H
#define WXSSCROLLBAR_H

#include "../wxswidget.h"

/** \brief Class for wxsScrollBar widget */
class wxsScrollBar: public wxsWidget
{
    public:

        wxsScrollBar(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        int Value;
    	int ThumbSize;
    	int Range;
    	int PageSize;

};

#endif
