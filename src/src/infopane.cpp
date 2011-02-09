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
#include "logmanager.h"

namespace
{
    int idClear = wxNewId();
    int idCopySelectedToClipboard = wxNewId();
    int idCopyAllToClipboard = wxNewId();
    int idNB = wxNewId();
    int idNB_TabTop = wxNewId();
    int idNB_TabBottom = wxNewId();
};

BEGIN_EVENT_TABLE(InfoPane, cbAuiNotebook)
    EVT_MENU(idClear,  InfoPane::OnClear)
    EVT_MENU_RANGE(idCopySelectedToClipboard, idCopyAllToClipboard,  InfoPane::OnCopy)
    EVT_MENU(wxID_ANY,  InfoPane::OnMenu)
    EVT_CONTEXT_MENU(InfoPane::ContextMenu)
    EVT_AUINOTEBOOK_TAB_RIGHT_UP(idNB, InfoPane::OnTabContextMenu)
    EVT_MENU(idNB_TabTop, InfoPane::OnTabPosition)
    EVT_MENU(idNB_TabBottom, InfoPane::OnTabPosition)
    EVT_AUINOTEBOOK_PAGE_CLOSE(idNB, InfoPane::OnCloseClicked)
 END_EVENT_TABLE()


InfoPane::InfoPane(wxWindow* parent) : cbAuiNotebook(parent, idNB, wxDefaultPosition, wxDefaultSize, infopane_flags)
{
    defaultBitmap = cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/edit_16x16.png"), wxBITMAP_TYPE_PNG);
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/infopane_tabs_bottom"), false))
        SetWindowStyleFlag(GetWindowStyleFlag() | wxAUI_NB_BOTTOM);
}

InfoPane::~InfoPane()
{
}

wxString InfoPane::SaveTabOrder()
{
    UpdateEffectiveTabOrder();
    page.Sort(&CompareIndexes);
    wxString layout;
    for (size_t i = 0 ; i < page.GetCount(); ++i)
    {
        layout << page.Item(i)->title;
        layout << _T("=");
        layout << page.Item(i)->indexInNB;
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
        for (size_t j = 0; j < page.GetCount();++j)
        {
            if (page.Item(j)->title == title)
            {
                long theIndex;
                if (theToken.AfterFirst('=').ToLong(&theIndex))
                    page.Item(j)->indexInNB = theIndex;
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
    page.Sort(cmp_f);
    if (page.GetCount() == 0)
        return;

    wxWindowUpdateLocker noUpdates(GetParent());
    for (size_t i = 0 ; i < page.GetCount(); ++i)
    {
        RemovePage(GetPageIndex(page.Item(i)->window));
        if (page.Item(i)->indexInNB < 0)
        {
            if (page.Item(i)->window)
                page.Item(i)->window->Hide();
        }
        else
            AddPagePrivate(page.Item(i)->window, page.Item(i)->title, page.Item(i)->icon);
    }
}

int InfoPane::AddPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    const wxBitmap& bmp = icon ? *icon : defaultBitmap;

    AddPage(p, title, false, bmp);
    return GetPageCount() - 1;
}

bool InfoPane::InsertPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon, int index)
{
    const wxBitmap& bmp = icon ? *icon : defaultBitmap;

    return InsertPage(index, p, title, false, bmp);
}

void InfoPane::UpdateEffectiveTabOrder()
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        // only update for visible tabs
        if (page.Item(i)->indexInNB >= 0)
            page.Item(i)->indexInNB = GetTabPositionFromIndex(GetPageIndex(page.Item(i)->window));

    }
}

void InfoPane::Toggle(size_t i)
{
    UpdateEffectiveTabOrder();
    if (page.Item(i)->indexInNB < 0)
    {
        if (InsertPagePrivate(page.Item(i)->window, page.Item(i)->title, page.Item(i)->icon, ~page.Item(i)->indexInNB))
            page.Item(i)->indexInNB = ~page.Item(i)->indexInNB ;
        else
            page.Item(i)->indexInNB = AddPagePrivate(page.Item(i)->window, page.Item(i)->title, page.Item(i)->icon);
    }
    else
    {
        // Hide the window, otherwise the controls remain partly visible on some windows-versions
        // if we toggle the active logger
        if (page.Item(i)->window)
            page.Item(i)->window->Hide();
        RemovePage(GetPageIndex(page.Item(i)->window));
        page.Item(i)->indexInNB = ~page.Item(i)->indexInNB;
    }
}

int InfoPane::GetPageIndexByWindow(wxWindow* win)
{
    for (size_t i=0; i < page.GetCount(); i++)
    {
        if (page.Item(i)->window == win)
            return i;
    }
    return -1;
}

int InfoPane::GetCurrentPage(bool &is_logger)
{
    int i = GetPageIndexByWindow(GetPage(GetSelection()));
    is_logger = page.Item(i)->islogger;
    return (is_logger?i:-1);
}

Logger* InfoPane::GetLogger(int index)
{
    if (index < 0 || (size_t)index > page.GetCount())
        return NULL;
    return page.Item(index)->islogger ? page.Item(index)->logger : NULL;
}

wxWindow* InfoPane::GetWindow(int index)
{
    if (index < 0 || (size_t)index > page.GetCount())
        return NULL;
    return !page.Item(index)->islogger ? page.Item(index)->window : NULL;
}

void InfoPane::Show(size_t i)
{
    if (page.Item(i)->window == 0)
        return;

    if (page.Item(i)->indexInNB < 0)
        Toggle(i);
    else
        SetSelection(GetPageIndex(page.Item(i)->window));
}

