//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : clKeyboardManager.h
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
// Modifed for Code::Blocks by pecan

#include <vector>
#include <algorithm>

#include <wx/app.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/log.h>
#include <algorithm>
#include <wx/tokenzr.h>
#include <wx/log.h>
#include <wx/ffile.h>   //(2019/04/3)

#include "manager.h"
#include "personalitymanager.h"
#include "annoyingdialog.h" //(2019/04/27)

#include "debugging.h" //(ICC 2019/05/3)
#include "clKeyboardManager.h"
#include "clKeyboardBindingConfig.h"

    //-int wxEVT_INIT_DONE = XRCID("wxEVT_INIT_DONE");

BEGIN_EVENT_TABLE( clKeyboardManager, wxEvtHandler )
    //-EVT_MENU( wxEVT_INIT_DONE, clKeyboardManager::OnStartupCompleted )
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
clKeyboardManager::clKeyboardManager()
// ----------------------------------------------------------------------------
{
    // A-Z
    for(size_t i = 65; i < 91; ++i) {
        char asciiCode = (char)i;
        m_keyCodes.insert(wxString() << asciiCode);
    }

    // 0-9
    for(size_t i = 48; i < 58; ++i) {
        char asciiCode = (char)i;
        m_keyCodes.insert(wxString() << asciiCode);
    }

    // Special chars
    m_keyCodes.insert(_T("`"));
    m_keyCodes.insert(_T("-"));
    m_keyCodes.insert(_T("*"));
    m_keyCodes.insert(_T("="));
    m_keyCodes.insert(_T("BACK"));
    m_keyCodes.insert(_T("TAB"));
    m_keyCodes.insert(_T("["));
    m_keyCodes.insert(_T("]"));
    m_keyCodes.insert(_T("ENTER"));
    m_keyCodes.insert(_T("CAPITAL"));
    m_keyCodes.insert(_T("SCROLL_LOCK"));
    m_keyCodes.insert(_T("PASUE"));
    m_keyCodes.insert(_T(";"));
    m_keyCodes.insert(_T("'"));
    m_keyCodes.insert(_T("\\"));
    m_keyCodes.insert(_T(","));
    m_keyCodes.insert(_T("."));
    m_keyCodes.insert(_T("/"));
    m_keyCodes.insert(_T("SPACE"));
    m_keyCodes.insert(_T("INS"));
    m_keyCodes.insert(_T("HOME"));
    m_keyCodes.insert(_T("PGUP"));
    m_keyCodes.insert(_T("PGDN"));
    m_keyCodes.insert(_T("DEL"));
    m_keyCodes.insert(_T("END"));
    m_keyCodes.insert(_T("UP"));
    m_keyCodes.insert(_T("DOWN"));
    m_keyCodes.insert(_T("RIGHT"));
    m_keyCodes.insert(_T("LEFT"));
    m_keyCodes.insert(_T("F1"));
    m_keyCodes.insert(_T("F2"));
    m_keyCodes.insert(_T("F3"));
    m_keyCodes.insert(_T("F4"));
    m_keyCodes.insert(_T("F5"));
    m_keyCodes.insert(_T("F6"));
    m_keyCodes.insert(_T("F7"));
    m_keyCodes.insert(_T("F8"));
    m_keyCodes.insert(_T("F9"));
    m_keyCodes.insert(_T("F10"));
    m_keyCodes.insert(_T("F11"));
    m_keyCodes.insert(_T("F12"));

    // There can be the following options:
    // Ctrl-Alt-Shift
    // Ctrl-Alt
    // Ctrl-Shift
    // Ctrl
    // Alt-Shift
    // Alt
    // Shift
    std::for_each(m_keyCodes.begin(), m_keyCodes.end(), [&](const wxString& keyCode) {
        m_allShorcuts.insert(_T("Ctrl-Alt-Shift-") + keyCode);
        m_allShorcuts.insert(_T("Ctrl-Alt-") + keyCode);
        m_allShorcuts.insert(_T("Ctrl-Shift-") + keyCode);
        m_allShorcuts.insert(_T("Ctrl-") + keyCode);
        m_allShorcuts.insert(_T("Alt-Shift-") + keyCode);
        m_allShorcuts.insert(_T("Alt-") + keyCode);
        m_allShorcuts.insert(_T("Shift-") + keyCode);
    });
}

// ----------------------------------------------------------------------------
clKeyboardManager::~clKeyboardManager()
// ----------------------------------------------------------------------------
{
    Save();
}

static clKeyboardManager* m_mgr = NULL;
// ----------------------------------------------------------------------------
clKeyboardManager* clKeyboardManager::Get()
// ----------------------------------------------------------------------------
{   // breaking/stepping gdb prior to 8.0 will crash here //(2019/05/3)
    if(NULL == m_mgr) {
        m_mgr = new clKeyboardManager();
    }
    return m_mgr;
}

void clKeyboardManager::Release()
{
    if(m_mgr) {
        delete m_mgr;
    }
    m_mgr = NULL;
}

