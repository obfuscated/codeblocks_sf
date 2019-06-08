/***************************************************************
 * Name:      cbkeyConfigPanel.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Pecan
 * Copyright: (c) Pecan Heber etal.
 * License:   GPL
 **************************************************************/
// RCS-ID:      $Id: cbkeybinder.cpp 11580 2019-03-31 16:55:24Z pecanh $

// The majority of this code was lifted from wxKeyBinder and
// its "minimal.cpp" sample program

// Modified CodeBlocks KeyBnder v2.0 2019/04/8

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma implementation "cbkeybinder.h"
#endif

#include <vector>

#include <sdk.h>
#ifndef CB_PRECOMP
    #include <wx/dynarray.h>
    #include <wx/intl.h>
    #include <wx/log.h>
    #include <wx/sizer.h>
    #include <wx/stdpaths.h>

    #include "cbeditor.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "manager.h"
    #include "pluginmanager.h"
    #include "personalitymanager.h"
#endif

#if defined(__WXMSW__) && wxCHECK_VERSION(3, 0, 0)
    #include <wx/msw/private/keyboard.h>
#endif
#include <wx/textfile.h>

#if defined(LOGGING)
    #include "debugging.h"
#endif
#include "menuutils.h"
#include "cbkeybinder.h"
#include "clKeyboardManager.h"
// ----------------------------------------------------------------------------
//  UsrConfigPanel
// ----------------------------------------------------------------------------
    BEGIN_EVENT_TABLE(UsrConfigPanel, cbConfigurationPanel)
        // add events here...
    END_EVENT_TABLE()

