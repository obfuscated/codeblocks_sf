#ifndef ENTERINFODLG_H
#define ENTERINFODLG_H

#include <wx/dialog.h>
#include <cbplugin.h> // SDK

class EnterInfoDlg : public wxDialog
{
    public:
        EnterInfoDlg(const PluginInfo& info);
        ~EnterInfoDlg();
		const PluginInfo& GetInfo(){ return m_Info; }
		void SetPluginInfo(const PluginInfo& info){ m_Info = info; }
	private:
		void OnOKClick(wxCommandEvent& event);
		
		PluginInfo m_Info;
		
        DECLARE_EVENT_TABLE()
};

#endif // ENTERINFODLG_H