// ----------------------------------------------------------------------------
void clKeyboardManager::DoGetFrames(wxFrame* parent, clKeyboardManager::FrameList_t& frames)
// ----------------------------------------------------------------------------
{
    frames.push_back(parent);
    const wxWindowList& children = parent->GetChildren();
    wxWindowList::const_iterator iter = children.begin();
    for(; iter != children.end(); ++iter) {
        wxFrame* frameChild = dynamic_cast<wxFrame*>(*iter);
        if(frameChild) {
            if(std::find(frames.begin(), frames.end(), frameChild) == frames.end()) {
                frames.push_back(frameChild);
                DoGetFrames(frameChild, frames);
            }
        }
    }
}
// ----------------------------------------------------------------------------
void clKeyboardManager::DoUpdateMenu(wxMenu* menu, MenuItemDataIntMap_t& accels, std::vector<wxAcceleratorEntry>& table)
// ----------------------------------------------------------------------------
{
    wxMenuItemList items = menu->GetMenuItems();
    wxMenuItemList::iterator iter = items.begin();
    for(; iter != items.end(); iter++) {
        wxMenuItem* item = *iter;
        if(item->GetSubMenu()) {
            DoUpdateMenu(item->GetSubMenu(), accels, table);
            continue;
        }
        MenuItemDataIntMap_t::iterator where = accels.find(item->GetId());
        if(where != accels.end())
        {
            wxString itemText = item->GetItemLabel();
            // remove the old shortcut
            itemText = itemText.BeforeFirst('\t');
            itemText << _T("\t") << where->second.accel;

            // Replace the item text (mnemonics + accel + label)
            item->SetItemLabel(itemText);

            // remove the matches entry from the accels map
            accels.erase(where);
        }
        else    //(2019/04/25)
        {   // Put only non-menu items in the accelerator table.
            // Else they'll fire events even after the menu item is deleted.
            wxAcceleratorEntry* a = wxAcceleratorEntry::Create(item->GetItemLabel());
            if(a) {
                a->Set(a->GetFlags(), a->GetKeyCode(), item->GetId());
                table.push_back(*a);
                wxDELETE(a);
            }
        }
    }//for iter
}
// ----------------------------------------------------------------------------
void clKeyboardManager::DoUpdateFrame(wxFrame* frame, MenuItemDataIntMap_t& accels)
// ----------------------------------------------------------------------------
{
    std::vector<wxAcceleratorEntry> table;

    // Update menus. If a match is found remove it from the 'accel' table
    wxMenuBar* menuBar = frame->GetMenuBar();
    if(!menuBar) return;
    for(size_t i = 0; i < menuBar->GetMenuCount(); ++i) {
        wxMenu* menu = menuBar->GetMenu(i);
        DoUpdateMenu(menu, accels, table);
    }

    if(!table.empty() || !accels.empty()) {
        wxAcceleratorEntry* entries = new wxAcceleratorEntry[table.size() + accels.size()];
        // append the globals
        for(MenuItemDataIntMap_t::iterator iter = accels.begin(); iter != accels.end(); ++iter) {
            wxString dummyText;
            dummyText << iter->second.action << _T("\t") << iter->second.accel;
            wxAcceleratorEntry* entry = wxAcceleratorEntry::Create(dummyText);
            if(entry) {
                wxString resourceIDstr = iter->second.resourceID;
                long ldResourceID; resourceIDstr.ToLong(&ldResourceID);
                //-entry->Set(entry->GetFlags(), entry->GetKeyCode(), wxXmlResource::GetXRCID(iter->second.resourceID));
                entry->Set(entry->GetFlags(), entry->GetKeyCode(), ldResourceID);
                table.push_back(*entry);
                wxDELETE(entry);
            }
        }

        for(size_t i = 0; i < table.size(); ++i) {
            entries[i] = table.at(i);
        }

        wxAcceleratorTable acceleTable(table.size(), entries);
        frame->SetAcceleratorTable(acceleTable);
        wxDELETEA(entries);
    }
}
// ----------------------------------------------------------------------------
void clKeyboardManager::Save()
// ----------------------------------------------------------------------------
{
    clKeyboardBindingConfig config;
    config.SetBindings(m_menuTable, m_globalTable).Save();
}
// ----------------------------------------------------------------------------
void clKeyboardManager::Initialize(bool isRefreshRequest)
// ----------------------------------------------------------------------------
{
    wxUnusedVar(isRefreshRequest);
    m_menuTable.clear();

    // First, try to load accelerators from %appdata% keybindings.conf
    //      containing merged default + user defined accerators
    // Second, try loading from default accerators in %appdata% + accerators.conf

    clKeyboardBindingConfig config;
    if( not config.Exists()) //does keybindings.conf exist?
    {
        #if defined(LOGGING)
        LOGIT( _T("[%s]"), _("Keyboard manager: No configuration found - importing old settings"));
        #endif
        //CL_DEBUG("Keyboard manager: No configuration found - importing old settings");
        // Decide which file we want to load, take the user settings file first
        // GetUserDataDir() == "c:\Users\<username>\AppData\Roaming\<appname>\config\keybindings.conf"
        // GetDataDir()     == executable directory

        // Old accererator setting are in %appdata%
        wxFileName fnOldSettings(wxStandardPaths::Get().GetTempDir(), _T("keyMnuAccels.conf"));
        wxString personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
        fnOldSettings.SetName(personality + _T(".") + fnOldSettings.GetName());

        wxFileName fnFileToLoad;
        bool canDeleteOldSettings(false);
        // If %appdata% accerators.conf exist, use it
        if(fnOldSettings.FileExists())
        {
            fnFileToLoad = fnOldSettings;
            //-canDeleteOldSettings = true;
        }
        else    // else use executable dir accerators.conf.default accerators
        {
            //-fnFileToLoad = fnDefaultOldSettings;
            wxASSERT_MSG(0, _("clKeyboardManager::Initialize() missing accerators.conf file"));
        }

        if(fnFileToLoad.FileExists())
        {
            #if defined(LOGGING)
            LOGIT( _T("KeyboardManager:Importing settings from:\n\t[%s]"), fnFileToLoad.GetFullPath().wx_str());
            #endif
            // Apply the old settings to the menus
            wxString content;
            if(not ReadFileContent(fnFileToLoad, content)) return;
            wxArrayString lines = ::wxStringTokenize(content, _T("\r\n"), wxTOKEN_STRTOK);
            for(size_t i = 0; i < lines.GetCount(); ++i)
            {
                #if defined(LOGGING)
                    #if wxVERSION_NUMBER > 3000
                    LOGIT( _T("AccelFile[%u:%s]"), (unsigned)i, lines.Item(i).wx_str() );
                    #else
                    LOGIT( _T("AccelFile[%u:%s]"), i, lines.Item(i).wx_str() );
                    #endif
                #endif
                wxArrayString parts = ::wxStringTokenize(lines.Item(i), _T("|"), wxTOKEN_RET_EMPTY);
                if(parts.GetCount() < 3) continue;
                MenuItemData binding;
                binding.resourceID = parts.Item(0);
                binding.parentMenu = parts.Item(1);
                binding.action = parts.Item(2);
                if(parts.GetCount() == 4) {
                    binding.accel = parts.Item(3);
                }
                m_menuTable.insert(std::make_pair(binding.resourceID, binding));
            }

            if(canDeleteOldSettings) {
                if (fnFileToLoad.FileExists())
                    ::wxRemoveFile(fnFileToLoad.GetFullPath());
            }
        }
    }
    else //config exists: "keybindings.conf"
    {
        config.Load();
        m_menuTable = config.GetBindings();
    }

    // Load the default settings and add any new entries from accerators.conf
    MenuItemDataMap_t defaultEntries = DoLoadDefaultAccelerators();

    // Remove any map items nolonger matching the menu structure
    for (MenuItemDataMap_t::iterator mapIter = m_menuTable.begin(); mapIter != m_menuTable.end(); ++mapIter)
    {
        mnuContinue:
        if (mapIter == m_menuTable.end()) break;
        //search menu structure map for map menuId
        if ( defaultEntries.count(mapIter->first) == 0)
        {   // menuID nolonger exists
            #if defined(LOGGING)
                wxString mapAccel = mapIter->second.accel;
                wxString mapParent = mapIter->second.parentMenu;
                wxString mapMnuID = mapIter->first;
                LOGIT( _T("Removing ID mismatch[%s][%s][%s]"), mapMnuID.wx_str(), mapParent.wx_str(), mapAccel.wx_str());
            #endif
            mapIter = m_menuTable.erase(mapIter);
            goto mnuContinue;
        }
        else //remove the found map item if its label doesn't match menu structure label//(pecan 2019/05/18)
        {   // Have matching map resoureID and menu structure resourceID (ie., menuItemID)
            MenuItemDataMap_t::iterator mnuIter = defaultEntries.find(mapIter->first);
            if (mnuIter == defaultEntries.end())
                continue;
            wxString mapParent = mapIter->second.parentMenu;
            if (mapParent.empty()) continue; //skip global accelerators
            wxString mnuParent = mnuIter->second.parentMenu;
            if (mnuParent.empty()) continue; //skip global accelerators
            if (mapParent.Lower() != mnuParent.Lower())
            {
                #if defined(LOGGING)
                    wxString mapMnuID = mapIter->first;
                    wxString mapAccel = mapIter->second.accel;
                    LOGIT( _T("Removing LabelMismatch[%s][%s][%s]"), mapMnuID.wx_str(), mapParent.wx_str(), mapAccel.wx_str());
                #endif
                mapIter = m_menuTable.erase(mapIter);
                goto mnuContinue;
            }//endif label compare
        }//endif else have matching resourceID
    }//endfor mapIter

    // Add any new entries from accerators.conf (the menu structure)
    std::for_each(defaultEntries.begin(), defaultEntries.end(), [&](const MenuItemDataMap_t::value_type& vdflt)
    {
        //-wxString vtValue = vdflt.first;         //The menu id number
        if(m_menuTable.count(vdflt.first) == 0) {  //searches map for like shortcut string
            m_menuTable.insert(vdflt);
        }
        // ----------------------------------------------------------------------------
        // NO!no! don't overwrite past user changes; m_menuTable already has user keybinder.conf changes.
        // while defaultEntries have original CodeBlocks menu accelerators
        // User must make (or made) m_menuTable changes via KeyBinder configuration dialog.
        // ----------------------------------------------------------------------------
        //-else //verify keyboard shortcut
        //-{
        //-    MenuItemDataMap_t::iterator mapIter = m_menuTable.find(vdflt.first);
        //-    if (mapIter != m_menuTable.end()) //should never be true!
        //-    if (mapIter->second.accel != vdflt.second.accel)
        //-    {
        //-        #if defined(LOGGING)
        //-        wxString mapMenuItem = mapIter->second.parentMenu + mapIter->second.accel;
        //-        wxString vdfltMenuItem = vdflt.second.parentMenu +vdflt.second.accel;
        //-        LOGIT( _T("Initialize changing accel[%s]to[%s]"), mapMenuItem.wx_str(), vdfltMenuItem.wx_str());
        //-        #endif
        //-        mapIter->second.accel = vdflt.second.accel;
        //-    }
        //-}
    });

    // Warn about duplicate shortcut entries (eg., (Print/PrevCallTip Ctrl-P) and (CC Search/Ctrl-Shift-.) have duplicates) //(2019/04/23)
    CheckForDuplicateAccels(m_menuTable);

    // Store the correct configuration; globalTable is inserted into menuTable
    config.SetBindings(m_menuTable, m_globalTable).Save();

    // And apply the changes
    Update();
}

