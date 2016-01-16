#ifndef WXSLCDWINDOW_H
#define WXSLCDWINDOW_H

#include "wxswidget.h"


class wxsLcdWindow : public wxsWidget
{
    public:
        wxsLcdWindow(wxsItemResData* Data);
        virtual ~wxsLcdWindow();
    protected:

        void        OnBuildCreatingCode();
        wxObject*   OnBuildPreview(wxWindow* Parent,long Flags);
        void        OnEnumWidgetProperties(long Flags);

        long int        NumberOfDigits;
        wxString        Content;
        wxsColourData   ColourLight;
        wxsColourData   ColourGray;
        wxsColourData   BackGround;

    private:
};

#endif // WXSLCDWINDOW_H
