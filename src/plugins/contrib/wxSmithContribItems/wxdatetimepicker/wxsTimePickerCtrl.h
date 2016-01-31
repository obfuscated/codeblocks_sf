#ifndef WXSTIMEPICKERCTRL_H
#define WXSTIMEPICKERCTRL_H

#include <wxWidgets/wxsWidget.h>


class wxsTimePickerCtrl : public wxsWidget
{
    public:
        wxsTimePickerCtrl(wxsItemResData* Data);
        virtual ~wxsTimePickerCtrl();

    protected:

        virtual void OnBuildCreatingCode();
        virtual wxObject* OnBuildPreview(wxWindow* Parent,long Flags);
        virtual void OnEnumWidgetProperties(long Flags);
};

#endif // WXSTIMEPICKERCTRL_H
