#include "wxsheaders.h"
#include "wxspropertiesman.h"

#include "wxsmith.h"
#include "wxseventseditor.h"

wxsPropertiesMan::wxsPropertiesMan():
    CurrentWidget(NULL),
    PropertiesPanel(NULL)
{
    if ( Singleton==NULL ) Singleton = this;
}

wxsPropertiesMan::~wxsPropertiesMan()
{
    if ( CurrentWidget )
    {
        CurrentWidget->KillPropertiesWindow();
        CurrentWidget = NULL;
    }
    if ( Singleton==this ) Singleton = NULL;
}

void wxsPropertiesMan::SetActiveWidget(wxsWidget* Widget)
{
    if ( !PropertiesPanel || !EventsPanel ) return;

	/** Rebuilding properties panel */
    if ( CurrentWidget == Widget ) return;

    PropertiesPanel->Freeze();

    if ( CurrentWidget )
    {
        CurrentWidget->KillPropertiesWindow();
        CurrentWidget = NULL;
    }

    PropertiesPanel->SetSizer(NULL);
    PropertiesPanel->DestroyChildren();

    CurrentWidget = Widget;

    wxSize Size = PropertiesPanel->GetSize();
    if ( CurrentWidget )
    {
        wxFlexGridSizer* NewSizer = new wxFlexGridSizer(1);
        PropertiesPanel->SetVirtualSizeHints(1,1);
        PropertiesPanel->SetSize(1,1);
        NewSizer->AddGrowableCol(0);
        NewSizer->AddGrowableRow(0);
        wxWindow* Wnd = CurrentWidget->CreatePropertiesWindow(PropertiesPanel);
        CurrentWidget->UpdatePropertiesWindow();
        NewSizer->Add(Wnd,1,wxGROW);
        PropertiesPanel->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(PropertiesPanel);

        wxsTREE()->SelectItem(Widget->GetTreeId());
    }

    PropertiesPanel->SetSize(Size);
    PropertiesPanel->Refresh();
    PropertiesPanel->Thaw();

    /** Rebuilding events panel */

    EventsPanel->Freeze();
    EventsPanel->SetSizer(NULL);
    EventsPanel->DestroyChildren();
    Size = EventsPanel->GetSize();

    if ( CurrentWidget )
    {
        wxFlexGridSizer* NewSizer = new wxFlexGridSizer(1);
        EventsPanel->SetSize(1,1);
        NewSizer->AddGrowableCol(0);
        NewSizer->AddGrowableRow(0);
        wxWindow* Wnd = new wxsEventsEditor(EventsPanel,CurrentWidget);
        NewSizer->Add(Wnd,1,wxGROW);
        EventsPanel->SetSizer(NewSizer);
        NewSizer->SetVirtualSizeHints(EventsPanel);
    }
    EventsPanel->SetSize(Size);
    EventsPanel->Refresh();
    EventsPanel->Thaw();
}

void wxsPropertiesMan::OnSelectWidget(wxsEvent& event)
{
    SetActiveWidget(event.GetWidget());
}

void wxsPropertiesMan::OnUnselectWidget(wxsEvent& event)
{
    if ( event.GetWidget() == CurrentWidget )
    {
        SetActiveWidget(NULL);
    }
}

void wxsPropertiesMan::OnSelectRes(wxsEvent& event)
{
}

void wxsPropertiesMan::OnUnselectRes(wxsEvent& event)
{
}

BEGIN_EVENT_TABLE(wxsPropertiesMan,wxEvtHandler)
    EVT_SELECT_WIDGET(wxsPropertiesMan::OnSelectWidget)
    EVT_UNSELECT_WIDGET(wxsPropertiesMan::OnUnselectWidget)
    EVT_SELECT_RES(wxsPropertiesMan::OnSelectRes)
    EVT_UNSELECT_RES(wxsPropertiesMan::OnUnselectRes)
END_EVENT_TABLE()

/** Singleton definition */
wxsPropertiesMan* wxsPropertiesMan::Singleton = NULL;
