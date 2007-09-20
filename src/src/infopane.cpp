#include <sdk.h>

#ifndef CB_PRECOMP
    #include <wx/event.h>
    #include <wx/menu.h>
    #include "cbexception.h"
#endif

#include "infopane.h"
#include <logmanager.h>


BEGIN_EVENT_TABLE(InfoPane, PieceOfShitBaseClass)
    EVT_MENU(wxID_ANY,  InfoPane::OnMenu)
    EVT_CONTEXT_MENU(InfoPane::ContextMenu)
END_EVENT_TABLE()


InfoPane::InfoPane(wxWindow* parent) : InfoPaneNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, infopane_flags), baseID(wxNewId())
{
    wxRegisterId(baseID + num_pages);
    for(int i = 0; i < num_pages; ++i)
    {
        page[i] = Page();
    }
}


void InfoPane::Toggle(size_t i)
{
    page[i].visible = 1 - page[i].visible;

    if(page[i].visible == true)
        AddPage(page[i].window, page[i].title);
    else
        RemovePage(GetPageIndex(page[i].window));
}

void InfoPane::Show(size_t i)
{
    if(page[i].window == 0)
        return;

    if(page[i].visible == false)
        Toggle(i);
    else
        SetSelection(i);
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

void InfoPane::ContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
    bool any_nonloggers = false;

    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window)
        {
            if(page[i].logger)
            {
                menu.Append(baseID + i, page[i].title, wxEmptyString, wxITEM_CHECK);
                if(page[i].visible)
                    menu.Check(baseID + i, true);
            }
            else
            {
                any_nonloggers = true;
            }
        }
    }

    if(any_nonloggers)
    {
        menu.AppendSeparator();
        for(int i = 0; i < num_pages; ++i)
        {
            if(page[i].window && !page[i].logger)
            {
                menu.Append(baseID + i, page[i].title, wxEmptyString, wxITEM_CHECK);
                if(page[i].visible)
                    menu.Check(baseID + i, true);
            }
        }
    }


    PopupMenu(&menu);
}




bool InfoPane::AddLogger(wxWindow* p, const wxString& title)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(!(page[i].window))
        {
            AddPage(p, title);
            page[i].window = p;
            page[i].title = title;
            page[i].visible = true;
            page[i].logger = true;
            return true;
        }
    }

   return false;
}

bool InfoPane::AddNonLogger(wxWindow* p, const wxString& title)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(!(page[i].window))
        {
            AddPage(p, title);
            page[i].window = p;
            page[i].title = title;
            page[i].visible = true;
            page[i].logger = false;
            return true;
        }
    }

    return false;
}


bool InfoPane::DeleteLogger(wxWindow* p, Logger* l)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].window == p)
        {
            LogManager::Get()->DeleteLog(LogManager::Get()->FindIndex(l));
            DeletePage(GetPageIndex(p));
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
            if(page[i].logger)
                cbThrow(_T("Bad API usage. Shame on you."));

            DeletePage(GetPageIndex(p));
            page[i] = Page();
            return true;
        }
    }

   return false;
}

