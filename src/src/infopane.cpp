/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/event.h>
    #include <wx/menu.h>
    #include "cbexception.h"
    #include "globals.h"
    #include "configmanager.h"
#endif

#include <wx/wupdlock.h>
#include <wx/tokenzr.h>
#include "infopane.h"
#include "loggers.h"

namespace
{
    long idNB_TabTop               = wxNewId();
    long idNB_TabBottom            = wxNewId();

    long idCopyAllToClipboard      = wxNewId();
    long idCopySelectedToClipboard = wxNewId();

    long idWrapMode                = wxNewId();
    long idClear                   = wxNewId();

    long idNB                      = wxNewId();
}

BEGIN_EVENT_TABLE(InfoPane, cbAuiNotebook)
    EVT_MENU(idNB_TabTop,    InfoPane::OnTabPosition)
    EVT_MENU(idNB_TabBottom, InfoPane::OnTabPosition)

    EVT_MENU_RANGE(idCopyAllToClipboard, idCopySelectedToClipboard,  InfoPane::OnCopy)

    EVT_MENU(idWrapMode, InfoPane::OnWrapMode)
    EVT_MENU(idClear,    InfoPane::OnClear)
    EVT_MENU(wxID_ANY,   InfoPane::OnMenu)

    EVT_CONTEXT_MENU(InfoPane::ContextMenu)

    EVT_AUINOTEBOOK_TAB_RIGHT_UP(idNB, InfoPane::OnTabContextMenu)
    EVT_AUINOTEBOOK_PAGE_CLOSE(idNB,   InfoPane::OnCloseClicked)
END_EVENT_TABLE()


InfoPane::InfoPane(wxWindow* parent) : cbAuiNotebook(parent, idNB, wxDefaultPosition, wxDefaultSize, infopane_flags)
{
    m_DefaultBitmap = cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/edit_16x16.png"), wxBITMAP_TYPE_PNG);
}

InfoPane::~InfoPane()
{
}

wxString InfoPane::SaveTabOrder()
{
    UpdateEffectiveTabOrder();
    m_Pages.Sort(&CompareIndexes);
    wxString layout;
    for (size_t i = 0 ; i < m_Pages.GetCount(); ++i)
    {
        layout << m_Pages.Item(i)->title;
        layout << _T("=");
        layout << m_Pages.Item(i)->indexInNB;
        layout << _T(";");
    }
    layout << _T("|selection=");
    layout << GetSelection();
    layout << _T(";");
    return layout;
}

void InfoPane::LoadTabOrder(wxString layout)
{
    wxString theLayout(layout);

    long selectedTab;
    theLayout.AfterLast('=').ToLong(&selectedTab);
    theLayout.Remove(theLayout.Find('|',true));
    wxStringTokenizer strTok(theLayout, _T(";"));
    wxString title;
    while (strTok.HasMoreTokens())
    {
        wxString theToken = strTok.GetNextToken();
        title = theToken.BeforeFirst('=');
        for (size_t j = 0; j < m_Pages.GetCount();++j)
        {
            if (m_Pages.Item(j)->title == title)
            {
                long theIndex;
                if (theToken.AfterFirst('=').ToLong(&theIndex))
                    m_Pages.Item(j)->indexInNB = theIndex;
                break;
            }
        }
    }
    ReorderTabs(&CompareIndexes);
    SetSelection(selectedTab);
}

int InfoPane::CompareIndexes(Page **p1, Page **p2)
{
    int index1 = ((*p1)->indexInNB >= 0?(*p1)->indexInNB:~(*p1)->indexInNB);
    int index2 = ((*p2)->indexInNB >= 0?(*p2)->indexInNB:~(*p2)->indexInNB);
    return index1 - index2;
}

