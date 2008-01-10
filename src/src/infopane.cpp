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
};

BEGIN_EVENT_TABLE(InfoPane, PieceOfShitBaseClass)
    EVT_MENU(idClear,  InfoPane::OnClear)
    EVT_MENU_RANGE(idCopySelectedToClipboard, idCopyAllToClipboard,  InfoPane::OnCopy)
    EVT_MENU(wxID_ANY,  InfoPane::OnMenu)
    EVT_CONTEXT_MENU(InfoPane::ContextMenu)
END_EVENT_TABLE()


InfoPane::InfoPane(wxWindow* parent) : InfoPaneNotebook(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, infopane_flags), baseID(wxNewId())
{
	defaultBitmap = cbLoadBitmap(ConfigManager::GetDataFolder() + _T("/images/edit_16x16.png"), wxBITMAP_TYPE_PNG);
	
    wxRegisterId(baseID + num_pages);
    for(int i = 0; i < num_pages; ++i)
    {
        page[i] = Page();
    }

#ifndef CB_USE_AUI_NOTEBOOK
    SetImageList(new wxFlatNotebookImageList);
#endif
}

InfoPane::~InfoPane()
{
    delete GetImageList();
}

int InfoPane::AddPagePrivate(wxWindow* p, const wxString& title, wxBitmap* icon)
{
	const wxBitmap& bmp = icon ? *icon : defaultBitmap;

#ifdef CB_USE_AUI_NOTEBOOK
	AddPage(p, title, false, bmp);
#else
    GetImageList()->push_back(bmp);
	AddPage(p, title, false, GetImageList()->size() - 1);
#endif
	return GetPageCount() - 1;
}

void InfoPane::Toggle(size_t i)
{
    if(page[i].indexInNB == -1)
        page[i].indexInNB = AddPagePrivate(page[i].window, page[i].title, page[i].icon);
    else
    {
        RemovePage(page[i].indexInNB);
        page[i].indexInNB = -1;
    }
}

void InfoPane::Show(size_t i)
{
    if(page[i].window == 0)
        return;

    if(page[i].indexInNB == -1)
        Toggle(i);
    else
        SetSelection(page[i].indexInNB);
}

void InfoPane::Show(Logger* logger)
{
    for(int i = 0; i < num_pages; ++i)
    {
        if(page[i].logger == logger)
        {
			if(page[i].indexInNB == -1)
				Toggle(i);
			else
				SetSelection(page[i].indexInNB);
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
				Toggle(i);
			else
				SetSelection(page[i].indexInNB);
			return;
        }
    }
}


void InfoPane::OnCopy(wxCommandEvent& event)
{
	int i = GetSelection();
	if (page[i].islogger)
	{
		if (event.GetId() == idCopyAllToClipboard)
			page[i].logger->CopyContentsToClipboard(false);
		else if (event.GetId() == idCopySelectedToClipboard)
			page[i].logger->CopyContentsToClipboard(true);
	}
}

void InfoPane::OnClear(wxCommandEvent& event)
{
	int i = GetSelection();
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

void InfoPane::ContextMenu(wxContextMenuEvent& event)
{
    wxMenu menu;
	wxMenu* view;
    bool any_nonloggers = false;

	if (page[GetSelection()].islogger)
	{
		view = new wxMenu;

		menu.Append(idCopyAllToClipboard, _("Copy contents to clipboard"));
		menu.Append(idCopySelectedToClipboard, _("Copy selection to clipboard"));
		menu.AppendSeparator();
		menu.Append(idClear, _("Clear contents"));
		menu.AppendSeparator();
	}
	else
		view = &menu;
	
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

	if (&menu != view)
		menu.AppendSubMenu(view, _("Toggle..."));
    PopupMenu(&menu);
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
				Manager::Get()->GetLogManager()->DeleteLog(index);
            
            if (page[i].indexInNB != -1)
				DeletePage(page[i].indexInNB);
            
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
                cbThrow(_T("Bad API usage. Shame on you."));

            RemovePage(page[i].indexInNB);
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
                cbThrow(_T("Bad API usage. Shame on you."));

			if (page[i].indexInNB != -1)
				DeletePage(page[i].indexInNB);
            page[i] = Page();
            return true;
        }
    }

   return false;
}

