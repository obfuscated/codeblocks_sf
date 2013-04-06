#ifndef WXSLED_H
#define WXSLED_H

#include "wxswidget.h"


class wxsLed : public wxsWidget
{
    public:
        wxsLed(wxsItemResData* Data);
        virtual ~wxsLed();
    protected:

        void      OnBuildCreatingCode();
        wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        void      OnEnumWidgetProperties(long Flags);

        wxsColourData	m_Disable;
        wxsColourData	m_EnableOn;
        wxsColourData	m_EnableOff;
        bool m_State;

    private:
};

#endif // WXSLED_H
