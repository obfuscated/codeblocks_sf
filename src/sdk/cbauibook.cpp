/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */


#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "prep.h"
    #include "cbauibook.h"
    #include "manager.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "cbeditor.h"
    #include <wx/dcclient.h>
    #include <wx/app.h>
#endif

    #include <wx/tooltip.h>

// static
bool cbAuiNotebook::s_AllowMousewheel = true;
cbAuiNotebookArray cbAuiNotebook::s_cbAuiNotebookArray;
wxString cbAuiNotebook::s_modKeys = _T("Ctrl");
bool cbAuiNotebook::s_modToAdvance = false;
int cbAuiNotebook::s_advanceDirection = 1;
int cbAuiNotebook::s_moveDirection = 1;


BEGIN_EVENT_TABLE(cbAuiNotebook, wxAuiNotebook)
#if wxCHECK_VERSION(2, 9, 0)
    EVT_NAVIGATION_KEY(cbAuiNotebook::OnNavigationKeyNotebook)
#else
    EVT_NAVIGATION_KEY(cbAuiNotebook::OnNavigationKey)
#endif
    EVT_IDLE(cbAuiNotebook::OnIdle)
    EVT_AUINOTEBOOK_DRAG_DONE(wxID_ANY, cbAuiNotebook::OnDragDone)
END_EVENT_TABLE()

cbAuiNotebook::cbAuiNotebook(wxWindow* pParent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxAuiNotebook(pParent, id, pos, size, style),
#ifdef __WXMSW__
          m_LastSelected(wxNOT_FOUND),
          m_LastId(0),
#endif
#if !wxCHECK_VERSION(2, 9, 4)
          m_HasToolTip(false),
#endif
          m_SetZoomOnIdle(false),
          m_MinimizeFreeSpaceOnIdle(false),
          m_TabCtrlSize(wxDefaultSize)
{
    //ctor
#ifdef __WXGTK__
    m_mgr.SetFlags((m_mgr.GetFlags() | wxAUI_MGR_VENETIAN_BLINDS_HINT) & ~wxAUI_MGR_TRANSPARENT_HINT);
#endif  // #ifdef __WXGTK__
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));
#if defined __WXMSW__ && wxCHECK_VERSION(2, 9, 4)
    wxToolTip::SetMaxWidth(-1);
#endif
    s_AllowMousewheel = cfg->ReadBool(_T("/environment/tabs_use_mousewheel"),true);
    s_modKeys = cfg->Read(_T("/environment/tabs_mousewheel_modifier"),_T("Ctrl"));
    s_modToAdvance = cfg->ReadBool(_T("/environment/tabs_mousewheel_advance"),false);
    cbAuiNotebook::InvertAdvanceDirection(cfg->ReadBool(_T("/environment/tabs_invert_advance"),false));
    cbAuiNotebook::InvertMoveDirection(cfg->ReadBool(_T("/environment/tabs_invert_move"),false));

    if (s_cbAuiNotebookArray.Index(this) == wxNOT_FOUND)
        s_cbAuiNotebookArray.Add(this);
}

cbAuiNotebook::~cbAuiNotebook()
{
    s_cbAuiNotebookArray.Remove(this);
}

bool cbAuiNotebook::CheckKeyModifier()
{
    bool result = true;
    // this search must be case-insensitive
    wxString str = s_modKeys;
    str.MakeUpper();

    if (result && str.Contains(wxT("ALT")))
        result = wxGetKeyState(WXK_ALT);
    if (result && str.Contains(wxT("CTRL")))
        result = wxGetKeyState(WXK_CONTROL);
#if defined(__WXMAC__) || defined(__WXCOCOA__)
    if (result && str.Contains(wxT("XCTRL")))
        result = wxGetKeyState(WXK_COMMAND);
#endif
    if (result && str.Contains(wxT("SHIFT")))
        result = wxGetKeyState(WXK_SHIFT);
    return result;
}