void InfoPane::ReorderTabs(CompareFunction cmp_f)
{
    if (m_Pages.GetCount() == 0)
        return;
    m_Pages.Sort(cmp_f);

    cbAuiNotebook::Hide();
    int index = 0;
    for (size_t i = 0 ; i < m_Pages.GetCount(); ++i)
    {
        int pageIndex = GetPageIndex(m_Pages.Item(i)->window);
        if (m_Pages.Item(i)->indexInNB < 0)
        {
            if (pageIndex >= 0)
                RemovePage(pageIndex);
            if (m_Pages.Item(i)->window)
                m_Pages.Item(i)->window->Hide();
        }
        else
        {
            if (pageIndex < 0)
                AddPagePrivate(m_Pages.Item(i)->window, m_Pages.Item(i)->title, m_Pages.Item(i)->icon);
            if (index++ != pageIndex)
                MovePage(m_Pages.Item(i)->window, index );
        }
    }
    cbAuiNotebook::Show();
}

int InfoPane::AddPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    const wxBitmap& bmp = icon ? *icon : m_DefaultBitmap;

    AddPage(p, title, false, bmp);
    return GetPageCount() - 1;
}

bool InfoPane::InsertPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon, int index)
{
    const wxBitmap& bmp = icon ? *icon : m_DefaultBitmap;

    return InsertPage(index, p, title, false, bmp);
}

void InfoPane::UpdateEffectiveTabOrder()
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        // only update for visible tabs
        if (m_Pages.Item(i)->indexInNB >= 0)
            m_Pages.Item(i)->indexInNB = GetTabPositionFromIndex(GetPageIndex(m_Pages.Item(i)->window));

    }
}

void InfoPane::Toggle(size_t i)
{
    UpdateEffectiveTabOrder();
    if (m_Pages.Item(i)->indexInNB < 0)
    {
        if (InsertPagePrivate(m_Pages.Item(i)->window, m_Pages.Item(i)->title, m_Pages.Item(i)->icon, ~m_Pages.Item(i)->indexInNB))
            m_Pages.Item(i)->indexInNB = ~m_Pages.Item(i)->indexInNB ;
        else
            m_Pages.Item(i)->indexInNB = AddPagePrivate(m_Pages.Item(i)->window, m_Pages.Item(i)->title, m_Pages.Item(i)->icon);
    }
    else
    {
        // Hide the window, otherwise the controls remain partly visible on some windows-versions
        // if we toggle the active logger
        if (m_Pages.Item(i)->window)
            m_Pages.Item(i)->window->Hide();
        RemovePage(GetPageIndex(m_Pages.Item(i)->window));
        m_Pages.Item(i)->indexInNB = ~m_Pages.Item(i)->indexInNB;
    }
}

int InfoPane::GetPageIndexByWindow(wxWindow* win)
{
    for (size_t i=0; i < m_Pages.GetCount(); i++)
    {
        if (m_Pages.Item(i)->window == win)
            return i;
    }
    return -1;
}

int InfoPane::GetCurrentPage(bool &is_logger)
{
    int i = GetPageIndexByWindow( GetPage(GetSelection()) );
    is_logger = m_Pages.Item(i)->islogger;
    return (is_logger ? i : -1);
}

Logger* InfoPane::GetLogger(int index)
{
    if (index < 0 || (size_t)index > m_Pages.GetCount())
        return NULL;
    return m_Pages.Item(index)->islogger ? m_Pages.Item(index)->logger : NULL;
}

wxWindow* InfoPane::GetWindow(int index)
{
    if (index < 0 || (size_t)index > m_Pages.GetCount())
        return NULL;
    return !m_Pages.Item(index)->islogger ? m_Pages.Item(index)->window : NULL;
}

void InfoPane::Show(size_t i)
{
    if (m_Pages.Item(i)->window == nullptr)
        return;

    if (m_Pages.Item(i)->indexInNB < 0)
        Toggle(i);
    else
        SetSelection(GetPageIndex(m_Pages.Item(i)->window));
}

void InfoPane::Hide(Logger* logger)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->logger == logger)
        {
            if (m_Pages.Item(i)->indexInNB >= 0)
                Toggle(i);
            return;
        }
    }
}

void InfoPane::Show(Logger* logger)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->logger == logger)
        {
            if (m_Pages.Item(i)->indexInNB < 0)
                Toggle(i);
            else
                SetSelection(GetPageIndex(m_Pages.Item(i)->window));
            return;
        }
    }
}

