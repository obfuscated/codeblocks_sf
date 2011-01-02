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
#endif

#include <wx/app.h>
#include <wx/tipwin.h>

const int wxAuiBaseTabCtrlId = 5380;

const long cbAuiNotebook::idNoteBookTimer = wxNewId();

static bool PointClose(wxPoint pt1, wxPoint pt2) {
    if (abs(pt1.x - pt2.x) > 3)
        return false;
    if (abs(pt1.y - pt2.y) > 3)
        return false;
    return true;
}

BEGIN_EVENT_TABLE(cbAuiNotebook, wxAuiNotebook)
#if wxCHECK_VERSION(2, 9, 0)
    EVT_NAVIGATION_KEY(cbAuiNotebook::OnNavigationKeyNotebook)
#else
    EVT_NAVIGATION_KEY(cbAuiNotebook::OnNavigationKey)
#endif
END_EVENT_TABLE()


cbAuiNotebook::cbAuiNotebook(wxWindow* pParent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : wxAuiNotebook(pParent, id, pos, size, style),
          m_pDwellTimer(nullptr),
          m_pToolTip(nullptr),
          m_LastMousePosition(wxPoint(-1,-1)),
          m_LastShownAt(wxPoint(-1,-1)),
          m_LastTime(0),
          m_AllowToolTips(true)
{
    //ctor
#ifdef __WXGTK__
    m_mgr.SetFlags((m_mgr.GetFlags() | wxAUI_MGR_VENETIAN_BLINDS_HINT) & ~wxAUI_MGR_TRANSPARENT_HINT);
#endif  // #ifdef __WXGTK__
}

cbAuiNotebook::~cbAuiNotebook()
{
    wxDELETE(m_pToolTip);
    wxDELETE(m_pDwellTimer);
}

void cbAuiNotebook::UpdateTabControlsArray()
{
    m_TabCtrls.Clear();
    // first get all tab-controls
    const size_t tab_Count = GetPageCount();
    for (size_t i = 0; i < tab_Count; ++i)
    {
        wxAuiTabCtrl* tabCtrl = 0;
        int idx = -1;
        if (FindTab(GetPage(i), &tabCtrl, &idx))
        {
            if(m_TabCtrls.Index(tabCtrl) == wxNOT_FOUND)
            {
                m_TabCtrls.Add(tabCtrl);
            }
        }
        else
        {
            continue;
        }
    }
}

void cbAuiNotebook::AllowToolTips(bool allow)
{
    m_AllowToolTips = allow;
    if(!m_AllowToolTips)
        CancelToolTip();
}

void cbAuiNotebook::OnDwellTimerTrigger(wxTimerEvent& /*event*/)
{
    if(!m_AllowToolTips ||
       !wxTheApp->IsActive())
    {
        CancelToolTip();
        return;
    }

    if (GetPageCount() < 1)
    {
        CancelToolTip();
        wxDELETE(m_pDwellTimer);
        m_StopWatch.Pause();
        return;
    }

    long curTime = m_StopWatch.Time();
    UpdateTabControlsArray();
    wxPoint screenPosition = wxGetMousePosition();
	wxPoint thePoint;
    wxWindow* win = 0;
    bool tabHit = false;

    for(size_t i = 0; i < m_TabCtrls.GetCount(); ++i)
    {
        thePoint = screenPosition - m_TabCtrls[i]->GetScreenPosition();
        if(m_TabCtrls[i]->TabHitTest(thePoint.x, thePoint.y, &win))
        {
            tabHit = true;
            if(PointClose(thePoint, m_LastMousePosition))
            {
                if(!PointClose(thePoint, m_LastShownAt))
                {
                    if(curTime - m_LastTime > 1000)
                    {
                        ShowToolTip(win);
                        m_LastShownAt = thePoint;
                    }
                    m_LastMousePosition = thePoint;
                    return;
                }
                return;
            }
            CancelToolTip();
            m_LastMousePosition = thePoint;
            m_LastShownAt = wxPoint(-1,-1);
            m_LastTime = curTime;
        }
    }
    if (!tabHit)
        CancelToolTip();
}

void cbAuiNotebook::ShowToolTip(wxWindow* win)
{
    CancelToolTip();
    wxString text = win->GetName();
    if(!text.IsEmpty())
        m_pToolTip = new wxTipWindow(Manager::Get()->GetAppWindow(),text, 640, &m_pToolTip);
}

void cbAuiNotebook::CancelToolTip()
{
    if(m_pToolTip != nullptr)
        m_pToolTip->Destroy();
}

void cbAuiNotebook::SetTabToolTip(wxWindow* win, wxString msg)
{
    if(m_pDwellTimer == nullptr)
    {
        m_pDwellTimer = new wxTimer(this, idNoteBookTimer);
        Connect(idNoteBookTimer,wxEVT_TIMER,(wxObjectEventFunction)&cbAuiNotebook::OnDwellTimerTrigger);
        m_pDwellTimer->Start(100,false);
        m_StopWatch.Start();
    }
    if(win)
        win->SetName(msg);

}

int cbAuiNotebook::GetTabPositionFromIndex(int index)
{
    if (GetPageCount() <= 1)
    {
        return wxNOT_FOUND;
    }

    UpdateTabControlsArray();

    wxAuiTabCtrl* tabCtrl = 0;
    int idx = -1;

    if (!FindTab(GetPage(index), &tabCtrl, &idx))
    {
        return wxNOT_FOUND;
    }

    if (!tabCtrl || idx < 0)
    {
        return wxNOT_FOUND;
    }

    int indexOffset = 0;
    wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
    const size_t pane_count = all_panes.GetCount();
    for (size_t i = 0; i < pane_count; ++i)
    {
        wxAuiPaneInfo& pane = all_panes.Item(i);
        if (pane.name == wxT("dummy"))
        {
            continue;
        }

        if(pane.window == GetTabFrameFromTabCtrl(tabCtrl))
        {
            break;
        }
        for (size_t j = 0; j < m_TabCtrls.GetCount(); ++j)
        {
            if(pane.window == GetTabFrameFromTabCtrl(m_TabCtrls.Item(j)))
            {
                indexOffset += m_TabCtrls.Item(j)->GetPageCount();
                break;
            }
        }
    }
    return idx + indexOffset;
}

void cbAuiNotebook::AdvanceSelection(bool forward)
{
    if (GetPageCount() <= 1)
    {
        return;
    }
    int currentSelection = GetSelection();


    wxAuiTabCtrl* tabCtrl = 0;
    int idx = -1;

    if (!FindTab(GetPage(currentSelection), &tabCtrl, &idx))
    {
        return;
    }

    if (!tabCtrl || idx < 0)
    {
        return;
    }

    wxWindow* page = 0;
    size_t maxPages = tabCtrl->GetPageCount();

    forward?idx++:idx--;

    if (idx < 0)
    {
        idx = maxPages - 1;
    }

    if ((size_t)idx < maxPages)
    {
        page = tabCtrl->GetPage(idx).window;
    }

    if (!page && maxPages > 0)
    {
        page = tabCtrl->GetPage(0).window;
    }

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
        {
            continue;
        }

        wxAuiTabCtrl* tabCtrl = 0;
        for (size_t j = 0; j < m_TabCtrls.GetCount(); ++j)
        {
            if(pane.window == GetTabFrameFromTabCtrl(m_TabCtrls.Item(j)))
            {
                tabCtrl = m_TabCtrls.Item(j);
                break;
            }
        }
        if(tabCtrl)
        {
            if (!tabs.empty())
            {
                tabs += wxT("|");
            }
            tabs += pane.name;
            tabs += wxT("=");

            // add tab id's
            size_t page_count = tabCtrl->GetPageCount();
            for (size_t p = 0; p < page_count; ++p)
            {
                wxAuiNotebookPage& page = tabCtrl->GetPage(p);
                const size_t page_idx = m_tabs.GetIdxFromWindow(page.window);

                if (p)
                {
                    tabs += wxT(",");
                }

                if ((int)page_idx == m_curpage)
                {
                    tabs += wxT("*");
                }
                else if ((int)p == tabCtrl->GetActivePage())
                {
                    tabs += wxT("+");
                }
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
//      while(1) {
//         wxString tab = tab_list.BeforeFirst(wxT(','));
//         if (tab.empty()) break;
//         tab_list = tab_list.AfterFirst(wxT(','));
//
//         // Check if this page has an 'active' marker
//         const wxChar c = tab[0];
//         if (c == wxT('+') || c == wxT('*')) {
//            tab = tab.Mid(1);
//         }
//
//         const size_t tab_idx = wxAtoi(tab.c_str());
//         if (tab_idx >= GetPageCount()) continue;
//
//         // Move tab to pane
//         wxAuiNotebookPage& page = m_tabs.GetPage(tab_idx);
//         const size_t newpage_idx = dest_tabs->GetPageCount();
//         dest_tabs->InsertPage(page.window, page, newpage_idx);
//
//         if (c == wxT('+')) dest_tabs->SetActivePage(newpage_idx);
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
