#ifndef WXSBITMAPBUTTON_H
#define WXSBITMAPBUTTON_H

#include "../wxswidget.h"

/** \brief Class for wxButton widget */
class wxsBitmapButton: public wxsWidget
{
    public:

        wxsBitmapButton(wxsItemResData* Data);

    private:

        virtual void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
        virtual void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        bool IsDefault;
        wxsBitmapData BitmapLabel;
        wxsBitmapData BitmapDisabled;
        wxsBitmapData BitmapSelected;
        wxsBitmapData BitmapFocus;
};

#endif
