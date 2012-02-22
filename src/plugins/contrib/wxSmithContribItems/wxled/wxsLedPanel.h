#ifndef WXSLEDPANEL_H
#define WXSLEDPANEL_H

#include "wxswidget.h"

#include <wx/wxledpanel.h>


class wxsLedPanel : public wxsWidget
{
    public:
        wxsLedPanel(wxsItemResData* Data);
        virtual ~wxsLedPanel();
    protected:

        void        OnBuildCreatingCode();
        wxObject*   OnBuildPreview(wxWindow* Parent,long Flags);
        void        OnEnumWidgetProperties(long Flags);

        wxsSizeData     LedMatrixSize;
        wxsSizeData     LedMatrix;
        long int        Space;
        long int        Colour;
        bool            Invert;
        bool            Inactiv;
        long int        ContentAlign;
        wxString        Text;
        long int        TextPlace;
        bool            Bold;
        long int        Align;
        long int        ScrollDirection;
        long int        ScrollSpeed;

    private:
};

#endif // WXSLEDPANEL_H
