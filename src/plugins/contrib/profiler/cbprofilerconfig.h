/***************************************************************
 * Name:      cbprofilerconfig.h
 * Purpose:   Code::Blocks plugin
 * Author:    Dark Lord
 * Created:   07/20/05 15:36:55
 * Copyright: (c) Dark Lord
 * Thanks:    Yiannis Mandravellos and his Source code formatter (AStyle) sources
 * License:   GPL
 **************************************************************/

#ifndef CBPROFILERCONFIG_H
#define CBPROFILERCONFIG_H

#include <wx/dialog.h>

class CBProfilerConfigDlg : public wxDialog
{
	public:
		CBProfilerConfigDlg(wxWindow* parent);
		virtual ~CBProfilerConfigDlg();
	protected:
        void CheckBoxEvent(wxCommandEvent& event);
        
        void LoadSettings();
        void SaveSettings();
        void EndModal(int retCode);
	private:
        DECLARE_EVENT_TABLE()
};

#endif // CBPROFILERCONFIG_H