// ----------------------------------------------------------------------------
void clKeyboardManager::GetAllAccelerators(MenuItemDataMap_t& accels) const
// ----------------------------------------------------------------------------
{
    accels.clear();
    accels.insert(m_menuTable.begin(), m_menuTable.end());
    accels.insert(m_globalTable.begin(), m_globalTable.end());
}

// ----------------------------------------------------------------------------
void clKeyboardManager::SetAccelerators(const MenuItemDataMap_t& accels)
// ----------------------------------------------------------------------------
{
    // separate the globals from the menu accelerators
    // The process is done by checking each item's parentMenu
    // If the parentMenu is empty -> global accelerator
    MenuItemDataMap_t globals, menus;
    MenuItemDataMap_t::const_iterator iter = accels.begin();
    for(; iter != accels.end(); ++iter) {
        if(iter->second.parentMenu.IsEmpty()) {
            globals.insert(std::make_pair(iter->first, iter->second));
        } else {
            menus.insert(std::make_pair(iter->first, iter->second));
        }
    }

    m_menuTable.swap(menus);
    m_globalTable.swap(globals);
    Update();
    Save();
}

// ----------------------------------------------------------------------------
void clKeyboardManager::Update(wxFrame* frame)
// ----------------------------------------------------------------------------
{
    // Since we keep the accelerators with their original resource ID in the form of string
    // we need to convert the map into a different integer with integer as the resource ID

    // Note that we place the items from the m_menuTable first and then we add the globals
    // this is because menu entries takes precedence over global accelerators
    MenuItemDataMap_t accels = m_menuTable;
    accels.insert(m_globalTable.begin(), m_globalTable.end());

    MenuItemDataIntMap_t intAccels;
    DoConvertToIntMap(accels, intAccels);

    if(!frame) {
        // update all frames
        wxFrame* topFrame = dynamic_cast<wxFrame*>(wxTheApp->GetTopWindow());
        CHECK_PTR_RET(topFrame);

        FrameList_t frames;
        DoGetFrames(topFrame, frames);
        for(FrameList_t::iterator iter = frames.begin(); iter != frames.end(); ++iter) {

            DoUpdateFrame(*iter, intAccels);
        }
    } else {
        // update only the requested frame
        DoUpdateFrame(frame, intAccels);
    }
}

