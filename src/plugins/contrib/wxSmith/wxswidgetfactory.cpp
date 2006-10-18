#include "wxsheaders.h"
#include "wxswidgetfactory.h"

#include "wxsmith.h"
#include "wxspropertiesman.h"

wxsWidgetFactory* wxsWidgetFactory::Singleton = NULL;

wxsWidgetFactory::wxsWidgetFactory()
{
    if ( Singleton==NULL ) Singleton = this;
}

wxsWidgetFactory::~wxsWidgetFactory()
{
    if ( Singleton==this ) Singleton = NULL;
}

const wxsWidgetInfo * wxsWidgetFactory::GetInfo(const wxString& Name)
{
    WidgetsMapI i = Widgets.find(&Name);
    if ( i == Widgets.end() ) return NULL;
    return (*i).second;
}

wxsWidget * wxsWidgetFactory::Generate(const wxString& Name,wxsWindowRes* Res)
{
    WidgetsMapI i = Widgets.find(&Name);
    if ( i == Widgets.end() ) return NULL;
    const wxsWidgetInfo* Info = (*i).second;
    return Info->Manager->ProduceWidget(Info->Id,Res);
}

void wxsWidgetFactory::Kill(wxsWidget* Widget)
{
    if ( Widget )
    {
        // Closing properties if are set to given widget
        wxsUnselectWidget(Widget);

        // First unbinding it from parent
        wxsWidget* Parent = Widget->GetParent();
        if ( Parent )
        {
            Parent->DelChild(Widget);
        }
        Widget->KillTree(wxsTREE());
        if ( Widget->GetPreview() ) Widget->KillPreview();

        if ( Widget->GetPropertiesWindow() ) Widget->KillPropertiesWindow();

        // Deleting widget
        if ( Widget->GetInfo().Manager )
        {
            Widget->GetInfo().Manager->KillWidget(Widget);
        }
        else
        {
            // Possibly unsafe
            delete Widget;
        }
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
        if ( Info && wxsValidateIdentifier(Info->Name) )
        {
            Widgets[&Info->Name] = Info;
        }
    }
}

