#ifndef WXSLEDNUMBER_H
#define WXSLEDNUMBER_H

#include "wxswidget.h"


class wxsLedNumber : public wxsWidget
{
    public:
        wxsLedNumber(wxsItemResData* Data);
        virtual ~wxsLedNumber();
    protected:

        void        OnBuildCreatingCode();
        wxObject*   OnBuildPreview(wxWindow* Parent,long Flags);
        void        OnEnumWidgetProperties(long Flags);

        wxString Content;
        long int Align;
        bool     Faded;

    private:
};

#endif // WXSLEDNUMBER_H