//int clKeyboardManager::PopupNewKeyboardShortcutDlg(wxWindow* parent, MenuItemData& menuItemData)
//{
//    NewKeyShortcutDlg dlg(parent, menuItemData);
//    if(dlg.ShowModal() == wxID_OK) {
//        menuItemData.accel = dlg.GetAccel();
//        return wxID_OK;
//    }
//    return wxID_CANCEL;
//}
// ----------------------------------------------------------------------------
bool clKeyboardManager::Exists(const wxString& accel) const
// ----------------------------------------------------------------------------
{
    if(accel.IsEmpty()) return false;

    MenuItemDataMap_t accels;
    GetAllAccelerators(accels);

    MenuItemDataMap_t::const_iterator iter = accels.begin();
    for(; iter != accels.end(); ++iter) {
        if(iter->second.accel == accel) {
            return true;
        }
    }
    return false;
}
// -----------------------------------------------------------------------------------------------------------------
MenuItemDataMap_t::iterator clKeyboardManager::ExistsALikeAccel(MenuItemDataMap_t& srcMap, MenuItemDataMap_t::iterator srcMapIter) const //(2019/04/22)
// -----------------------------------------------------------------------------------------------------------------
{   // search for a like accelerator starting from specified map iterator

    MenuItemDataMap_t& accels = srcMap;
    if (srcMapIter == accels.end()) return accels.end();
    //-if (srcMapIter->second.parentMenu.empty() ) return accels.end(); //skip global accelerators //(pecan 2019/05/5)-

    const wxString srcAccel = srcMapIter->second.accel;
    if(srcAccel.IsEmpty()) return accels.end();

    MenuItemDataMap_t::iterator srcIter = srcMapIter;
    MenuItemDataMap_t::iterator iter = ++srcIter;
    for(; iter != accels.end(); ++iter)
    {
        if(iter->second.accel == srcAccel)
        {
            #if defined(LOGGING)
                // found a duplicate accelerator further down the accelerator map
                wxString srcAction = srcMapIter->second.action;
                wxString dupAccel  = iter->second.accel;
                wxString dupAction = iter->second.action;
                long srcMenuID; srcMapIter->first.ToLong(&srcMenuID);
                long dupMenuID; iter->first.ToLong(&dupMenuID);
            #endif
            if (iter->second.parentMenu.empty() )
                continue; //skip global accelerator
            return iter;
        }
    }
    return accels.end();
}
// -----------------------------------------------------------------------------------------------------------------
void clKeyboardManager::CheckForDuplicateAccels(MenuItemDataMap_t& accelMap) const //(2019/04/22)
// -----------------------------------------------------------------------------------------------------------------
{
    // Warn about duplicate Menu accelerators //(2019/04/22)

    wxArrayString dupMsgs;
    for(MenuItemDataMap_t::iterator accelIter = accelMap.begin(); accelIter != accelMap.end(); ++accelIter)
    {
        if (accelIter->second.accel.empty()) continue;
        if (accelIter->second.parentMenu.empty()) continue; //skip global accelerators
        MenuItemDataMap_t::iterator foundIter   = accelMap.end();
        MenuItemDataMap_t::iterator patternIter = accelIter;
        while (accelMap.end() != (foundIter = ExistsALikeAccel(accelMap, patternIter)) )
        {
            #if defined(LOGGING)
            wxString patternAccel  = patternIter->second.accel;
            wxString patternAction = patternIter->second.action;
            wxString dupAccel      = foundIter->second.accel;
            wxString dupAction     = foundIter->second.action;
            #endif
            //skip found global accelerators
            if (foundIter->second.parentMenu.empty())
            {
                patternIter = foundIter;
                continue;
            }

            // found a duplicate menu accelerator further down the accelerator map
            MenuItemDataMap_t::iterator srcIter = patternIter;


            wxString srcMenuLabel = srcIter->second.parentMenu;
            srcMenuLabel.Replace(_T("\t"), _T(" "));
            srcMenuLabel.Replace(_T("&"), _T(""));
            srcMenuLabel.Replace(_T("::"), _T("/"));

            wxString foundMenuLabel = foundIter->second.parentMenu;
            foundMenuLabel.Replace(_T("\t"), _T(" "));
            foundMenuLabel.Replace(_T("&"), _T(""));
            foundMenuLabel.Replace(_T("::"), _T("/"));

            long srcMenuID; srcIter->first.ToLong(&srcMenuID);
            long foundMenuID; foundIter->first.ToLong(&foundMenuID);

            wxString msg = wxString::Format(_T("Conflicting menu items: \'%s\' && \'%s\'"),
                                            srcMenuLabel.wx_str(), foundMenuLabel.wx_str())
                         + wxString::Format(_T("\n   Both using shortcut: \'%s\'"), foundIter->second.accel.wx_str())
                         + wxString::Format(_T(" (IDs [%ld] [%ld])"),srcMenuID, foundMenuID );
            msg += _T("\n\n");
            dupMsgs.Add(msg);
            patternIter = foundIter;

        }//end while
    }
    if (dupMsgs.GetCount())
    {
        bool isParentWindowDialog = false;
        // Get top window to solve msg window getting hidden behind keybinder dialog
        wxWindow* pMainWin = nullptr;
        if ( (pMainWin = wxFindWindowByLabel(_T("Configure editor"))) )
        {   pMainWin = wxFindWindowByLabel(_T("Configure editor"));
            isParentWindowDialog = true;
        }
        else pMainWin = Manager::Get()->GetAppWindow();
        wxString msg = _T("Keyboard shortcut conflicts found.\n");
        if (not isParentWindowDialog)
            msg += _T("Use Settings/Editor/KeyboardShortcuts to resolve conflicts.\n\n");
        for (size_t ii=0; ii<dupMsgs.GetCount(); ++ii)
            msg += dupMsgs[ii];
        //-cbMessageBox(msg, _T("Keyboard shortcuts conflicts"), wxOK, pMainWin);
        AnnoyingDialog dlg(_("Keyboard shortcuts conflicts"), msg, wxART_INFORMATION,  AnnoyingDialog::OK);
        dlg.ShowModal();
    }//endif dupMsgs

    return;
}
// ----------------------------------------------------------------------------
void clKeyboardManager::AddGlobalAccelerator(const wxString& resourceID,
                                             const wxString& keyboardShortcut,
                                             const wxString& description)