void cbAuiNotebook::UpdateTabControlsArray()
{
    cbAuiTabCtrlArray saveTabCtrls = m_TabCtrls;
    m_TabCtrls.Clear();
    // first get all tab-controls
    const size_t tab_Count = GetPageCount();
    for (size_t i = 0; i < tab_Count; ++i)
    {
        wxAuiTabCtrl* tabCtrl = 0;
        int idx = -1;
        if (FindTab(GetPage(i), &tabCtrl, &idx))
        {
            if (tabCtrl && m_TabCtrls.Index(tabCtrl) == wxNOT_FOUND)
                m_TabCtrls.Add(tabCtrl);
        }
        else
            continue;
    }
    bool needEventRebind = m_TabCtrls.GetCount() != saveTabCtrls.GetCount();
    if (!needEventRebind)
    {
        for (size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
        {
            if (saveTabCtrls.Index(m_TabCtrls[i]) == wxNOT_FOUND)
            {
                needEventRebind = true;
                break;
            }
        }
    }
    if (needEventRebind)
    {
        ResetTabCtrlEvents();
    }
}

void cbAuiNotebook::ResetTabCtrlEvents()
{
    for (size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
    {
#if !wxCHECK_VERSION(2, 9, 4)
        m_TabCtrls[i]->Disconnect(wxEVT_MOTION, wxMouseEventHandler(cbAuiNotebook::OnMotion));
        m_TabCtrls[i]->Connect(wxEVT_MOTION ,   wxMouseEventHandler(cbAuiNotebook::OnMotion));
#endif
        m_TabCtrls[i]->Disconnect(wxEVT_LEFT_DCLICK, wxMouseEventHandler(cbAuiNotebook::OnTabCtrlDblClick));
        m_TabCtrls[i]->Connect(wxEVT_LEFT_DCLICK,    wxMouseEventHandler(cbAuiNotebook::OnTabCtrlDblClick));
        m_TabCtrls[i]->Disconnect(wxEVT_SIZE,        wxSizeEventHandler(cbAuiNotebook::OnResize));
        m_TabCtrls[i]->Connect(wxEVT_SIZE,           wxSizeEventHandler(cbAuiNotebook::OnResize));
        m_TabCtrls[i]->Disconnect(wxEVT_MOUSEWHEEL,  wxMouseEventHandler(cbAuiNotebook::OnTabCtrlMouseWheel));
#ifdef __WXMSW__
        m_TabCtrls[i]->Disconnect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(cbAuiNotebook::OnEnterTabCtrl));
        m_TabCtrls[i]->Disconnect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(cbAuiNotebook::OnLeaveTabCtrl));
#endif
        if (GetPageCount() > 1)
        {
            if (s_AllowMousewheel)
                m_TabCtrls[i]->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler(cbAuiNotebook::OnTabCtrlMouseWheel));
#ifdef __WXMSW__
            m_TabCtrls[i]->Connect(wxEVT_ENTER_WINDOW, wxMouseEventHandler(cbAuiNotebook::OnEnterTabCtrl));
            m_TabCtrls[i]->Connect(wxEVT_LEAVE_WINDOW, wxMouseEventHandler(cbAuiNotebook::OnLeaveTabCtrl));
#endif
        }
    }
}

