#ifndef WXSCUSTOMBUTTON_H
#define WXSCUSTOMBUTTON_H

#include <wxwidgets/wxswidget.h>


class wxsCustomButton : public wxsWidget
{
    public:

        wxsCustomButton(wxsItemResData* Data);
        virtual ~wxsCustomButton();

    private:

        void OnBuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);
        wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        void OnEnumWidgetProperties(long Flags);
        void OnEnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

        long m_Type;
        bool m_Flat;
        wxString m_Label;
        long m_LabelPosition;
        wxsBitmapData m_Bitmap;
        wxsBitmapData m_BitmapSelected;
        wxsBitmapData m_BitmapFocused;
        wxsBitmapData m_BitmapDisabled;
        wxsSizeData m_Margins;
        wxsSizeData m_LabelMargins;
        wxsSizeData m_BitmapMargins;
};

#endif