// ----------------------------------------------------------------------------
{
    MenuItemData mid;
    mid.action = description;
    mid.accel = keyboardShortcut;
    mid.resourceID = resourceID;
    m_globalTable.insert(std::make_pair(mid.resourceID, mid));
}
// ----------------------------------------------------------------------------
void clKeyboardManager::RestoreDefaults()
// ----------------------------------------------------------------------------
{
    wxASSERT_MSG(0, _T("RestoreDefaults not support yet !"));
    return ;

    // Decide which file we want to load, take the user settings file first
    wxFileName fnOldSettings(wxStandardPaths::Get().GetTempDir(), _T("keyMnuAccels.conf"));
    wxString personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    fnOldSettings.SetName(personality + _T(".") + fnOldSettings.GetName());

    wxFileName fnNewSettings(wxStandardPaths::Get().GetUserDataDir(), _T("cbKeyBinder20.conf"));
    fnNewSettings.SetName(personality + _T(".") + fnNewSettings.GetName());


    if(fnOldSettings.FileExists()) {
        wxRemoveFile(fnOldSettings.GetFullPath());
    }

    if(fnNewSettings.FileExists()) {
        wxRemoveFile(fnNewSettings.GetFullPath());
    }

    // Call initialize again
    bool isRefreshRequest = false;
    Initialize(isRefreshRequest);
}
//// ----------------------------------------------------------------------------
//void clKeyboardManager::OnStartupCompleted(wxCommandEvent& event)
//// ----------------------------------------------------------------------------
//{
//    event.Skip();
//    this->Initialize();
//}
// ----------------------------------------------------------------------------
void clKeyboardManager::DoConvertToIntMap(const MenuItemDataMap_t& strMap, MenuItemDataIntMap_t& intMap)
// ----------------------------------------------------------------------------
{
    // Convert the string map into int based map
    MenuItemDataMap_t::const_iterator iter = strMap.begin();
    for(; iter != strMap.end(); ++iter)
    {
        wxString resourceIDStr = iter->second.resourceID;
        long lnResourceID; resourceIDStr.ToLong(&lnResourceID);
        //-intMap.insert(std::make_pair(wxXmlResource::GetXRCID(iter->second.resourceID), iter->second));
        intMap.insert(std::make_pair(lnResourceID, iter->second));
    }
}
// ----------------------------------------------------------------------------
wxArrayString clKeyboardManager::GetAllUnasignedKeyboardShortcuts() const
// ----------------------------------------------------------------------------
{
    MenuItemDataMap_t accels;
    GetAllAccelerators(accels);

    wxStringSet_t usedShortcuts;
    std::for_each(accels.begin(), accels.end(), [&](const std::pair<wxString, MenuItemData>& p) {
        if(!p.second.accel.IsEmpty()) {
            usedShortcuts.insert(p.second.accel);
        }
    });

    // Remove all duplicate entries
    wxArrayString allUnasigned;
    std::set_difference(m_allShorcuts.begin(),
                        m_allShorcuts.end(),
                        usedShortcuts.begin(),
                        usedShortcuts.end(),
                        std::back_inserter(allUnasigned));
    return allUnasigned;
}
// ----------------------------------------------------------------------------
MenuItemDataMap_t clKeyboardManager::DoLoadDefaultAccelerators()
// ----------------------------------------------------------------------------
{
    MenuItemDataMap_t entries;
    wxFileName fnDefaultOldSettings(wxStandardPaths::Get().GetTempDir(), _T("keyMnuAccels.conf"));
    wxString personality = Manager::Get()->GetPersonalityManager()->GetPersonality();
    fnDefaultOldSettings.SetName(personality + _T(".") + fnDefaultOldSettings.GetName());

    if(fnDefaultOldSettings.FileExists())
    {
        wxString content;
        if(not ReadFileContent(fnDefaultOldSettings, content)) {
            return entries;
        }
        wxArrayString lines = ::wxStringTokenize(content, _T("\r\n"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < lines.GetCount(); ++i)
        {
            wxArrayString parts = ::wxStringTokenize(lines.Item(i), _T("|"), wxTOKEN_RET_EMPTY);
            //-if(parts.GetCount() < 3) continue; //(pecan 2019/04/27)
            if(parts.GetCount() < 2) continue;
            MenuItemData binding;
            binding.resourceID = parts.Item(0);
            binding.parentMenu = parts.Item(1);
            if (parts.GetCount() > 2)               //(pecan 2019/04/27)
            binding.action = parts.Item(2);
            if(parts.GetCount() == 4) {
                binding.accel = parts.Item(3);
            }
            entries.insert(std::make_pair(binding.resourceID, binding));
        }
    }
    return entries;
}
// ----------------------------------------------------------------------------
void clKeyboardShortcut::Clear()
// ----------------------------------------------------------------------------
{
    m_ctrl = false;
    m_alt = false;
    m_shift = false;
    m_keyCode.Clear();
}
// ----------------------------------------------------------------------------
wxString clKeyboardManager::KeyCodeToString(int keyCode) //(2019/02/25)
// ----------------------------------------------------------------------------
{
	wxString res;

    //LOGIT("KeyCodeToString_IN:keyCode[%d]char[%c]", keyCode, keyCode );

	switch (keyCode)
	{
		// IGNORED KEYS
		// ---------------------------
	case WXK_START:
	case WXK_LBUTTON:
	case WXK_RBUTTON:
	case WXK_MBUTTON:
	case WXK_CLEAR:

	case WXK_PAUSE:
	case WXK_NUMLOCK:
	case WXK_SCROLL :
		wxLogDebug(wxT("wxKeyBind::KeyCodeToString - ignored key: [%d]"), keyCode);
		return wxEmptyString;

		// these must be ABSOLUTELY ignored: they are key modifiers
		// we won't output any LOG message since these keys could be pressed
		// for long time while the user choose its preferred keycombination:
		// this would result into a long long queue of "ignored key" messages
		// which would be useless even in debug builds...
	case WXK_SHIFT:
	case WXK_CONTROL:
	case WXK_ALT:                           //+v0.5
		return wxEmptyString;




		// FUNCTION KEYS
		// ---------------------------

	case WXK_F1: case WXK_F2:
	case WXK_F3: case WXK_F4:
	case WXK_F5: case WXK_F6:
	case WXK_F7: case WXK_F8:
	case WXK_F9: case WXK_F10:
	case WXK_F11: case WXK_F12:
	case WXK_F13: case WXK_F14:
    case WXK_F15: case WXK_F16:
    case WXK_F17: case WXK_F18:
    case WXK_F19: case WXK_F20:
    case WXK_F21: case WXK_F22:
    case WXK_F23: case WXK_F24:
		res << wxT('F') << wxString::Format(_T("%d"), keyCode - WXK_F1 + 1);
		break;


		// MISCELLANEOUS KEYS
		// ---------------------------

	case WXK_BACK:
        res << wxT("BACK"); break;
	case WXK_TAB:
        res << wxT("TAB"); break;
	case WXK_RETURN:
        res << wxT("RETURN"); break;
	case WXK_ESCAPE:
        res << wxT("ESCAPE"); break;
	case WXK_SPACE:
        res << wxT("SPACE"); break;
	case WXK_DELETE:
        res << wxT("DELETE"); break;
	case WXK_MULTIPLY:
		res << wxT("*"); break;
	case WXK_ADD:
		res << wxT("+"); break;
	case WXK_SEPARATOR:
		res << wxT("SEPARATOR"); break;
	case WXK_SUBTRACT:
		res << wxT("-"); break;
	case WXK_DECIMAL:
		res << wxT("."); break;
	case WXK_DIVIDE:
		res << wxT("/"); break;

#if not wxCHECK_VERSION(2, 8, 0)
	case WXK_PRIOR:
        res << wxT("PgUp"); break;
	case WXK_NEXT:
        res << wxT("PgDn"); break;
#endif
	case WXK_PAGEUP:
		res << wxT("PAGEUP"); break;
	case WXK_PAGEDOWN:
		res << wxT("PAGEDOWN"); break;
	case WXK_LEFT:
        res << wxT("LEFT"); break;
	case WXK_UP:
        res << wxT("UP"); break;
	case WXK_RIGHT:
        res << wxT("RIGHT"); break;
	case WXK_DOWN:
        res << wxT("DOWN"); break;
	case WXK_SELECT:
        res << wxT("SELECT"); break;
	case WXK_PRINT:
        res << wxT("PRINT"); break;
	case WXK_EXECUTE:
        res << wxT("EXECUTE"); break;
	case WXK_SNAPSHOT:
        res << wxT("SNAPSHOT"); break;
	case WXK_INSERT:
        res << wxT("INSERT"); break;
	case WXK_HELP:
        res << wxT("HELP"); break;
	case WXK_CANCEL:
        res << wxT("CANCEL"); break;
	case WXK_MENU:
        res << wxT("MENU"); break;
	case WXK_CAPITAL:
        res << wxT("CAPITAL"); break;
	case WXK_END:
        res << wxT("END"); break;
	case WXK_HOME:
        res << wxT("HOME"); break;

//+V.05 (Pecan#1#): wxIsalnm is excluding keys not num or a-z like }{ etc
//+v.05 (Pecan#1#): Holding Alt shows ALT+3 A: added WXK_ALT: to above case
//+v.05 (Pecan#1#): ALT +Ctrl Left/Right show in Dlg, up/Down dont. Printable?
//               A: wxWidgets2.6.2 returns false on modifier keys for Ctrl+Alt+UP/DOWN combination.
//                  It returns Ctrl+Alt+PRIOR instead of UP/DOWN and shows false for ctrl & alt.
//                  Same is true for Ctrl+Shift+UP/Down.
//                  Alt+Shift+Up/Down work ok.
	default:
		// ASCII chars...
		if (wxIsalnum(keyCode))
		{
			res << (wxChar)keyCode;
			break;

		} else if ((res=NumpadKeyCodeToString(keyCode)) != wxEmptyString) {

			res << wxT(" (numpad)");		// so it is clear it's different from other keys
			break;

		} else if (wxIsprint(keyCode)) { //v+0.5
			res << (wxChar)keyCode;
			break;

		} else {

			// we couldn't create a description for the given keycode...
			wxLogDebug(wxT("wxKeyBind::KeyCodeToString - unknown key: [%d]"), keyCode);
			return wxEmptyString;
		}
	}//default

    //#if LOGGING
    // LOGIT(_T("KeyCodeToStringOUT:keyCode[%d]char[%c]Desc[%s]"),
    //            keyCode, keyCode, res.GetData() );
    //#endif

	return res;

}//KeyCodeToString
// ----------------------------------------------------------------------------
wxString clKeyboardManager::NumpadKeyCodeToString(int keyCode) //(2019/02/25)
// ----------------------------------------------------------------------------
{
	wxString res;

	switch (keyCode)
	{
		// NUMPAD KEYS
		// ---------------------------

	case WXK_NUMPAD0:
	case WXK_NUMPAD1:
	case WXK_NUMPAD2:
	case WXK_NUMPAD3:
	case WXK_NUMPAD4:
	case WXK_NUMPAD5:
	case WXK_NUMPAD6:
	case WXK_NUMPAD7:
	case WXK_NUMPAD8:
	case WXK_NUMPAD9:
		res << wxString::Format(_T("%d"), keyCode - WXK_NUMPAD0);
		break;

	case WXK_NUMPAD_SPACE:
		res << wxT("SPACE"); break;
	case WXK_NUMPAD_TAB:
		res << wxT("TAB"); break;
	case WXK_NUMPAD_ENTER:
		res << wxT("ENTER"); break;

	case WXK_NUMPAD_F1:
	case WXK_NUMPAD_F2:
	case WXK_NUMPAD_F3:
	case WXK_NUMPAD_F4:
		res << wxT("F") << wxString::Format(_T("%d"), keyCode - WXK_NUMPAD_F1);
		break;

	case WXK_NUMPAD_LEFT:
		res << wxT("LEFT"); break;
	case WXK_NUMPAD_UP:
		res << wxT("UP"); break;
	case WXK_NUMPAD_RIGHT:
		res << wxT("RIGHT"); break;
	case WXK_NUMPAD_DOWN:
		res << wxT("DOWN"); break;
	case WXK_NUMPAD_HOME:
		res << wxT("HOME"); break;
#if not wxCHECK_VERSION(2, 8, 0)
	case WXK_NUMPAD_PRIOR:
		res << wxT("PgUp"); break;
	case WXK_NUMPAD_NEXT:
		res << wxT("PgDn"); break;
#endif
	case WXK_NUMPAD_PAGEUP:
		res << wxT("PAGEUP"); break;
	case WXK_NUMPAD_PAGEDOWN:
		res << wxT("PAGEDOWN"); break;
	case WXK_NUMPAD_END:
		res << wxT("END"); break;
	case WXK_NUMPAD_BEGIN:
		res << wxT("BEGIN"); break;
	case WXK_NUMPAD_INSERT:
		res << wxT("INSERT"); break;
	case WXK_NUMPAD_DELETE:
		res << wxT("DELETE"); break;
	case WXK_NUMPAD_EQUAL:
		res << wxT("="); break;
	case WXK_NUMPAD_MULTIPLY:
		res << wxT("*"); break;
	case WXK_NUMPAD_ADD:
		res << wxT("+"); break;
	case WXK_NUMPAD_SEPARATOR:
		res << wxT("SEPARATOR"); break;
	case WXK_NUMPAD_SUBTRACT:
		res << wxT("-"); break;
	case WXK_NUMPAD_DECIMAL:
		res << wxT("."); break;
	case WXK_NUMPAD_DIVIDE:
		res << wxT("/"); break;
    default:
        break;
	}

	return res;
}
// ----------------------------------------------------------------------------
bool clKeyboardManager::WriteFileContent(const wxFileName& fn, const wxString& content, const wxMBConv& conv)  //(2019/04/3)
// ----------------------------------------------------------------------------

{
    wxFFile file(fn.GetFullPath(), wxT("w+b"));
    if(!file.IsOpened()) { return false; }

    if(!file.Write(content, conv)) { return false; }
    return true;
}
// ----------------------------------------------------------------------------
bool clKeyboardManager::ReadFileContent(const wxFileName& fn, wxString& data, const wxMBConv& conv)    //(2019/04/3)
// ----------------------------------------------------------------------------

{
    wxString filename = fn.GetFullPath();
    wxFFile file(filename, wxT("rb"));
    if(file.IsOpened() == false) {
        // Nothing to be done
        return false;
    }
    return file.ReadAll(&data, conv);
}
// ----------------------------------------------------------------------------
void clKeyboardShortcut::FromString(const wxString& accelString)
// ----------------------------------------------------------------------------
{
    Clear();
    wxArrayString tokens = ::wxStringTokenize(accelString, _T("-+"), wxTOKEN_STRTOK);
    for(size_t i = 0; i < tokens.GetCount(); ++i) {
        wxString token = tokens.Item(i);
        token.MakeLower();
        if(token == _T("shift")) {
            m_shift = true;
        } else if(token == _T("alt")) {
            m_alt = true;
        } else if(token == _T("ctrl")) {
            m_ctrl = true;
        } else {
            m_keyCode = tokens.Item(i);
        }
    }
}
// ----------------------------------------------------------------------------
wxString clKeyboardShortcut::ToString() const
// ----------------------------------------------------------------------------
{
    // An accelerator must contain a key code
    if(m_keyCode.IsEmpty()) {
        return _T("");
    }

    wxString str;
    if(m_ctrl) {
        str << _T("Ctrl-");
    }
    if(m_alt) {
        str << _T("Alt-");
    }
    if(m_shift) {
        str << _T("Shift-");
    }
    str << m_keyCode;
    return str;
}