void InfoPane::HideNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->window == p)
        {
            if (m_Pages.Item(i)->indexInNB >= 0)
                Toggle(i);
            return;
        }
    }
}

void InfoPane::ShowNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->window == p)
        {
            if (m_Pages.Item(i)->indexInNB < 0)
                Toggle(i);
            else
                SetSelection(GetPageIndex(p));
            return;
        }
    }
}


void InfoPane::OnCopy(wxCommandEvent& event)
{
    int i = GetPageIndexByWindow( GetPage(GetSelection()) );
    if (m_Pages.Item(i)->islogger)
    {
        if      (event.GetId() == idCopyAllToClipboard)
            m_Pages.Item(i)->logger->CopyContentsToClipboard(false);
        else if (event.GetId() == idCopySelectedToClipboard)
            m_Pages.Item(i)->logger->CopyContentsToClipboard(true);
    }
}

void InfoPane::OnWrapMode(cb_unused wxCommandEvent& event)
{
    int i = GetPageIndexByWindow( GetPage(GetSelection()) );
    if (m_Pages.Item(i)->islogger && m_Pages.Item(i)->logger->HasFeature(Logger::Feature::IsWrappable))
    {
        TextCtrlLogger* tcl = static_cast<TextCtrlLogger*>(m_Pages.Item(i)->logger);
        if (tcl) tcl->ToggleWrapMode();
    }
}

void InfoPane::OnClear(cb_unused wxCommandEvent& event)
{
    int i = GetPageIndexByWindow( GetPage(GetSelection()) );
    if (m_Pages.Item(i)->islogger)
        m_Pages.Item(i)->logger->Clear();
}

void InfoPane::OnMenu(wxCommandEvent& event)
{
    int eventID = event.GetId();
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (eventID == m_Pages.Item(i)->eventID)
        {
            Toggle(i);
            return;
        }
    }

    event.Skip();
    return;
}

void InfoPane::ContextMenu(cb_unused wxContextMenuEvent& event)
{
    DoShowContextMenu();
}

void InfoPane::OnTabContextMenu(wxAuiNotebookEvent& event)
{
    if (event.GetSelection() == -1)
        return;
    // select the notebook that sends the event, because the context menu-entries act on the actual selected tab
    SetSelection(event.GetSelection());
    DoShowContextMenu();
}

void InfoPane::OnCloseClicked(wxAuiNotebookEvent& event)
{
    if (event.GetSelection() == -1)
        return;
    // veto the close-event, because we don't want to remove the page (just toggle it)
    // this avoids an assert-message in debug-build (and wx2.9)
    event.Veto();
    // toggle the notebook, that sends the event
    Toggle(GetPageIndexByWindow( GetPage(event.GetSelection())) );
}

void InfoPane::DoShowContextMenu()
{
    UpdateEffectiveTabOrder();
    m_Pages.Sort(&CompareIndexes);

    wxMenu menu;

    int selection = GetSelection();
    if (   (selection >= 0)
        && (selection < static_cast<int>(GetPageCount()))
        && (m_Pages.Item(GetPageIndexByWindow( GetPage(GetSelection()) ))->islogger) )
    {
        Logger* l = m_Pages.Item(GetPageIndexByWindow( GetPage(GetSelection()) ))->logger;

        if (l->HasFeature(Logger::Feature::CanCopy))
        {
            menu.Append(idCopyAllToClipboard,      _("Copy contents to clipboard"));
            menu.Append(idCopySelectedToClipboard, _("Copy selection to clipboard"));
        }
        bool needSeparator = true;
        if (l->HasFeature(Logger::Feature::IsWrappable))
        {
            if (menu.GetMenuItemCount() > 0)
            {
                needSeparator = false;
                menu.AppendSeparator();
            }
            menu.AppendCheckItem(idWrapMode, _("Toggle wrap mode"));
            menu.Check(idWrapMode, l->GetWrapMode());
        }
        if (l->HasFeature(Logger::Feature::CanClear))
        {
            if (needSeparator && menu.GetMenuItemCount() > 0)
                menu.AppendSeparator();
            menu.Append(idClear, _("Clear contents"));
        }
        if (l->HasFeature(Logger::Feature::Additional))
            l->AppendAdditionalMenuItems(menu);
    }

    if (menu.GetMenuItemCount() > 0)
        menu.AppendSeparator();

    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/infopane_tabs_bottom"), false))
    	menu.Append(idNB_TabTop, _("Tabs at top"));
    else
    	menu.Append(idNB_TabBottom, _("Tabs at bottom"));

    // add toggle sub-menu
    wxMenu* view = new wxMenu;
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->window)
        {
            view->Append(m_Pages.Item(i)->eventID, m_Pages.Item(i)->title, wxEmptyString, wxITEM_CHECK);
            view->Check(m_Pages.Item(i)->eventID, m_Pages.Item(i)->indexInNB >= 0);
        }
    }

    if (view->GetMenuItemCount() > 0)
        menu.AppendSubMenu(view, _("Toggle..."));
    else
        delete view;

    PopupMenu(&menu);
}

