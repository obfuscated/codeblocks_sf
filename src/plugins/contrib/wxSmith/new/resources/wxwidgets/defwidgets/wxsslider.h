#ifndef WXSSLIDER_H
#define WXSSLIDER_H

#include "../wxswidget.h"

/** \brief Class for wxsSlider widget */
class wxsSlider: public wxsWidget
{
    public:

        /** \brief Widget's info, in public scope because it will be accessed
         *         from widget manager
         */
        static wxsItemInfo Info;

        /** \brief Ctor
         *
         * Ctor in final classes should have only one argument - pointer to
         * resource. That's because manager will have only one argument when
         * creating widget also.
         */
        wxsSlider(wxsWindowRes* Resource);

        /** \brief Function building creating code */
        virtual void BuildCreatingCode(wxString& Code,const wxString& WindowParent,wxsCodingLang Language);

    protected:

        /** \brief Function building preview */
        virtual wxObject* DoBuildPreview(wxWindow* Parent,bool Exact);

        /** \brief Function enumerating properties specific for this widget
         *         only. Other properties are added automatically.
         */
        virtual void EnumWidgetProperties(long Flags);

        /** \brief Enumerating header files */
        virtual void EnumDeclFiles(wxArrayString& Decl,wxArrayString& Def,wxsCodingLang Language);

    private:

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