// ----------------------------------------------------------------------------
//              Setting/Editor configuration panel
// ----------------------------------------------------------------------------
UsrConfigPanel::UsrConfigPanel(wxWindow* parent, const wxString& /*title*/, int mode)
// ----------------------------------------------------------------------------
    :m_pConfigWindow(parent), m_mode(mode)
{
    // create a minimal Settings/Editor/Keyboard shortcut panel.
    // The rest of the panel will be create by GetKeyConfigPanelPhaseII()
    // if/when the user clicks on Settings/Editor/Keyboard shortcuts .

    cbConfigurationPanel::Create(parent, -1, wxDefaultPosition, wxDefaultSize,
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    m_pkbMgr = clKeyboardManager::Get();
    m_pwxKeyConfigPanel = nullptr;
    m_pKeyProfileArray = nullptr;

    // we can do our task in two ways:
    // 1) we can use wxKeyConfigPanel::ImportMenuBarCmd which gives
    //    better appearances (for me, at least, :-))
    // 2) we can use wxKeyConfigPanel::ImportKeyBinderCmd

    m_pKeyProfileArray = new wxKeyProfileArray();

    //-wxKeyProfile* pPrimary; move to header
    wxFrame* pTopFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
    wxMenuBar* pMenuBar = pTopFrame->GetMenuBar();
    m_pPrimaryProfile = new wxKeyProfile(_("Primary"), _("Our primary keyprofile"));

    // remove keyprofiles from our array
    for (int i=0; i < m_pKeyProfileArray->GetCount();i++)
      m_pKeyProfileArray->Remove(m_pKeyProfileArray->Item(i));
    // clear out old array
    m_pKeyProfileArray->Cleanup();

    wxMenuCmd::Register(pMenuBar);
    m_pKeyProfileArray->Add(m_pPrimaryProfile);

    // attach to this window the default primary keybinder
    m_pKeyProfileArray->SetSelProfile(0);
    m_pPrimaryProfile = m_pKeyProfileArray->GetSelProfile();

    return;
}
// ----------------------------------------------------------------------------
UsrConfigPanel::~UsrConfigPanel()
// ----------------------------------------------------------------------------
{
    if (m_pwxKeyConfigPanel)
        wxDELETE( m_pwxKeyConfigPanel);
    if (m_pKeyProfileArray)
        wxDELETE( m_pKeyProfileArray);
}
// ----------------------------------------------------------------------------
void UsrConfigPanel::GetKeyConfigPanelPhaseII(wxMenuBar* pMenuBar, UsrConfigPanel* pUsrConfigPanel, int mode)
// ----------------------------------------------------------------------------
{
    // Add all mapped menu items to the primary profile table
    // Then scan the menu structure to catch any new menu items
    // Return to allow CB to display the configuration panel.
    // OnApply() will be called when the user clicks OK

    MenuItemDataMap_t accels;
    m_pkbMgr->GetAllAccelerators(accels);
    #if defined(LOGGING)
        size_t knt = accels.size(); wxUnusedVar(knt);
    #endif

    // --------------------------------------------------
    // Add core menu entries from the MenuItemDataMap
    // --------------------------------------------------
    //struct  MenuItemData {
    //    wxString resourceID;
    //    wxString accel;
    //    wxString action;
    //    wxString parentMenu; // For display purposes

    typedef std::unordered_multimap<int, wxString> GlobalAccelMap_t;
    GlobalAccelMap_t globalAccelMap;
    bool updateMenuStructure = false;

    for(MenuItemDataMap_t::const_iterator iter = accels.begin(); iter != accels.end(); ++iter)
    {
        wxString resourceIDString = iter->first;
        MenuItemData itemData = iter->second;
        long resourceID; itemData.resourceID.ToLong(&resourceID);
        wxString accel      = itemData.accel;
        wxString desc       = itemData.action;      //Help description
        wxString parentMenu = itemData.parentMenu;  //menu path with :: separators
        #if defined(LOGGING)
        LOGIT( _T("SetAccel[%s/%s/%s/%s/"), resourceIDString.wx_str(), accel.wx_str(), desc.wx_str(), parentMenu.wx_str());
        #endif

        if (parentMenu.empty()) //Empty parent menu means a <global> accelerator
        {
            if (not desc.StartsWith(_T("<global>")) )
                desc.Prepend(_T("<global>"));
            // Hold global commands until later
            globalAccelMap.insert(std::make_pair(resourceID, accel));
            continue;
        }

        wxString mnuPath = parentMenu.BeforeFirst(_T('|'));
        mnuPath = GetFullMenuPath(resourceID);
        mnuPath.Replace(_T("::"), _T("\\"));
        mnuPath.Trim(true);

        wxCmd* pCmd =  nullptr;
        if ( not (pCmd = m_pPrimaryProfile->GetCmd(resourceID)) )
        {
            pCmd = wxCmd::CreateNew(mnuPath, wxMENUCMD_TYPE, resourceID, updateMenuStructure);
            if (pCmd)
                m_pPrimaryProfile->AddCmd (pCmd);
        }
        if (pCmd)
        {
            pCmd->AddShortcut(accel);
        }
    }

    // add global entries last so that menu entries have priority in the wxCmd accel list
    for(GlobalAccelMap_t::const_iterator iter = globalAccelMap.begin(); iter != globalAccelMap.end(); ++iter)
    {
        int resourceID = iter->first;
        wxString accel = iter->second;

        wxCmd* pCmd =  nullptr;
        if ( not (pCmd = m_pPrimaryProfile->GetCmd(resourceID)) )
        {
            wxString mnuPath = GetFullMenuPath(resourceID);
            mnuPath.Replace(_T("::"), _T("\\"));
            mnuPath.Trim(true);

            pCmd = wxCmd::CreateNew(mnuPath, wxMENUCMD_TYPE, resourceID, updateMenuStructure);
            if (pCmd)
                m_pPrimaryProfile->AddCmd (pCmd);
        }
        if (pCmd)
        {
            pCmd->AddShortcut(accel);
        }
    }//endfor globalAccelMap

    // STEP #1: create a simple wxKeyConfigPanel
    //wxKeyConfigPanel*

        m_pwxKeyConfigPanel = new wxKeyConfigPanel(this, mode);
        wxSize clientSize = pUsrConfigPanel->GetClientSize();
        m_pwxKeyConfigPanel->SetSize(clientSize);

    // STEP #2: add a profile array to the wxKeyConfigPanel
    //-m_p->AddProfiles(prof);
    m_pwxKeyConfigPanel->AddProfiles(*m_pKeyProfileArray);
    m_pwxKeyConfigPanel->EnableKeyProfiles(false); //Dont show multi-keyProfiles combo box

    // STEP #3: populate the wxTreeCtrl widget of the panel by walking the menu structure
    m_pwxKeyConfigPanel->ImportMenuBarCmd(pMenuBar);

    // and embed it in a little sizer
    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);
    main->Add(m_pwxKeyConfigPanel, 1, wxGROW);
    SetSizer(main);
    //-main->SetSizeHints(pUsrConfigPanel); causes panel to shrink

    // on return from here, wxWidgets will display the panel and menu tree
    // CB will call OnAppy() or OnCanel() when the user finishes

}//UsrConfigDlg