void InfoPane::OnTabPosition(wxCommandEvent& event)
{
    long style = GetWindowStyleFlag();
    style &= ~wxAUI_NB_BOTTOM;

    if (event.GetId() == idNB_TabBottom)
        style |= wxAUI_NB_BOTTOM;
    SetWindowStyleFlag(style);
    Refresh();
    // (style & wxAUI_NB_BOTTOM) saves info only about the the tabs position
    Manager::Get()->GetConfigManager(_T("app"))->Write(_T("/environment/infopane_tabs_bottom"), (bool)(style & wxAUI_NB_BOTTOM));
}

bool InfoPane::AddLogger(Logger* logger, wxWindow* p, const wxString& title, wxBitmap* icon)
{
    if (p)
    {
        Page* pg      = new Page();
        pg->indexInNB = AddPagePrivate(p, title, icon);
        pg->window    = p;
        pg->logger    = logger;
        pg->icon      = icon;
        pg->title     = title;
        pg->eventID   = wxNewId();
        pg->islogger  = true;

        m_Pages.Add(pg);

        return true;
    }
    return false;
}

bool InfoPane::AddNonLogger(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    if (p)
    {
        p->Reparent(this);

        Page* pg      = new Page();
        pg->indexInNB = AddPagePrivate(p, title, icon);
        pg->window    = p;
        pg->icon      = icon;
        pg->title     = title;
        pg->eventID   = wxNewId();
        pg->islogger  = false;

        m_Pages.Add(pg);

        return true;
    }
    return false;
}


bool InfoPane::DeleteLogger(Logger* l)
{
    if (!l)
        return false;

    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->logger == l)
        {
            int index = Manager::Get()->GetLogManager()->FindIndex(l);
            if (index >= 0)
                Manager::Get()->GetLogManager()->DeleteLog(index);

            if (m_Pages.Item(i)->indexInNB >= 0)
                DeletePage(GetPageIndex(m_Pages.Item(i)->window));

            delete(m_Pages.Item(i));
            m_Pages.RemoveAt(i);
            return true;
        }
    }

   return false;
}

bool InfoPane::RemoveNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->window == p)
        {
            if (m_Pages.Item(i)->islogger)
                cbThrow(_T("Bad API usage. Shame on you."));

            RemovePage(GetPageIndex(m_Pages.Item(i)->window));
            m_Pages.RemoveAt(i);
            return true;
        }
    }

   return false;
}

bool InfoPane::DeleteNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < m_Pages.GetCount(); ++i)
    {
        if (m_Pages.Item(i)->window == p)
        {
            if (m_Pages.Item(i)->islogger)
                cbThrow(_T("Bad API usage. Shame on you."));

            if (m_Pages.Item(i)->indexInNB >= 0)
                DeletePage(GetPageIndex(m_Pages.Item(i)->window));

            delete(m_Pages.Item(i));
            m_Pages.RemoveAt(i);
            return true;
        }
    }

   return false;
}

