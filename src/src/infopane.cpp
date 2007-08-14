#include <wx/event.h>
#include "infopane.h"
#include <logmanager.h>


BEGIN_EVENT_TABLE(InfoPane, InfoPaneNotebook)
    EVT_MENU(-1,  InfoPane::OnMenu)
    EVT_RIGHT_UP(InfoPane::OnRightClick)
END_EVENT_TABLE()


/*
*  TODO: something is wrong yet: OnRightClick is never called?
*        also, might think of a better way to hide/show items
*/


void InfoPane::OnMenu(wxCommandEvent& event)
{
    if(event.GetId() < baseID)          // pushing an event handler might be more elegant than listening on all, but oh well...
    {
        event.Skip();
        return;
    }

    int index = event.GetId() - baseID; // get back our indices
                                        // another (cleaner) way would be to use only one ID, get the menu item from the event, then its title,
                                        // and finally search our list of Loggers

    /*
    *  TODO: now, actually do something, hide the tab, or whatever
    */
}

void InfoPane::OnRightClick(wxMouseEvent& event)
{

    // We will boldly use the ID ranges above this unique value, this shold work fine too, as we use the IDs in a blocking call
    // Also, other than during startup, nobody else should be creating unique IDs anyway...
    baseID = wxNewId();

    wxMenu menu;

    for(size_t i = LogManager::app_log; i < ::max_logs; ++i)
    {
        if(!!(LogManager::Get()->Slot(i).title))
            menu.Append(baseID + i, LogManager::Get()->Slot(i).title);
    }

    PopupMenu(&menu, event.GetX(), event.GetY());
}

