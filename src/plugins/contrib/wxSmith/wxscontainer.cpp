#include "wxscontainer.h"

#include "wxswidgetfactory.h"

wxsContainer::~wxsContainer()
{
    DeletingAll = true;
    for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i )
    {
        wxsFACTORY()->Kill(*i);
    }
    for ( ExtraI i = Extra.begin(); i!=Extra.end(); ++i )
    {
    	DelExtra(*i);
    }
    DeletingAll = false;
}

int wxsContainer::FindChild(wxsWidget* Widget,int Level)
{
    if ( Level == 1 )
    {
        int Cnt = 0;
        for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i, ++Cnt )
        {
            if ( (*i)==Widget )
                return Cnt;
        }
        return -1;
    }
    
    while ( Widget )
    {
        Widget = Widget->GetParent();
        if ( Widget == this || --Level == 0 ) break;
    }
    
    return ( Widget == this ) ? 0 : 1;
}
