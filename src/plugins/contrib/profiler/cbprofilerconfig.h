#ifndef CBPROFILERCONFIG_H
#define CBPROFILERCONFIG_H

#include <wx/intl.h>
#include <wx/string.h>
#include "configurationpanel.h"

class wxCommandEvent;
class wxWindow;

class CBProfilerConfigDlg : public cbConfigurationPanel
{
    public:
        CBProfilerConfigDlg(wxWindow* parent);
        virtual ~CBProfilerConfigDlg();

        wxString GetTitle() const { return _("Profiler settings"); }
        wxString GetBitmapBaseName() const { return _T("profiler"); }
        void OnApply();
        void OnCancel(){}
    private:
        void CheckBoxEvent(wxCommandEvent& event);

        void LoadSettings();
        void SaveSettings();

        DECLARE_EVENT_TABLE()
};

#endif // CBPROFILERCONFIG_H
