/***************************************************************
 * Name:      cbKeyConfigPanel.h
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan Heber
 * Copyright: (c) Pecan Heber
 * License:   GPL
 **************************************************************/
// RCS-ID:      $Id: cbkeybinder.h 11580 2019-03-31 16:55:24Z pecanh $

#ifndef CBKEYCONFIGPANEL_H
#define CBKEYCONFIGPANEL_H

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

// Modified Keybinder for CodeBlocks KeyBnder v2.0

// ----------------------------------------------------------------------------
class UsrConfigPanel : public cbConfigurationPanel
// ----------------------------------------------------------------------------
{

    public:
    // ctor(s)
    UsrConfigPanel(wxWindow* parent, const wxString& title, int);
	virtual ~UsrConfigPanel();


    wxString GetTitle() const { return _("Keyboard shortcuts"); }
    wxString GetBitmapBaseName() const { return _T("onekeytobindthem"); }
	void OnApply();
	void OnCancel(){}

	wxKeyConfigPanel* GetKeyConfigPanel(){return m_pwxKeyConfigPanel;} //(2019/04/6)
    void GetKeyConfigPanelPhaseII(wxMenuBar* pMenuBar, UsrConfigPanel* pUsrConfigPanel, int mode);

    private:
    //-cbKeyBinderPlgn* m_pBinder;
    clKeyboardManager*  m_pkbMgr;
    wxWindow*           m_pConfigWindow;
    wxKeyProfileArray*  m_pKeyProfileArray;
    wxKeyProfile*       m_pPrimaryProfile;
    wxKeyConfigPanel*   m_pwxKeyConfigPanel;
    MenuItemDataMap_t   m_cachedGlobalAccelMap;
    int                 m_mode;

    void CreateGlobalAccel(wxCmd* pCmd);
    // any class wishing to process wxWindows events must use this macro
    DECLARE_EVENT_TABLE()
};

#endif // CBKEYCONFIGPANEL_H