void cbAuiNotebook::FocusActiveTabCtrl()
{
    UpdateTabControlsArray();
    int sel = GetSelection();
    if (sel < 0)
        return;

    wxWindow* wnd = GetPage(static_cast<size_t>(sel));
    if (!wnd)
        return;

    for (size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
    {
        wxWindow* win = m_TabCtrls[i]->GetWindowFromIdx(m_TabCtrls[i]->GetActivePage());
        if (win && (win == wnd))
        {
            m_TabCtrls[i]->SetFocus();
            break;
        }
    }
}

void cbAuiNotebook::SetZoom(int zoom)
{
    // we only set zoom-factor for active (visible) tabs,
    // all others are set if system is idle
    UpdateTabControlsArray();
    for (size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
    {
        wxWindow* win = m_TabCtrls[i]->GetWindowFromIdx(m_TabCtrls[i]->GetActivePage());
        if (win && static_cast<EditorBase*>(win)->IsBuiltinEditor())
            static_cast<cbEditor*>(win)->SetZoom(zoom);
    }
    m_SetZoomOnIdle = true;
}

void cbAuiNotebook::OnIdle(wxIdleEvent& /*event*/)
{
    if (m_SetZoomOnIdle)
    {
        m_SetZoomOnIdle = false;
        int zoom = Manager::Get()->GetEditorManager()->GetZoom();
        for (size_t i = 0; i < GetPageCount(); ++i)
        {
            wxWindow* win = GetPage(i);
            if (win && static_cast<EditorBase*>(win)->IsBuiltinEditor())
                static_cast<cbEditor*>(win)->SetZoom(zoom);
        }
    }

    if (m_MinimizeFreeSpaceOnIdle)
    {
        m_MinimizeFreeSpaceOnIdle = false;
        UpdateTabControlsArray();
        for (size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
            MinimizeFreeSpace(m_TabCtrls[i]);
    }

}

void cbAuiNotebook::OnDragDone(wxAuiNotebookEvent& /*event*/)
{
    UpdateTabControlsArray();
}

#ifdef __WXMSW__
void cbAuiNotebook::OnEnterTabCtrl(wxMouseEvent& event)
{
    if (!wxTheApp->IsActive())
        return;

    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (tabCtrl)
    {
        cbAuiNotebook* nb = (cbAuiNotebook*)tabCtrl->GetParent();
        if (nb)
        {
            if (   s_AllowMousewheel
                && (nb->m_LastSelected == wxNOT_FOUND)
                && (nb->m_LastId == 0) )
            {
                nb->StoreFocus();
                tabCtrl->SetFocus();
            }
        }
    }
}

void cbAuiNotebook::OnLeaveTabCtrl(wxMouseEvent& event)
{
    if (!wxTheApp->IsActive())
        return;

    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (tabCtrl)
    {
        cbAuiNotebook* nb = (cbAuiNotebook*)tabCtrl->GetParent();
        if (nb)
            nb->RestoreFocus();
    }

}

bool cbAuiNotebook::IsFocusStored(wxWindow* page)
{
    wxWindow* win = FindWindowById(m_LastId);
    while (win)
    {
        if (win == page)
        {
            return true;
            break;
        }
        win = win->GetParent();
    }
    return false;
}

void cbAuiNotebook::StoreFocus()
{
    // save Id of last focused window and last selected tab
    wxWindow* win = wxWindow::FindFocus();
    if(win)
        m_LastId = win->GetId();
    else
        m_LastId = 0;
    m_LastSelected = GetSelection();
}

void cbAuiNotebook::RestoreFocus()
{
    // if selected tab has changed, we set the focus on the window it belongs too
    if ((m_LastSelected != wxNOT_FOUND) && (GetSelection() != m_LastSelected))
    {
        wxWindow* win = GetPage(GetSelection());
        if (win)
            win->SetFocus();
    }
    // otherwise, we restore the former focus, if the window
    // with the saved Id still exists
    else if (m_LastId != 0)
    {
        wxWindow* win = FindWindowById(m_LastId);
        if (win)
            win->SetFocus();
    }
    m_LastSelected = wxNOT_FOUND;
    m_LastId = 0;
}
#endif // #ifdef __WXMSW__

void cbAuiNotebook::OnTabCtrlDblClick(wxMouseEvent& event)
{
    wxWindow* win = nullptr;
    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (tabCtrl && tabCtrl->TabHitTest(event.GetX(), event.GetY(), &win))
    {
        if (win != nullptr)
        {
            // send double-click-event
            CodeBlocksEvent theEvent(cbEVT_CBAUIBOOK_LEFT_DCLICK, GetParent()->GetId());
            theEvent.SetEventObject(win);
            GetParent()->GetEventHandler()->ProcessEvent(theEvent);
        }
    }
}

void cbAuiNotebook::OnTabCtrlMouseWheel(wxMouseEvent& event)
{
    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (!tabCtrl)
        return;
    cbAuiNotebook* nb = (cbAuiNotebook*)tabCtrl->GetParent();
    if (!nb)
        return;

    nb->SetSelection(nb->GetPageIndex(tabCtrl->GetWindowFromIdx(tabCtrl->GetActivePage())));

    bool modkeys = CheckKeyModifier();

    bool advance = (!s_modToAdvance && !modkeys) || (s_modToAdvance &&  modkeys);

    if (advance)
        nb->AdvanceSelection((event.GetWheelRotation() * s_advanceDirection) < 0);
    else
    {
        size_t tabOffset = tabCtrl->GetTabOffset();
        size_t lastTabIdx = tabCtrl->GetPageCount()-1;
        wxWindow* win = nb->GetPage(nb->GetSelection());
        if (win)
        {
            wxClientDC dc(win);
            if ((event.GetWheelRotation() * s_moveDirection) > 0)
            {
                if (!tabCtrl->IsTabVisible(lastTabIdx,tabOffset,&dc,win))
                    tabOffset++;
            }
            else
            {
                if (tabOffset > 0)
                    tabOffset--;
            }
            tabCtrl->SetTabOffset(tabOffset);
            nb->Refresh();
        }
    }
}

void cbAuiNotebook::OnResize(wxSizeEvent& event)
{
    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (tabCtrl)
    {
        cbAuiNotebook* nb = (cbAuiNotebook*)tabCtrl->GetParent();
        if (nb)
        {
            if(nb->m_TabCtrlSize != event.GetSize())
            {
                nb->m_TabCtrlSize = event.GetSize();
                nb->MinimizeFreeSpace();
            }
        }
    }
    event.Skip();
}

#if !wxCHECK_VERSION(2, 9, 4)
void cbAuiNotebook::OnMotion(wxMouseEvent& event)
{
    event.Skip();
    wxAuiTabCtrl* tabCtrl = (wxAuiTabCtrl*)event.GetEventObject();
    if (!tabCtrl)
        return;
    cbAuiNotebook* nb = (cbAuiNotebook*)tabCtrl->GetParent();
    if (!nb || !nb->m_HasToolTip)
        return;

    wxWindow* win = nullptr;
    if (event.Moving() && tabCtrl->TabHitTest(event.m_x, event.m_y, &win))
    {
        if (!win)
        {
            tabCtrl->UnsetToolTip();
            return;
        }
        wxString text(win->GetName());
        // If the text changes, set it else, keep old, to avoid
        // 'moving tooltip' effect
        wxToolTip* tooltip = tabCtrl->GetToolTip();
        if (!tooltip || tooltip->GetTip() != text)
            tabCtrl->SetToolTip(text);
    }
    else
        tabCtrl->UnsetToolTip();
}

bool cbAuiNotebook::SetPageToolTip(size_t idx, const wxString & text )
{
    if (!m_HasToolTip)
        UpdateTabControlsArray();

    m_HasToolTip = true;
    wxWindow* win = GetPage(idx);
    if (win && win->GetName() != text)
        win->SetName(text);
    else
        return false;
    return true;
}
wxString cbAuiNotebook::GetPageToolTip(size_t idx )
{
    wxWindow* win = GetPage(idx);
    if (win)
        return win->GetName();
    return wxEmptyString;
}
#endif
void cbAuiNotebook::MinimizeFreeSpace()
{
    if (GetPageCount() < 2)
        return;
    m_MinimizeFreeSpaceOnIdle = true;
}

void cbAuiNotebook::MinimizeFreeSpace(wxAuiTabCtrl* tabCtrl)
{
    if (!tabCtrl || tabCtrl->GetPageCount() < 2 || !IsWindowReallyShown(this))
        return;

    int ctrl_idx = tabCtrl->GetActivePage();
    wxWindow* win = GetPage(ctrl_idx);
    if (win)
    {
        int tabOffset = tabCtrl->GetTabOffset();

        wxClientDC dc(win);
        size_t lastTabIdx = tabCtrl->GetPageCount() - 1;

        if(!tabCtrl->IsTabVisible(ctrl_idx, tabOffset, & dc, win))
        {
            for (int i = lastTabIdx ; i >= 0; --i)
            {
                if (tabCtrl->IsTabVisible(ctrl_idx, i, & dc, win))
                {
                    tabOffset = i;
                    break;
                }
            }
        }
        while (tabOffset > 0 && tabCtrl->IsTabVisible(lastTabIdx, tabOffset-1, & dc, win))
            --tabOffset;

        tabCtrl->SetTabOffset(tabOffset);
    }
    tabCtrl->Refresh();
}

bool cbAuiNotebook::DeletePage(size_t page)
{
#ifdef __WXMSW__
    if (IsFocusStored(GetPage(page)))
    {
        m_LastSelected = wxNOT_FOUND;
        m_LastId = 0;
    }
#endif // #ifdef __WXMSW__
    bool result = wxAuiNotebook::DeletePage(page);
    MinimizeFreeSpace();
    return result;
}

bool cbAuiNotebook::RemovePage(size_t page)
{
#ifdef __WXMSW__
    if (IsFocusStored(GetPage(page)))
    {
        m_LastSelected = wxNOT_FOUND;
        m_LastId = 0;
    }
#endif // #ifdef __WXMSW__
    bool result = wxAuiNotebook::RemovePage(page);
    MinimizeFreeSpace();
    return result;
}

bool cbAuiNotebook::MovePage(wxWindow* page, size_t new_idx)
{
    UpdateTabControlsArray();
    bool result = false;
    if (m_TabCtrls.GetCount() > 0)
    {
        result = m_TabCtrls[0]->MovePage(page, new_idx);
        Refresh();
        MinimizeFreeSpace();
    }
    return result;
}

int cbAuiNotebook::GetTabPositionFromIndex(int index)
{
    if (GetPageCount() <= 0)
        return wxNOT_FOUND;

    UpdateTabControlsArray();

    wxAuiTabCtrl* tabCtrl = 0;
    int idx = -1;

    if (!FindTab(GetPage(index), &tabCtrl, &idx))
        return wxNOT_FOUND;

    if (!tabCtrl || idx < 0)
        return wxNOT_FOUND;

    int indexOffset = 0;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();
    for (size_t i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes[i];
        if (pane.name == wxT("dummy"))
            continue;

        if (pane.window == GetTabFrameFromTabCtrl(tabCtrl))
            break;

        for (size_t j = 0; j < m_TabCtrls.GetCount(); ++j)
        {
            if (pane.window == GetTabFrameFromTabCtrl(m_TabCtrls[j]))
            {
                indexOffset += m_TabCtrls[j]->GetPageCount();
                break;
            }
        }
    }
    return idx + indexOffset;
}

void cbAuiNotebook::AdvanceSelection(bool forward)
{
    if (GetPageCount() <= 1)
        return;

    int currentSelection = GetSelection();

    wxAuiTabCtrl* tabCtrl = 0;
    int idx = -1;

    if (!FindTab(GetPage(currentSelection), &tabCtrl, &idx))
        return;

    if (!tabCtrl || idx < 0)
        return;

    wxWindow* page = 0;
    size_t maxPages = tabCtrl->GetPageCount();

    forward?idx++:idx--;

    if (idx < 0)
        idx = maxPages - 1;

    if ((size_t)idx < maxPages)
        page = tabCtrl->GetPage(idx).window;

    if (!page && maxPages > 0)
        page = tabCtrl->GetPage(0).window;

    if (page)
    {
        currentSelection = GetPageIndex(page);
        SetSelection(currentSelection);
    }
}

#if wxCHECK_VERSION(2, 9, 0)
void cbAuiNotebook::OnNavigationKeyNotebook(wxNavigationKeyEvent& event)
#else
void cbAuiNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
#endif
{
    // if we change window, we call our own AdvanceSelection
    if ( event.IsWindowChange() )
    {
        AdvanceSelection(event.GetDirection());
    }
    else // otherwise we call the event-handler from the parent-class
    {
#if wxCHECK_VERSION(2, 9, 0)
        wxAuiNotebook::OnNavigationKeyNotebook(event);
#else
        wxAuiNotebook::OnNavigationKey(event);
#endif
    }
}

wxString cbAuiNotebook::SavePerspective()
{
    // Build list of panes/tabs
    wxString tabs;

    // first get all tab-controls
    UpdateTabControlsArray();

    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();
    for (size_t i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.name == wxT("dummy"))
            continue;

        wxAuiTabCtrl* tabCtrl = 0;
        for (size_t j = 0; j < m_TabCtrls.GetCount(); ++j)
        {
            if (pane.window == GetTabFrameFromTabCtrl(m_TabCtrls.Item(j)))
            {
                tabCtrl = m_TabCtrls.Item(j);
                break;
            }
        }
        if (tabCtrl)
        {
            if (!tabs.empty())
                tabs += wxT("|");

            tabs += pane.name;
            tabs += wxT("=");

            // add tab id's
            size_t page_count = tabCtrl->GetPageCount();
            for (size_t p = 0; p < page_count; ++p)
            {
                wxAuiNotebookPage& page = tabCtrl->GetPage(p);
                const size_t page_idx = m_tabs.GetIdxFromWindow(page.window);

                if (p)
                    tabs += wxT(",");

#if wxCHECK_VERSION(2, 9, 3)
                if ((int)page_idx == m_curPage)
#else
                if ((int)page_idx == m_curpage)
#endif
                    tabs += wxT("*");
                else if ((int)p == tabCtrl->GetActivePage())
                    tabs += wxT("+");

                tabs += wxString::Format(wxT("%u"), page_idx);
            }
        }
    }
    tabs += wxT("@");

    // Add frame perspective
    tabs += m_mgr.SavePerspective();

    return tabs;
}

//bool cbAuiNotebook::LoadPerspective(const wxString& layout) {
//   // Remove all tab ctrls (but still keep them in main index)
//   const size_t tab_count = m_tabs.GetPageCount();
//   for (size_t i = 0; i < tab_count; ++i) {
//      wxWindow* wnd = m_tabs.GetWindowFromIdx(i);
//
//      // find out which onscreen tab ctrl owns this tab
//      wxAuiTabCtrl* ctrl;
//      int ctrl_idx;
//      if (!FindTab(wnd, &ctrl, &ctrl_idx))
//         return false;
//
//      // remove the tab from ctrl
//      if (!ctrl->RemovePage(wnd))
//         return false;
//   }
//   RemoveEmptyTabFrames();
//
//   size_t sel_page = 0;
//
//   wxString tabs = layout.BeforeFirst(wxT('@'));
//   while (1)
//    {
//      const wxString tab_part = tabs.BeforeFirst(wxT('|'));
//
//      // if the string is empty, we're done parsing
//        if (tab_part.empty())
//            break;
//
//      // Get pane name
//      const wxString pane_name = tab_part.BeforeFirst(wxT('='));
//
//      // create a new tab frame
//      wxTabFrame* new_tabs = new wxTabFrame;
//      new_tabs->m_tabs = new wxAuiTabCtrl(this,
//                                 m_tab_id_counter++);
////                            wxDefaultPosition,
////                            wxDefaultSize,
////                            wxNO_BORDER|wxWANTS_CHARS);
//      new_tabs->m_tabs->SetArtProvider(m_tabs.GetArtProvider()->Clone());
//      new_tabs->SetTabCtrlHeight(m_tab_ctrl_height);
//      new_tabs->m_tabs->SetFlags(m_flags);
//      wxAuiTabCtrl *dest_tabs = new_tabs->m_tabs;
//
//      // create a pane info structure with the information
//      // about where the pane should be added
//      wxAuiPaneInfo pane_info = wxAuiPaneInfo().Name(pane_name).Bottom().CaptionVisible(false);
//      m_mgr.AddPane(new_tabs, pane_info);
//
//      // Get list of tab id's and move them to pane
//      wxString tab_list = tab_part.AfterFirst(wxT('='));
//      while (1)
//      {
//         wxString tab = tab_list.BeforeFirst(wxT(','));
//         if (tab.empty()) break;
//         tab_list = tab_list.AfterFirst(wxT(','));
//
//         // Check if this page has an 'active' marker
//         const wxChar c = tab[0];
//         if (c == wxT('+') || c == wxT('*'))
//            tab = tab.Mid(1);
//
//         const size_t tab_idx = wxAtoi(tab.c_str());
//         if (tab_idx >= GetPageCount()) continue;
//
//         // Move tab to pane
//         wxAuiNotebookPage& page = m_tabs.GetPage(tab_idx);
//         const size_t newpage_idx = dest_tabs->GetPageCount();
//         dest_tabs->InsertPage(page.window, page, newpage_idx);
//
//         if      ( c == wxT('+')) dest_tabs->SetActivePage(newpage_idx);
//         else if ( c == wxT('*')) sel_page = tab_idx;
//      }
//      dest_tabs->DoShowHide();
//
//      tabs = tabs.AfterFirst(wxT('|'));
//   }
//
//   // Load the frame perspective
//   const wxString frames = layout.AfterFirst(wxT('@'));
//   m_mgr.LoadPerspective(frames);
//
//   // Force refresh of selection
//   m_curpage = -1;
//   SetSelection(sel_page);
//
//   return true;
//}

// static functions(common to all cbAuiNotebooks)

void cbAuiNotebook::AllowScrolling(bool allow)
{
    s_AllowMousewheel = allow;
    for (size_t i = 0; i < s_cbAuiNotebookArray.GetCount(); ++i)
    {
        s_cbAuiNotebookArray[i]->UpdateTabControlsArray();
        s_cbAuiNotebookArray[i]->ResetTabCtrlEvents();
    }
}

void cbAuiNotebook::SetModKeys(wxString keys)
{
    s_modKeys = keys;
}

void cbAuiNotebook::UseModToAdvance(bool use)
{
    s_modToAdvance = use;
}

void cbAuiNotebook::InvertAdvanceDirection(bool invert)
{
    s_advanceDirection=invert ? -1 : 1;
}

void cbAuiNotebook::InvertMoveDirection(bool invert)
{
    s_moveDirection=invert ? -1 : 1;
}
