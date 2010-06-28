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

#include "infopane.h"
#include <logmanager.h>

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


InfoPane::InfoPane(wxWindow* parent) : cbAuiNotebook(parent, idNB, wxDefaultPosition, wxDefaultSize, infopane_flags), baseID(wxNewId())
{
    defaultBitmap = cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/edit_16x16.png"), wxBITMAP_TYPE_PNG);
    if (Manager::Get()->GetConfigManager(_T("app"))->ReadBool(_T("/environment/infopane_tabs_bottom"), false))
        SetWindowStyleFlag(GetWindowStyleFlag() | wxAUI_NB_BOTTOM);

    wxRegisterId(baseID + num_pages);
    for(int i = 0; i < num_pages; ++i)
    {
        page[i] = Page();
    }
}

InfoPane::~InfoPane()
{
}

int InfoPane::AddPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    const wxBitmap& bmp = icon ? *icon : defaultBitmap;

    AddPage(p, title, false, bmp);
    return GetPageCount() - 1;
}

void InfoPane::Toggle(size_t i)
{
    if(page[i].indexInNB == -1)
        page[i].indexInNB = AddPagePrivate(page[i].window, page[i].title, page[i].icon);
    else
    {
        // Hide the window, otherwise the controls remain partly visible on some windows-versions
        // if we toggle the active logger
        if(page[i].window)
        {
            page[i].window->Hide();
        }
        RemovePage(GetPageIndex(page[i].window));
        page[i].indexInNB = -1;
    }
}

int InfoPane::GetPageIndexByWindow(wxWindow* win)
{
    for (int i=0; i < num_pages; i++)
    {
        if (page[i].window == win)
        {
            return i;
        }
    }
    return -1;
}

void InfoPane::Show(size_t i)
{
    if(page[i].window == 0)
        return;

    if(page[i].indexInNB == -1)
    {
        Toggle(i);
    }
    else
    {
        SetSelection(GetPageIndex(page[i].window));
    }
}

void InfoPane::Show(Logger* logger)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].logger == logger)
        {
            if(page[i].indexInNB == -1)
            {
                Toggle(i);
            }
            else
            {
                SetSelection(GetPageIndex(page[i].window));
            }
            return;
        }
    }
}

void InfoPane::ShowNonLogger(wxWindow* p)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window == p)
        {
            if(page[i].indexInNB == -1)
            {
                Toggle(i);
            }
            else
            {
                SetSelection(GetPageIndex(p));
            }
            return;
        }
    }
}


void InfoPane::OnCopy(wxCommandEvent& event)
{
    int i = GetPageIndexByWindow(GetPage(GetSelection()));
    if (page[i].islogger)
    {
        if (event.GetId() == idCopyAllToClipboard)
            page[i].logger->CopyContentsToClipboard(false);
        else if (event.GetId() == idCopySelectedToClipboard)
            page[i].logger->CopyContentsToClipboard(true);
    }
}

void InfoPane::OnClear(wxCommandEvent& /*event*/)
{
    int i = GetPageIndexByWindow(GetPage(GetSelection()));
    if (page[i].islogger)
        page[i].logger->Clear();
}

void InfoPane::OnMenu(wxCommandEvent& event)
{
    if(event.GetId() < baseID || event.GetId() > baseID + num_pages)
    {
        event.Skip();
        return;
    }

    int i = event.GetId() - baseID; // get back our index
    Toggle(i);
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
    wxMenu menu;
    wxMenu* view = new wxMenu;
    menu.Append(idNB_TabTop, _("Tabs at top"));
    menu.Append(idNB_TabBottom, _("Tabs at bottom"));
    bool any_nonloggers = false;

    if (page[GetPageIndexByWindow(GetPage(GetSelection()))].islogger)
    {
        menu.AppendSeparator();
        menu.Append(idCopyAllToClipboard, _("Copy contents to clipboard"));
        menu.Append(idCopySelectedToClipboard, _("Copy selection to clipboard"));
        menu.AppendSeparator();
        menu.Append(idClear, _("Clear contents"));
    }
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window)
        {
            if(page[i].islogger)
            {
                view->Append(baseID + i, page[i].title, wxEmptyString, wxITEM_CHECK);
                view->Check(baseID + i, page[i].indexInNB != -1);
            }
            else
            {
                any_nonloggers = true;
            }
        }
    }
    if(any_nonloggers)
    {
        view->AppendSeparator();
        for(int i = 0; i < num_pages; ++i)
        {
            if(page[i].window && !page[i].islogger)
            {
                view->Append(baseID + i, page[i].title, wxEmptyString, wxITEM_CHECK);
                view->Check(baseID + i, page[i].indexInNB != -1);
            }
        }
    }

//    if (view->GetMenuItemCount() > 0)
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

int InfoPane::AddLogger(Logger* logger, wxWindow* p, const wxString& title, wxBitmap* icon)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(!(page[i].window))
        {
            page[i].indexInNB = AddPagePrivate(p, title, icon);
            page[i].window = p;
            page[i].logger = logger;
            page[i].icon = icon;
            page[i].title = title;
            page[i].islogger = true;
            return i;
        }
    }

   return -1;
}

int InfoPane::AddNonLogger(wxWindow* p, const wxString& title, wxBitmap* icon)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(!(page[i].window))
        {
            p->Reparent(this);
            page[i].indexInNB = AddPagePrivate(p, title, icon);
            page[i].window = p;
            page[i].icon = icon;
            page[i].title = title;
            page[i].islogger = false;
            return i;
        }
    }

    return -1;
}


bool InfoPane::DeleteLogger(Logger* l)
{
    if (!l)
    {
        return false;
    }

    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].logger == l)
        {
            int index = Manager::Get()->GetLogManager()->FindIndex(l);
            if (index != -1)
            {
                Manager::Get()->GetLogManager()->DeleteLog(index);
            }

            if (page[i].indexInNB != -1)
            {
                DeletePage(GetPageIndex(page[i].window));
            }

            page[i] = Page();
            return true;
        }
    }

   return false;
}

bool InfoPane::RemoveNonLogger(wxWindow* p)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window == p)
        {
            if(page[i].islogger)
            {
                cbThrow(_T("Bad API usage. Shame on you."));
            }

            RemovePage(GetPageIndex(page[i].window));
            page[i] = Page();
            return true;
        }
    }

   return false;
}

bool InfoPane::DeleteNonLogger(wxWindow* p)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window == p)
        {
            if(page[i].islogger)
            {
                cbThrow(_T("Bad API usage. Shame on you."));
            }

            if (page[i].indexInNB != -1)
            {
                DeletePage(GetPageIndex(page[i].window));
            }
            page[i] = Page();
            return true;
        }
    }

   return false;
}

