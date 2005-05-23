#include "wxswidgetfactory.h"

wxsWidgetFactory* wxsWidgetFactory::Singleton = new wxsWidgetFactory;

wxsWidgetFactory::wxsWidgetFactory()
{
}

wxsWidgetFactory::~wxsWidgetFactory()
{
}

const wxsWidgetInfo * wxsWidgetFactory::GetInfo(const char* Name)
{
    WidgetsMapI i = Widgets.find(Name);
    if ( i == Widgets.end() ) return NULL;
    return (*i).second;
}

wxsWidget * wxsWidgetFactory::Generate(const char* Name)
{
    WidgetsMapI i = Widgets.find(Name);
    if ( i == Widgets.end() ) return NULL;
    const wxsWidgetInfo* Info = (*i).second;
    return Info->Manager->ProduceWidget(Info->Id);
}

void wxsWidgetFactory::Kill(wxsWidget* Widget)
{
    if ( Widget )
    {
// TODO (SpOoN#1#): Check if manager exists, if not, try to delete this widget manually
        Widget->GetInfo().Manager->KillWidget(Widget);
    }
}

const wxsWidgetInfo * wxsWidgetFactory::GetFirstInfo()
{
    Iterator = Widgets.begin();
    return Iterator == Widgets.end() ? NULL : (*Iterator).second;
}

const wxsWidgetInfo* wxsWidgetFactory::GetNextInfo()
{
    if ( Iterator == Widgets.end() ) return NULL;
    if ( ++Iterator == Widgets.end() ) return NULL;
    return (*Iterator).second;
}

void wxsWidgetFactory::RegisterManager(wxsWidgetManager* Manager)
{
    if ( !Manager ) return;
    
    int Count = Manager->GetCount();
    for ( int i = 0; i<Count; i++ )
    {
        const wxsWidgetInfo* Info = Manager->GetWidgetInfo(i);
        if ( Info && Info->Name && *Info->Name )
        {
            Widgets[Info->Name] = Info;
        }
    }
}

