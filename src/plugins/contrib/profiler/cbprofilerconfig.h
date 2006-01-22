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

#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/font.h>
#include <wx/checkbox.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <cbplugin.h>

class CBProfilerConfigDlg : public cbConfigurationPanel
{
	public:
		CBProfilerConfigDlg(wxWindow* parent);
		virtual ~CBProfilerConfigDlg();

        wxString GetTitle(){ return _("Profiler settings"); }
        wxString GetBitmapBaseName(){ return _T("generic-plugin"); }
        void OnApply();
        void OnCancel(){}
	protected:
        void CheckBoxEvent(wxCommandEvent& event);

        void LoadSettings();
        void SaveSettings();
	private:
        DECLARE_EVENT_TABLE()
};

#endif // CBPROFILERCONFIG_H