// ----------------------------------------------------------------------------
void UsrConfigPanel::OnApply()
// ----------------------------------------------------------------------------
{
    clKeyboardManager* pKBmgr = clKeyboardManager::Get();

    if (not m_pwxKeyConfigPanel)
        return;

    m_pwxKeyConfigPanel->ApplyChanges(); //update selected profile with changes
    *m_pKeyProfileArray = m_pwxKeyConfigPanel->GetProfiles();
    // copy all wxCmds to our selected wxKeyProfile
    wxKeyProfile* pKeyProfile = m_pKeyProfileArray->GetSelProfile();
    wxCmdArray* pCmdArray = pKeyProfile->GetArray();
    size_t cmdCount = pCmdArray->GetCount();

    if (not cmdCount) return; //nothing to do

    MenuItemDataMap_t accelMap;
    pKBmgr->GetAllAccelerators(accelMap);

    // Iterate through the accelMap comparing to the wxCmdArray.
    // Comparing between menu vs global accels.
    // The first shortcut in the wxCmd accerators array is the menu accelerator, all others are global.
    // Decision table:

    // Scanning Accelerator Map against wxCmd array
    //                      Menu Accel          Menu Accel          Global Accel
    //                      -----------         ----------          ------------
    //                      Has no shortcut     Has shortcut        Has no shortcut
    //                    | ----------------    -------------       ----------------
    // wxCmd shortcuts 0  |  continue            clear shortcut      erase
    // wxCmd shortcuts 1  |  set wx shortcut     set wx shortcut     erase
    // wxCmd shortcuts 2  |  Add Global Accel    Add Global accel    erase

    for(MenuItemDataMap_t::iterator iter = accelMap.begin(); iter != accelMap.end(); ++iter)
    {
        nextItem:   //'goto nextItem;' is necessary since 'iter = prev(iter)' causes crashes after erase
                    //After an erase, iter has pointer to the following item.
                    //The item following erase() will be missed if the 'for' statement is allowed to execute.

        if (iter == accelMap.end() ) break;
        wxString resourceIDString = iter->first;
        MenuItemData itemData = iter->second;
        long resourceID; itemData.resourceID.ToLong(&resourceID);
        wxString accel      = itemData.accel;
        wxString desc       = itemData.action;      //Help description
        wxString parentMenu = itemData.parentMenu;  //menu path with :: separators
        #if defined(LOGGING)
            LOGIT( _T("Apply[%s/%s/%s/%s]"), resourceIDString.wx_str(), accel.wx_str(), desc.wx_str(), parentMenu.wx_str());
        #endif
        wxCmd* pCmd = pKeyProfile->GetCmd(resourceID);
        if (not pCmd) //menu item no longer exists
            {iter = accelMap.erase(iter); goto nextItem;}
        // erase <global accels>; they'll be updated by the menu accel review
        if ( parentMenu.empty() or desc.StartsWith(_T("<global>")) )
            {iter = accelMap.erase(iter); goto nextItem;}
        if (pCmd) switch(true) //now have a matching wxCmd array entry
        {
            default:
            wxArrayString cmdShortcuts = pCmd->GetShortcutsList();
            size_t shortcutCount = cmdShortcuts.GetCount();

            if (accel.empty() ) switch(shortcutCount)
            {
                case 0: continue;
                case 2: CreateGlobalAccel(pCmd);
                case 1: iter->second.accel = cmdShortcuts[0];
                continue;
            }
            if (not accel.empty() ) switch(shortcutCount)
            {
                case 0: iter->second.accel = _T(""); continue;
                case 2: CreateGlobalAccel(pCmd);
                case 1: iter->second.accel = cmdShortcuts[0];
                continue;
            }
        }//endif pCmd switch(true)
    }//endfor accelMap

    // Remove duplicate Menu accelerators, saving only the first occurance //(2019/04/22)
    m_pkbMgr->CheckForDuplicateAccels(accelMap);

    // Append cashed global accelerators to end of accelMap
    for(MenuItemDataMap_t::iterator iter = m_cachedGlobalAccelMap.begin(); iter != m_cachedGlobalAccelMap.end(); ++iter)
    {
        #if defined(LOGGING)
            wxString resourceIDString = iter->first;
            MenuItemData itemData = iter->second;
            long resourceID; itemData.resourceID.ToLong(&resourceID);
            wxString accel      = itemData.accel;
            wxString desc       = itemData.action;      //Help description
            wxString parentMenu = itemData.parentMenu;  //menu path with :: separators
        #endif

        accelMap.insert(std::make_pair(iter->first, iter->second));
    }

    // update the menu and global accelerators
    pKBmgr->SetAccelerators(accelMap);
    //-pKBmgr->Update(); the statement above does this anyway

    // update/rewrite keybindings.conf
    m_pkbMgr->Save();

}//end OnAppy()
// ----------------------------------------------------------------------------
void UsrConfigPanel::CreateGlobalAccel(wxCmd* pCmd)
// ----------------------------------------------------------------------------
{
    // Create a global accelerator and hold for later addition to accelMap
        wxArrayString cmdShortcuts = pCmd->GetShortcutsList();
        wxASSERT(cmdShortcuts.GetCount() >1);

        MenuItemData itemData;
        itemData.resourceID = wxString::Format(_T("%d"), pCmd->GetId());
        itemData.accel      = cmdShortcuts[1];
        itemData.action     = _T("<global>") + pCmd->GetDescription();    //Help description
        itemData.parentMenu = _T("");                                     //globals have no parent
        m_cachedGlobalAccelMap.insert(std::make_pair(itemData.resourceID, itemData));
}
