#include "wxscontainer.h"

#include "wxswidgetfactory.h"

wxsContainer::~wxsContainer()
{
    DeletingAll = true;
    for ( WidgetsI i = Widgets.begin(); i!=Widgets.end(); ++i )
    {
        wxsWidgetFactory::Get()->Kill(*i);
    }
    for ( ExtraI i = Extra.begin(); i!=Extra.end(); ++i )
    {
    	DelExtra(*i);
    }
    DeletingAll = false;
}