void InfoPane::Hide(Logger* logger)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->logger == logger)
        {
            if (page.Item(i)->indexInNB >= 0)
                Toggle(i);
            return;
        }
    }
}

void InfoPane::Show(Logger* logger)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->logger == logger)
        {
            if (page.Item(i)->indexInNB < 0)
                Toggle(i);
            else
                SetSelection(GetPageIndex(page.Item(i)->window));
            return;
        }
    }
}

void InfoPane::HideNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->window == p)
        {
            if (page.Item(i)->indexInNB >= 0)
                Toggle(i);
            return;
        }
    }
}

void InfoPane::ShowNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->window == p)
        {
            if (page.Item(i)->indexInNB < 0)
                Toggle(i);
            else
                SetSelection(GetPageIndex(p));
            return;
        }
    }
}


void InfoPane::OnCopy(wxCommandEvent& event)
{
    int i = GetPageIndexByWindow(GetPage(GetSelection()));
    if (page.Item(i)->islogger)
    {
        if (event.GetId() == idCopyAllToClipboard)
            page.Item(i)->logger->CopyContentsToClipboard(false);
        else if (event.GetId() == idCopySelectedToClipboard)
            page.Item(i)->logger->CopyContentsToClipboard(true);
    }
}

void InfoPane::OnClear(wxCommandEvent& /*event*/)
{
    int i = GetPageIndexByWindow(GetPage(GetSelection()));
    if (page.Item(i)->islogger)
        page.Item(i)->logger->Clear();
}

void InfoPane::OnMenu(wxCommandEvent& event)
{
    int eventID = event.GetId();
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (eventID == page.Item(i)->eventID)
        {
            Toggle(i);
            return;
        }
    }

    event.Skip();
    return;
}

void InfoPane::ContextMenu(wxContextMenuEvent& /*event*/)
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
    // toggle the notebook, that sends the event
    Toggle(GetPageIndexByWindow(GetPage(event.GetSelection())));
}

void InfoPane::DoShowContextMenu()
{
    UpdateEffectiveTabOrder();
    page.Sort(&CompareIndexes);
    wxMenu menu;
    wxMenu* view = new wxMenu;
    menu.Append(idNB_TabTop, _("Tabs at top"));
    menu.Append(idNB_TabBottom, _("Tabs at bottom"));
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/infopane_tabs_bottom"), false))
    	menu.FindItem(idNB_TabBottom)->Enable(false);
    else
    	menu.FindItem(idNB_TabTop)->Enable(false);

    if (page.Item(GetPageIndexByWindow(GetPage(GetSelection())))->islogger)
    {
        menu.AppendSeparator();
        menu.Append(idCopyAllToClipboard, _("Copy contents to clipboard"));
        menu.Append(idCopySelectedToClipboard, _("Copy selection to clipboard"));
        menu.AppendSeparator();
        menu.Append(idClear, _("Clear contents"));
    }
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->window)
        {
            view->Append(page.Item(i)->eventID, page.Item(i)->title, wxEmptyString, wxITEM_CHECK);
            view->Check(page.Item(i)->eventID, page.Item(i)->indexInNB >= 0);
        }
    }

    if (view->GetMenuItemCount() > 0)
    {
        menu.AppendSeparator();
        menu.AppendSubMenu(view, _("Toggle..."));
    }
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
        Page *tmp=new Page();
        tmp->indexInNB = AddPagePrivate(p, title, icon);
        tmp->window = p;
        tmp->logger = logger;
        tmp->icon = icon;
        tmp->title = title;
        tmp->eventID = wxNewId();
        tmp->islogger = true;
        page.Add(tmp);
        return true;
    }
    return false;
}

bool InfoPane::AddNonLogger(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    if (p)
    {
        p->Reparent(this);
        Page *tmp=new Page();
        tmp->indexInNB = AddPagePrivate(p, title, icon);
        tmp->window = p;
        tmp->icon = icon;
        tmp->title = title;
        tmp->eventID = wxNewId();
        tmp->islogger = false;
        page.Add(tmp);
        return true;
    }
    return false;
}


bool InfoPane::DeleteLogger(Logger* l)
{
    if (!l)
        return false;

    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->logger == l)
        {
            int index = Manager::Get()->GetLogManager()->FindIndex(l);
            if (index >= 0)
                Manager::Get()->GetLogManager()->DeleteLog(index);

            if (page.Item(i)->indexInNB >= 0)
                DeletePage(GetPageIndex(page.Item(i)->window));

            delete(page.Item(i));
            page.RemoveAt(i);
            return true;
        }
    }

   return false;
}

bool InfoPane::RemoveNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->window == p)
        {
            if (page.Item(i)->islogger)
                cbThrow(_T("Bad API usage. Shame on you."));

            RemovePage(GetPageIndex(page.Item(i)->window));
            page.RemoveAt(i);
            return true;
        }
    }

   return false;
}

bool InfoPane::DeleteNonLogger(wxWindow* p)
{
    for (size_t i = 0; i < page.GetCount(); ++i)
    {
        if (page.Item(i)->window == p)
        {
            if (page.Item(i)->islogger)
                cbThrow(_T("Bad API usage. Shame on you."));

            if (page.Item(i)->indexInNB >= 0)
                DeletePage(GetPageIndex(page.Item(i)->window));

            delete(page.Item(i));
            page.RemoveAt(i);
            return true;
        }
    }

   return false;
}

