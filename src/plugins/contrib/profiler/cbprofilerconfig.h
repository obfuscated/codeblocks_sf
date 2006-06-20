/***************************************************************
 * Name:      cbprofilerconfig.h
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord & Zlika
 * Created:   07/20/05 15:36:55
 * Copyright: (c) Dark Lord & Zlika
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

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
